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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <exception>

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/mat.hpp>

#include "inputsequence.hpp"
#include "angledescriptor.hpp"
#include "amplitudedescriptor.hpp"
#include "histogramfile.hpp"
#include "sfterminalparser.hpp"
#include "sf2trackerfile.hpp"
#include "timefilereader.hpp"
#include "velocitymatrix.hpp"
#include "opticalflowvideo.hpp"
#include "framespeed.hpp"

#include "scene_flow_impair.h"
#include "flofile.hpp"
#include "config.hpp"

using namespace std;
using namespace gk;
using namespace cv;

/*
 * 
 */
int main(int argc, char** argv) {
    SFTerminalParser terminalParser(argc, argv, VERSION_MAJOR, VERSION_MINOR);
    terminalParser.parseInput();
    
    
    /// [Main objects]
    InputSequence* imageInputSequence [2];
    InputSequence* depthInputSequence [2];

    imageInputSequence[0] = new InputSequence(terminalParser.imageRootFilename, terminalParser.flowArg.startFrame);
    imageInputSequence[1] = new InputSequence(terminalParser.imageRootFilename, terminalParser.flowArg.startFrame + 1);
    
    depthInputSequence[0] = new InputSequence(terminalParser.depthRootFilename, terminalParser.flowArg.startFrame);
    depthInputSequence[1] = new InputSequence(terminalParser.depthRootFilename, terminalParser.flowArg.startFrame + 1);
    
    
    PD_flow_opencv* sceneflow = NULL;
    
    //cout << "Time file name: " << terminalParser.timeFilename << endl;
    long timeStamps[2];
    TimeFileReader* timeFiles [2];
    timeFiles[0] = new TimeFileReader(terminalParser.timeFilename, terminalParser.flowArg.startFrame);
    timeFiles[1] = new TimeFileReader(terminalParser.timeFilename, terminalParser.flowArg.startFrame + 1);
    
    FrameSpeed frameSpeed;
    
    SF2TrackerFile* trackerFile = NULL;
    if (!terminalParser.trackerFilename.empty()) {
        trackerFile = new SF2TrackerFile(terminalParser.trackerFilename, terminalParser.flowArg.startFrame + 1);
    }
    
    HistogramFile histogramFile(terminalParser.featuresFilename);
    
    OpticalFlowVideo* videoWriter = NULL;
    Size matrixSize;
    
    
    
    std::shared_ptr<gk::FloFile> floFile = NULL;
    if (!terminalParser.floFilename.empty()) {
        floFile = std::make_shared<FloFile>(terminalParser.floFilename, terminalParser.floFrameCount);
    }
    /// [Main objects]
    
    
    
    
    
    
    
    
    
    
    /// [Main loop Config]
    string imageFilename [2];
    string depthFilename [2];
    ifstream file;

    Mat angle, normalizedAngle, magnitude;
    std::shared_ptr<Rect2d> roi = NULL;
    
    AngleDescriptor angleDescriptor(terminalParser.angleDescriptor.binCount, terminalParser.angleDescriptor.maxNorm);
    vector<float> normalizedHistogram;
    
    AmplitudeDescriptor amplitudeDescriptor(
            terminalParser.amplitudeDescriptor.binCount, 
            terminalParser.amplitudeDescriptor.minAmplitude, 
            terminalParser.amplitudeDescriptor.scale, 
            terminalParser.amplitudeDescriptor.maxNorm
    );
    vector<float> amplitudeHistogram;
    bool confident = true;
    int frameNumber;
    VelocityMatrix* velocityMatrix = NULL;

    /// [Main loop config]
    
    
    
    
    
    
    
    
    
    
    
    
    /// [Main loop]
    //string rowPixelString, colPixelString, vxString, vyString, vzString;
    for(int i= terminalParser.flowArg.startFrame;; i++){
        
        // Get next filenames and times
        for(int j=0; j<2; j++){
            if(!imageInputSequence[j]->getFilename(imageFilename[j])){
                if(j == 1) {
                    // No file found. Probably last file.
                    cout << "Second BGR image not found." << endl;
                    cout << "Last file: " << imageFilename[0] <<  endl;
                    cout << "EXIT SUCCESS.";
                    exit(EXIT_SUCCESS);
                } else{
                    cerr << "First BGR image not found." << endl;
                    cerr << "File: " << imageFilename[0] << endl;
                    cerr << "EXIT FAILURE";
                    exit(EXIT_FAILURE);
                }
            }
            if(!depthInputSequence[j]->getFilename(depthFilename[j])){
                    cerr << "DEPTH image not found, but BGR image exists!" << endl;
                    cerr << "File: " << depthFilename[0] << endl;
                    cerr << "EXIT FAILURE";
                    exit(EXIT_FAILURE);
            }
            timeStamps[j] = timeFiles[j]->getNext();
        }
        
        
        
        // Calculate scene flow          
        sceneflow = new PD_flow_opencv(terminalParser.flowArg.rows, 
                terminalParser.flowArg.ctf,
                imageFilename[0].c_str(),
                imageFilename[1].c_str(),
                depthFilename[0].c_str(),
                depthFilename[1].c_str());
        
        matrixSize = Size(sceneflow->cols, sceneflow->rows);
        
        sceneflow->initializeCUDA();
        if(sceneflow->loadRGBDFrames()) {
            sceneflow->solveSceneFlowGPU();
            
            if (velocityMatrix) {
                delete velocityMatrix;
                velocityMatrix = NULL;
            }
            velocityMatrix = new VelocityMatrix(sceneflow->dxp, sceneflow->dyp, 
                    sceneflow->dzp, sceneflow->rows, sceneflow->cols, 
                    frameSpeed.getFps(timeStamps[0], timeStamps[1]));
            
            // Clean up
            sceneflow->freeGPUMemory();
            if(sceneflow){
                delete sceneflow;
                sceneflow = NULL;
            }
        } else{
            cerr << "Images were not loaded to scene flow object. Exiting...";
            cerr << endl;
            exit(EXIT_FAILURE);
        }
        
        // Init video writer
        if (!terminalParser.videoFilename.empty() && i == terminalParser.flowArg.startFrame) {
            int fourcc = CV_FOURCC('M', 'J', 'P', 'G');
            videoWriter = new OpticalFlowVideo(terminalParser.videoFilename, fourcc, 30, matrixSize);
        }
        
        
        
        
        // Read bounding box
        if(trackerFile){
            roi = trackerFile->getNext();
            if(roi->area() > 0){
                confident = true;
                
            } else{
                cout << endl;
                cout << "ROI No. " << i << " not confident or not tracked. Histograms will be empty." << endl;
                confident = false;    
            }
        }
        
        if (confident) {
            if (trackerFile) {
                velocityMatrix->cropVelocityMatrix(*roi);
                //velocityMatrix->showVelocityDebug();
            }
            try{
                velocityMatrix->getSemiSpherical(angle, magnitude);
                
            } catch(std::exception& e){
                cerr << endl;
                cerr << e.what() << endl;
                cerr << "Error for images: " << imageFilename[0] << endl;
                cerr << "Aborting..." << endl;
                exit(EXIT_FAILURE);
            }


            // Normalize angles
            angle.copyTo(normalizedAngle);
            AngleDescriptor::normalizeAngles(normalizedAngle);

            // Calculate normalized histogram
            try {
                angleDescriptor.getHistogram(normalizedAngle, magnitude, normalizedHistogram);
                
            } catch(std::exception& e){
                cerr << endl;
                cerr << e.what() << endl;
                cerr << "Error for images: " << imageFilename[0] << endl;
                cerr << "Aborting..." << endl;
                exit(EXIT_FAILURE);
            }

            amplitudeDescriptor.getHistogram(magnitude, amplitudeHistogram);
            normalizedHistogram.insert(
                    normalizedHistogram.end(),
                    amplitudeHistogram.begin(),
                    amplitudeHistogram.end()
                    );

        } else {
            normalizedHistogram = vector<float>(angleDescriptor.getBinCount() + amplitudeDescriptor.getBinCount());
            fill(normalizedHistogram.begin(), normalizedHistogram.end(), 0);
        }
           
        // Write normalized histogram to csv file
        histogramFile.write(normalizedHistogram);
        normalizedHistogram.clear();
        amplitudeHistogram.clear();
        
        
        // For writing FLO
        if (floFile) {
            if ((i-1) == floFile->getFrameNumber()) {
                if(!floFile->write(angle, magnitude)){
                    cerr << endl;
                    cerr << "Couldn't write .flo file!" << endl;
                    cerr << "Continuing." << endl;
                }
            }
        }
        
        if(videoWriter){
            videoWriter->write(angle, magnitude);
        }
      
        if(imageInputSequence) {
            if (!imageInputSequence[0]->sequenceEnabled()) {
                // No sequence so exit.
                file.close();
                cout << endl;
                cout << "The last opened file number was: " << imageInputSequence[1]->getFrameNumber() - 1 << endl;
                break;
            }     
        }
    }
    /// [Main loop]
    
    if(trackerFile){
        delete trackerFile;
    }
    if(velocityMatrix){
        delete velocityMatrix;
    }
    for(int i = 0; i < 2; i++){
        if(imageInputSequence[i]){
            delete imageInputSequence[i];
        }
        if(depthInputSequence[i]){
            delete depthInputSequence[i];
        }
        if(timeFiles[i]){
            delete timeFiles[i];
        }
    }
    if(videoWriter){
        delete videoWriter;
    }
    
    
    return 0;
}

