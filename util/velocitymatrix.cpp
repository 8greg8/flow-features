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

#include "velocitymatrix.hpp"

using namespace gk;

AngleException::AngleException(const string& className, const int lineNumber, 
        const int errorNum, const int row, const int col, const float value ){
    
    message = "Exception in " + className +
            " line " + to_string(lineNumber) + "\n" +
            "Error: " + std::strerror(errorNum) + "\n" +
            "row: " + to_string(row) + "\tcol: " + to_string(col) +
            "\tvalue: " + to_string(value) + "\n"; 
}

const char* AngleException::what() const throw(){
    return message.c_str();
}

        
VelocityMatrix::VelocityMatrix(const float * const x, const float * const y,
        const float * const z, const unsigned int rows, const unsigned int cols,
        const float fps) {

/*VelocityMatrix::VelocityMatrix(const float& x, const float& y,
        const float& z, const unsigned int& rows, const unsigned int& cols,
        const float& fps) {*/
    
       velocity = Mat::zeros(rows, cols, CV_32FC3);
       //generateVelocityMatrix(in, filename, velocity);
       generateVelocityMatrix(x, y, z, velocity);
       
       //cout << "Matrix 1 "  << velocity << endl;
       velocity *= fps;
       //cout << "Matrix 2 " << velocity << endl;
}

VelocityMatrix::~VelocityMatrix(){
    
}

void VelocityMatrix::generateVelocityMatrix(const float * const x, 
        const float * const y,
        const float * const z, 
        Mat& velocity) {

/*void VelocityMatrix::generateVelocityMatrix(const float& x,
        const float& y,
        const float& z,
        Mat & velocity) {*/
    
    float dx, dy, dz, maxX, maxY, maxZ;
    bool speedBreached = false;
    // Read line by line and convert it to angle and magnitude array
    for (unsigned int v = 0; v < velocity.rows; v++){
        for (unsigned int u = 0; u < velocity.cols; u++) {
            // If velocity overcomes max human speed then make it zero!
            dx = x[v + u * velocity.rows];
            dy = y[v + u * velocity.rows];
            dz = z[v + u * velocity.rows];

            if(abs(dx) > MAX_SPEED){//mps
                speedBreached = true;
                if(abs(dx) > maxX){
                    maxX = dx;
                }
                dx = 0;
            }
            
            if (abs(dy) > MAX_SPEED) {//mps
                speedBreached = true;
                if(abs(dy) > maxY){
                    maxY = dy;
                }
                dy = 0;
            }
            
            if (abs(dz) > MAX_SPEED) {//mps
                speedBreached = true;
                if(abs(dz) > maxZ){
                    maxZ = dz;
                }
                dz = 0;
            }

            velocity.at<Vec3f>(v, u)[0] = dx;
            velocity.at<Vec3f>(v, u)[1] = dy;
            velocity.at<Vec3f>(v, u)[2] = dz;
        }
    }
    
    /*if(speedBreached){
        fprintf(stderr, "\nMax speed limit breached! maxX: %g\tmaxY: %g\tmaxZ: %g",
                maxX, maxY, maxZ);
    }*/
}

void VelocityMatrix::cropVelocityMatrix(const Rect2d& roi) {
    if (!Roi::insideImage(velocity, roi)) {
        cerr << endl;
        cerr << CLASS_NAME << " line 46" << endl;
        cerr << "ROI not inside image. Check Tracker files." << endl;
        cerr << "ROI: \tx: " << roi.x 
                << "\ty: " << roi.y 
                << "\twidth: " << roi.width 
                << "\theight: " << roi.height << endl;
        cerr << "Velociy Matrix \twidth: " << velocity.cols 
                << "\theight: " << velocity.rows << endl;
        cerr << "Velocity file: " << filename << endl; 
        exit(EXIT_FAILURE);
    }

    velocity = Roi::crop<Rect2d>(velocity, roi);
    //cout << velocity << endl;
}

void VelocityMatrix::getSemiSpherical(Mat& angle, Mat& magnitude) {
    // Split channels and get flow magnitude
    vector<Mat> channel;
    split(velocity, channel);

    angle = Mat::zeros(channel[0].rows, channel[0].cols, channel[0].type());
    magnitude = Mat::zeros(channel[0].rows, channel[0].cols, channel[0].type());
    float r;
    for (int i = 0; i < channel[0].rows; i++) {

        const float* xRow = channel[0].ptr<float>(i);
        const float* yRow = channel[1].ptr<float>(i);
        const float* zRow = channel[2].ptr<float>(i);
        float* magnitudeRow = magnitude.ptr<float>(i);
        float* angleRow = angle.ptr<float>(i);

        for (int j = 0; j < channel[0].cols; j++) {
            magnitudeRow[j] = std::sqrt(
                    std::pow(xRow[j], 2) + std::pow(yRow[j], 2) + std::pow(zRow[j], 2)
                    );

            if (magnitudeRow[j] > 0) {
                r = std::sqrt(
                        std::pow(xRow[j], 2) + std::pow(yRow[j], 2)
                        );
                
                errno = 0;
                angleRow[j] = std::acos(r / magnitudeRow[j]);
                
                // Error handling
                if(errno){
                    throw AngleException(CLASS_NAME, 108, errno, i, j, angleRow[j]);
                }
            } else {
                angleRow[j] = 0.0f;
            }
        }
    }
}

#ifdef DEBUG
void VelocityMatrix::showVelocityDebug(){
    // Norm to max element.
    Mat debug = velocity.clone();
    Mat normalized;
    cv::normalize(debug, normalized, 1.0, 0.0, NORM_INF);
    
    vector<Mat> channel;
    split(normalized, channel);
    
    /*vector<double> maxValues(channel.size());
    double maxValue;
    for(Mat ch : channel){
        cv::minMaxIdx(ch, NULL, &maxValue);
        maxValues.push_back(maxValue);
    }*/

    Mat bgr = cv::Mat::zeros(velocity.rows, velocity.cols, CV_8UC3);
    for (int row = 0; row < bgr.rows; row++) {
        const float* xRow = channel[0].ptr<float>(row);
        const float* yRow = channel[1].ptr<float>(row);
        const float* zRow = channel[2].ptr<float>(row);

        for (int col = 0; col < bgr.cols; col++) {         
            bgr.at<Vec3b>(row, col)[0] = static_cast<unsigned char> (255 * xRow[col] );
            bgr.at<Vec3b>(row, col)[1] = static_cast<unsigned char> (255 * yRow[col] );
            bgr.at<Vec3b>(row, col)[2] = static_cast<unsigned char> (255 * zRow[col] );
        }
    }
    cv::imshow("Debug", bgr);
    cv::waitKey(0);
    cv::destroyWindow("Debug");
}
#endif