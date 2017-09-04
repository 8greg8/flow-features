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

#include "sf2databox.hpp"

using namespace gk;

//int SF2DataBox::ID = 0;

SF2DataBox::SF2DataBox(const string& imageFilename, 
        const string& depthFilename, 
        const string& trackerFilename, 
        const string& timeFilename,
        const string& intrinsicFilename,
        const string& extrinsicFilename,
        const long startFrame,
        const SceneFlowData& sceneFlowData)
: BaseDataBox(startFrame), sceneFlowData(sceneFlowData){

    configInput( imageFilename, depthFilename, startFrame);
    configTracker(trackerFilename, startFrame);
    configTime(timeFilename, startFrame);
    configCameraCalib(intrinsicFilename, extrinsicFilename);
}


void SF2DataBox::configInput(const string& imageFilename, 
        const string& depthFilename, 
        const long startFrame) {
    
    // N-th and (N+1)-th frame
    imageInputSequence = vector<std::shared_ptr<InputSequence>>(2);
    depthInputSequence = vector<std::shared_ptr<InputSequence>>(2);
    
    for(int i=0; i < imageInputSequence.size(); i++){
        imageInputSequence[i] = 
                std::make_shared<InputSequence>(imageFilename, startFrame + i);
        depthInputSequence[i] = 
                std::make_shared<InputSequence>(depthFilename, startFrame + i);
    }
    
    imageFilenames = vector<string>(2);
    depthFilenames = vector<string>(2);
}


void SF2DataBox::configTracker(const string& trackerFilename, const long startFrame){
    // Synced with (N+1)-th frame
    trackerFile = std::make_shared<SF2TrackerFile>(trackerFilename, startFrame + 1);
    
}

void SF2DataBox::configTime(const string& timeFilename, const long startFrame){
    
    // N-th and (N+1)-th frame
    timeFileReaders = vector<std::shared_ptr<TimeFileReader>>(2);
    for (int i = 0; i < timeFileReaders.size(); i++) {
        timeFileReaders[i] =
                std::make_shared<TimeFileReader>(timeFilename, startFrame + i);
    } 
    
    frameSpeed = std::make_shared<FrameSpeed>();
}



bool SF2DataBox::update(){
    
    // Update image filenames
    // Update depth filenames
    // Update time stamps
    for(int i=0; i<2; i++){
        if (!imageInputSequence[i]->getFilename(imageFilenames[i])) {
            if (i == 1) {
                // No file found. Probably last file.
                cout << "Second BGR image not found." << endl;
                cout << "Last file: " << imageFilenames[0] << endl;
                cout << "EXIT SUCCESS.";
                exit(EXIT_SUCCESS);
            } else {
                cerr << "First BGR image not found." << endl;
                cerr << "File: " << imageFilenames[0] << endl;
                cerr << "EXIT FAILURE";
                exit(EXIT_FAILURE);
            }
        }
        if (!depthInputSequence[i]->getFilename(depthFilenames[i])) {
            cerr << "DEPTH image not found, but BGR image exists!" << endl;
            cerr << "File: " << depthFilenames[0] << endl;
            cerr << "EXIT FAILURE";
            exit(EXIT_FAILURE);
        }
        timeStamps[i] = timeFileReaders[i]->getNext();       
    }
    
    // Update fps;
    fps = frameSpeed->getFps(timeStamps[0], timeStamps[1]);
    
    
    
    // Update roi
    roi = trackerFile->getNext();
    if (roi->area() > 0) {
        confident = true;

    } else {
        confident = false;
    }
    
    
    if(confident){
        // Calculate scene flow   
        calculateSceneFlow();
    }
    
    // Update metric center
    // Update depth
    // synced with (N+1)-th frame
    depthImage = std::make_shared<DepthImage>(depthFilenames[1]);
    metricCenter = Roi::getMetricCenter(*roi, depthImage->depth, homography);
    depthImage.reset();
    
    if(confident){
        if (trackerFile) {
            velocityMatrix->cropVelocityMatrix(*roi);
        }
        
        try {
            velocityMatrix->getSemiSpherical(angle, magnitude);
            velocityMatrix.reset();

        } catch (std::exception& e) {
            cerr << endl;
            cerr << e.what() << endl;
            cerr << "Error for images: " << imageFilenames[0] << endl;
            cerr << "Aborting..." << endl;
            exit(EXIT_FAILURE);
        }
    }
    
    return true;
}


void SF2DataBox::calculateSceneFlow(){
    sceneflow = std::make_shared<PD_flow_opencv>(sceneFlowData.rows,
            sceneFlowData.ctf,
            imageFilenames[0].c_str(),
            imageFilenames[1].c_str(),
            depthFilenames[0].c_str(),
            depthFilenames[1].c_str());

    matrixSize = Size(sceneflow->cols, sceneflow->rows);

    sceneflow->initializeCUDA();
    if (sceneflow->loadRGBDFrames()) {
        sceneflow->solveSceneFlowGPU();
        
        velocityMatrix = std::make_shared<VelocityMatrix>(sceneflow->dxp, sceneflow->dyp,
                sceneflow->dzp, sceneflow->rows, sceneflow->cols, fps);

        // Clean up
        sceneflow->freeGPUMemory();
        sceneflow.reset();
    } else {
        cerr << "Images were not loaded to scene flow object. Exiting...";
        cerr << endl;
        exit(EXIT_FAILURE);
    }
}