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

#include "opticalflow.hpp"

using namespace gk;

OpticalFlow::OpticalFlow(const OpticalFlowData& config, 
        const TrackerData& trackerData, 
        const std::shared_ptr<AmplitudeFactor> amplitudeFactor)
: config(config), trackerData(trackerData), amplitudeFactor(amplitudeFactor){
    
}

void OpticalFlow::calculateOpticalFlow(const UMat& uprevgray, const UMat& ugray, Mat& flow) {
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

void OpticalFlow::getPolarFlow(const UMat& uprevgray, const UMat& ugray, const Rect2d& roi,
        Mat& flowAngle, Mat& flowMagnitude) {

    calculateOpticalFlow(uprevgray, ugray, flow);
    
    // If no flow then angle = 0 and magnitude = 0
    if(flow.empty()){
        flowAngle = Mat::zeros(roi.size(), CV_32FC1);
        flowMagnitude = Mat::zeros(roi.size(), CV_32FC1);
        return;
    }

    // If we have enabled tracker we must crop our flow 
    if(trackerData.trackerUsed){
        Rect2d scaledRoi;
        if (trackerData.trackerDownScale > 0) {
            Scaler::scaleRoi(roi, scaledRoi, trackerData.trackerUpScale);
        } else {
            scaledRoi = roi;
        }

        // Correct roi
        Roi::correct<Rect2d>(scaledRoi, flow);
        if (!Roi::insideImage(flow, scaledRoi)) {
            string message = "ROI not inside image.";
            throw Exception(__FILE__, __LINE__, message);
        }
        flow = Roi::crop<Rect2d>(flow, scaledRoi);
    }
    // After cropping there could be empty flow
    if(flow.empty()){
        flowAngle = Mat::zeros(roi.size(), CV_32FC1);
        flowMagnitude = Mat::zeros(roi.size(), CV_32FC1);
        return;
    }
    
    // Split channels and get flow magnitude
    vector<Mat> channel;
    split(flow, channel);

    cartToPolar(channel[0], channel[1], flowMagnitude, flowAngle, false);

    if (amplitudeFactor) {
        amplitudeFactor->scale(flowMagnitude, roi);
    }
}
