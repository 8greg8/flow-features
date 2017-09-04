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

#include "of2trackerfile.hpp"

using namespace gk;

OF2TrackerFile::OF2TrackerFile(const string& filename, const long startFrame)
: BaseTrackerFile(filename, startFrame){

    
}

std::shared_ptr<Rect2d> OF2TrackerFile::getNext() {
    std::shared_ptr<Rect2d> roi = std::make_shared<Rect2d>();
    
    istringstream lineStream;
    string line, item;
    if (isGood()) {
        if (getline(is, line)) {
            vector<string> items;

            // split line by ,
            lineStream.str(line);
            while (getline(lineStream, item, ',')) {
                items.push_back(item);
            }

            // Check if I have 5 items
            if (items.size() == 4) {

                    roi->x = stof(items.at(0)); // x
                    roi->y = stof(items.at(1)); // y
                    roi->width = stof(items.at(2)); // width
                    roi->height = stof(items.at(3)); // height
                    
                    return roi;
               
            } else {
                cerr << "Check tracker's bounding boxes file." << endl;
                cerr << "It appears that there are more/less than 5 items" << endl;
                cerr << "Number of items found: " << items.size() << endl;
                cerr << "Exiting..." << endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    return roi;
}