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

#ifndef OPTICALFLOW_HPP
#define OPTICALFLOW_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/optflow.hpp>

#include <iostream>
#include <memory>
#include <string>

#include "amplitudefactor.hpp"
#include "roi.hpp"
#include "scaler.hpp"
#include "exception.hpp"
#include "basetrackerfile.hpp"

using namespace cv;
using namespace std;

namespace gk{
    
    enum OpticalFlowType {
        FARNEBACK,
        LUCAS_KANADE
    };

    struct OpticalFlowData {
        bool needVideo;
        int startFrame;
        string outFlowVideo;
        string outVideo;
        bool displayFlow;
        OpticalFlowType flowType;
        // Farneback optical flow parameters
        double pyramidScale;
        int pyramidLayers;
        int windowSize;
        int iterationsCount;
        int neighbourSize;
        double gaussianDeviation;
        int operationFlags;
    };
    
    class OpticalFlow{
    private:
        const string CLASS_NAME = "OpticalFlow";
        
        OpticalFlowData config;
        std::shared_ptr<AmplitudeFactor> amplitudeFactor;
        Mat flow;
        TrackerData trackerData;

        void calculateOpticalFlow(const UMat& uprevgray, const UMat& ugray, Mat& flow);
        
    public:
        OpticalFlow(const OpticalFlowData& config, const TrackerData& trackerData, const std::shared_ptr<AmplitudeFactor> amplitudeFactor);
        void getPolarFlow(const UMat& uprevgray, const UMat& ugray, const Rect2d& roi,
                Mat& flowAngle, Mat& flowMagnitude);
    };
}

#endif /* OPTICALFLOW_HPP */

