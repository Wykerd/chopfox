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

#ifndef TEXT_DETECT_H
#define TEXT_DETECT_H

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>

namespace chopfox {
    struct TextBlock {
        cv::Rect bounding_box;
        char* text;
    };

    /**
     * Transcibe chopped up panel
     * @param frame The panel to transcribe
     * @param detector The EAST CNN to use for text ROI detection
     * @param score_thresh The minimum score for text regions found
     * @param lang The language to use for tesseract-ocr text recognition
     * @param ppi The frame ppi (used for tesseract)
     * @returns Structure containing the identified text strings and regions
     */
    std::vector<struct TextBlock> transcribe (
        cv::Mat frame, 
        cv::dnn::Net detector, 
        float score_thresh = 0.4f, 
        const char* lang = "eng", 
        int ppi = 300
    );
}

#endif