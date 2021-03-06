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

#include <sstream>
#include <vector>

#include "intrinsicfile.hpp"

gk::IntrinsicFile::IntrinsicFile(const std::string& filename)
: BaseFileReader<IntrinsicData>(filename){
    
}

gk::IntrinsicData gk::IntrinsicFile::getNext(){
    IntrinsicData intrinsicData;
    istringstream lineStream;
    string line, item;
    vector<string> items;
    
    // go to beginning
    is.seekg(std::ios::beg);
    int lineCount = 0;
    while(is.good()){
        if(getline(is, line)) {
            lineCount++;
            
            lineStream.str(line);
            while(getline(lineStream, item, ',')){
                items.push_back(item);
            }
            lineStream.str(std::string());
            lineStream.clear();
            
            if(items.size() == 8 && lineCount == 4){
                intrinsicData.colorCameraParams.focalLengthX = stof(items.at(0));
                intrinsicData.colorCameraParams.focalLengthY = stof(items.at(1));
                intrinsicData.colorCameraParams.principaPointX = stof(items.at(2));
                intrinsicData.colorCameraParams.principalPointY = stof(items.at(3));
                intrinsicData.depthCameraParams.focalLengthX = stof(items.at(4));
                intrinsicData.depthCameraParams.focalLengthY = stof(items.at(5));
                intrinsicData.depthCameraParams.principaPointX = stof(items.at(6));
                intrinsicData.depthCameraParams.principalPointY = stof(items.at(7));
                
                return intrinsicData; 
            }
        }
    }
    if(items.size() != 8) {
        cerr << "Check intrinsic file." << endl;
        cerr << "It appears that there are more/less than 8 items found" << endl;
        cerr << "Number of items found: " << items.size() << endl;
        cerr << "Aborting program..." << endl;
        exit(EXIT_FAILURE);
    }
    return intrinsicData;
}