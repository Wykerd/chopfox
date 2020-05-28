#include "extract.hpp"

namespace chopfox {
    PanelArray get_panels_rgb (cv::Mat img, double precision) {
        cv::Mat gray, lapl, thresh;
        
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

        cv::Laplacian(gray, lapl, CV_8U);

        cv::threshold(lapl, thresh, 50, 255, cv::THRESH_BINARY);

        std::vector<std::vector<cv::Point>> contours;

        PanelArray retVal;

        cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (auto &contour : contours) {
            std::vector<cv::Point> approx;
            double epsilon = precision * cv::arcLength(contour, true);
            cv::approxPolyDP(contour, approx, epsilon, true);
            double area = cv::contourArea(contour);
            double min_area = (img.size().height / 20.0) * (img.size().width / 20.0);
            if (area > min_area) {
                struct PanelInfo info;
                info.contour = approx;
                info.bounding_box = cv::boundingRect(approx);
                retVal.push_back(info);
            }
        }

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
        std::vector<cv::Mat> retVal;
        for (auto &panel : panels) {
            cv::Mat crop = src(panel.bounding_box);
            cv::Mat mask(crop.size(), CV_8UC1, cv::Scalar(0));
            cv::Mat isolated;
            std::vector<std::vector<cv::Point>> contours = { panel.contour };
            cv::fillPoly(mask, contours, 255, 8, 0, cv::Point(-panel.bounding_box.x, -panel.bounding_box.y));
            crop.copyTo(isolated, mask);
            crop.release();
            mask.release();
            retVal.push_back(isolated);
        }
        return retVal;
    }

    void draw_panel_bounds (PanelArray panels, cv::Mat dst, cv::Scalar color, int thickness) {
        for (auto &panel : panels) {
            cv::rectangle(dst, panel.bounding_box, color, thickness);
        }
    }
}