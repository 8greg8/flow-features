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


#ifndef SF2DATABOX_HPP
#define SF2DATABOX_HPP

#include <vector>
#include <memory>
#include <string>
#include <sstream>

#include "inputsequence.hpp"
#include "sf2trackerfile.hpp"
#include "timefilereader.hpp"
#include "exception.hpp"
#include "depthimage.hpp"
#include "framespeed.hpp"
#include "roi.hpp"
#include "sf2terminalparser.hpp"
#include "velocitymatrix.hpp"
#include "basedatabox.hpp"

#include "scene_flow_impair.h"

namespace gk{
    
    class SF2DataBox : public BaseDataBox{
    private:
        SceneFlowData sceneFlowData;
        
        std::vector< std::shared_ptr<InputSequence> > imageInputSequence;
        std::vector< std::shared_ptr<InputSequence> > depthInputSequence;
        std::shared_ptr<DepthImage> depthImage;
        
        std::vector< std::shared_ptr<TimeFileReader> > timeFileReaders;
        float fps;

        std::shared_ptr<BaseTrackerFile> trackerFile;
        std::shared_ptr<FrameSpeed> frameSpeed;
        std::shared_ptr<PD_flow_opencv> sceneflow;
        std::shared_ptr<VelocityMatrix> velocityMatrix;
        
        void configInput(const string& imageFilename,
                const string& depthFilename,
                const long startFrame) override;
        
        void configTracker(const string& trackerFilename, const long startFrame) override;
        
        void configTime(const string& timeFilename, const long startFrame) override;
        
        void calculateSceneFlow();
        
    public:
        std::vector<string> imageFilenames;
        std::vector<string> depthFilenames;
        long timeStamps[2];
        Size matrixSize;
        
        std::shared_ptr<Rect2d> roi;
        Point3d metricCenter;
        bool confident;
        
        SF2DataBox(const string& imageFilename,
                const string& depthFilename,
                const string& trackerFilename,
                const string& timeFilename,
                const string& intrinsicFilename,
                const string& extrinsicFilename,
                const long startFrame,
                const SceneFlowData& sceneFlowData);
        
        bool update() override;
        
    };
}


#endif /* SF2DATABOX_HPP */

