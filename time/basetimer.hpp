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


#ifndef BASETIMER_HPP
#define BASETIMER_HPP

#include <string>
#include <iostream>
#include <ctime>
#include <sstream>
#include <memory>

namespace gk{
    class BaseTimer{
    protected:
        time_t elapsedTimeStart, elapsedTimeEnd;
        time_t showUserTimeStart, showUserTimeEnd;
        
        double timeToShowUser;
        
        std::shared_ptr<std::string> output;
                
        double calculateElapsedTime();
        
    public:
        BaseTimer(double timeToShowUser);
        
        virtual void start();

        virtual bool isTimeToShowOutput();

        virtual std::shared_ptr<std::string> getElapsedTime();
        
    };
}

#endif /* BASETIMER_HPP */

