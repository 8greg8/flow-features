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

#ifndef TIMER_HPP
#define TIMER_HPP

#include <opencv2/videoio.hpp>
#include <string>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <memory>

#include "basetimer.hpp"

using namespace cv;
using namespace std;

namespace gk {

    class VideoTimer : public BaseTimer {
    private:
        std::shared_ptr<VideoCapture> videoCapture;

        double selectTimeStart, selectTimeEnd;

        double selectionSeconds;
        double selectionMilliSeconds;

        double maxFps;

        double calculateCompletion();
        
    public:
        VideoTimer(std::shared_ptr<VideoCapture> videoCapture, double timeToShowUser);
        VideoTimer(std::shared_ptr<VideoCapture> videoCapture, double timeToShowuser, double selectionSeconds);

        string getVideoTime();

        void start() override;

        bool isTimeToSelect();

        void startTimeToSelect();

        void restartTimeToSelect();

        string getCompletion();

        string getEstimatedTime();

        string getFps();

        string getMaxFps();

        bool isRealTime();
    };
}


#endif /* TIMER_HPP */

