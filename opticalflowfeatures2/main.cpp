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

// std
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// local
#include "angledescriptor.hpp"
#include "amplitudedescriptor.hpp"
#include "histogramfile.hpp"
#include "roi.hpp"
#include "videotimer.hpp"
#include "userinteraction.hpp"
#include "of2terminalparser.hpp"

#include "opticalflowvideo.hpp"
#include "cameraselector.hpp"
#include "selectorfile.hpp"
#include "timefilewriter.hpp"
#include "of2databox.hpp"
#include "flofile.hpp"
#include "config.hpp"

using namespace cv;
using namespace std;
using namespace gk;

static const string DISPLAY_WINDOW_FLOW = "Optical flow";
static const int BOUNDING_BOX_THICKNESS = 2;

//Mat frame, image;
static void printErrorHeader(int line){
    cerr << endl;
    cerr << "File: " << __FILE__ << " line: " << line << endl;
}

static void printErrorFooter(){
    cerr << "Aborting program..." << endl;
    cerr << endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {

    /// TERMINAL PARSER
    OF2TerminalParser terminalParser = OF2TerminalParser(argc, const_cast<const char**> (argv), VERSION_MAJOR, VERSION_MINOR);
    try {
        terminalParser.parseInput();
    } catch (std::exception& e) {
        printErrorHeader(__LINE__);
        cerr << e.what() << endl;
        printErrorFooter();
    }
    /// TERMINAL PARSER


    /// DESCRIPTORS
    // Get descriptors
    std::shared_ptr<gk::AmplitudeDescriptor> amplitudeDescriptor = NULL;
    if (terminalParser.amplitudeDescriptorData.binCount > 0) {
        amplitudeDescriptor =
                std::make_shared<AmplitudeDescriptor>(terminalParser.amplitudeDescriptorData);
    }

    std::shared_ptr<gk::AngleDescriptor> angleDescriptor = NULL;
    if (terminalParser.angleDescriptorData.binCount > 0) {
        angleDescriptor = std::make_shared<AngleDescriptor>(terminalParser.angleDescriptorData);
    }

    // All initial values are 0.0
    vector<float> angleHistogram;
    vector<float> amplitudeHistogram;  
    vector<float> normalizedHistogram;
    /// DESCRIPTORS


    /// CAMERA SELECTOR
    SelectorFile selectorFile(terminalParser.cameraSelectorFilename);
    CameraSelectorData data;
    try{
        data = selectorFile.getNext();
    } catch(std::exception& e){
        printErrorHeader(__LINE__);
        cerr << e.what() << endl;
        printErrorFooter();

    }
    CameraSelector cameraSelector(data);
    /// CAMERA SELECTOR
    

    
    /// FILE WRITERS  
    std::shared_ptr<gk::FloFile> floFile = NULL;
    if(!terminalParser.floFilename.empty()){
        floFile = std::make_shared<FloFile>(terminalParser.floFilename, terminalParser.floFrameCount);
    }
    HistogramFile histogramFile(terminalParser.outHistFilename);
    TimeFileWriter timeFileWriter(terminalParser.outTimeFilename);
    /// FILE WRITERS

    
    /// CONTAINERS
    std::vector< std::shared_ptr<OF2DataBox> > dataBoxes;

    for (int i = 0; i < terminalParser.videoFilenames.size(); i++) {
        auto dataBox = std::make_shared<OF2DataBox>(terminalParser.videoFilenames[i],
                terminalParser.depthFilenames[i],
                terminalParser.trackerFilenames[i],
                terminalParser.timeFilenames[i],
                terminalParser.intrinsicFilenames[i],
                terminalParser.extrinsicFilenames[i],
                terminalParser.diagFilenames[i],
                terminalParser.startFrame,
                terminalParser.opticalFlowData,
                terminalParser.trackerData);

        dataBoxes.push_back(dataBox);
    }
    /// CONTAINERS
    
    

    /// VIDEOS
    std::shared_ptr<OpticalFlowVideo> opticalFlowVideoWriter = NULL;
    std::shared_ptr<cv::VideoWriter> videoWriter = NULL;
    if (terminalParser.opticalFlowData.needVideo || terminalParser.opticalFlowData.displayFlow) {
        opticalFlowVideoWriter = std::make_shared<OpticalFlowVideo>(
                terminalParser.opticalFlowData.outFlowVideo,
                dataBoxes[0]->codecNum, dataBoxes[0]->fps, dataBoxes[0]->videoSize);
        
        videoWriter = std::make_shared<cv::VideoWriter>(terminalParser.opticalFlowData.outVideo, 
                dataBoxes[0]->codecNum, dataBoxes[0]->fps, dataBoxes[0]->videoSize);

        UserInteraction::printVideoProperties(dataBoxes[0]->videoSize, dataBoxes[0]->codecNum, dataBoxes[0]->fps, 0,
                terminalParser.opticalFlowData.outFlowVideo);
    }
    /// VIDEOS
    

    cout << "=======================" << endl;
    cout << "Starting optical flow estimation..." << endl;

    Mat opticalFlowImage, opticalFlowFrame;
    bool constructDisplayWindowFlow = true;
    bool changeDisplayWindowImageFlow = false;

    vector<Point3d> metricCenters;  
    int selected = -1;
    vector<bool> videosEnd;
    
    
    for (int f = 0;; f++) {     
        
        for(auto dataBox : dataBoxes){
            if(!dataBox->update()){
                videosEnd.push_back(true);
            }
            metricCenters.push_back(dataBox->metricCenter);
        }

        if(videosEnd.size() == dataBoxes.size()){
            break;
            
        }else{
            videosEnd.clear();
        }

        
 
        
        if(f > 0){
            selected = cameraSelector.select(metricCenters);


            if (angleDescriptor) {
                angleHistogram = vector<float>(amplitudeDescriptor->getBinCount());
                
                if(dataBoxes[selected]->confident) {
                    // Normalize angles
                    Mat normalizedFlowAngle;
                    dataBoxes[selected]->angle.copyTo(normalizedFlowAngle);
                    AngleDescriptor::normalizeAngles(normalizedFlowAngle);

                    // Calculate normalized histogram
                    angleDescriptor->getHistogram(normalizedFlowAngle, dataBoxes[selected]->magnitude, angleHistogram);
                }
            }

            if (amplitudeDescriptor) {
                amplitudeHistogram = vector<float>(amplitudeDescriptor->getBinCount());
                
                if(dataBoxes[selected]->confident) {
                    amplitudeDescriptor->getHistogram(dataBoxes[selected]->magnitude, amplitudeHistogram);
                }
            }
            
            
            normalizedHistogram.clear();
            if (amplitudeDescriptor && angleDescriptor) {
                normalizedHistogram.insert(normalizedHistogram.begin(),
                        angleHistogram.begin(), angleHistogram.end());
                normalizedHistogram.insert(normalizedHistogram.end(),
                        amplitudeHistogram.begin(), amplitudeHistogram.end());
                
            } else if(angleDescriptor){
                normalizedHistogram.insert(normalizedHistogram.begin(),
                        angleHistogram.begin(), angleHistogram.end());
                
            } else if (amplitudeDescriptor) {
                normalizedHistogram.insert(normalizedHistogram.begin(),
                        amplitudeHistogram.begin(), amplitudeHistogram.end());
            }
            
            // Write normalized histogram to csv file
            histogramFile.write(normalizedHistogram);
            timeFileWriter.write(dataBoxes[selected]->timeStamp);

        }


     



        // Show % for user
        if (dataBoxes[0]->timer->isTimeToShowOutput()) {
            const char* completion = dataBoxes[0]->timer->getCompletion().c_str();
            printf("\rFPS: %s\tCompletion: %s %%\tVideo time: %.2f\tFrame: %.2f\tElapsed: %s\tEstimated: %s",
                    dataBoxes[0]->timer->getFps().c_str(),
                    dataBoxes[0]->timer->getCompletion().c_str(),
                    dataBoxes[0]->video->get(CAP_PROP_POS_MSEC) / 1000.0,
                    dataBoxes[0]->video->get(CAP_PROP_POS_FRAMES),
                    dataBoxes[0]->timer->getElapsedTime()->c_str(),
                    dataBoxes[0]->timer->getEstimatedTime().c_str()
                    );
            fflush(stdout);
        }


        

        // If we read more than one frame
        if (f > 0) {
            // For writing FLO
            if (floFile) {
                if(f == floFile->getFrameNumber()){
                    floFile->write(dataBoxes[selected]->angle, dataBoxes[selected]->magnitude);
                }
            }

            // For writing optical flow video
            if (terminalParser.opticalFlowData.needVideo) {
                if(opticalFlowVideoWriter){
                    opticalFlowVideoWriter->write(dataBoxes[selected]->angle, dataBoxes[selected]->magnitude);
                } else {
                    printErrorHeader(__LINE__);
                    cerr << "No video writer for optical flow!" << endl;
                    printErrorFooter();

                }
                if (videoWriter) {
                    Mat image = dataBoxes[selected]->frame.clone();
                    rectangle(image, *dataBoxes[selected]->roi.get(), Scalar(0,255,0), 2);
                    videoWriter->write(image);
                } else {
                    printErrorHeader(__LINE__);
                    cerr << "No video writer for normal video!" << endl;
                    printErrorFooter();

                }
               
            }

            if (terminalParser.opticalFlowData.displayFlow) {
                if (constructDisplayWindowFlow) {
                    namedWindow(DISPLAY_WINDOW_FLOW, WINDOW_NORMAL);
                    constructDisplayWindowFlow = false;
                    changeDisplayWindowImageFlow = true;
                }
                if (changeDisplayWindowImageFlow) {
                    if(opticalFlowVideoWriter){
                        opticalFlowVideoWriter->getImage(dataBoxes[selected]->angle, dataBoxes[selected]->magnitude, opticalFlowImage);
                        opticalFlowVideoWriter->getFrame(opticalFlowImage, opticalFlowFrame);
                    } else{
                        opticalFlowFrame = Mat::zeros(dataBoxes[selected]->videoSize, CV_8UC3);
                        printErrorHeader(__LINE__);
                        cerr << "No optical flow video writer!" << endl;
                        cerr << "Output image on DISPLAY WINDOW will be empty!" << endl;
                    }
                    
                    imshow(DISPLAY_WINDOW_FLOW, opticalFlowFrame);
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
        
        
        // Clear all vectors!!!
        selected = -1;
        metricCenters.clear();
        normalizedHistogram.clear();
        amplitudeHistogram.clear();
        angleHistogram.clear();

    }


    cout << endl;
    printf("Elapsed time: %s\n", dataBoxes[0]->timer->getElapsedTime()->c_str());
    cout << "Video time: " << dataBoxes[0]->timer->getVideoTime() << endl;
    cout << "Max fps: " << dataBoxes[0]->timer->getMaxFps() << endl;
    cout << "Is real time: " << dataBoxes[0]->timer->isRealTime() << endl;
    cout << "=========================================================" << endl;
    
    exit(EXIT_SUCCESS);
}