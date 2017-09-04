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

#include "cameracalib.hpp"

using namespace gk;

CameraCalib::CameraCalib(const IntrinsicData& intrinsicData, const ExtrinsicData& extrinsicData){
    getIntrinsic(intrinsic, intrinsicData);
    
    ExtrinsicData correctedExtrinsicData;
    correctExtrinsicData(extrinsicData, correctedExtrinsicData);
    getExtrinsic(extrinsic, correctedExtrinsicData);
    
    getHomography(homography, intrinsic, extrinsic);    
}

void CameraCalib::getIntrinsic(Mat& intrinsic, const IntrinsicData& intrinsicData){
    intrinsic = (Mat_<float>(4, 4) 
            << intrinsicData.depthCameraParams.focalLengthX, 0, intrinsicData.depthCameraParams.principaPointX, 0,
            0, intrinsicData.depthCameraParams.focalLengthY, intrinsicData.depthCameraParams.principalPointY, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
            );
    
#ifdef DEBUG
    cout << "Intrinsic matrix: " << endl;
    cout << intrinsic << endl;
#endif
}

void CameraCalib::correctExtrinsicData(const ExtrinsicData& extrinsicData, ExtrinsicData& correctedExtrinsicData){
    //Convert to rad
    correctedExtrinsicData = extrinsicData;
    
    correctedExtrinsicData.roll *= (CV_PI / 180);
    correctedExtrinsicData.pitch *= (CV_PI / 180);
    correctedExtrinsicData.yaw *= (CV_PI / 180);
    
#ifdef DEBUG
    cout << "Old extrinsic data: " << endl;
    //cout << "ID\t" << extrinsicData.id << endl;
    cout << "tx\t" << extrinsicData.translationX << endl;
    cout << "ty\t" << extrinsicData.translationY << endl;
    cout << "tz\t" << extrinsicData.translationZ << endl;
    cout << "roll\t" << extrinsicData.roll << endl;
    cout << "pitch\t" << extrinsicData.pitch << endl;
    cout << "yaw\t" << extrinsicData.yaw << endl;
    
    cout << "New extrinsic data: " << endl;
    //cout << "ID\t" << correctedExtrinsicData.id << endl;
    cout << "tx\t" << correctedExtrinsicData.translationX << endl;
    cout << "ty\t" << correctedExtrinsicData.translationY << endl;
    cout << "tz\t" << correctedExtrinsicData.translationZ << endl;
    cout << "roll\t" << correctedExtrinsicData.roll << endl;
    cout << "pitch\t" << correctedExtrinsicData.pitch << endl;
    cout << "yaw\t" << correctedExtrinsicData.yaw << endl;
#endif
}

void CameraCalib::getExtrinsic(Mat& extrinsic, const ExtrinsicData& extrinsicData){
    Mat t;
    
    //if(extrinsicData.id == MASTER){
    //    t = (Mat_<float>(3,1) <<
    //            0, 0, 0
    //            );
    //}else if(extrinsicData.id == SLAVE){
        t = (Mat_<float>(3,1) <<
                - extrinsicData.translationX,
                - extrinsicData.translationY,
                - extrinsicData.translationZ
                );
    //}
     
    
    Mat pitch = ( Mat_<float>(3,3) <<
            1, 0, 0,
            0, cos(extrinsicData.pitch), - sin(extrinsicData.pitch),
            0, sin(extrinsicData.pitch), cos(extrinsicData.pitch)
            );
    
    Mat yaw = ( Mat_<float>(3,3) <<
            cos(extrinsicData.yaw), 0, sin(extrinsicData.yaw),
            0, 1, 0,
            - sin(extrinsicData.yaw), 0, cos(extrinsicData.yaw)
            );

    Mat roll = ( Mat_<float>(3,3) <<
            cos(extrinsicData.roll), - sin(extrinsicData.roll), 0,
            sin(extrinsicData.roll), cos(extrinsicData.roll), 0,
            0, 0, 1 
            );

    Mat rotation = roll * pitch * yaw;
    Mat z = ( Mat_<float>(1,4) <<
            0,0,0,1
            );
    
    hconcat(rotation, t, extrinsic);
    vconcat(extrinsic, z, extrinsic);
    
#ifdef DEBUG
    cout << "Extrinsic matrix: " << endl;
    cout << extrinsic << endl;
#endif
}

void CameraCalib::getHomography(Mat& homography, const Mat& intrinsic, const Mat& extrinsic){
    homography = intrinsic * extrinsic;
    
#ifdef DEBUG
    cout << "Homography:" << endl;
    cout << homography << endl;
#endif
}