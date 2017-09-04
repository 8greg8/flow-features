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


#include "inputsequence.hpp"

using namespace gk;

InputSequence::InputSequence(const string& basename, const long startFrame, 
        const string& fileType, const string& defaultSequenceFormat) : 
        basename(basename), 
        fileType(fileType), 
        defaultSequenceFormat(defaultSequenceFormat){
    
    this->sequenceFormat = NULL;
    this->hasSequence = getSequenceFormat(basename, &sequenceFormat);
    
    if (startFrame < 1){
        this->frameCount = 1;
    }else{
        this->frameCount = startFrame;
    }
}

bool InputSequence::sequenceEnabled() const{
    return hasSequence;
}

int InputSequence::getFrameNumber() const{
    return frameNumber;
}

bool InputSequence::getFilename(string& filename){
    // If we have filename with %d 
    // we increment frame count for every call.
    frameNumber = frameCount;
    if(hasSequence){
        frameCount++;
    }
    
    // Build flo filename
    if (sequenceFormat){
        filename = boost::str(*sequenceFormat % frameNumber);   
        
    } else{
        filename = basename;
    }
    if(boost::filesystem::exists(filename)){
        return true;
        
    } else{
        return false;
    }
}

bool InputSequence::getSequenceFormat(const string& filename, boost::format** sequenceFormat){    
    // If string contains format character
    string::size_type sequenceSpecifierPosition = filename.find('%');
    
    // First delete old pointer
    if (*sequenceFormat){
        delete *sequenceFormat;
    }
    
    *sequenceFormat = new boost::format(filename.substr(0, sequenceSpecifierPosition) + defaultSequenceFormat + fileType);

    // File has sequence
    return true;        
}