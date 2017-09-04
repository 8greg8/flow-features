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

#ifndef OF2DATABOX_HPP
#define OF2DATABOX_HPP

#include <opencv2/video/video.hpp>

#include <vector>
#include <memory>
#include <string>
#include <sstream>

#include "inputsequence.hpp"
#include "of2trackerfile.hpp"
#include "timefilereader.hpp"
#include "exception.hpp"
#include "depthimage.hpp"
#include "framespeed.hpp"
#include "roi.hpp"
#include "of2terminalparser.hpp"
#include "userinteraction.hpp"
#include "opticalflow.hpp"
#include "diagfile.hpp"
#include "amplitudefactor.hpp"
#include "videotimer.hpp"
#include "scaler.hpp"
#include "depthimage.hpp"
#include "basedatabox.hpp"
#include "basetrackerfile.hpp"
#include "roi.hpp"

using namespace std;

namespace gk {

    class OF2DataBox : public BaseDataBox{
    private:
        OpticalFlowData opticalFlowData;
        TrackerData trackerData;
        string videoFilename;
        
        std::shared_ptr<InputSequence> depthInputSequence;
        std::shared_ptr<DepthImage> depthImage;

        std::shared_ptr<TimeFileReader> timeFileReader;

        std::shared_ptr<BaseTrackerFile> trackerFile;
        std::shared_ptr<OpticalFlow> opticalFlow;
        
        UMat ugray, uprevgray;
        

        void configInput(const string& imageFilename,
                const string& depthFilename,
                const long startFrame) override;

        void configTracker(const string& trackerFilename, const long startFrame) override;

        void configTime(const string& timeFilename, const long startFrame) override;
        
        void configFlow(const string& diagFilename,
                const OpticalFlowData& opticalFlowData,
                const TrackerData& trackerData);
        
    public:
        Mat frame;
        string depthFilename;
        long timeStamp;
        Size matrixSize;

        std::shared_ptr<Rect2d> roi;
        Point3d metricCenter;
        bool confident;

        std::shared_ptr<VideoCapture> video;
        std::shared_ptr<VideoTimer> timer;
        int codecNum;
        double fps;
        Size videoSize;
        double frameCount;

        OF2DataBox(const string& videoFilename,
                const string& depthFilename,
                const string& trackerFilename,
                const string& timeFilename,
                const string& intrinsicFilename,
                const string& extrinsicFilename,
                const string& diagFilename,
                const long startFrame,
                const OpticalFlowData& opticalFlowData,
                const TrackerData& trackerData);

        bool update() override;

    };
}


#endif /* OF2DATABOX_HPP */

