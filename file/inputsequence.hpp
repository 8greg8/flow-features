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

#ifndef INPUTSEQUENCE_HPP
#define INPUTSEQUENCE_HPP

#include <string>
// boost
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>

using namespace std;
using namespace boost;

namespace gk{
    class InputSequence{
    private:
        // Start with 1nd frame.
        int frameCount = 1;
                
        string basename;
        string fileType;
        string defaultSequenceFormat;
        
        bool hasSequence = false;
        boost::format* sequenceFormat;
        bool needed = false;
        long frameNumber;
        
        
        bool getSequenceFormat(const string& filename, boost::format** sequenceFormat);
        
    public:
        InputSequence(const string& basename, const long startFrame = 1, const string& fileType = ".png", const string& defaultSequenceFormat = "%04d");
        
        bool getFilename(string& filename);
        bool sequenceEnabled() const;
        virtual int getFrameNumber() const;
    };
}


#endif /* INPUTSEQUENCE_HPP */

