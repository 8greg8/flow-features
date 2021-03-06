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
#ifndef CAMERASELECTOR_HPP
#define CAMERASELECTOR_HPP

#include <opencv2/core/core.hpp>
#include <vector>
#include "selectorfile.hpp"

using namespace cv;
using namespace std;

namespace gk{
    const int MASTER = 0;
    const int SLAVE = 1;
    
    
    
    class CameraSelector{
    private:
        int previousSelected;
        float metricIntersection;
        float threshold;  
        float thresholdMax;
        float thresholdMin;
        bool isOverIntersection(const Point3d& metricCenter);

        int sgn(float x);
        
    public:
        CameraSelector(const CameraSelectorData& config);
        CameraSelector(const float metricIntersection, const float threshold, const int firstSelected);
        int select(const vector<Point3d>& metricCenters);
    };
}

#endif /* CAMERASELECTOR_HPP */

