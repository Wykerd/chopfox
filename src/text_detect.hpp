#ifndef TEXT_DETECT_H
#define TEXT_DETECT_H

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>

namespace chopfox {
    struct TextBlock {
        cv::Rect bounding_box;
        char* text;
    };

    std::vector<struct TextBlock> transcribe (
        cv::Mat frame, 
        cv::dnn::Net detector, 
        float score_thresh = 0.4f, 
        const char* lang = "eng", 
        int ppi = 300
    );
}

#endif