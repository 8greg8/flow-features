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

#include "amplitudedescriptor.hpp"

using namespace gk;

AmplitudeDescriptor::AmplitudeDescriptor(const DescriptorData& descriptorData)
: AmplitudeDescriptor(descriptorData.binCount, descriptorData.minAmplitude, descriptorData.scale, descriptorData.maxNorm) {
    
}

AmplitudeDescriptor::AmplitudeDescriptor(
        int binCount, 
        float minAmplitude, 
        float scaleAmplitude, 
        float maxNormRange) 
: BaseDescriptor(binCount,maxNormRange), 
  minAmplitude(minAmplitude), 
  scaleAmplitude(scaleAmplitude){
    
}


void AmplitudeDescriptor::getHistogram(
        const Mat& amplitude, 
        vector<float>& normalizedHistogram) const{
    
    // Clear descriptor
    normalizedHistogram.clear();
    vector<float> descriptor(binCount);
    
    float pixValue;
    unsigned int binValue;
    
    const float* row;
    for(int y = 0; y < amplitude.rows; y++){

        row = amplitude.ptr<float>(y);
        for(int x = 0; x < amplitude.cols; x++){
            /* Make sure amplitudes are positive values */
            pixValue = row[x] * row[x];
            pixValue = sqrt(pixValue) * scaleAmplitude;
            
            /* Skip everything below minAmplitude, do NOT saturate */
            if (pixValue >= minAmplitude){
                pixValue = pixValue - minAmplitude;

                /* Chop off the fractional part (rounding towards zero) */ 
                binValue = (unsigned int)pixValue;

                /* Clip at top end, do NOT saturate */
                if (binValue < binCount){
                    /* We count by ONE */
                    descriptor.at(binValue) += 1.0;
                }
            }
        }        
    }
    // Normalize to probability. This means, sum of all descriptors is maxNormRange
    if (maxNormRange > 0){
        cv::normalize(descriptor, normalizedHistogram, maxNormRange, 0.0, NORM_L1);
    }else{
        normalizedHistogram = descriptor;
    }
}