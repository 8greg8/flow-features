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

#ifndef DATABOX_HPP
#define DATABOX_HPP

#include <opencv2/core/core.hpp>
#include <string>

#include "intrinsicfile.hpp"
#include "extrinsicfile.hpp"
#include "cameracalib.hpp"

using namespace cv;
using namespace std;

namespace gk{
    class BaseDataBox{
    protected:
        long startFrame;
        Mat homography;
        
        virtual void configInput(const string& imageFilename,
                const string& depthFilename,
                const long startFrame) = 0;
        
        virtual void configTracker(const string& trackerFilename, 
        const long startFrame) = 0;

        virtual void configTime(const string& timeFilename, 
        const long startFrame) = 0;
        
        void configCameraCalib(const string& intrinsicFilename,
                const string& extrinsicFilename);
        
    public:
        Mat angle, magnitude;
        
        BaseDataBox(const long startFrame);
        
        virtual bool update() = 0;

        
    };
}

#endif /* DATABOX_HPP */

