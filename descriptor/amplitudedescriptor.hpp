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

#ifndef AMPLITUDEDESCRIPTOR_HPP
#define AMPLITUDEDESCRIPTOR_HPP

#include <opencv2/core/core.hpp>
#include <vector>

#include "basedescriptor.hpp"

using namespace std;
using namespace cv;

namespace gk{
    class AmplitudeDescriptor : public BaseDescriptor{
    private:
        float minAmplitude;
        float scaleAmplitude;
        
    public:
        AmplitudeDescriptor(const DescriptorData& descriptorData);
        AmplitudeDescriptor(int binCount, float minAmplitude, float scaleAmplitude, float maxNormRange);
        
        void getHistogram(const cv::Mat& flowMagnitude, vector<float>& histogram) const;
    };
}

#endif /* AMPLITUDEDESCRIPTOR_HPP */

