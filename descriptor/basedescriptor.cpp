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

#include "basedescriptor.hpp"

using namespace gk;

BinOutOfRangeException::BinOutOfRangeException(const string& className,
        const int lineNumber,
        const int bin,
        const int row,
        const int col,
        const float value)
: Exception(className, lineNumber) {

    message += "Bin " + to_string(bin) + " out of range!\n" +
            "row: " + to_string(row) + "\tcol: " + to_string(col) +
            "\tvalue: " + to_string(value) + "\n";
}


BaseDescriptor::BaseDescriptor(const unsigned int binCount, const float maxNormRange)
: binCount(binCount), maxNormRange(maxNormRange){
}

unsigned int BaseDescriptor::getBinCount() const{
    return binCount;
}