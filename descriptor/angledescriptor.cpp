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

#include "angledescriptor.hpp"

using namespace gk;

const float AngleDescriptor::MIN_VALUE = - CV_PI/2;
const float AngleDescriptor::MAX_VALUE = CV_PI/2;


AngleDescriptor::AngleDescriptor(const DescriptorData& data) 
: AngleDescriptor(data.binCount, data.maxNorm){
    
}

AngleDescriptor::AngleDescriptor(int binCount, float maxNormRange)
	: BaseDescriptor(binCount, maxNormRange){

	this->binWidth = (MAX_VALUE - MIN_VALUE)/binCount;
}

void AngleDescriptor::getHistogram(
		const Mat& angles, const Mat& magnitudes, vector<float>& normalizedHistogram){
        
    normalizedHistogram.clear();
    
    vector<float> histogram(binCount);      

    int bin;
    const float* row;

    for(int y = 0; y < angles.rows; y++){

        row = angles.ptr<float>(y);
        for(int x = 0; x < angles.cols; x++){

                bin = calculateBin(row[x]);
                //if(bin < 0 || bin > binCount){
                    //throw BinOutOfRangeException(CLASS_NAME, 57, bin, y, x, row[x]);
                //}
                if(bin > 0 && bin < binCount){
                    histogram.at(bin) += magnitudes.at<float>(y,x);
                }
        }
    }

    if(maxNormRange > 0){
        normalize(histogram, normalizedHistogram, maxNormRange, 0.0, NORM_L1);

    } else{
        normalizedHistogram = histogram;
    }
}

int AngleDescriptor::calculateBin(float angle) const{

	return cvFloor((angle - MIN_VALUE)/binWidth);
}

void AngleDescriptor::normalizeAngles(Mat& flowAngles){
    
    float* row;
    for(int y = 0; y < flowAngles.rows; y++ ){

        row = flowAngles.ptr<float>(y);
        int x = 0;
        while(x < flowAngles.cols){

            float angle = row[x];

            // If angle is on interval (90°, 270°] (II. or III. quadrant)
            // on the left side of vertical line
            // move it to correspondent right side of vertical line, that is,
            // convert it to intverval (90°, -90°]
            if(angle > CV_PI/2 && angle <= 3*CV_PI/2){
                row[x] = CV_PI - angle;

            // If angle is on interval (270°, 360°] (in IV. quadrant)
            // convert it to interval (-90, 0]
            } else if(angle > 3*CV_PI/2 && angle <= 2*CV_PI ){
                row[x] = angle - 2*CV_PI;

            // If angle > 360° change it to interval [0°, 360°]
            // and repeat the process.
            } else if(angle > 2*CV_PI){
                row[x] = angle - 2*CV_PI;
                continue;
            }
            x++;
        }
    }
}