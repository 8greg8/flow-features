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

#include <opencv2/core/core.hpp>

#include "opticalflowvideo.hpp"

using namespace gk;

OpticalFlowVideo::OpticalFlowVideo(const string& filename, const int fourcc, 
        const double fps, const Size& frameSize) 
: frameSize(frameSize), filename(filename) {
    if(!(filename.empty() || filename == "")) {
        videoWriter.open(filename, fourcc, fps, frameSize);
    }
}

OpticalFlowVideo::~OpticalFlowVideo(){
    
}

void OpticalFlowVideo::getImage(const Mat& flowAngle, const Mat& flowMagnitude, Mat& image){
    if(!flowAngle.empty() && !flowMagnitude.empty()){
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
        cvtColor(hsvImage_8bit, image, CV_HSV2BGR);
        
    } else{
        if(image.empty()){
            image = Mat(frameSize, CV_8UC3, Scalar(255,255,255));
        } else{
            image = cv::Scalar(255,255,255);
        }
    }
}

void OpticalFlowVideo::getFrame(const Mat& image, Mat& frame){
    // If size is not the same 
    // make new empty white video frame 
    // and add optical flow to the middle of frame.
    frame = Mat::zeros(frameSize, image.type());
    int xPosition = cvRound(frameSize.width / 2.0 - image.cols / 2.0);
    int yPosition = cvRound(frameSize.height / 2.0 - image.rows / 2.0);

    image.copyTo(frame(
        Rect(xPosition, yPosition, image.cols, image.rows)
    ));
}

void OpticalFlowVideo::write(const Mat& flowAngle, const Mat& flowMagnitude){
    if(videoWriter.isOpened()){
        Mat image;
        getImage(flowAngle, flowMagnitude, image);

        if (image.cols == frameSize.width && image.rows == frameSize.height) {
/* 
#ifdef DEBUG
            cv::imshow("Debug", image);
            cv::waitKey(0);
            cv::destroyWindow("Debug");
#endif      
            */
            videoWriter << image;

        } else {
            Mat frame;
            getFrame(image, frame);
            /*
#ifdef DEBUG
            cv::imshow("Debug", frame);
            cv::waitKey(0);
            cv::destroyWindow("Debug");
#endif 
             * */
            videoWriter << frame;
        }   
    } else{
        cerr << endl;
        cerr << " File: " << __FILE__ << " line: " << __LINE__ << endl;
        cerr << "Video writer was not opened for file: " 
             << filename << endl;
        cerr << "Video will not be written." << endl;
    }
}