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

#ifndef FLO_HPP
#define FLO_HPP

#include <string>
// boost
#include <boost/format.hpp>

using namespace std;
using namespace boost;

namespace gk{
    class Flo{
    private:
        // Start with 2nd frame. First frame doesn't have optical flow.
        static int floFrameCount;
        
        static const string FLO_TYPE;
        static const string DEFAULT_FLO_FORMAT;
        
        bool floHasSequence = false;
        boost::format floSequenceFormat;
        bool floNeeded = false;
        
        bool getFloSequenceFormat(const string& floFilename, boost::format& floSequenceFormat);
        
    public:
        Flo(string floFilename, int floFrameCount);
        
        
        
        
        string getFloFilename();
        bool hasFloSequence();
        int getFloFrameNumber();
    };
}

#endif /* FLO_HPP */

