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

#ifndef BASEDESCRIPTOR_HPP
#define BASEDESCRIPTOR_HPP

#include "exception.hpp"

namespace gk{

    struct DescriptorData {
        int binCount;
        float minAmplitude;
        float scale;
        float maxNorm;
    };
    
    class BinOutOfRangeException : public gk::Exception {
    public:
        BinOutOfRangeException(const string& className, const int lineNumber,
                const int bin, const int row, const int col, const float value);
    };
    
    class BaseDescriptor{
    protected:
        unsigned int binCount;
        float maxNormRange;

    public:        
        BaseDescriptor(const unsigned int binCount, const float maxNormRange);
        unsigned int getBinCount() const;
        
    };
}

#endif /* BASEDESCRIPTOR_HPP */

