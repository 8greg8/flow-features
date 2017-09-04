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

#ifndef BASEFILEREADER_HPP
#define BASEFILEREADER_HPP

#include <string>
#include <fstream>
//#include "extrinsicfile.hpp"



namespace gk{
    
    template<typename T>
    class BaseFileReader;
    
    template<typename T> 
    class BaseFileReader{
    protected:
        std::string filename;
        std::ifstream is;
        
        std::ifstream& goToLine(std::ifstream& stream, const long startFrame);
        bool isGood();
    public:
        BaseFileReader(const std::string& filename);
        ~BaseFileReader();
        virtual T getNext() = 0;
    };
    
    
    template<typename T>
    BaseFileReader<T>::BaseFileReader(const std::string& filename)
    : filename(filename) {
        is.open(filename);
    }

    template<typename T>
    BaseFileReader<T>::~BaseFileReader() {
        if (is.is_open()) {
            is.close();
        }
    }  
    
    template<typename T>
    std::ifstream& BaseFileReader<T>::goToLine(std::ifstream& stream, const long startFrame) {
        // go to beginning
        stream.seekg(std::ios::beg);
        std::string line;
        for (long i = 0; i < startFrame - 1; i++) {
            getline(stream, line);
        }
        return stream;
    }
    
    template<typename T>
    bool BaseFileReader<T>::isGood() {
        if (is.is_open() && is.good()) {
            return true;
        } else {
            return false;
        }
    }
}



#endif /* BASEFILEREADER_HPP */

