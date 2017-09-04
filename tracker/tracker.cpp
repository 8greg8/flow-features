/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "tracker.hpp"

gk::Tracker::Tracker(string trackerType) : trackerType(trackerType) {
    this->tracker = create(trackerType);
}

void gk::Tracker::init(const Mat& frame, const Rect2d& roi){
    // If tracker is not empty we want it to reinitialize.
    // We get target state from model and set its bounding box.
    if(tracker->getModel()){
        tracker->clear();
        tracker = create(trackerType);
    }
    // Because of the OPENCV BUG that I found we must round our roi
    // before using it for initialization!!!
    Rect2d roundedRoi = roi;
    roundRoi(roundedRoi);
    
    // Initialize tracker
    if(!tracker->init(frame, roundedRoi)){
        cerr << "Could not initialize tracker" << endl;
        exit(EXIT_FAILURE);
    }
}
void gk::Tracker::roundRoi(Rect2d& roi){
    roi.x = cvRound(roi.x);
    roi.y = cvRound(roi.y);
    roi.width = cvRound(roi.width);
    roi.height = cvRound(roi.height);
}

bool gk::Tracker::update(const Mat& frame, Rect2d& roi){
    // Update tracking result
    // If updating tracker returns true, that means that
    // target was located
    // If target was not located, do nothing
    return tracker->update(frame, roi);
}

Ptr<cv::Tracker> gk::Tracker::create(string trackerType){
    Ptr<cv::Tracker> tracker;
    if(trackerType == "TLD"){
        tracker = cv::Tracker::create(trackerType);
        
    }else if(trackerType == "MIL"){
        TrackerMIL::Params milParams;
        //milParams.featureSetNumFeatures;
        //milParams.samplerInitInRadius;
        // ...
        
    }else if(trackerType == "BOOSTING"){
        tracker = cv::Tracker::create(trackerType);
        
    }else if(trackerType == "MEDIANFLOW"){
        tracker = cv::Tracker::create(trackerType);
        
    }else if(trackerType == "KCF"){
        // Tracker KCF default parameters
        /*
         * sigma=0.2; gaussian kernel bandwidth
         * interp_factor=0.075; linear interpolation factor for adaptation
         * lambda=0.01; regularization
         * max_patch_size=80*80; threshold for the ROI size
         * output_sigma_factor=1.0/16.0; spatial bandwidth (proportional to target)
         * resize=true; activate the resize feature to improve the processing speed
         * split_coeff=true; split the training coefficients into two matrices
         * wrap_kernel=false; wrap arround the kernel values
         * desc_npca = GRAY; non-compressed descriptors of TrackerKCF::MODE
         * desc_pca = CN; compressed descriptors of TrackerKCF::MODE
         * 
         * // feature compression
         * compress_feature=true; activate the pca method to compress the features
         * compressed_size=2; feature size after compression
         * pca_learning_rate=0.15; compression learning rate
         * 
         */
        //TrackerKCF::Params kcfParams = TrackerKCF::Params();
        //kcfParams.desc_npca = TrackerKCF::CN;
        //kcfParams.desc_pca = TrackerKCF::CN;
        //kcfParams.interp_factor;
        //kcfParams.lambda;
        //kcfParams.max_patch_size;
        //kcfParams.output_sigma_factor;
        //kcfParams.pca_learning_rate;
        //kcfParams.resize;
        //kcfParams.sigma;
        //kcfParams.split_coeff;
        //kcfParams.wrap_kernel;
        //outputTracker = TrackerKCF::createTracker(kcfParams);
        tracker = cv::Tracker::create(trackerType);
    }
    
    return tracker;
}