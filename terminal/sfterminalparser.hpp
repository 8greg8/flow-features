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

#ifndef SFTERMINALPARSER_HPP
#define SFTERMINALPARSER_HPP

#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <wordexp.h>

#include "abstractterminalparser.hpp"
#include "basedescriptor.hpp"
#include "basetrackerfile.hpp"

using namespace std;
using namespace boost::program_options;
using namespace boost::filesystem;

namespace gk{

    struct flowArgument{
        int startFrame;
        unsigned int ctf;
        unsigned int rows;
    };
    
    class SFTerminalParser: public AbstractTerminalParser{
    public:
        string imageRootFilename;
        string depthRootFilename;
        string featuresFilename;
        string trackerFilename;
        string timeFilename;
        string videoFilename;
        flowArgument flowArg; 
        DescriptorData angleDescriptor;
        DescriptorData amplitudeDescriptor;
        
        string floFilename;
        long floFrameCount;
        

        SFTerminalParser(int argc, char** argv, int majorVersion, int minorVersion);
        void parseInput() override;

    private:
        options_description description;
        variables_map parseMap;
        
        void parseHelp() override;
        void parseRootFilename();
        void parseFlowArguments();
        void parseFeaturesFilename();
        void parseTrackerFilename();
        void parseTimeFilename();
        void parseVideoFilename();
        void parseAngleDescriptor();
        void parseAmplitudeDescriptor();
    };
}


#endif /* SFTERMINALPARSER_HPP */

