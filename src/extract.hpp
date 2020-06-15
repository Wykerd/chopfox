/**
 *  This file is part of Chopfox.
 *
 *  Chopfox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Chopfox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with Chopfox.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef EXTRACT_H
#define EXTRACT_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

namespace chopfox {
    struct PanelInfo {
        std::vector<cv::Point> contour;
        cv::Rect bounding_box;
    };

    enum SortingParams {
        TOP_DOWN,
        LEFT_RIGHT,
        RIGHT_LEFT,
        BOTTOM_UP
    };

    #define PanelArray std::vector<struct PanelInfo>

    /// Extraction

    /**
     * Extract the panel ROIs from the input image
     * @param img The input image to process
     * @param precision The accuracy of the approximated contour (lower is more precise)
     * @param panel_min_area_divider Min area of panel calculates as min_area = (strip.width / panel_min_area_divider) * (strip.height / panel_min_area_divider)
     * @returns An vector containing the regoins of interest and contour for each panel
     */
    PanelArray get_panels_rgb (cv::Mat img, double precision = 0.001, double min_area_divider = 15.0);

    /**
     * Sort the extracted panels into the order they appear
     */
    void sort_panels (PanelArray panels, int sorting_params);

    /// Cropping

    /**
     * Free all the Mat objects in a vector
     * @param arr The vector to free.
     */
    void free_mat_vector (std::vector<cv::Mat> arr);

    /**
     * Chop the image into panels using array of PanelInfo, usually generated from get_panels_rgb
     * @param src The source image
     * @param panels The vector containing the regoins of interest and contour for each panel
     * @returns A vector of the chopped Mats
     * @remarks Free the returned vector with free_mat_vector
     */
    std::vector<cv::Mat> crop_frames (cv::Mat src, PanelArray panels);

    /// Drawing functions

    /**
     * Draw the panel bouding boxes
     * @param panels The panels to draw
     * @param dst The opencv Mat to draw to
     * @param color The color of the box
     * @param thickness The thickness of the lines to draw
     */
    void draw_panel_bounds (PanelArray panels, cv::Mat dst, cv::Scalar color = cv::Scalar(255,255,0), int thickness = 2);
}

#endif