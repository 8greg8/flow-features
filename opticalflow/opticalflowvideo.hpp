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

#ifndef OPTICALFLOWVIDEO_HPP
#define OPTICALFLOWVIDEO_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <string>
#include <iostream>

using namespace std;
using namespace cv;

namespace gk {

    class OpticalFlowVideo {
    private:
        VideoWriter videoWriter;
        string filename;
        Size frameSize;
        
        
    public:
        OpticalFlowVideo(const string& filename, const int fourcc, const double fps, const Size& frameSize);
        ~OpticalFlowVideo();

        void getImage(const Mat& flowAngle, const Mat& flowMagnitude, Mat& image);
        void getFrame(const Mat& image, Mat& frame);
        void write(const Mat& flowAngle, const Mat& flowMagnitude);

    };
}


#endif /* OPTICALFLOWVIDEO_HPP */

