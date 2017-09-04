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

#ifndef OFTERMINALPARSER_HPP
#define OFTERMINALPARSER_HPP

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <iostream>
#include <memory>

// posix
#include <wordexp.h> // for expanding ~ to $HOME directory

// boost
#include <boost/format.hpp>

#include "tracker.hpp"
#include "roi.hpp"
#include "scaler.hpp"
#include "opticalflow.hpp"
#include "amplitudedescriptor.hpp"
#include "of2trackerfile.hpp"
#include "angledescriptor.hpp"
#include "flo.hpp"
#include "amplitudefactor.hpp"

using namespace std;
using namespace boost;

namespace gk{
    class OFTerminalParser{
    private:
        // Start with 2nd frame. First frame doesnt hanve optical flow.
        static int flowFrameCount; 
        static const String KEYS;
        
        CommandLineParser parser;

        void initTracker(Rect2d& roi);
        string parseOpticalFlowVideoname(string input);
        void parseImageFilenames();
        string expandName(string input);
        string parseMainInput(string input);
        
        void initAmplitudeDescriptor();
        void initAngleDescriptor();
        void initAmplitudeFactor();
        
        
        /* FIELDS */
        std::shared_ptr<gk::OpticalFlowData> ofConfig;
        string inputVideoname;
        string histogramFilename;
        
        int frameNumber;
        bool needImage;
        
        string originalImageFilename;
        string opticalFlowImageFilename;
        
        static const string FLO_TYPE;
        static const string DEFAULT_FLO_FORMAT;
        bool floHasSequence = false;
        boost::format floSequenceFormat;
        bool floNeeded = false;
        
        std::shared_ptr<gk::Tracker> tracker;
        bool trackerEnabled;
        bool selectRoiOnFirstFrame = false;
        double selectionSeconds = 0;
        bool selectionEnabled = false;
        bool displayTracker;
        bool trackerScaling = false;
        double trackerScale;
        
        std::shared_ptr<Flo> flo = NULL;
        
        // Descriptors
        std::shared_ptr<AngleDescriptor> histogramDescriptor = NULL;
        std::shared_ptr<AmplitudeDescriptor> amplitudeDescriptor = NULL;
        
        std::shared_ptr<OF2TrackerFile> trackerFile = NULL;
        std::shared_ptr<AmplitudeFactor> amplitudeFactor = NULL;

    public:
        OFTerminalParser(int argc, const char *const argv[], Rect2d& roi);
        
        void help();
     
        void initFlo();
        bool getFloSequenceFormat(const string& floFilename, boost::format& floSequenceFormat);
        string getFloFilename();
        bool hasFloSequence();
        bool isFloNeeded();
        int getFloFrameNumber();

        string getInputVideoname();
        string getHistogramFilename();
        int getBinCount();
        std::shared_ptr<gk::Tracker> getTracker();
        double getSelectionSeconds();
        bool isSelectionEnabled();
        bool needSelectRoiOnFirstFrame();
        bool isTrackerEnabled();
        bool isImageNeeded();
        int getFrameNumber();
        string getOriginalImageFilename();
        bool needDisplayTracker();
        bool isTrackerScalingEnabled();
        double getTrackerDownScale();
        double getTrackerUpScale();
        
        std::shared_ptr<OpticalFlowData> getOpticalFlowConfig();
        
        std::shared_ptr<Flo> getFlo();
        
        bool isAmplitudeDescriptorUsed();
        bool isAngleDescriptorUsed();
        std::shared_ptr<AmplitudeDescriptor> getAmplitudeDescriptor();
        std::shared_ptr<AngleDescriptor> getAngleDescriptor();
        
        bool trackerFromFile();
        std::shared_ptr<OF2TrackerFile> getOF2TrackerFile();
        
        std::shared_ptr<AmplitudeFactor> getAmplitudeFactor();
    };
}

#endif /* OFTERMINALPARSER_HPP */

