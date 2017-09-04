/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ofterminalparser.hpp"

using namespace gk;

int OFTerminalParser::flowFrameCount = 2;

const string OFTerminalParser::FLO_TYPE = ".flo";
const string OFTerminalParser::DEFAULT_FLO_FORMAT = "-%04d";

gk::OFTerminalParser::OFTerminalParser(int argc, const char *const argv[], Rect2d& roi) : 
parser(argc, argv, KEYS){
    tracker = NULL;
    
    // Check for frame number to start
    if(parser.has("f")){
        frameNumber = parser.get<int>("f");
    }
    initFlo();
    ofConfig = std::make_shared<OpticalFlowData>();
    ofConfig->flowType = static_cast<OpticalFlowType>(parser.get<int>("s"));
    
    // Optical flow farneback parameters
    if(ofConfig->flowType == FARNEBACK){
        ofConfig->pyramidScale = parser.get<double>("ps");
        ofConfig->pyramidLayers = parser.get<int>("pl");
        ofConfig->windowSize = parser.get<int>("ws");
        ofConfig->iterationsCount = parser.get<int>("it");
        ofConfig->neighbourSize = parser.get<int>("ns");
        ofConfig->gaussianDeviation = parser.get<double>("gd");
        ofConfig->operationFlags = parser.get<int>("operationFlags");
    }
    
    // Other inputs
    inputVideoname = parseMainInput(parser.get<string>("v"));
    histogramFilename = parseMainInput(parser.get<string>("csv"));
    
    // If user wants image files
    needImage = parser.has("i");
    if(needImage){
        parseImageFilenames();
    }
    
    // For optical flow video
    ofConfig->needVideo = parser.has("o");
    if(ofConfig->needVideo){
        ofConfig->outVideo = parseOpticalFlowVideoname(parser.get<string>("o"));
    }
    
    // If predator is enabled
    trackerEnabled = parser.get<bool>("tEnabled");
    if(trackerEnabled || parser.has("trackerFile")){
        initTracker(roi);
    }
    
    // If user wants help
    if (parser.has("help")){
        help();
        exit(EXIT_SUCCESS);
    }
    
    displayTracker = parser.get<bool>("displayTracker");
    ofConfig->displayFlow = parser.get<bool>("displayFlow");
    
    // init descriptors
    initAmplitudeDescriptor();
    initAngleDescriptor();
    initAmplitudeFactor();
    
}

// Parse input string
const String OFTerminalParser::KEYS =
"{help h usage ?    |        | Print this message             }"
"{hdb               | 60     | Number of bins for histogram descriptor  }"
"{hdMaxNorm         | 0.0    | For determinig max norm range for histogram descriptor. If 0, norm will not be used.}"
"{f                 | 150    | Frame number to capture as image}"
"{s                 | 0      | Optical flow algorithm         }"
"{v                 | <none> | Input video for optical flow   }"
"{csv               | <none> | CSV file for appending features}"
"{i                 |        | File name for video image and optical flow image.}"
"{flo               | 2      | .flo Middlebury filename or sequence of files.}"
"{floFrame          |        | Starting frame for .flo file or sequence of files.}"
"{o                 |        | Output optical flow video      }"
"{t                 | KCF    | Enabling tracker with specific type }"
"{roi               |        | File with player bounding boxes}"
"{n                 | 1      | Player ID to track             }"
"{tEnabled          | false  | Disable or enable tracker      }"
"{select            |        | Select ROI on every N seconds. }"
"{displayTracker    | false  | If video with tracker is shown.}"
"{displayFlow       | false  | If video with optical flow is shown.}"
"{ps pyramidScale   | 0.5    | Pyramid scale.}"
"{pl pyramidLayers  | 3      | Pyramid layers.}"
"{ws windowSize     | 15     | Averaging window size.}"
"{it iter iterations| 3      | Number of iterations at each pyramid layer.}"
"{ns neighbourSize  | 5      | Size of the pixel neighbourhood.}"
"{gd gaussianDeviation sigma | 1.2    | Standard deviation of the gaussian.}"
"{operationFlags    | 0      | Operation flags.}"
"{ts trackerScale   |        | Factor for scaling video when using tracker.}"
"{adb ampDescBin    |        | Number of bins for amplitude descriptor. }"
"{adMin             | 0      | Amplitudes below min amplitudes are noise  }"
"{adScale           | 1.0    | for large displacements set this < 1 to prevent clipping, for now should be 1.0}"
"{adMaxNorm         | 0.0    | For determining max norm range. If 0 norm will not be used.}"
"{trackerFile       |        | File name of a csv with tracked regions of interest.}"
"{diag              |        | Diagonal for amplitude factor. }"

;//End parse key

void OFTerminalParser::help(){
    cout <<
    "\nThis program calculates HOOF features with dense optical flow "
    "algorithm by Gunnar Farneback\n"
    "Call:\n"
    "./opticalFlowFeatures "
    "{help | h | usage | ?} "
    "{-[parameter name]=[parameter value]} "
    "\n"
    "Parameters:\n"
    "\t-hdb: default 60\n"
    "\t-f: optional\n"
    "\t-s:\n"
        "\t\t0 - Farneback algorithm (default)\n"
        "\t\t1 - Lucas-Kanade with pyramids\n"
    "\t-v: must have\n"
    "\t-csv: must have\n"
    "\t-i: optional\n"
    "\t-flo: optional\n"
    "\t-floFrame: optional\n"
    "\t-o: optional\n"
    "\t-c: optional\n"
        "\t\tfalse\n"
        "\t\ttrue (default)\n"
    "\t-t: optional\n"
        "\t\tTLD\n"
        "\t\tMIL\n"
        "\t\tBOOSTING\n"
        "\t\tMEDIANFLOW\n"
        "\t\tKCF\n"
    "\t-roi: optional\n"
    "\t-n: must have for roi. Defalut 1\n"
    "\t-tEnabled: default false\n"
    "\t-select: optional\n"
    "\t-displayTracker: optional\n"
    "\t-displayFlow: optional\n"
    "\tps | pyramidScale: optional\n"
    "\tpl | pyramidLayers: optional\n"
    "\tws | windowSize: optional\n"
    "\tit | iter | iterations: optional\n"
    "\tns | neighbourSize: optional\n"
    "\tgd | gaussianDeviation | sigma: optional\n"
    "\toperationFlags: optional\n"
    "\tts | trackerScale: optional\n"
    "\tadb | ampDescBin: optional\n"
    "\tadMin: optional\n" 
    "\tadScale: optional\n"
    "\ttrackerFile: optional\n"
    "\tdiag: optional\n"
            << endl;
}

void OFTerminalParser::initFlo(){
    // Get basename and possible sequencing.
    if(parser.has("flo")){
        floNeeded = true;
        flo = std::make_shared<Flo>(
                    expandName(parser.get<string>("flo")),
                    parser.get<int>("floFrame")
                );
    }
}


bool OFTerminalParser::isFloNeeded(){
    return floNeeded;
}

void OFTerminalParser::initTracker(Rect2d& roi){
    trackerEnabled = parser.get<bool>("tEnabled");
    if(trackerEnabled || parser.has("trackerFile")){
        trackerEnabled = true;

    } else{
        return;
    }
    
    cout << "=======================" << endl;
    cout << "Initializing tracker..." << endl;
    
    // Check for tracker scale
    if(parser.has("ts")){
        trackerScaling = true;
        trackerScale = parser.get<double>("ts");
    } else{
        trackerScaling = false;
    }
    
    if(parser.has("trackerFile")){
        string trackerFilename = expandName(parser.get<string>("trackerFile"));
        cout << "Getting tracker from file: " << trackerFilename << endl;
        trackerFile = std::make_shared<OF2TrackerFile>(trackerFilename, 1);            
        cout << "=======================" << endl;   
        
    }else{
        // Create tracker
        string trackerType = parser.get<string>("t");
        tracker = std::make_shared<gk::Tracker>(trackerType);

        // Get ROI from file
        if(parser.has("roi")){
            string roiFilename = parser.get<string>("roi");
            int playerId = parser.get<int>("n");

            // Get ROI before any file was opened
            roi = Roi::selectRect<Rect2d>(roiFilename, inputVideoname, playerId);

            // If ROI was not defined in file show user a new window
            // to select new ROI
            if(roi.area() <= 0){
                cout << "Region of interest was not defined in file." << endl;
                cout << "Selecting region of interest on a frame." << endl;
                selectRoiOnFirstFrame = true;

            } else{
                // Downscale ROI if tracker scaling is used.
                if(trackerScaling){
                    Scaler::scaleRoi(roi, roi, getTrackerDownScale());
                }
                selectRoiOnFirstFrame = false;
                cout << "=======================" << endl;
            }
        } else{
            cout << "File with player bounding boxes was not specified" << endl;
            cout << "Selecting region of interest on a frame." << endl;
            selectRoiOnFirstFrame = true;
        }

        // Check for selection
        if(parser.has("select")){
            selectionSeconds = parser.get<double>("select");
            if(selectionSeconds > 0){
                selectionEnabled = true;
            }
        }
    }
}

string OFTerminalParser::parseOpticalFlowVideoname(string input){
    string output = expandName(input);
    
    //string::size_type videoLastPoint = input.find_last_of('.');
    //return input.substr(0, videoLastPoint) + ".mp4";
    return output;
}

void OFTerminalParser::parseImageFilenames(){
    string imageFilename = expandName(parser.get<string>("i"));
        
    string::size_type imageLastPoint = imageFilename.find_last_of('.');
        
    originalImageFilename = imageFilename.substr(0, imageLastPoint) + 
            "-original-frame-" + to_string(frameNumber) + ".png";

    opticalFlowImageFilename = imageFilename.substr(0, imageLastPoint) + 
            "-opticalFlow-frame-" + to_string(frameNumber) + ".png";
}

string OFTerminalParser::expandName(string input){
    if(input == ""){
        return input;
    }
    
    wordexp_t nameExpansion;
    wordexp(input.c_str(), &nameExpansion, 0);
    
    string expandedName = nameExpansion.we_wordv[0];
    wordfree(&nameExpansion);
    
    return expandedName;
}

string OFTerminalParser::parseMainInput(string input){
    string expandedName = expandName(input);
    
    if(expandedName.empty()){
        help();
        exit(EXIT_FAILURE);
    } else{
        return expandedName;
    }
}

string OFTerminalParser::getInputVideoname(){
    return inputVideoname;
}

string OFTerminalParser::getHistogramFilename(){
    return histogramFilename;
}

std::shared_ptr<gk::Tracker> OFTerminalParser::getTracker(){
    return tracker;
}

double OFTerminalParser::getSelectionSeconds(){
    return selectionSeconds;
}

bool OFTerminalParser::isSelectionEnabled(){
    return selectionEnabled;
}

bool OFTerminalParser::needSelectRoiOnFirstFrame(){
    return selectRoiOnFirstFrame;
}

bool OFTerminalParser::isTrackerEnabled(){
    return trackerEnabled;
}

bool OFTerminalParser::isImageNeeded(){
    return needImage;
}

int OFTerminalParser::getFrameNumber(){
    return frameNumber;
}

string OFTerminalParser::getOriginalImageFilename(){
    return originalImageFilename;
}

bool OFTerminalParser::needDisplayTracker(){
    return displayTracker;
}

bool OFTerminalParser::isTrackerScalingEnabled(){
    return trackerScaling;
}

double OFTerminalParser::getTrackerDownScale(){
    return trackerScale;
}

double OFTerminalParser::getTrackerUpScale(){
    return (1.0/trackerScale);
}

std::shared_ptr<OpticalFlowData> OFTerminalParser::getOpticalFlowConfig(){
    return ofConfig;
}

std::shared_ptr<Flo> OFTerminalParser::getFlo(){
    return this->flo;
}

void OFTerminalParser::initAmplitudeDescriptor(){
    if(parser.has("adb")){
        amplitudeDescriptor = std::make_shared<AmplitudeDescriptor>(
                    parser.get<int>("adb"),
                    parser.get<float>("adMin"),
                    parser.get<float>("adScale"),
                    parser.get<float>("adMaxNorm")
                );
    } else{
        amplitudeDescriptor = NULL;
    }
    
}

void OFTerminalParser::initAngleDescriptor(){
    int hdb = parser.get<int>("hdb");
    if(hdb > 0){
        histogramDescriptor = std::make_shared<AngleDescriptor>(
                hdb,
                parser.get<float>("hdMaxNorm")
                );
    } else{
        histogramDescriptor = NULL;
    }
    
}

bool OFTerminalParser::isAmplitudeDescriptorUsed(){
    if(amplitudeDescriptor){
        return true;
    } else{
        return false;
    }
}

bool OFTerminalParser::isAngleDescriptorUsed() {
    if (histogramDescriptor) {
        return true;
    } else {
        return false;
    }
}

std::shared_ptr<AmplitudeDescriptor> OFTerminalParser::getAmplitudeDescriptor(){
    return this->amplitudeDescriptor;
}

std::shared_ptr<AngleDescriptor> OFTerminalParser::getAngleDescriptor(){
    return this->histogramDescriptor;
}

bool OFTerminalParser::trackerFromFile(){
    if(trackerFile){
        return true;
        
    }else{
        return false;
    }
}

std::shared_ptr<OF2TrackerFile> OFTerminalParser::getOF2TrackerFile(){
    return this->trackerFile;
}

void OFTerminalParser::initAmplitudeFactor(){
    if(parser.has("diag")){
        amplitudeFactor = std::make_shared<AmplitudeFactor>( 
                parser.get<float>("diag")
                );
    }
}

std::shared_ptr<AmplitudeFactor> OFTerminalParser::getAmplitudeFactor(){
    return this->amplitudeFactor;
}