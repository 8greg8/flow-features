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

#include "roi.hpp"

using namespace gk;

void Roi::findRegionItems(vector<string>& items, string filename, string videoFilename, int playerId){
    wordexp_t videoFilenameExpansion, filenameExpansion, itemExpansion;
    
    wordexp(videoFilename.c_str(), &videoFilenameExpansion, 0);
    string videoPath = videoFilenameExpansion.we_wordv[0];
    wordfree(&videoFilenameExpansion);

    string line, item;
    
    wordexp(filename.c_str(), &filenameExpansion, 0);
    ifstream file(filenameExpansion.we_wordv[0]);
    wordfree(&filenameExpansion);
    
    istringstream lineStream;
    bool found = false;
    if(file.is_open()){
        while( getline(file, line) ){
            
            // split line by ,
            lineStream.str(line);
            while( getline(lineStream, item, ',')){
                items.push_back(item);
                
                // Get first two items which are video path and player ID
                if(items.size() == 2){
                    string path = items.front();
                    int id = stoi(items.at(1));

                    // If names are not the same, clear items and break
                    wordexp(path.c_str(), &itemExpansion, 0);
                    string itemPath = itemExpansion.we_wordv[0];
                    wordfree(&itemExpansion);
                   
                    if( itemPath.find(videoPath) != string::npos && id == playerId){
                        found = true;
                        
                    } else{
                        found = false;
                        items.clear();
                        break;
                    }
                    
                }
            }
            if(found){
                break;
            }
        }
        file.close();
    }
}

template<typename T>
T Roi::selectRect(string filename, string videoName, int playerId){    
    vector<string> items;
    findRegionItems(items, filename, videoName, playerId);
    
    /*cout << "Vector size: " << items.size() << endl;
    for(string item : items){
        cout << "Item: " << item << endl;
    }*/
    
    // Check if template type is correct.
    const std::type_info& templateTypeId = typeid(T);
    
    if(items.size() == 6){
        if(templateTypeId == typeid(Rect2d)){
                double xd, yd, widthd, heightd;

                xd = atof(items.at(2).c_str());
                yd = atof(items.at(3).c_str());
                widthd = atof(items.at(4).c_str());
                heightd = atof(items.at(5).c_str());
                return T(xd, yd, widthd, heightd);
                
        }else if(templateTypeId == typeid(Rect)){
                int x, y, width, height;

                x = atoi(items.at(2).c_str());
                y = atoi(items.at(3).c_str());
                width = atoi(items.at(4).c_str());
                height = atoi(items.at(5).c_str());
                return T(x, y, width, height);
                
        }else {
                cerr << "Invalid type in Roi::selectRect<T>" << endl;
                exit(1);
        }
    } else{
        return T();
    }
}

template Rect2d Roi::selectRect<Rect2d>(string filename, string videoName, int playerId);
template Rect Roi::selectRect<Rect>(string filename, string videoName, int playerId);

template<typename T>
Mat Roi::extract(const Mat& frame, const T& roi){
    // Make black image same size as frame
    Mat mask = Mat::zeros(frame.size(), CV_8UC1);
    // Make white polygon on black image, that represents ROI
    if(roi.area() > 0){
        //cout << "x: " << roi.x << "\ty: " << roi.y << "\twidth: " << roi.width << "\theight: " << roi.height << endl;
        mask(roi).setTo(Scalar::all(255));
    }
    Mat extractedROI;
    frame.copyTo(extractedROI, mask);
    return extractedROI;
    //return frame;
}

template Mat Roi::extract<Rect2d>(const Mat& frame, const Rect2d& roi);
template Mat Roi::extract<Rect>(const Mat& frame, const Rect& roi);

template<typename T>
void Roi::correct(T& roi, const Mat& frame){
    const T currentRoi = roi;
    //cout << "frame.cols " << frame.cols << endl;

    // Correct left edge - check left side
    if(currentRoi.x < 0){
        roi.x = 0;
        roi.width += currentRoi.x;        
        if(roi.width < 0){
            roi.width = 0;
        }
    } 
    
    double maxX = frame.cols - 1;
    double currentMaxX = roi.x + roi.width - 1;
    if(currentMaxX > maxX){
        roi.width -= currentMaxX - maxX;
        if(roi.width < 0){
            roi.width = 0;
        }
    }
    
    // Correct upper edge - check upper side
    if(currentRoi.y < 0){
        roi.y = 0;
        roi.height += currentRoi.y;
        if(roi.height < 0){
            roi.height = 0;
        }
    } 

    double maxY = frame.rows - 1;
    double currentMaxY = roi.y + roi.height - 1;
    if(currentMaxY > maxY){
        roi.height -= currentMaxY - maxY;
        if(roi.height < 0){
            roi.height = 0;
        }
    }
}

template void Roi::correct<Rect2d>(Rect2d& roi, const Mat& frame);
template void Roi::correct<Rect>(Rect& roi, const Mat& frame);

template<typename T>
Mat Roi::crop(const Mat& frame, const T& roi){
    
    Mat extractedROI = frame(roi);
    //frame.copyTo(extractedROI, mask);
    return extractedROI;
    //return frame;
}

template Mat Roi::crop<Rect2d>(const Mat& frame, const Rect2d& roi);
template Mat Roi::crop<Rect>(const Mat& frame, const Rect& roi);

template<typename T>
bool Roi::insideImage(const Mat& frame, const T& roi){
    T imageRect(0, 0, frame.cols, frame.rows);
    T intersection = roi & imageRect;
    if(intersection.area() == roi.area() && roi.area() > 0){
        return true;
    }
    return false;
}

template bool Roi::insideImage<Rect2d>(const Mat& frame, const Rect2d& roi);
template bool Roi::insideImage<Rect>(const Mat& frame, const Rect& roi);

Point2i Roi::getClosestValid(const Point2d& suggestedCenter, const Mat& depth, const Rect2d& roi){
    return findClosestValid(suggestedCenter, depth, roi, 0);
}

Point2i Roi::findClosestValid(const Point2d& center, const Mat& depth, const Rect2d& roi, int iteration){
    unsigned int x0 = (unsigned int) center.x;
    unsigned int y0 = (unsigned int) center.y;
    Point2i startPoint(x0 - iteration, y0 - iteration);
    Point2i stopPoint(x0 + iteration, y0 + iteration);
    
    if(!roi.contains(startPoint) || !roi.contains(stopPoint) ){
        return Point2i(-1, -1);
    }
    
    bool skipRow = false;
    bool skipCol = false;
    const float* zRow = NULL;
    float distance;
    float minDistance = roi.width > roi.height ? roi.width : roi.height;
    int minRow = -1;
    int minCol = -1;
    
    for(int r = startPoint.y; r <= stopPoint.y; r++){
        zRow = depth.ptr<float>(r);
        if(iteration != 0 && r > startPoint.y && r < stopPoint.y){
            skipRow = true;
        }else{
            skipRow = false;
        }
        
        for(int c = startPoint.x; c <= stopPoint.x; c++){
            if(iteration != 0 && c > startPoint.x && c < stopPoint.x && skipRow){
                skipCol = true;
            } else{
                skipCol = false;
            }
            
            if(skipCol && skipRow){
                continue;
            } else{
                if(zRow[c] > 1e-7){
                    distance = Roi::calculateDistance(center, Point2d(c, r));
                    
                    if(distance < minDistance){
                        minDistance = distance;
                        minCol = c;
                        minRow = r;
                    }
                }
            }
        }  
    }
    
    if( minRow != -1 && minCol != -1){
        return Point2i(minCol, minRow);
    } else{
        return findClosestValid(center, depth, roi, iteration + 1);
    }
}

float Roi::calculateDistance(const Point2d& center, const Point2d& pixel){
    float xx = (center.x - pixel.x)*(center.x - pixel.x);
    float yy = (center.y - pixel.y)*(center.y - pixel.y);
    return sqrt(xx + yy);
}

Point2d Roi::getLocalCenter(const Rect2d& roi){
    return Point2d(roi.x + roi.width/2.0, roi.y + roi.height/2.0);
}

Point3d Roi::getMetricCenter(const Rect2d& roi, const Mat& depth, const Mat& homography) {
    
    Point2d localCenter = getLocalCenter(roi);
    
    Point2i validPixel;
    if(Roi::isEmpty(roi)) {
        validPixel = Point2i(0, 0);
    } else{
        validPixel = getClosestValid(localCenter, depth, roi);
    }
    
    
    Rect2d imageRect(0, 0, depth.cols, depth.rows);
    if(!validPixel.inside(imageRect)){
        /*stringstream ss;
        ss << "No valid pixel for metric center." << endl;
        ss << "Calculated valid pixel: (" << validPixel.x << ", " << validPixel.y << ")" << endl;
        ss << "Local center: (" << localCenter.x << ", " << localCenter.y << ")" << endl;
        ss << "Image rect: (" << imageRect.x << ", " << imageRect.y << ", " << imageRect.width << ", " << imageRect.height << ")" << endl;
        ss << "Roi: (" << roi.x << ", " << roi.y << ", " << roi.width << ", " << roi.height << ")" << endl;
        string message = ss.str();
        throw gk::Exception(__FILE__, __LINE__, message);*/
        validPixel = Point2i(0,0);
    }
    Point3d localPoint(validPixel.x, validPixel.y, depth.at<float>(validPixel.y, validPixel.x));
            
    return Roi::getXYZWorld(localPoint, homography);
}

Point3d Roi::getXYZWorld(const Point3d& localPoint, const Mat& homography) {
    // In millimeters
    Mat hInv = homography.inv();
    Mat u = (Mat_<float>(4,1) << localPoint.x * localPoint.z, localPoint.y * localPoint.z, localPoint.z, 1);
    Mat world = hInv * u;
    Point3d worldPoint(world.at<float>(0), world.at<float>(1), world.at<float>(2));
    return worldPoint;        
}

bool Roi::isEmpty(const Rect2d& roi){
    if (roi.area() <= 1e-15) {
        return true;
    }
    return false;
}