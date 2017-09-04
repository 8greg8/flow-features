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

#include "of2databox.hpp"

using namespace gk;

OF2DataBox::OF2DataBox(const string& videoFilename,
        const string& depthFilename,
        const string& trackerFilename,
        const string& timeFilename,
        const string& intrinsicFilename,
        const string& extrinsicFilename,
        const string& diagFilename,
        const long startFrame,
        const OpticalFlowData& opticalFlowData,
        const TrackerData& trackerData)
: BaseDataBox(startFrame), videoFilename(videoFilename),
opticalFlowData(opticalFlowData),
trackerData(trackerData) {

    configInput(videoFilename, depthFilename, startFrame);
    configTracker(trackerFilename, startFrame);
    configTime(timeFilename, startFrame);
    configCameraCalib(intrinsicFilename, extrinsicFilename);
    configFlow(diagFilename, opticalFlowData, trackerData);
}

void OF2DataBox::configInput(const string& videoFilename,
        const string& depthFilename,
        const long startFrame) {

    video = make_shared<VideoCapture>(videoFilename);
    if (!video->isOpened()) {

        string message = "Could not open the input video: " + videoFilename;
        throw Exception(__FILE__, __LINE__, message);
    }

    // Get codec type - 4-char code for codec
    codecNum = static_cast<int> (video->get(CAP_PROP_FOURCC));
    fps = video->get(CAP_PROP_FPS);
    videoSize = Size((int) video->get(CAP_PROP_FRAME_WIDTH),
            (int) video->get(CAP_PROP_FRAME_HEIGHT));
    frameCount = video->get(CAP_PROP_FRAME_COUNT);

    UserInteraction::printVideoProperties(
            videoSize, codecNum, fps, frameCount, videoFilename);



    depthInputSequence = std::make_shared<InputSequence>(depthFilename, startFrame);

    timer = std::make_shared<VideoTimer>(video, 1.0, 0);
    timer->start();
}

void OF2DataBox::configTracker(const string& trackerFilename, const long startFrame) {
    // Synced with (N)-th frame
    trackerFile = std::make_shared<OF2TrackerFile>(trackerFilename, startFrame);

}

void OF2DataBox::configTime(const string& timeFilename, const long startFrame) {
    // N-th  frame
    timeFileReader = std::make_shared<TimeFileReader>(timeFilename, startFrame);

}

void OF2DataBox::configFlow(const string& diagFilename,
        const OpticalFlowData& opticalFlowData,
        const TrackerData& trackerData) {

    DiagFile diagFile(diagFilename);
    std::shared_ptr<AmplitudeFactor> amplitudeFactor = 
            std::make_shared<AmplitudeFactor>(diagFile.getNext());

    opticalFlow = std::make_shared<OpticalFlow>(opticalFlowData, trackerData, amplitudeFactor);

}

bool OF2DataBox::update() {
    /// 
    /// VIDEO
    ///
    if (!video->read(frame)) {
        cout << endl;
        cout << "=======================" << endl;
        cout << "Frame " << video->get(CAP_PROP_POS_FRAMES)
                << " is last frame." << endl;
        cout << "Video time at end: " << timer->getVideoTime() << endl;
        cout << "=======================" << endl;
        return false;
    }

    // Get gray frame for optical flow calculation
    if (frame.empty()) {
        throw Exception(__FILE__, __LINE__, "Frame is empty. Skipping it.");
    }

    if (frame.channels() == 3) {
        cvtColor(frame, ugray, COLOR_BGR2GRAY);

    } else if (frame.channels() == 1) {
        cout << "Frame is greyscale." << endl;
        frame.copyTo(ugray);

    } else {
        throw Exception(__FILE__, __LINE__, "Frame has oddly number of channels");
    }

    if (trackerData.trackerDownScale > 0) {
        Scaler::scaleFrame(frame, frame, trackerData.trackerDownScale);
    }

    ///
    /// TRACKER
    /// 
    roi = trackerFile->getNext();
    if (Roi::isEmpty(*roi)) {
        confident = false;
    } else {
        confident = true;
    }

    /// 
    /// DEPTH
    ///  
    depthInputSequence->getFilename(depthFilename);
    depthImage = std::make_shared<DepthImage>(depthFilename);
    metricCenter = Roi::getMetricCenter(*roi, depthImage->depth, homography);
    depthImage.reset();

    /// 
    /// TIMES 
    /// 
    timeStamp = timeFileReader->getNext();




    // If previous gray not empty calculate optical flow
    if (!uprevgray.empty()) {

        // Something went wrong because sizes are different.
        if (uprevgray.size() != ugray.size()) {
            std::stringstream ss;
            ss << "Something went wrong because sizes of previous"
                    << "frame and current frame are different" << endl;
            ss << "Maybe the problem is in video " << videoFilename << endl;
            ss << "Previous frame size: "
                    << uprevgray.rows << "x" << uprevgray.cols << endl;
            ss << "Current frame size: "
                    << ugray.rows << "x" << ugray.cols << endl;
            ss << "Current timestamp in ms: "
                    << video->get(CAP_PROP_POS_MSEC) << endl;
            ss << "Next frame: "
                    << video->get(CAP_PROP_POS_FRAMES) << endl;
            string message = ss.str();
            throw Exception(__FILE__, __LINE__, message);

        }


        // If tracker is enabled and ROI is empty
        // then histogram will be empty (All zero values).
        // 1e-15 is double precision
        if (confident) {
            if (opticalFlow) {
                opticalFlow->getPolarFlow(uprevgray, ugray, *roi, angle, magnitude);
            } else {
                string message = "No optical flow object!";
                throw Exception(__FILE__, __LINE__, message);
            }

        } else {
            // Type is same as type returned by calcOpticalFlowFarneback()
            angle = Mat::zeros(roi->size(), CV_32FC1);
            magnitude = Mat::zeros(roi->size(), CV_32FC1);
        }
    }

    // Make current gray previous gray
    std::swap(uprevgray, ugray);

    return true;
}