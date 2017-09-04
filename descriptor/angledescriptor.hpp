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

#ifndef ANGLEDESCRIPTOR_HPP
#define ANGLEDESCRIPTOR_HPP

#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>

#include "basedescriptor.hpp"

using namespace cv;
using namespace std;

namespace gk{
    
    
    class AngleDescriptor : public BaseDescriptor {
        private:
            float binWidth;
            static const float MIN_VALUE;
            static const float MAX_VALUE;

            int calculateBin(float angle) const;      

        public:            
            AngleDescriptor(const DescriptorData& data);
            AngleDescriptor(int binCount, float maxNormRange);

            void getHistogram(const Mat& angles, const Mat& magnitudes, vector<float>& histogram);

            static void normalizeAngles(Mat& flowAngles);
    };
}

#endif /* ANGLEDESCRIPTOR_HPP */
