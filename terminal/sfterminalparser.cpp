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


#include "sfterminalparser.hpp"

using namespace gk;

SFTerminalParser::SFTerminalParser(int argc, char** argv, int majorVersion, int minorVersion)
: AbstractTerminalParser(majorVersion, minorVersion), description("Allowed options") {
    description.add_options()
        ("help,h", "Produce help message")
        ("image", value<string>(), "Path name and root filename for BGR image.")
        ("depth", value<string>(), "Path name and root filename for DEPTH image.")
        ("startFrame", value<int>()->default_value(1), "Start frame for BGR and DEPTH images")
        ("ctf", value<unsigned int>()->default_value(3), "Coarse to fine levels. Test values from 3 (default) to 5.")
        ("rows", value<unsigned int>()->default_value(424), "Number of rows at the finest level of the pyramid.\nOptions: r=15, r=30, r=60, r=120, r=240, r=424 (if VGA)")
        ("out", value<string>(), "Filename for histogram features.")
        ("trackerFile", value<string>(), "Tracker file.")
        ("timeFile", value<string>(), "Time file.")
        ("videoFile", value<string>(), "Video file.")
        //flo
        ("flo-file", value<string>(), "Filename for FLO file")
        ("flo-frame", value<long>(), "Frame number for FLO file")
        //other
        ("hdb", value<int>()->default_value(60), "Bin count for histogram descriptor")
        ("hdMin", value<float>()->default_value(0), "Amplitudes below min amplitudes are noise")
        ("hdScale", value<float>()->default_value(1.0), 
        "for large displacements set this < 1 to prevent clipping, for now should be 1.0")
        ("hdMaxNorm", value<float>()->default_value(0.0), 
        "For determining max norm range. If 0 norm will not be used.")
        ("adb", value<int>()->default_value(60), "Bin count for amplitude descriptor")
        ("adMin", value<float>()->default_value(0), "Amplitudes below min amplitudes are noise")
        ("adScale", value<float>()->default_value(1.0), 
        "for large displacements set this < 1 to prevent clipping, for now should be 1.0")
        ("adMaxNorm", value<float>()->default_value(0.0), 
        "For determining max norm range. If 0 norm will not be used.")
        ;
    store(parse_command_line(argc, argv, description), parseMap);
    notify(parseMap);  
}

void SFTerminalParser::parseInput(){
    parseHelp();
    
    parseRootFilename();
    parseFlowArguments();
    parseFeaturesFilename();
    parseTrackerFilename();
    parseTimeFilename();
    parseVideoFilename();
    parseAngleDescriptor();
    parseAmplitudeDescriptor();   
}

void SFTerminalParser::parseHelp(){
    if(parseMap.count("help")){
        cout << endl;
        cout << "Version " << majorVersion << "." << minorVersion << endl;
        cout << description << endl;
        exit(EXIT_SUCCESS);
    }
}

void SFTerminalParser::parseRootFilename(){
    if (parseMap.count("image")){
        imageRootFilename = expandName(parseMap["image"].as<string>());
    } else {
        cout << endl;
        cout << "You must specify image root filename with --image" << endl;
        cout << endl;
        cout << description << endl;
        exit(EXIT_FAILURE);
    }
    
    if (parseMap.count("depth")){
        depthRootFilename = expandName(parseMap["depth"].as<string>());
    } else {
        cout << endl;
        cout << "You must specify depth root filename with --depth" << endl;
        cout << endl;
        cout << description << endl;
        exit(EXIT_FAILURE);
    }
    
    if (parseMap.count("flo-file")) {
        floFilename = expandName(parseMap["flo-file"].as< string >());

        if (parseMap.count("flo-frame")) {
            floFrameCount = parseMap["flo-frame"].as< long >();

        } else {
            cout << endl;
            cout << "You must specify flo frame number with --flo-frame" << endl;
            cout << endl;
            cout << description << endl;
            exit(EXIT_FAILURE);
        }
    }
}

void SFTerminalParser::parseFlowArguments(){
    flowArg.startFrame = parseMap["startFrame"].as<int>();
    flowArg.ctf = parseMap["ctf"].as<unsigned int>();
    flowArg.rows = parseMap["rows"].as<unsigned int>();
  
}

void SFTerminalParser::parseFeaturesFilename(){
    if (parseMap.count("out")){
        featuresFilename = expandName(parseMap["out"].as<string>());
        
    } else{
        cout << endl;
        cout << "You must specify features file with --out" << endl;
        cout << endl;
        cout << description << endl;
        exit(EXIT_FAILURE);
    }
}

void SFTerminalParser::parseTrackerFilename(){
    if(parseMap.count("trackerFile")){
        trackerFilename = parseMap["trackerFile"].as<string>();
    }
}

void SFTerminalParser::parseTimeFilename(){
    if (parseMap.count("timeFile")) {
        timeFilename = parseMap["timeFile"].as<string>();
    } else {
        cout << endl;
        cout << "You must specify time file with --timeFile" << endl;
        cout << endl;
        cout << description << endl;
        exit(EXIT_FAILURE);
    }
}

void SFTerminalParser::parseVideoFilename(){
    if(parseMap.count("videoFile")){
        videoFilename = parseMap["videoFile"].as<string>();
    }
}


void SFTerminalParser::parseAngleDescriptor(){
    
    if(parseMap.count("hdb")){
        angleDescriptor.binCount = parseMap["hdb"].as<int>();
    }   
    if(parseMap.count("hdMin")){
        angleDescriptor.minAmplitude = parseMap["hdMin"].as<float>();
    }
    if(parseMap.count("hdScale")){
        angleDescriptor.scale = parseMap["adScale"].as<float>();
    }
    if(parseMap.count("hdMaxNorm")){
        angleDescriptor.maxNorm = parseMap["hdMaxNorm"].as<float>();
    }
}

void SFTerminalParser::parseAmplitudeDescriptor(){
    if(parseMap.count("adb")){
        amplitudeDescriptor.binCount = parseMap["adb"].as<int>();
    }
    if(parseMap.count("adMin")){
        amplitudeDescriptor.minAmplitude = parseMap["adMin"].as<float>();
    }
    if(parseMap.count("adScale")){
        amplitudeDescriptor.scale = parseMap["adScale"].as<float>();
    }
    if(parseMap.count("adMaxNorm")){
        amplitudeDescriptor.maxNorm = parseMap["adMaxNorm"].as<float>();
    }
}