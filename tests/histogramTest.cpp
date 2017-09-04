/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   histogramTest.cpp
 * Author: gregork
 *
 * Created on June 26, 2016, 1:41 PM
 */

/*
 * Simple C++ Test Suite
 */

#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>

// std
#include <iostream>
#include <string>
#include <fstream>

// other
#include "histogramdescriptor.hpp"

using namespace cv;
using namespace std;


int main(int argc, char** argv)
{
	int binCount = 4;
	std::string histogramFileName = "/home/gregork/Videos/featuresTest.txt";

    cout << "======================" << endl;
    cout << "Starting optical flow estimation..." << endl;

    Histogram histogram(binCount);
	Mat normalizedHistogram;

	std::ofstream histogramFile;
	histogramFile.open(histogramFileName.c_str(), ofstream::app);


	Mat flowAngle1 = (Mat_<float>(2,2) << -90*CV_PI/180, -1*CV_PI/180, 20*CV_PI/180, 45*CV_PI/180);
	Mat flowMagnitude1 = (Mat_<float>(2,2) << 1, 2, 3, 4);
	// Calculate normalized histogram
	histogram.calculateNormalizedHistogram(flowAngle1, flowMagnitude1, normalizedHistogram);


	// Write normalized histogram to csv file
	if(normalizedHistogram.rows == 1 && normalizedHistogram.cols == binCount){

		float* row = normalizedHistogram.ptr<float>(0);
		for(int x = 0; x < normalizedHistogram.cols; x++){
			cout << row[x] << endl;
			histogramFile << row[x];

			if(x != (normalizedHistogram.cols - 1) ){
				histogramFile << ", ";
			}
		}
		histogramFile << endl;

	} else{
		cout << "Histogram was not calculated correctly" << endl;
		return -1;
	}

    cout << "Optical flow estimation is finnished." << endl;
    cout << "=====================================" << endl;

    return(EXIT_SUCCESS);
}

