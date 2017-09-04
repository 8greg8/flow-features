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

#include "userinteraction.hpp"

using namespace gk;

void UserInteraction::printVideoProperties(const Size& videoSize, int codecNum, double fps, double frameCount, const string& name){
    // Transform from int to char via Bitwise operators
    char CODEC[] = {
    			(char)(codecNum & 0XFF) ,
        		(char)((codecNum & 0XFF00) >> 8),
                        (char)((codecNum & 0XFF0000) >> 16),
                        (char)((codecNum & 0XFF000000) >> 24), 0};
    
    // Tell user for output video properties
    cout << "=======================" << endl;
    cout << "Video properties: " << endl;
    cout << "Size: " << videoSize.width << "x" << videoSize.height << endl;
    cout << "Codec: " << CODEC << endl;
    cout << "FPS: " << fps << endl;
    
    if(frameCount != 0){
        cout << "Frame count: " << frameCount << endl;
    }
    
    cout << "Name: " << name << endl;
    cout << "=======================" << endl;
}
