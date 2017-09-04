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

#include "basetimer.hpp"

using namespace gk;

BaseTimer::BaseTimer(double timeToShowUser)
: timeToShowUser(timeToShowUser){
    
}

void BaseTimer::start() {
    time(&elapsedTimeStart);
    time(&showUserTimeStart);
}

bool BaseTimer::isTimeToShowOutput() {
    time(&showUserTimeEnd); // get current time
    double timeDifference = difftime(showUserTimeEnd, showUserTimeStart);


    if (timeDifference > timeToShowUser) {
        time(&showUserTimeStart); // Restart timer.
        return true;

    } else {
        return false;
    }

}

std::shared_ptr<std::string> BaseTimer::getElapsedTime() {
    double elapsedTimeDiff = calculateElapsedTime();
    int elapsedMillis = (int) (elapsedTimeDiff * 1000) % 1000;
    int elapsedSec = (int) elapsedTimeDiff % 60;
    int elapsedMin = (int) (elapsedTimeDiff / 60.0) % 60;
    int elapsedHour = (int) (elapsedTimeDiff / 3600.0) % 24;

  
    
    output = std::make_shared<std::string>();
    
    //char output[8];
    const char* format = "%02d:%02d:%02d";
    if (std::sprintf( const_cast<char*>(output->c_str()), format, elapsedHour, elapsedMin, elapsedSec) < 0) {
        //output[0] = '\0';
        //*output = "";
    }
    return output;
}

double BaseTimer::calculateElapsedTime() {
    time(&elapsedTimeEnd);
    return difftime(elapsedTimeEnd, elapsedTimeStart);
}