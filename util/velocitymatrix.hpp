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

#ifndef VELOCITYMATRIX_HPP
#define VELOCITYMATRIX_HPP

#include <fstream>
#include <iostream>
#include <cmath>
#include <cerrno>
#include <exception>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "roi.hpp"

using namespace std;
using namespace cv;

namespace gk{
    class AngleException : public std::exception{
    private:
        string message;
        
    public:
        AngleException(const string& className, const int lineNumber,
                const int errorNum, const int row, const int col, 
                const float value);
        virtual const char* what() const throw() override;
    };
    
    class VelocityMatrix{
    public:
        const string CLASS_NAME = "VelocityMatrix";
        const int ROW_COUNT = 424;
        const int COLUMN_COUNT = 512;
        const int MAX_SPEED = 15; //mps ~54 kmph
        
        VelocityMatrix(const float * const x, const float * const y, 
        const float * const z, const unsigned int rows, const unsigned int cols, 
        const float fps);
        /*VelocityMatrix(const float& x, const float& y,
                const float& z, const unsigned int& rows, const unsigned int& cols,
                const float& fps);*/
        
        ~VelocityMatrix();

        void cropVelocityMatrix(const Rect2d& roi);
        void getSemiSpherical(Mat& angle, Mat& magnitude);
#ifdef DEBUG
        void showVelocityDebug();
#endif
    private:
        string filename;
        std::ifstream in;
        int rowCount;
        int columnCount;
        cv::Mat velocity;

        void generateVelocityMatrix(const float * const x, const float * const y,
                const float * const z, Mat& velocity);
        /*void generateVelocityMatrix(const float& x,
                const float& y,
                const float& z,
                Mat & velocity);*/


    };
}


#endif /* VELOCITYMATRIX_HPP */

