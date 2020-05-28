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
    PanelArray get_panels_rgb (cv::Mat img, double precision = 0.001);

    void sort_panels (PanelArray panels, int sorting_params);

    /// Cropping
    void free_mat_vector (std::vector<cv::Mat> arr);

    std::vector<cv::Mat> crop_frames (cv::Mat src, PanelArray panels);

    /// Drawing functions
    void draw_panel_bounds (PanelArray panels, cv::Mat dst, cv::Scalar color = cv::Scalar(255,255,0), int thickness = 2);

    cv::Mat draw_full_mask (PanelArray panels, cv::Size size);

    void draw_panel_countours (PanelArray panels, cv::Mat dst, cv::Scalar color = cv::Scalar(255,255,0), int thickness = 2);
}

#endif