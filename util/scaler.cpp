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

#include "scaler.hpp"

using namespace gk;

void Scaler::scaleFrame(const InputArray& inputImage, OutputArray& outputImage, double scaleFactor){
    cv::resize(inputImage, outputImage, Size(), scaleFactor, scaleFactor);
}

void Scaler::scaleRoi(const Rect2d& inputRoi, Rect2d& outputRoi, double scaleFactor){
    outputRoi = inputRoi;
    // Don't use cvRound because you have double values in rectangles :)
    
    double scaledWidth = (inputRoi.width*(scaleFactor));
    double scaledHeight = (inputRoi.height*(scaleFactor));
    
    // Using ratio / proportion
    outputRoi.x = (scaledWidth/inputRoi.width*inputRoi.x);
    outputRoi.y = (scaledHeight/inputRoi.height*inputRoi.y);
    outputRoi.width = scaledWidth;
    outputRoi.height = scaledHeight;
}