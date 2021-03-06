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

#include "framespeed.hpp"

using namespace gk;
FrameSpeed::FrameSpeed() {
}

FrameSpeed::FrameSpeed(const FrameSpeed& orig) {
}

FrameSpeed::~FrameSpeed() {
}

float FrameSpeed::getFps(const long startTime, const long  endTime) const {    
    // Get difference
    float difference = endTime - startTime;

    // If nonpositive difference then no fps
    if(difference <= 0){
        return 0.0f;
    }

    return (1000.0f / difference);
        
}