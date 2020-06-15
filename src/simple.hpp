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

#ifndef SIMPLE_H
#define SIMPLE_H

#include "text_detect.hpp"
#include "extract.hpp"
#include <tinyxml.h>

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
        double panel_precision;
        double panel_min_area_divider;
    };

    /**
     * Create a new SimpleProcessor structure.
     * @param east_model_path Path to the EAST model .pb file
     * @param log_level Value between 0-3 for logging to stdout
     * @param lang The language to use for tesseract-ocr
     * @param text_score_thresh Threshold for minimum convidence of text detected
     * @param panel_precision Used in contour appoximation
     * @param image_ppi Used for tesseract-ocr
     * @param panel_min_area_divider Min area of panel calculates as min_area = (strip.width / panel_min_area_divider) * (strip.height / panel_min_area_divider)
     * @returns The SimpleProcessor containing the EAST model and processing parameters
     */
    struct SimpleProcessor* simple_processor_init (
        const char* east_model_path, 
        uint8_t log_level = 0,
        const char* lang = "eng",
        float text_score_thresh = 0.4f,
        double panel_precision = 0.001,
        int image_ppi = 300,
        double panel_min_area_divider = 15.0
    );

    /**
     * Create a new SimpleProcessor structure.
     * @param text_detector The EAST text detection network to use
     * @param log_level Value between 0-3 for logging to stdout
     * @param lang The language to use for tesseract-ocr
     * @param text_score_thresh Threshold for minimum convidence of text detected
     * @param panel_precision Used in contour appoximation
     * @param image_ppi Used for tesseract-ocr
     * @param panel_min_area_divider Min area of panel calculates as min_area = (strip.width / panel_min_area_divider) * (strip.height / panel_min_area_divider)
     * @returns The SimpleProcessor containing the EAST model and processing parameters
     */
    struct SimpleProcessor* simple_processor_init (
        cv::dnn::Net text_detector, 
        uint8_t log_level = 0,
        const char* lang = "eng",
        float text_score_thresh = 0.4f,
        double panel_precision = 0.001,
        int image_ppi = 300,
        double panel_min_area_divider = 15.0
    );

    /**
     * Create a new SimpleProcessor structure.
     * @param log_level Value between 0-3 for logging to stdout
     * @param panel_precision Used in contour appoximation
     * @param panel_min_area_divider Min area of panel calculates as min_area = (strip.width / panel_min_area_divider) * (strip.height / panel_min_area_divider)
     * @returns The SimpleProcessor
     */
    struct SimpleProcessor* simple_processor_init_notext (
        uint8_t log_level = 0,
        double panel_precision = 0.001,
        double panel_min_area_divider = 15.0
    );

    /**
     * Get the panel regions from imput image
     * @param proc The processor struct to use containing the options
     * @param img The input image
     * @param out The resulting data
     */
    void simple_process_panels (
        struct SimpleProcessor* proc, 
        cv::Mat img,
        struct SimpleComicData* out
    );

    /**
     * Chop up the images after getting the panels with simple_process_panels
     * @param proc The processor struct to use containing the options
     * @param img The input image to chop up
     * @param out The resulting data
     */ 
    void simple_process_chop (
        struct SimpleProcessor* proc, 
        cv::Mat img,
        struct SimpleComicData* out
    );

    /**
     * Extract the text from the chopped up panels
     * @param proc The processor struct to use containing the options
     * @param out The resulting data
     */
    void simple_process_text (
        struct SimpleProcessor* proc, 
        struct SimpleComicData* out
    );

    /**
     * Free the SimpleProcessor
     * @param ptr Pointer to the struct to free
     */
    void simple_processor_free (struct SimpleProcessor* ptr);

    /**
     * Free the chopped up panels
     * @param data The output data structure
     */
    void simple_data_free (struct SimpleComicData data);

    /**
     * Draw the bounding boxes generated by simple_process_chop & simple_process_text
     * @param data The data containing the bonding boxes & contours
     * @param dst The destination image to draw to
     * @param text_color Color of the text boxes
     * @param panel_color Color of the panel contours
     * @param text_thickness Thickness of the text boxes
     * @param panel_thickness Thickness of the panel contours
     */
    void simple_draw_bounding_boxes (
        struct SimpleComicData* data,
        cv::Mat dst,
        cv::Scalar text_color = cv::Scalar(255,255,0),
        cv::Scalar panel_color = cv::Scalar(0,255,0),
        int text_thickness = 2,
        int panel_thickness = 2
    );

    /**
     * Generate XML representation of the data
     * @param include_text Include the text processed in the XML output
     * @param include_contour Include the contour points of the panels in the XML output
     * @returns The resulting TinyXML document
     */
    TiXmlDocument simple_xml_info (struct SimpleComicData* data, bool include_text, bool include_contour = true);
}

#endif