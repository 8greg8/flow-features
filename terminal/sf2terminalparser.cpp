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

#include "sf2terminalparser.hpp"

using namespace gk;

SF2TerminalParser::SF2TerminalParser(int argc, const char** argv, int majorVersion, int minorVersion)
: AbstractTerminalParser(majorVersion, minorVersion), description("Allowed options") {

    description.add_options()
            //
            // help
            ("help,h", "Produce help message")
            //
            // files
            ("image-files", value< vector<string> >()->multitoken(), "Path name and root filename for BGR image.")
            ("depth-files", value< vector<string> >()->multitoken(), "Path name and root filename for DEPTH image.")
            ("tracker-files", value< vector<string> >()->multitoken(), "Tracker files")
            ("time-files", value< vector<string> >()->multitoken(), "Time stamps files")
            ("intrinsic-files", value< vector<string> >()->multitoken(), "Intrinsic files")
            ("extrinsic-files", value< vector<string> >()->multitoken(), "Extrinsic files")
            ("selector-file", value< string >(), "Camera selector config file")
            //
            // out
            ("flo-file", value<string>(), "Filename for FLO file")
            ("flo-frame", value<long>(), "Frame number for FLO file")
            ("out-hist", value<string>(), "Filename for histogram features.")
            ("out-time", value<string>(), "Filename for merged time stamps.")
            //
            // tracker data
            ("tracker-scale", value<float>()->default_value(0), "Scale for tracker ROI")
            ("player-id", value<int>()->default_value(1), "Player ID to track")
            ("display-tracker", value<bool>()->default_value(false), "Display tracker on optical flow video")
            //
            // scene flow data
            ("start-frame", value<long>()->default_value(1), "Start frame for BGR and DEPTH images")
            ("ctf", value<unsigned int>()->default_value(3), "Coarse to fine levels. Test values from 3 (default) to 5.")
            ("rows", value<unsigned int>()->default_value(424), "Number of rows at the finest level of the pyramid.\nOptions: r=15, r=30, r=60, r=120, r=240, r=424 (if VGA)")
            ("sf-video", value<string>(), "Output optical flow video")
            ("display-flow", value<bool>()->default_value(false), "Display flow during calculation")
            //
            // angle descriptor data
            ("hd-b", value<int>()->default_value(60), "Bin count for angle descriptor")
            ("hd-min", value<float>()->default_value(0), "Amplitudes below min amplitudes are noise")
            ("hd-scale", value<float>()->default_value(1.0),
            "for large displacements set this < 1 to prevent clipping, for now should be 1.0")
            ("hd-max-norm", value<float>()->default_value(0.0),
            "For determining max norm range. If 0 norm will not be used.")
            //
            // amplitude descriptor data
            ("ad-b", value<int>()->default_value(60), "Bin count for amplitude descriptor")
            ("ad-min", value<float>()->default_value(0), "Amplitudes below min amplitudes are noise")
            ("ad-scale", value<float>()->default_value(1.0),
            "for large displacements set this < 1 to prevent clipping, for now should be 1.0")
            ("ad-max-norm", value<float>()->default_value(0.0),
            "For determining max norm range. If 0 norm will not be used.")
            ;
    store(parse_command_line(argc, argv, description), parseMap);
    notify(parseMap);
}

void SF2TerminalParser::parseInput() {
    parseHelp();

    parseFiles();
    parseTrackerData();
    parseSceneFlowData();
    parseAngleDescriptor();
    parseAmplitudeDescriptor();
    parseCameraSelectorData();
}

void SF2TerminalParser::parseHelp() {
    if (parseMap.count("help")) {
        cout << endl;
        cout << "Version " << majorVersion << "." << minorVersion << endl;
        cout << description << endl;
        exit(EXIT_SUCCESS);
    }
}

void SF2TerminalParser::parseFiles() {
    if (parseMap.count("tracker-files")) {
        trackerFilenames = expandNames(parseMap["tracker-files"].as< vector<string> >());
        trackerData.trackerUsed = true;
    } else {
        trackerData.trackerUsed = false;
    }

    if (parseMap.count("time-files")) {
        timeFilenames = expandNames(parseMap["time-files"].as< vector<string> >());

    } else {
        throw InvalidInputException(__FILE__, __LINE__, "--time-files");
    }
    if (parseMap.count("intrinsic-files")) {
        intrinsicFilenames = expandNames(parseMap["intrinsic-files"].as< vector<string> >());

    } else {
        throw InvalidInputException(__FILE__, __LINE__, "--intrinsic-files");
    }
    if (parseMap.count("extrinsic-files")) {
        extrinsicFilenames = expandNames(parseMap["extrinsic-files"].as< vector<string> >());

    } else {
        throw InvalidInputException(__FILE__, __LINE__, "--extrinsic-files");
    }
    
    if (parseMap.count("depth-files")) {
        depthFilenames = expandNames(parseMap["depth-files"].as< vector<string> >());

    } else {
        throw InvalidInputException(__FILE__, __LINE__, "--depth-files");
    }
    
    if (parseMap.count("image-files")) {
        imageFilenames = expandNames(parseMap["image-files"].as< vector<string> >());

    } else {
        throw InvalidInputException(__FILE__, __LINE__, "--image-files");
    }

    if (parseMap.count("out-hist")) {
        outHistFilename = expandName(parseMap["out-hist"].as< string >());

    } else {
        throw InvalidInputException(__FILE__, __LINE__, "--out-hist");
    }
    
    if (parseMap.count("out-time")) {
        outTimeFilename = expandName(parseMap["out-time"].as< string >());

    } else {
        throw InvalidInputException(__FILE__, __LINE__, "--out-time");
    }
    if (parseMap.count("flo-file")) {
        floFilename = expandName(parseMap["flo-file"].as< string >());

        if (parseMap.count("flo-frame")) {
            floFrameCount = parseMap["flo-frame"].as< long >();

        } else {
            throw InvalidInputException(__FILE__, __LINE__, "--flo-frame");
        }
    }

#ifdef DEBUG
    std::ostream_iterator<string> oit(std::cout, "\t");
    cout << "video-files\t";
    std::copy(videoFilenames.begin(), videoFilenames.end(), oit);
    cout << endl;
    cout << "tracker-files\t";
    std::copy(trackerFilenames.begin(), trackerFilenames.end(), oit);
    cout << endl;
    cout << "time-files\t";
    std::copy(timeFilenames.begin(), timeFilenames.end(), oit);
    cout << endl;
    cout << "intrinsic-files\t";
    std::copy(intrinsicFilenames.begin(), intrinsicFilenames.end(), oit);
    cout << endl;
    cout << "extrinsic-files\t";
    std::copy(extrinsicFilenames.begin(), extrinsicFilenames.end(), oit);
    cout << endl;
    cout << "depth-files\t";
    std::copy(depthFilenames.begin(), depthFilenames.end(), oit);
    cout << endl;
    cout << "image-files\t";
    std::copy(imageFilenames.begin(), imageFilenames.end(), oit);
    cout << endl;
    cout << "out-hist\t" << outHistFilename << endl;
    cout << "out-time\t" << outTimeFilename << endl;
#endif
}

void SF2TerminalParser::parseTrackerData() {
    trackerData.trackerDownScale = parseMap["tracker-scale"].as<float>();
    if (trackerData.trackerDownScale > 0) {
        trackerData.trackerUpScale = (1.0 / trackerData.trackerDownScale);
    } else {
        trackerData.trackerUpScale = 1.0;
    }
    trackerData.playerID = parseMap["player-id"].as<int>();
    trackerData.displayTracker = parseMap["display-tracker"].as<bool>();
}

void SF2TerminalParser::parseSceneFlowData() {
    sceneFlowData.startFrame = parseMap["start-frame"].as<long>();
    startFrame = sceneFlowData.startFrame;
    sceneFlowData.displayFlow = parseMap["display-flow"].as<bool>();
    sceneFlowData.ctf = parseMap["ctf"].as<unsigned int>();
    sceneFlowData.rows = parseMap["rows"].as<unsigned int>();
    if (parseMap.count("sf-video")) {
        sceneFlowData.outVideo = expandName(parseMap["sf-video"].as<string>());
        sceneFlowData.needVideo = true;
    } else {
        sceneFlowData.needVideo = false;
    }
}

void SF2TerminalParser::parseAngleDescriptor() {

    angleDescriptorData.binCount = parseMap["hd-b"].as<int>();
    angleDescriptorData.minAmplitude = parseMap["hd-min"].as<float>();
    angleDescriptorData.scale = parseMap["ad-scale"].as<float>();
    angleDescriptorData.maxNorm = parseMap["hd-max-norm"].as<float>();
}

void SF2TerminalParser::parseAmplitudeDescriptor() {
    amplitudeDescriptorData.binCount = parseMap["ad-b"].as<int>();
    amplitudeDescriptorData.minAmplitude = parseMap["ad-min"].as<float>();
    amplitudeDescriptorData.scale = parseMap["ad-scale"].as<float>();
    amplitudeDescriptorData.maxNorm = parseMap["ad-max-norm"].as<float>();
}

void SF2TerminalParser::parseCameraSelectorData() {
    if (parseMap.count("selector-file")) {
        cameraSelectorFilename = expandName(parseMap["selector-file"].as< string >());
#ifdef DEBUG
        cout << "Camera selector file: " << cameraSelectorFilename << endl;
#endif
    } else {
        throw InvalidInputException(__FILE__, __LINE__, "--selector-file");
    }
}