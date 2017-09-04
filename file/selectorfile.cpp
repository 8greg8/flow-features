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

#include "selectorfile.hpp"
#include "basefilereader.hpp"

gk::SelectorFile::SelectorFile(const std::string& filename)
: BaseFileReader<CameraSelectorData>(filename) {

}

gk::CameraSelectorData gk::SelectorFile::getNext() {
    CameraSelectorData cameraSelectorData;
    istringstream lineStream;
    string line, item;
    vector<string> items;

    // go to beginning
    is.seekg(std::ios::beg);
    int lineCount = 0;
    while (is.good()) {
        if (getline(is, line)) {
            lineCount++;

            lineStream.str(line);
            vector<string> temp;
            while (getline(lineStream, item, '=')) {
                temp.push_back(item);
            }
            lineStream.str(std::string());
            lineStream.clear();

            items.push_back(temp.at(1));

            if (items.size() == 3 && lineCount == 3) {
                string camera = items.at(0);
                to_lower(camera);
                if( camera == "right"){
                    cameraSelectorData.firstSelected = 1;
                } else if(camera == "left"){
                    cameraSelectorData.firstSelected = 0;
                } else{
                    throw Exception(__FILE__, __LINE__, "first selected is not right or left");
                }
                cameraSelectorData.metricIntersection = stof(items.at(1));
                cameraSelectorData.threshold = stof(items.at(2));

                return cameraSelectorData;

            }
        }
    }
    if (items.size() != 3) {
        cerr << "Check camera selector file." << endl;
        cerr << "It appears that there are more/less than 3 items" << endl;
        cerr << "Number of items found: " << items.size() << endl;
        cerr << "Aborting program..." << endl;
        exit(EXIT_FAILURE);
    }

    return cameraSelectorData;
}