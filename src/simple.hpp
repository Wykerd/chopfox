#ifndef SIMPLE_H
#define SIMPLE_H

#include "text_detect.hpp"
#include "extract.hpp"

namespace chopfox {
    struct SimpleComicData {
        PanelArray panels;
        std::vector<cv::Mat> frames;
        std::vector<std::vector<struct TextBlock>> dialogue;
    };

    struct SimpleProcessor {
        cv::dnn::Net text_detector;
        const char* text_lang;
        float text_score_thresh;
        int image_ppi;
        uint8_t log_level;
    };

    struct SimpleProcessor* simple_processor_init (
        const char* east_model_path, 
        uint8_t log_level = 0,
        const char* lang = "eng",
        float text_score_thresh = 0.4f,
        int image_ppi = 300
    );

    struct SimpleProcessor* simple_processor_init (
        cv::dnn::Net text_detector, 
        uint8_t log_level = 0,
        const char* lang = "eng",
        float text_score_thresh = 0.4f,
        int image_ppi = 300
    );

    void simple_process_mat (
        struct SimpleProcessor* proc, 
        cv::Mat img,
        struct SimpleComicData* out
    );

    void simple_processor_free (struct SimpleProcessor* ptr);

    void simple_data_free (struct SimpleComicData data);


    void simple_draw_bounding_boxes (
        struct SimpleComicData* data,
        cv::Mat dst,
        cv::Scalar text_color = cv::Scalar(255,255,0),
        cv::Scalar panel_color = cv::Scalar(0,255,0),
        int text_thickness = 2,
        int panel_thickness = 2
    );

    void simple_xml_save (struct SimpleComicData* data, const char* filename);
}

#endif