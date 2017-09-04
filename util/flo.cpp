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

#include "flo.hpp"

using namespace gk;

int Flo::floFrameCount = 2;

const string Flo::FLO_TYPE = ".flo";
const string Flo::DEFAULT_FLO_FORMAT = "-%04d";

Flo::Flo(string floFilename, int floFrameCount){
    floHasSequence = getFloSequenceFormat(floFilename, floSequenceFormat);
    this->floFrameCount = floFrameCount;
}

bool Flo::hasFloSequence(){
    return floHasSequence;
}

int Flo::getFloFrameNumber(){
    return floFrameCount;
}

string Flo::getFloFilename(){
    // If we have filename with %d 
    // we increment frame count for every call.
    int frameNumber = floFrameCount;
    if(floHasSequence){
        floFrameCount++;
    }
    
    // Build flo filename
    string floFilename = boost::str(floSequenceFormat % frameNumber);   
    return floFilename;
}

bool Flo::getFloSequenceFormat(const string& floFilename, boost::format& floSequenceFormat){
    string::size_type floLastPoint = floFilename.find_last_of('.');
    
    // If string contains format character
    if(floFilename.find('%') != std::string::npos){
        floSequenceFormat = boost::format(floFilename.substr(0, floLastPoint) + FLO_TYPE);
        
        // Flo file has sequence
        return true;
        
    } else{
        floSequenceFormat = boost::format(floFilename.substr(0, floLastPoint) + DEFAULT_FLO_FORMAT + FLO_TYPE);
        // Flo file doesn't have a sequence
        return false;
    }
}