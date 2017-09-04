/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tracker.hpp
 * Author: gregork
 *
 * Created on November 6, 2016, 8:33 PM
 */

#ifndef TRACKER_HPP
#define TRACKER_HPP

#include <opencv2/tracking.hpp>

#include <string>

#include "roi.hpp"

using namespace std;
using namespace cv;

namespace gk{
    class Tracker{
    private:
        Ptr<cv::Tracker> tracker;
        string trackerType;
        
        Ptr<cv::Tracker> create(string trackerType);
        
    public:
        Tracker(string trackerType);
        
        void init(const Mat& frame, const Rect2d& roi);
        
        bool update(const Mat& frame, Rect2d& roi);
        
        void roundRoi(Rect2d& roi);
    };
}


#endif /* TRACKER_HPP */

