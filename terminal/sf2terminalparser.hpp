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

#ifndef SF2TERMINALPARSER_HPP
#define SF2TERMINALPARSER_HPP

#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "abstractterminalparser.hpp"
#include "basedescriptor.hpp"
#include "basetrackerfile.hpp"

using namespace std;
using namespace boost::program_options;
using namespace boost::filesystem;

namespace gk{
    struct SceneFlowData {
        long startFrame;
        bool displayFlow;
        unsigned int ctf;
        unsigned int rows;
        string outVideo;
        bool needVideo;
    };
    
    class SF2TerminalParser : public AbstractTerminalParser {     
    private:
        options_description description;
        variables_map parseMap;
        
        void parseHelp() override;
        void parseFiles();
        void parseTrackerData();
        void parseSceneFlowData();
        void parseAngleDescriptor();
        void parseAmplitudeDescriptor();
        void parseCameraSelectorData();
        
    public:
        vector<string> videoFilenames;
        vector<string> trackerFilenames;
        vector<string> timeFilenames;
        vector<string> intrinsicFilenames;
        vector<string> extrinsicFilenames;
        vector<string> imageFilenames;
        vector<string> depthFilenames;

        string cameraSelectorFilename;
        string floFilename;
        long floFrameCount;
        string outHistFilename;
        string outTimeFilename;

        int startFrame;
        TrackerData trackerData;
        SceneFlowData sceneFlowData;
        DescriptorData angleDescriptorData;
        DescriptorData amplitudeDescriptorData;

        SF2TerminalParser(int argc, const char** argv, int majorVersion, int minorVersion);
        void parseInput() override;
    };
}


#endif /* SF2TERMINALPARSER_HPP */

