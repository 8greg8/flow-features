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

#include "cameraselector.hpp"

using namespace gk;
CameraSelector::CameraSelector(const CameraSelectorData& config)
: CameraSelector(config.metricIntersection, config.threshold, config.firstSelected){
    
}

CameraSelector::CameraSelector(const float metricIntersection, const float threshold, const int firstSelected) 
: previousSelected(firstSelected), metricIntersection(metricIntersection), threshold(threshold) {
    thresholdMax = metricIntersection + threshold;
    thresholdMin = metricIntersection - threshold;
}

int CameraSelector::select(const vector<Point3d>& metricCenters) {
    if(isOverIntersection(metricCenters[previousSelected])){
        previousSelected = SLAVE;
    } else{
        previousSelected = MASTER;
    }
    return previousSelected;
}

bool CameraSelector::isOverIntersection(const Point3d& metricCenter){
    //Hysteresis based checking
    int saturation;
    if(previousSelected == MASTER){
        saturation = sgn(metricCenter.x - thresholdMax);
    } else{
        saturation = sgn(metricCenter.x - thresholdMin);
    }
    if(saturation == 1){
        return true;
    } else{
        return false;
    }
}

int CameraSelector::sgn( float x){
    float eps = 1e-7;
    return x<-eps ? -1 : x>eps;
}