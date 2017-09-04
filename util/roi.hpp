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

#ifndef ROI_HPP
#define ROI_HPP

#include <opencv2/core/core.hpp>
#include <wordexp.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <cmath>

#include "exception.hpp"

using namespace cv;
using namespace std;

namespace gk {

    /**
     * Static utility class for selecting ROI bounding boxes, extracting ROI from
     * video frames and for correcting ROI if it lies outside frame margins.
     * 
     * @author Gregor Koporec
     * @since 17. 10. 2016
     * @version 1.0
     */
    class Roi {
    private:
        // Don't instantiante this class

        /**
         * Private constructor
         */
        Roi() {
        }

        static void findRegionItems(vector<string>& items, string filename, string videoName, int playerId);

        static Point2i findClosestValid(const Point2d& center, const Mat& depth, const Rect2d& roi, int iteration);

        static float calculateDistance(const Point2d& center, const Point2d& pixel);

        static Point2d getLocalCenter(const Rect2d& roi);

        static Point3d getXYZWorld(const Point3d& localPoint, const Mat& homography);
    public:
        /**
         * Temporary method for selecting bounding box of player with playerNumber
         * 
         * @param playerNumber Which player to select.
         * @return Bounding box of selected player.
         */
        template<typename T>
        static T selectRect(string filename, string videoName, int playerId);

        /**
         * Method for extracting region of interest from video frame. 
         * This method makes mask from ROI and generates new frame with the
         * help of this mask to extract ROI from frame and copy it to new output
         * frame.
         * 
         * @param frame Frame from which we want to extract ROI
         * @param roi Region of interest or bounding box.
         * @return Frame with extracted ROI.
         */
        template<typename T>
        static Mat extract(const Mat& frame, const T& roi);

        /**
         * Method for correcting ROI if its bounding box lies outside the frame.
         * 
         * @param roi ROI bounding box to correct
         * @param frame Frame where ROI lies.
         */
        template<typename T>
        static void correct(T& roi, const Mat& frame);

        /**
         * Method for cropping region of interest from video frame. 
         * 
         * @param frame Frame from which we want to crop ROI
         * @param roi Region of interest or bounding box.
         * @return Cropped frame, that has different size.
         */
        template<typename T>
        static Mat crop(const Mat& frame, const T& roi);

        template<typename T>
        static bool insideImage(const Mat& frame, const T& roi);

        static Point2i getClosestValid(const Point2d& suggestedCenter, const Mat& depth, const Rect2d& roi);

        static Point3d getMetricCenter(const Rect2d& roi, const Mat& depth, const Mat& homography);

        static bool isEmpty(const Rect2d& roi);
    };
}

#endif /* ROI_HPP */

