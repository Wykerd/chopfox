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

#include "extract.hpp"
#include <assert.h>

namespace chopfox {
    PanelArray get_panels_rgb (cv::Mat img, double precision) {
        assert(!img.empty());

        cv::Mat gray, lapl, thresh, dilated;
        
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

        cv::Laplacian(gray, lapl, CV_8U);

        cv::threshold(lapl, thresh, 50, 255, cv::THRESH_BINARY);

        // Close some small imperfections in the countours
        cv::Mat dilation_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
        cv::dilate(thresh, dilated, dilation_kernel);

        dilation_kernel.release();

        std::vector<std::vector<cv::Point>> contours;

        PanelArray retVal;

        cv::findContours(dilated, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (auto &contour : contours) {
            std::vector<cv::Point> approx;
            double epsilon = precision * cv::arcLength(contour, true);
            cv::approxPolyDP(contour, approx, epsilon, true);
            double area = cv::contourArea(contour);
            double min_area = (img.size().height / 15.0) * (img.size().width / 15.0);
            if (area > min_area) {
                struct PanelInfo info;
                info.contour = approx;
                info.bounding_box = cv::boundingRect(approx);
                retVal.push_back(info);
            }
        }

        dilated.release();
        thresh.release();
        lapl.release();
        gray.release();

        return retVal;
    }

    void sort_panels (PanelArray panels, int sorting_params) {
        for (auto &panel : panels) {
            
        }
    }

    void draw_panel_countours (PanelArray panels, cv::Mat dst, cv::Scalar color, int thickness) {
        assert(!dst.empty());

        std::vector<std::vector<cv::Point>> contours;
        for (auto &panel : panels) {
            contours.push_back(panel.contour);
        }
        cv::drawContours(dst, contours, -1, color, thickness);
    }

    cv::Mat draw_full_mask (PanelArray panels, cv::Size size) {
        cv::Mat mask(size, CV_8UC1, cv::Scalar(0));

        std::vector<std::vector<cv::Point>> contours;
        for (auto &panel : panels) {
            contours.push_back(panel.contour);
        }
        cv::fillPoly(mask, contours, 255);

        return mask;
    }

    void free_mat_vector (std::vector<cv::Mat> arr) {
        for (auto &i : arr) {
            i.release();
        }
    }

    std::vector<cv::Mat> crop_frames (cv::Mat src, PanelArray panels) {
        assert(!src.empty());

        std::vector<cv::Mat> retVal;
        for (auto &panel : panels) {
            // Create the Mats we're gonna use
            cv::Mat crop = src(panel.bounding_box);
            cv::Mat mask(crop.size(), CV_8UC1, cv::Scalar(0));
            cv::Mat isolated;

            // Create the cropping mask
            std::vector<std::vector<cv::Point>> contours = { panel.contour };
            cv::fillPoly(mask, contours, 255, 8, 0, cv::Point(-panel.bounding_box.x, -panel.bounding_box.y));
            
            // Remove the dilation added by the extraction process
            // NOTE: This isn't working well on straight edges... Look for better solution.
            cv::Mat eroded;
            cv::Mat erode_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
            cv::erode(mask, eroded, erode_kernel);
            erode_kernel.release();

            // Crop the image
            crop.copyTo(isolated, eroded);

            // Release the data
            crop.release();
            mask.release();
            eroded.release();

            // Add to return vector
            retVal.push_back(isolated);
        }
        return retVal;
    }

    void draw_panel_bounds (PanelArray panels, cv::Mat dst, cv::Scalar color, int thickness) {
        assert(!dst.empty());

        for (auto &panel : panels) {
            cv::rectangle(dst, panel.bounding_box, color, thickness);
        }
    }
}