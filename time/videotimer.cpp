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

#include <opencv2/highgui/highgui.hpp>

#include "videotimer.hpp"

using namespace gk;

VideoTimer::VideoTimer(std::shared_ptr<VideoCapture> videoCapture,
        double timeToShowUser)
: VideoTimer(videoCapture, timeToShowUser, 0.0) {

}

VideoTimer::VideoTimer(std::shared_ptr<VideoCapture> videoCapture,
        double timeToShowUser,
        double selectionSeconds)
: BaseTimer(timeToShowUser),
videoCapture(videoCapture),
maxFps(0.0),
selectionSeconds(selectionSeconds) {

    selectionMilliSeconds = selectionSeconds * 1000.0;
}

string VideoTimer::getVideoTime() {
    int milliseconds = (int) videoCapture->get(CAP_PROP_POS_MSEC);
    int hundrets = milliseconds % 1000;
    int seconds = (milliseconds / 1000) % 60;
    int minutes = (milliseconds / (1000 * 60)) % 60;
    int hours = (milliseconds / (1000 * 60 * 60)) % 24;

    char output[12];
    const char* format = "%02d:%02d:%02d.%03d";
    if (sprintf(output, format, hours, minutes, seconds, hundrets) < 0) {
        output[0] = '\0';

    }
    return output;
}

void VideoTimer::start() {
    BaseTimer::start();

    if (selectionSeconds > 0) {
        this->startTimeToSelect();
    }
}

bool VideoTimer::isTimeToSelect() {
    if (selectionMilliSeconds > 0) {
        //time(&selectTimeEnd); // get current time
        //double timeDifference = difftime(selectTimeEnd, selectTimeStart);
        selectTimeEnd = videoCapture->get(CAP_PROP_POS_MSEC);
        double timeDifference = selectTimeEnd - selectTimeStart;

        if (timeDifference > selectionMilliSeconds) {
            //time(&selectTimeStart); // Restart timer
            return true;

        } else {
            return false;
        }
    } else {
        cerr << "Couldn't start timer for ROI selection.";
        cerr << "Defined seconds are <= 0" << endl;
    }
}

void VideoTimer::startTimeToSelect() {
    //time(&selectTimeStart); // Restart timer
    selectTimeStart = videoCapture->get(CAP_PROP_POS_MSEC);
}

void VideoTimer::restartTimeToSelect() {
    this->startTimeToSelect(); // Restart timer
}

string VideoTimer::getCompletion() {
    double completion = calculateCompletion();

    char output[6];
    const char* format = "%.2f";
    if (sprintf(output, format, completion) < 0) {
        output[0] = '\0';

    }
    return output;
}

double VideoTimer::calculateCompletion() {
    double frameCount = videoCapture->get(CAP_PROP_FRAME_COUNT);
    double nextFrameIndex = videoCapture->get(CAP_PROP_POS_FRAMES);
    return cvRound((nextFrameIndex / frameCount)*10000.0) / 100.0;
}

string VideoTimer::getEstimatedTime() {
    double completion = calculateCompletion();
    double timeDifference = calculateElapsedTime();
    double speed = completion / timeDifference;
    double estimated = (100 - completion) / speed;

    int estimatedMillis = (int) (estimated * 1000) % 1000;
    int estimatedSec = (int) estimated % 60;
    int estimatedMin = (int) (estimated / 60.0) % 60;
    int estimatedHour = (int) (estimated / 3600.0) % 60;

    //char* output = (char*) malloc(sizeof(char)*(charCount + 1));
    char output[8];
    const char* format = "%02d:%02d:%02d";
    if (sprintf(output, format, estimatedHour, estimatedMin, estimatedSec) < 0) {
        output[0] = '\0';

    }
    return output;
}

string VideoTimer::getFps() {
    double frameCount = videoCapture->get(CAP_PROP_POS_FRAMES);
    double elapsedTime = calculateElapsedTime();
    double fps = frameCount / calculateElapsedTime();

    if (fps > maxFps) {
        maxFps = fps;
    }

    char output[6];
    const char* format = "%.2f";
    if (sprintf(output, format, fps) < 0) {
        output[0] = '\0';

    }
    return output;
}

string VideoTimer::getMaxFps() {
    char output[6];
    const char* format = "%.2f";
    if (sprintf(output, format, maxFps) < 0) {
        output[0] = '\0';

    }
    return output;
}

bool VideoTimer::isRealTime() {
    return maxFps >= videoCapture->get(CAP_PROP_FPS);
}
