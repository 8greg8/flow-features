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

#include "histogramfile.hpp"

using namespace gk;

HistogramFile::HistogramFile(const string& filename)
: BaseFileWriter<vector<float>>(filename){
    
}

bool HistogramFile::write(const vector<float>& histogram){
    int binCount = histogram.size();
    //outputStream.write(reinterpret_cast<const char*>(&histogram[0]), binCount * sizeof(histogram[0]));
    
    for(int x = 0; x < binCount; x++){
        os << histogram.at(x);

        if(x != (binCount - 1) ){
                os << ",";
        }
    }
    os << endl;
    return true;
}