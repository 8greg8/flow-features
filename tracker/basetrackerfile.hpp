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

#ifndef BASETRACKERFILE_HPP
#define BASETRACKERFILE_HPP

#include <opencv2/core/core.hpp>
#include <string>
#include <memory>

#include "basefilereader.hpp"

using namespace std;
using namespace cv;

namespace gk{

    struct TrackerData {
        int playerID;
        double trackerDownScale;
        double trackerUpScale;
        bool displayTracker;
        bool trackerUsed;
    };
    
    class BaseTrackerFile : public BaseFileReader<std::shared_ptr<Rect2d>> {
    protected:
        long startFrame;
        
    public:
        BaseTrackerFile(const string& filename, const long startFrame);
    };
}

#endif /* BASETRACKERFILE_HPP */

