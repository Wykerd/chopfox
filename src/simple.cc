#include "simple.hpp"
#include <stdio.h>
#include <tinyxml.h>

namespace chopfox {
    struct SimpleProcessor* simple_processor_init (
        const char* east_model_path, 
        uint8_t log_level,
        const char* lang,
        float text_score_thresh,
        int image_ppi 
    ) {
        return simple_processor_init(cv::dnn::readNet(east_model_path), log_level, lang, text_score_thresh, image_ppi);
    }

    struct SimpleProcessor* simple_processor_init (
        cv::dnn::Net text_detector, 
        uint8_t log_level,
        const char* lang,
        float text_score_thresh,
        int image_ppi
    ) {
        struct SimpleProcessor* ptr = new struct SimpleProcessor;

        ptr->text_detector = text_detector;
        ptr->text_lang = lang;
        ptr->text_score_thresh = text_score_thresh;
        ptr->image_ppi = image_ppi;
        ptr->log_level = log_level;

        return ptr;
    }

    void simple_process_mat (
        struct SimpleProcessor* proc, 
        cv::Mat img,
        struct SimpleComicData* out
    ) {
        out->panels = get_panels_rgb(img);

        if (proc->log_level >= 1) printf("[Chopfox] Found %d frames\n", out->panels.size());

        out->frames = crop_frames(img, out->panels);

        if (proc->log_level >= 1) printf("[Chopfox] Trascribing...\n");

        for (int i = 0; i < out->frames.size(); i++) {
            std::vector<struct TextBlock> text = transcribe(out->frames[i], proc->text_detector, proc->text_score_thresh, proc->text_lang, proc->image_ppi);
            if (proc->log_level >= 2) printf("[Chopfox] Found %d text regions in frame %d\n", text.size(), i);
            out->dialogue.push_back(text);
        }
    }

    void simple_processor_free (struct SimpleProcessor* ptr) {
        delete ptr;
    }

    void simple_data_free (struct SimpleComicData data) {
        free_mat_vector(data.frames);
    }

    void simple_draw_bounding_boxes (
        struct SimpleComicData* data,
        cv::Mat dst,
        cv::Scalar text_color,
        cv::Scalar panel_color,
        int text_thickness,
        int panel_thickness
    ) {
        for (int i = 0; i < data->panels.size(); i++) {
            std::vector<std::vector<cv::Point>> cnt;
            cnt.push_back(data->panels[i].contour);
            cv::drawContours(dst, cnt, -1, panel_color, panel_thickness);
            for (auto &text: data->dialogue[i]) {
                cv::Rect offset_rec(
                    data->panels[i].bounding_box.x + text.bounding_box.x, 
                    data->panels[i].bounding_box.y + text.bounding_box.y,
                    text.bounding_box.width,
                    text.bounding_box.height
                );
                cv::rectangle(dst, offset_rec, text_color, text_thickness);
            }
        }
    }

    void simple_xml_save (struct SimpleComicData* data, const char* filename) {
        TiXmlDocument doc;
        TiXmlElement root("comic-strip");
        for (int i = 0; i < data->panels.size(); i++) {
            TiXmlElement panel("panel");
            panel.SetAttribute("top", data->panels[i].bounding_box.x);
            panel.SetAttribute("left", data->panels[i].bounding_box.y);
            panel.SetAttribute("width", data->panels[i].bounding_box.width);
            panel.SetAttribute("height", data->panels[i].bounding_box.height);
            for (auto &text : data->dialogue[i]) {
                TiXmlElement dialogue("dialogue");
                dialogue.SetAttribute("top", text.bounding_box.x);
                dialogue.SetAttribute("left", text.bounding_box.y);
                dialogue.SetAttribute("width", text.bounding_box.width);
                dialogue.SetAttribute("height", text.bounding_box.height);
                TiXmlText dialogue_text(text.text.c_str());
                dialogue.InsertEndChild(dialogue_text);
                panel.InsertEndChild(dialogue);
            }
            root.InsertEndChild(panel);
        }
        doc.InsertEndChild(root);
        doc.SaveFile(filename);
    }
}