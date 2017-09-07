/*
 * Copyright (C) 2017 Gregor Koporec <gregor.koporec@gmail.com>, University of Ljubljana
 * Copyright (C) 2017 Janez Pers <janez.pers@fe.uni-lj.si>, University of Ljubljana
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// opencv
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/optflow.hpp>
#include <opencv2/tracking.hpp>

// std
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
#include <ctime>
#include <memory>

// posix
#include <wordexp.h> // for expanding ~ to $HOME directory

// local
#include "angledescriptor.hpp"
#include "histogramfile.hpp"
#include "roi.hpp"
#include "videotimer.hpp"
#include "userinteraction.hpp"
#include "tracker.hpp"
#include "ofterminalparser.hpp"
#include "scaler.hpp"
#include "of2trackerfile.hpp"
#include "flo.hpp"
#include "amplitudefactor.hpp"

using namespace cv;
using namespace std;
using namespace gk;

static const string SELECTION_WINDOW_NAME = "Selection";
static const string DISPLAY_WINDOW_TRACKER = "Tracker";
static const string DISPLAY_WINDOW_FLOW = "Optical flow";
static const int BOUNDING_BOX_THICKNESS = 2;

Point *startSelectionPoint = NULL, *endSelectionPoint = NULL;
Rect2d *selectedRoi = NULL;
Rect2d roi;
Mat frame, image;
int drag = 0;

static void calculateOpticalFlow(const UMat& uprevgray, const UMat& ugray,
        Mat& flow,
        const OpticalFlowData& config) {
    UMat uflow;
    switch (config.flowType) {
        case FARNEBACK:
            /*
             * Parameters:
             *
             * previous image
             * next image
             * computed flow image of type CV_32FC2 - 32 bit float 2-channel,
             * where 1-ch is u (velocity in x)
             * where 2-ch is v (velocity in y)
             *
             * pyramid scale 0.5 default
             * number of pyramid layers
             * averaging window size
             * number of iterations at each pyramid level
             * size of the pixel neighbourhood 5 or 7 default
             * standard deviation of the gaussian 1.1 or 1.2
             * operation flags
             */
            calcOpticalFlowFarneback(uprevgray, ugray, uflow,
                    config.pyramidScale,
                    config.pyramidLayers,
                    config.windowSize,
                    config.iterationsCount,
                    config.neighbourSize,
                    config.gaussianDeviation,
                    config.operationFlags
                    );
            break;

        case LUCAS_KANADE:
            break;

        default:
            cerr << endl;
            cerr << "Optical flow algorithm is not implemented." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
    }
    uflow.copyTo(flow);
}

static void getPolarFlow(
        const std::shared_ptr<AmplitudeFactor>& amplitudeFactor,
        OFTerminalParser& terminalParser,
        const UMat& uprevgray, const UMat& ugray, Mat& flow,
        Mat& flowAngle, Mat& flowMagnitude) {

    calculateOpticalFlow(uprevgray, ugray, flow, *terminalParser.getOpticalFlowConfig());

    // If we have enabled tracker we must crop our flow 
    if (terminalParser.isTrackerEnabled()) {
        Rect2d scaledRoi;
        if (terminalParser.isTrackerScalingEnabled()) {
            Scaler::scaleRoi(roi, scaledRoi, terminalParser.getTrackerUpScale());
            // Correct upscaled roi, because there can be numeric problems
            // With double we can get decimal places.
            Roi::correct<Rect2d>(scaledRoi, flow);
        } else {
            scaledRoi = roi;
        }
        /*
        Mat gray;
        ugray.copyTo(gray);
        rectangle(gray, scaledRoi, Scalar(0, 0, 255),
                2, 8, 0);
        imshow("Test", gray);
        waitKey(0);
        */
        //flow = Roi::extract<Rect2d>(flow, roi);
        if( !Roi::insideImage(flow, scaledRoi) ){
            cout << "ROI not inside image. Check Tracker files." << endl;
            exit(EXIT_FAILURE);
        }
        
        flow = Roi::crop<Rect2d>(flow, scaledRoi);
    }

    // Split channels and get flow magnitude
    vector<Mat> channel;
    split(flow, channel);

    cartToPolar(channel[0], channel[1], flowMagnitude, flowAngle, false);
    
    if (amplitudeFactor){
        amplitudeFactor->scale(flowMagnitude, roi);
    }
}

static void getOpticalFlowImage(const Mat& flowAngle, const Mat& flowMagnitude, Mat& opticalFlowImage) {
    // Convert optical flow to HSV image
    vector<Mat> hsvChannels;
    Mat hChannel, vChannel;

    // h channel 
    // Convert 0..2*pi -> 0..1 because using CV_32F
    normalize(flowAngle, hChannel, 0, 180, CV_MINMAX);
    hsvChannels.push_back(hChannel);

    // s channel
    // Normalize to 0..1 because using CV_32F
    normalize(flowMagnitude, vChannel, 0, 255, CV_MINMAX);
    hsvChannels.push_back(vChannel);

    // v channel
    // Channel is black so all ones because using CV_32F
    hsvChannels.push_back(Mat(hChannel.rows, hChannel.cols, CV_32FC1, 255));

    // Merge channels to image
    Mat hsvImage, hsvImage_8bit;
    //cout << hsvChannels[0].depth() << " " << hsvChannels[1].depth() << " " << hsvChannels[2].depth() << endl;
    merge(hsvChannels, hsvImage);
    hsvImage.convertTo(hsvImage_8bit, CV_8UC3);

    // Convert HSV to BGR image
    cvtColor(hsvImage_8bit, opticalFlowImage, CV_HSV2BGR);
}

static void getOpticalFlowFrame(const Mat& opticalFlowImage, Mat& opticalFlowFrame, const Size& videoSize) {
    // If size is not the same 
    // make new empty white video frame 
    // and add optical flow to the middle of frame.
    opticalFlowFrame = Mat::zeros(videoSize.height, videoSize.width, opticalFlowImage.type());
    int xPosition = cvRound(videoSize.width / 2.0 - opticalFlowImage.cols / 2.0);
    int yPosition = cvRound(videoSize.height / 2.0 - opticalFlowImage.rows / 2.0);

    opticalFlowImage.copyTo(
            opticalFlowFrame(
            Rect(xPosition, yPosition, opticalFlowImage.cols, opticalFlowImage.rows)
            )
            );

}

static void onMouseEvent(int event, int x, int y, int flags, void* param) {
    string* windowName = static_cast<string*> (param);

    if (event == CV_EVENT_LBUTTONDOWN && !drag) {
        // Left button clicked
        startSelectionPoint = new Point(x, y);
        drag = 1;

    } else if (event == CV_EVENT_RBUTTONDOWN && !drag) {
        // Right button clicked
        if (roi.area() > 0) {
            image = frame.clone();
            rectangle(image, roi, Scalar(0, 255, 0),
                    BOUNDING_BOX_THICKNESS, 8, 0);
            imshow(*windowName, image);
            drag = 0;
        }

    } else if (event == CV_EVENT_MOUSEMOVE && drag) {
        // Mouse dragged
        image = frame.clone();
        endSelectionPoint = new Point(x, y);
        rectangle(image, *startSelectionPoint,
                *endSelectionPoint, Scalar(0, 0, 255),
                BOUNDING_BOX_THICKNESS, 8, 0);
        imshow(*windowName, image);

    } else if (event == CV_EVENT_LBUTTONUP) {
        endSelectionPoint = new Point(x, y);
        selectedRoi = new Rect2d(startSelectionPoint->x, startSelectionPoint->y,
                endSelectionPoint->x - startSelectionPoint->x,
                endSelectionPoint->y - startSelectionPoint->y);
        Roi::correct(*selectedRoi, frame);
        drag = 0;
    }
}

static void selectRoiFromImage(const string& completion) {
    bool isSelected = false;
    string windowName = SELECTION_WINDOW_NAME + " " + completion + " %";
    namedWindow(windowName, WINDOW_NORMAL);
    // Init mouse callback
    setMouseCallback(windowName, onMouseEvent, &windowName);
    Mat rawFrame = frame.clone();
    do {
        // If ROI already exists, show it as green box and make it as selection
        if (roi.area() > 0) {
            rectangle(rawFrame, roi, Scalar(0, 255, 0), BOUNDING_BOX_THICKNESS, 8, 0);
            selectedRoi = new Rect2d(roi.x, roi.y, roi.width, roi.height);

        } else {
            // If ROI didn't exist.
            selectedRoi = new Rect2d();
        }
        imshow(windowName, rawFrame);
        int key = waitKey(0);
        switch ((char) key) {
            case 'q':
                cout << endl;
                cout << "You wanted to exit. Exiting..." << endl;
                exit(EXIT_SUCCESS);
                break;
            case 'n':
                cout << endl;
                cout << "You said no subject on a frame." << endl;
                selectedRoi->x = 0;
                selectedRoi->y = 0;
                selectedRoi->width = 0;
                selectedRoi->height = 0;
                break;
        }
        if (selectedRoi) {
            if (selectedRoi->width < 0 || selectedRoi->height < 0) {
                cout << endl;
                cout << "You must drag from top left to bottom right corner." << endl;
                cout << "Please do it again." << endl;
                isSelected = false;

            } else {
                destroyWindow(windowName);
                isSelected = true;
            }
        } else {
            cout << endl;
            cout << "You must drag from top left to bottom right corner." << endl;
            cout << "Please do it again." << endl;
            isSelected = false;
        }
    } while (isSelected == false);

    if (selectedRoi) {
        roi = *selectedRoi;

        // clear all variables
        delete selectedRoi;
        selectedRoi = NULL;
        delete startSelectionPoint;
        startSelectionPoint = NULL;
        delete endSelectionPoint;
        endSelectionPoint = NULL;

        // Show user
        cout << endl;
        cout << "New region of interest was selected: " << endl;
        cout << "x: " << cvRound(roi.x) << "\ty: " << cvRound(roi.y) << "\twidth: ";
        cout << cvRound(roi.width) << "\theight: " << cvRound(roi.height) << endl;
    }
}

int main(int argc, char** argv) {
    OFTerminalParser terminalParser = OFTerminalParser(argc, argv, roi);

    std::shared_ptr<gk::Tracker> tracker = NULL;
    tracker = terminalParser.getTracker();

    // Get descriptors
    std::shared_ptr<gk::AmplitudeDescriptor> amplitudeDescriptor = NULL;
    if (terminalParser.isAmplitudeDescriptorUsed()) {
        amplitudeDescriptor = terminalParser.getAmplitudeDescriptor();
    }

    std::shared_ptr<gk::AngleDescriptor> histogramDescriptor = NULL;
    if(terminalParser.isAngleDescriptorUsed()){
        histogramDescriptor = terminalParser.getAngleDescriptor();
    }

    std::shared_ptr<Flo> flo = NULL;

    // Open video for optical flow estimation
    // If video is not opened end this program with error
    std::shared_ptr<VideoCapture> videoCapture = std::make_shared<VideoCapture>(terminalParser.getInputVideoname());
    if (!videoCapture->isOpened()) {
        cout << endl;
        cout << "Could not open the input video: " << terminalParser.getInputVideoname() << endl;
        exit(EXIT_FAILURE);
    }

    // Get codec type - 4-char code for codec
    int codecNum = static_cast<int> (videoCapture->get(CAP_PROP_FOURCC));
    double fps = videoCapture->get(CAP_PROP_FPS);
    Size videoSize((int) videoCapture->get(CAP_PROP_FRAME_WIDTH),
            (int) videoCapture->get(CAP_PROP_FRAME_HEIGHT));
    double frameCount = videoCapture->get(CAP_PROP_FRAME_COUNT);

    UserInteraction::printVideoProperties(
            videoSize, codecNum, fps, frameCount, terminalParser.getInputVideoname()
            );

    if (terminalParser.getOpticalFlowConfig()->flowType == LUCAS_KANADE) {
        // automatic initialization
        //goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
        //cornerSubPix(gray, points[1], subPixWinSize, Size(-1,-1), termcrit);
    }

    VideoWriter opticalFlowVideoWriter;
    if (terminalParser.getOpticalFlowConfig()->needVideo) {
        opticalFlowVideoWriter.open(
                terminalParser.getOpticalFlowConfig()->outVideo,
                codecNum,
                fps,
                videoSize,
                true);

        if (!opticalFlowVideoWriter.isOpened()) {
            cout << endl;
            cout << "Could not open the output video for write: "
                    << terminalParser.getOpticalFlowConfig()->outVideo << endl;
            cout << "Terminating..." << endl;
            return -1;
        }

        UserInteraction::printVideoProperties(videoSize, codecNum, fps, 0,
                terminalParser.getOpticalFlowConfig()->outVideo);
    }

    cout << "=======================" << endl;
    cout << "Starting optical flow estimation..." << endl;

    Mat scaledFrame;
    Mat flow, trackerFrame;
    Mat flowAngle, flowMagnitude;
    Mat opticalFlowImage, opticalFlowFrame;
    UMat ugray, uprevgray;
    double frameNumber;

    // For writing histogram features to file
    HistogramFile histogramFile(terminalParser.getHistogramFilename());

    std::shared_ptr<OF2TrackerFile> trackerFile = NULL;
    if (terminalParser.trackerFromFile()) {
        trackerFile = terminalParser.getOF2TrackerFile();
    }

    // All initial values are 0.0
    vector<float> normalizedHistogram;
    if(histogramDescriptor){
        normalizedHistogram = vector<float>(histogramDescriptor->getBinCount());
    }

    vector<float> amplitudeHistogram;
    if (amplitudeDescriptor) {
        amplitudeHistogram = vector<float>(amplitudeDescriptor->getBinCount());
    }
    
    std::shared_ptr<AmplitudeFactor> amplitudeFactor = terminalParser.getAmplitudeFactor();

    bool constructDisplayWindow = true, constructDisplayWindowFlow = true;
    bool changeDisplayWindowImage = false, changeDisplayWindowImageFlow = false;

    VideoTimer timer(videoCapture, 1.0, terminalParser.getSelectionSeconds());
    timer.start();
    if (terminalParser.isSelectionEnabled() &&
            !terminalParser.needSelectRoiOnFirstFrame()) {
        timer.startTimeToSelect();
    }
    

    for (;;) {
        // Get new frame from video
        // If there are no more frames in video file stop for loop     
        if (!videoCapture->read(frame)) {
            cout << endl;
            cout << "=======================" << endl;
            cout << "Frame " << videoCapture->get(CAP_PROP_POS_FRAMES)
                    << " is last frame." << endl;
            cout << "Video time at end: " << timer.getVideoTime() << endl;
            cout << "=======================" << endl;
            break;
        }
        frameNumber = videoCapture->get(CAP_PROP_POS_FRAMES); // 0-based index

        // Get gray frame for optical flow calculation
        if (frame.empty()) {
            cerr << "Frame " << frameNumber << " is empty. Skipping it..." << endl;
            continue;
        }

        if (frame.channels() == 3) {
            cvtColor(frame, ugray, COLOR_BGR2GRAY);

        } else if (frame.channels() == 1) {
            cout << "Frame is greyscale." << endl;
            frame.copyTo(ugray);

        } else {
            cerr << "Frame " << frameNumber
                    << " has oddly number of channels. Histograms will be empty. " << endl;
            cerr << "Skipping this frame..." << endl;
            if (frameNumber > 0) {
                
                if(histogramDescriptor){
                    fill(normalizedHistogram.begin(), normalizedHistogram.end(), 0);
                    histogramFile.write(normalizedHistogram);
                }else if (amplitudeDescriptor){
                    fill(amplitudeHistogram.begin(), amplitudeHistogram.end(), 0);
                    histogramFile.write(amplitudeHistogram);
                }
            }
            continue;
        }

        if (terminalParser.isTrackerScalingEnabled()) {
            Scaler::scaleFrame(frame, frame, terminalParser.getTrackerDownScale());
        }

        // If previous gray not empty calculate optical flow
        if (!uprevgray.empty())
            //if(videoCapture.get(CAP_PROP_POS_FRAMES) > 274310)
        {
            // Get part of tracked frame if tracker is enabled
            if (terminalParser.isTrackerEnabled()) {

                if (terminalParser.trackerFromFile()) {
                    roi = *trackerFile->getNext();
                    if (roi.area() > 0) {
                        cout << endl;
                        cerr << "Couldn't read a line. " << "Roi will be empty." << endl;
                        // Roi should be empty.
                        roi = Rect2d(0, 0, 0, 0);

                    }

                } else {
                    // Update tracker with downscaled frame and roi
                    // If tracker found object, it is updated, so correct roi
                    // And continue
                    if (tracker->update(frame, roi)) {
                        Roi::correct<Rect2d>(roi, frame);
                    } else {
                        // Tracker didn't found tracked object
                        // Roi should be empty.
                        roi = Rect2d(0, 0, 0, 0);
                    }

                    // If it is time to reselect ROI
                    // select ROI and reinitialize tracker
                    if (terminalParser.isSelectionEnabled()) {
                        if (timer.isTimeToSelect()) {
                            selectRoiFromImage(timer.getCompletion());
                            // Restart timer
                            timer.restartTimeToSelect();

                            // Init tracker with downscaled frame and roi.
                            // If subject is on a frame
                            if (roi.area() > 0) {
                                tracker->init(frame, roi);
                            }
                        }
                    }
                }
            }

            if (terminalParser.needDisplayTracker()) {
                if (constructDisplayWindow) {
                    namedWindow(DISPLAY_WINDOW_TRACKER, WINDOW_NORMAL);
                    constructDisplayWindow = false;
                    changeDisplayWindowImage = true;
                }
                if (changeDisplayWindowImage) {
                    frame.copyTo(trackerFrame);
                    // Test if crop is working
                    //Roi::crop(frame, roi).copyTo(trackerFrame);
                    if (roi.area() > 0) {
                        cv::rectangle(trackerFrame, roi, Scalar(255, 0, 0),
                                BOUNDING_BOX_THICKNESS, 8, 0);
                    }
                    imshow(DISPLAY_WINDOW_TRACKER, trackerFrame);
                    int key = waitKey(1);
                    switch ((char) key) {
                        case 'q':
                            cout << endl;
                            cout << "You wanted to exit. Exiting..." << endl;
                            exit(EXIT_SUCCESS);
                            break;
                        case 'w':
                            cout << endl;
                            cout << "You don't want output to be shown..." << endl;
                            destroyWindow(DISPLAY_WINDOW_TRACKER);
                            changeDisplayWindowImage = false;
                            break;
                    }
                }
            }

            // Something went wrong because sizes are different.
            if (uprevgray.size() != ugray.size()) {
                cerr << endl;
                cerr << "Something went wrong because sizes of previous"
                        << "frame and current frame are different" << endl;
                cerr << "Maybe the problem is in video." << endl;
                cerr << "Previous frame size: "
                        << uprevgray.rows << "x" << uprevgray.cols << endl;
                cerr << "Current frame size: "
                        << ugray.rows << "x" << ugray.cols << endl;
                cerr << "Current timestamp in ms: "
                        << videoCapture->get(CAP_PROP_POS_MSEC) << endl;
                cerr << "Next frame: "
                        << videoCapture->get(CAP_PROP_POS_FRAMES) << endl;
                cerr << "Exiting..." << endl;
                exit(EXIT_FAILURE);
            }

            // If tracker is enabled and ROI is empty
            // then histogram will be empty (All zero values).
            // 1e-15 is double precision
            if ((terminalParser.isTrackerEnabled() ||
                    terminalParser.trackerFromFile()) && roi.area() <= 1e-15) {
                
                if (histogramDescriptor){
                    fill(normalizedHistogram.begin(), normalizedHistogram.end(), 0);
                }

                // Type is same as type returned by calcOpticalFlowFarneback()
                flowAngle = Mat::zeros(frame.size(), CV_32FC1);
                flowMagnitude = Mat::zeros(frame.size(), CV_32FC1);

                if (amplitudeDescriptor) {
                    fill(amplitudeHistogram.begin(), amplitudeHistogram.end(), 0);
                }
            } else {
                
                getPolarFlow(amplitudeFactor, terminalParser, uprevgray, ugray, flow, flowAngle, flowMagnitude);
                
                if(histogramDescriptor){                
                    // Normalize angles
                    Mat normalizedFlowAngle;
                    flowAngle.copyTo(normalizedFlowAngle);
                    AngleDescriptor::normalizeAngles(normalizedFlowAngle);

                    // Calculate normalized histogram
                    histogramDescriptor->getHistogram(normalizedFlowAngle, flowMagnitude, normalizedHistogram);
                }
                
                

                if (amplitudeDescriptor) {
                    // Uncomment to meausure noise!
                    //cout << endl;
                    //cout << flowMagnitude.at<float>(359, 349) << endl;
                    amplitudeDescriptor->getHistogram(flowMagnitude, amplitudeHistogram);
                }
            }

            if (amplitudeDescriptor && histogramDescriptor) {
                normalizedHistogram.insert(
                        normalizedHistogram.end(),
                        amplitudeHistogram.begin(),
                        amplitudeHistogram.end()
                        );
            }
            
            if(histogramDescriptor){
            // Write normalized histogram to csv file
                histogramFile.write(normalizedHistogram);
                
            } else if(amplitudeDescriptor){
                histogramFile.write(amplitudeHistogram);
            }

            // If we have first frame and tracker enabled
            // we initialize region of interest or "roi"
        } else //if(videoCapture.get(CAP_PROP_POS_FRAMES) > 274309)
        {
            if (!terminalParser.trackerFromFile()) {
                if (terminalParser.isTrackerEnabled()) {
                    if (terminalParser.needSelectRoiOnFirstFrame()) {
                        // If we need to select ROI for the first time.
                        selectRoiFromImage(timer.getCompletion());
                        // Restart timer
                        timer.restartTimeToSelect();
                    }

                    // Init tracker with downscaled frame and roi.
                    getBuildInformation();
                    tracker->init(frame, roi);

                }
            }
        }

        // Make current gray previous gray
        std::swap(uprevgray, ugray);

        // Show % for user
        if (timer.isTimeToShowOutput()) {
            const char* completion = timer.getCompletion().c_str();
            printf("\rFPS: %s\tCompletion: %s %%\tVideo time: %.2f\tFrame: %.2f\tElapsed: %s\tEstimated: %s",
                    timer.getFps().c_str(),
                    timer.getCompletion().c_str(),
                    videoCapture->get(CAP_PROP_POS_MSEC) / 1000.0,
                    videoCapture->get(CAP_PROP_POS_FRAMES),
                    (timer.getElapsedTime()->c_str()),
                    timer.getEstimatedTime().c_str()
                    );
            fflush(stdout);
        }

        // If we read more than one frame
        if (frameNumber > 0) {
            // For saving flo file
            flo = terminalParser.getFlo();
            if (flo && frameNumber == flo->getFloFrameNumber()) {
                if (!optflow::writeOpticalFlow(flo->getFloFilename(), flow)) {
                    cout << endl;
                    cout << "Could not write optical flow to .flo file." << endl;
                }
            }

            // For saving selected image
            if (terminalParser.isImageNeeded() && frameNumber == terminalParser.getFrameNumber()) {
                imwrite(terminalParser.getOriginalImageFilename(), frame);

                getOpticalFlowImage(flowAngle, flowMagnitude, opticalFlowImage);
                imwrite(
                        terminalParser.getOpticalFlowConfig()->outVideo,
                        opticalFlowImage);
            }

            // For writing optical flow video
            if (terminalParser.getOpticalFlowConfig()->needVideo) {
                getOpticalFlowImage(flowAngle, flowMagnitude, opticalFlowImage);
                if (opticalFlowImage.cols == videoSize.width && opticalFlowImage.rows == videoSize.height) {
                    opticalFlowVideoWriter << opticalFlowImage;

                } else {
                    getOpticalFlowFrame(opticalFlowImage, opticalFlowFrame, videoSize);
                    opticalFlowVideoWriter << opticalFlowFrame;
                }
            }

            if (terminalParser.getOpticalFlowConfig()->displayFlow) {
                if (constructDisplayWindowFlow) {
                    namedWindow(DISPLAY_WINDOW_FLOW, WINDOW_NORMAL);
                    constructDisplayWindowFlow = false;
                    changeDisplayWindowImageFlow = true;
                }
                if (changeDisplayWindowImageFlow) {
                    getOpticalFlowImage(flowAngle, flowMagnitude, opticalFlowImage);
                    imshow(DISPLAY_WINDOW_FLOW, opticalFlowImage);
                    int key = waitKey(1);
                    switch ((char) key) {
                        case 'q':
                            cout << endl;
                            cout << "You wanted to exit. Exiting..." << endl;
                            exit(EXIT_SUCCESS);
                            break;
                        case 'w':
                            cout << endl;
                            cout << "You don't want output to be shown..." << endl;
                            destroyWindow(DISPLAY_WINDOW_FLOW);
                            changeDisplayWindowImageFlow = false;
                            break;
                    }
                }
            }
        }
    }
    if (trackerFile) {
        cout << endl;
        cout << "Tracking is finnished." << endl;
    }

    cout << endl;
    printf("Elapsed time: %s\n", timer.getElapsedTime()->c_str());
    cout << "Video time: " << timer.getVideoTime() << endl;
    cout << "Max fps: " << timer.getMaxFps() << endl;
    cout << "Is real time: " << timer.isRealTime() << endl;
    cout << "=========================================================" << endl;

    return 0;
}