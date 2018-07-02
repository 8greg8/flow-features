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

#include "angledescriptor.hpp"
#include "amplitudedescriptor.hpp"
#include "histogramfile.hpp"
#include "sf2terminalparser.hpp"
#include "opticalflowvideo.hpp"
#include "selectorfile.hpp"
#include "cameraselector.hpp"
#include "timefilewriter.hpp"

#include "sf2databox.hpp"
#include "basetimer.hpp"
#include "flofile.hpp"
#include "config.hpp"

using namespace std;
using namespace gk;
using namespace cv;

static const string DISPLAY_WINDOW_FLOW = "Scene flow";

static void printErrorHeader(int line) {
    cerr << endl;
    cerr << "File: " << __FILE__ << " line: " << line << endl;
}

static void printErrorFooter() {
    cerr << "Aborting program..." << endl;
    cerr << endl;
    exit(EXIT_FAILURE);
}

/*
 * 
 */
int main(int argc, char** argv) {

    /// TERMINAL
    SF2TerminalParser terminalParser(argc, const_cast<const char**>(argv), VERSION_MAJOR, VERSION_MINOR);
    try {
        terminalParser.parseInput();
    } catch (std::exception& e) {
        printErrorHeader(__LINE__);
        cerr << e.what() << endl;
        printErrorFooter();
    }
    /// TERMINAL



    /// VIDEOS
    std::shared_ptr<OpticalFlowVideo> videoWriter = NULL;
    /// VIDEOS




    /// DESCRIPTORS
    // Get descriptors
    std::shared_ptr<gk::AmplitudeDescriptor> amplitudeDescriptor = NULL;
    if (terminalParser.amplitudeDescriptorData.binCount > 0) {
        amplitudeDescriptor =
                std::make_shared<AmplitudeDescriptor>(terminalParser.amplitudeDescriptorData);
    }

    std::shared_ptr<gk::AngleDescriptor> angleDescriptor = NULL;
    if (terminalParser.angleDescriptorData.binCount > 0) {
        angleDescriptor = std::make_shared<AngleDescriptor>(terminalParser.angleDescriptorData);
    }

    // All initial values are 0.0
    vector<float> angleHistogram;
    vector<float> amplitudeHistogram;
    vector<float> normalizedHistogram;
    /// DESCRIPTORS








    /// CAMERA SELECTOR
    SelectorFile selectorFile(terminalParser.cameraSelectorFilename);
    CameraSelectorData data;
    try {
        data = selectorFile.getNext();
    } catch (std::exception& e) {
        printErrorHeader(__LINE__);
        cerr << e.what() << endl;
        printErrorFooter();

    }
    CameraSelector cameraSelector(data);
    /// CAMERA SELECTOR






    /// FILE WRITERS  
    std::shared_ptr<gk::FloFile> floFile = NULL;
    if (!terminalParser.floFilename.empty()) {
        floFile = std::make_shared<FloFile>(terminalParser.floFilename, terminalParser.floFrameCount);
    }
    HistogramFile histogramFile(terminalParser.outHistFilename);
    TimeFileWriter timeFileWriter(terminalParser.outTimeFilename);
    /// FILE WRITERS



    /// CONTAINERS
    std::vector< std::shared_ptr<SF2DataBox> > dataBoxes;

    for (int i = 0; i < terminalParser.imageFilenames.size(); i++) {
        auto dataBox = std::make_shared<SF2DataBox>(terminalParser.imageFilenames[i],
                terminalParser.depthFilenames[i],
                terminalParser.trackerFilenames[i],
                terminalParser.timeFilenames[i],
                terminalParser.intrinsicFilenames[i],
                terminalParser.extrinsicFilenames[i],
                terminalParser.startFrame,
                terminalParser.sceneFlowData);

        dataBoxes.push_back(dataBox);
    }
    /// CONTAINERS


    /// [Main loop Config]
    Mat normalizedAngle;
    /// [Main loop config]





    Mat opticalFlowImage, opticalFlowFrame;
    bool constructDisplayWindow = true, constructDisplayWindowFlow = true;
    bool changeDisplayWindowImage = false, changeDisplayWindowImageFlow = false;
    std::shared_ptr<string> elapsedTime;
    
    int selected = -1;
    vector<Point3d> metricCenters;
    
    
    BaseTimer timer(1.0);
    timer.start();
    

    /// [Main loop]
    // Start f for second sequence (from image 1,2,3... instead of 0,1,2,...)
    for (int f = terminalParser.startFrame + 1;; f++) {

        // Get next filenames and times
        for (auto dataBox : dataBoxes) {
            dataBox->update();
            metricCenters.push_back(dataBox->metricCenter);
        }

        selected = cameraSelector.select(metricCenters);
        metricCenters.clear();


        // Init video writer
        if ((terminalParser.sceneFlowData.needVideo && f - 1 == terminalParser.startFrame) || terminalParser.sceneFlowData.displayFlow) {
            int fourcc = CV_FOURCC('M', 'J', 'P', 'G');
            videoWriter = std::make_shared<OpticalFlowVideo>(terminalParser.sceneFlowData.outVideo, fourcc, 30, dataBoxes[selected]->matrixSize);
        }


        if (dataBoxes[selected]->confident) {
            // Normalize angles
            dataBoxes[selected]->angle.copyTo(normalizedAngle);

            AngleDescriptor::normalizeAngles(normalizedAngle);

            // Calculate normalized histogram
            try {
                angleDescriptor->getHistogram(normalizedAngle, dataBoxes[selected]->magnitude, angleHistogram);

            } catch (std::exception& e) {
                printErrorHeader(__LINE__);
                cerr << e.what() << endl;
                printErrorFooter();
            }

            amplitudeDescriptor->getHistogram(dataBoxes[selected]->magnitude, amplitudeHistogram);

            normalizedHistogram.insert(normalizedHistogram.begin(),
                    angleHistogram.begin(), angleHistogram.end());
            normalizedHistogram.insert(normalizedHistogram.end(),
                    amplitudeHistogram.begin(), amplitudeHistogram.end());

        } else {
            normalizedHistogram = vector<float>(angleDescriptor->getBinCount() + amplitudeDescriptor->getBinCount());
            fill(normalizedHistogram.begin(), normalizedHistogram.end(), 0);
        }

        // Write normalized histogram to csv file
        histogramFile.write(normalizedHistogram);
        timeFileWriter.write(dataBoxes[selected]->timeStamps[1]);

        angleHistogram.clear();
        amplitudeHistogram.clear();
        normalizedHistogram.clear();








        // For writing FLO
        if (floFile) {
            if ( f == floFile->getFrameNumber()) {
                floFile->write(dataBoxes[selected]->angle, dataBoxes[selected]->magnitude);
            }
        }

        if (terminalParser.sceneFlowData.needVideo) {
            videoWriter->write(dataBoxes[selected]->angle, dataBoxes[selected]->magnitude);
        }
        if (terminalParser.sceneFlowData.displayFlow) {
            if (constructDisplayWindowFlow) {
                namedWindow(DISPLAY_WINDOW_FLOW, WINDOW_NORMAL);
                constructDisplayWindowFlow = false;
                changeDisplayWindowImageFlow = true;
            }
            if (changeDisplayWindowImageFlow) {
                if (videoWriter) {
                    videoWriter->getImage(dataBoxes[selected]->angle, dataBoxes[selected]->magnitude, opticalFlowImage);
                    videoWriter->getFrame(opticalFlowImage, opticalFlowFrame);
                } else {
                    opticalFlowFrame = Mat::zeros(dataBoxes[selected]->matrixSize, CV_8UC3);
                    printErrorHeader(__LINE__);
                    cerr << "No optical flow video writer!" << endl;
                    cerr << "Output image on DISPLAY WINDOW will be empty!" << endl;
                }

                imshow(DISPLAY_WINDOW_FLOW, opticalFlowFrame);
                int key = waitKey(1);
                switch ((char) key) {
                    case 'q':
                        cout << endl;
                        cout << "You wanted to exit. Exiting..." << endl;
                        exit(EXIT_SUCCESS);
                        break;
                    case 'w':
                        cout << endl;
                        cout << "You don't want output to be shown..." << endl;
                        destroyWindow(DISPLAY_WINDOW_FLOW);
                        changeDisplayWindowImageFlow = false;
                        break;
                }
            }
        }
        
        
        

        // Show % for user
        if (timer.isTimeToShowOutput()) {
            //elapsedTime = timer.getElapsedTime();
            printf("\rFrame: %d\tElapsed: %s", f, timer.getElapsedTime()->c_str());
            fflush(stdout);
        }
    }
    /// [Main loop]


    return 0;
}

