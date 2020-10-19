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

#include "text_detect.hpp"
#include <opencv2/imgproc.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

namespace chopfox {
    // From OpenCV example: https://github.com/opencv/opencv/blob/master/samples/dnn/text_detection.cpp
    void decodeBoundingBoxes(const cv::Mat& scores, const cv::Mat& geometry, float scoreThresh,
                            std::vector<cv::RotatedRect>& detections, std::vector<float>& confidences)
    {
        detections.clear();
        CV_Assert(scores.dims == 4); CV_Assert(geometry.dims == 4); CV_Assert(scores.size[0] == 1);
        CV_Assert(geometry.size[0] == 1); CV_Assert(scores.size[1] == 1); CV_Assert(geometry.size[1] == 5);
        CV_Assert(scores.size[2] == geometry.size[2]); CV_Assert(scores.size[3] == geometry.size[3]);

        const int height = scores.size[2];
        const int width = scores.size[3];
        for (int y = 0; y < height; ++y)
        {
            const float* scoresData = scores.ptr<float>(0, 0, y);
            const float* x0_data = geometry.ptr<float>(0, 0, y);
            const float* x1_data = geometry.ptr<float>(0, 1, y);
            const float* x2_data = geometry.ptr<float>(0, 2, y);
            const float* x3_data = geometry.ptr<float>(0, 3, y);
            const float* anglesData = geometry.ptr<float>(0, 4, y);
            for (int x = 0; x < width; ++x)
            {
                float score = scoresData[x];
                if (score < scoreThresh)
                    continue;

                // Decode a prediction.
                // Multiple by 4 because feature maps are 4 time less than input image.
                float offsetX = x * 4.0f, offsetY = y * 4.0f;
                float angle = anglesData[x];
                float cosA = std::cos(angle);
                float sinA = std::sin(angle);
                float h = x0_data[x] + x2_data[x];
                float w = x1_data[x] + x3_data[x];

                cv::Point2f offset(offsetX + cosA * x1_data[x] + sinA * x2_data[x],
                            offsetY - sinA * x1_data[x] + cosA * x2_data[x]);
                cv::Point2f p1 = cv::Point2f(-sinA * h, -cosA * h) + offset;
                cv::Point2f p3 = cv::Point2f(-cosA * w, sinA * w) + offset;
                cv::RotatedRect r(0.5f * (p1 + p3), cv::Size2f(w, h), -angle * 180.0f / (float)CV_PI);
                detections.push_back(r);
                confidences.push_back(score);
            }
        }
    }

    std::vector<struct TextBlock> transcribe (cv::Mat frame, cv::dnn::Net detector, float score_thresh, const char* lang, int ppi) {
        cv::Mat blob, color;

        bool has_alpha = frame.channels() == 4;
    
        if (has_alpha) {
            std::vector<cv::Mat> src_channels(4);
            cv::split(frame, src_channels);
            std::vector<cv::Mat> rgb(src_channels.begin(), src_channels.end() - 1);
            cv::merge(rgb, color);
        } else color = frame;

        int blob_w = color.size().width / 32;
        if (blob_w <= 0) blob_w = 1;
        int blob_h = color.size().height / 32;
        if (blob_h <= 0) blob_h = 1;

        cv::dnn::blobFromImage(color, blob, 1.0, cv::Size(blob_w * 32,blob_h * 32), cv::Scalar(123.68, 116.78, 103.94), true, false);
        detector.setInput(blob);
        std::vector<cv::Mat> outs;
        std::vector<cv::String> outNames(2);
        outNames[0] = "feature_fusion/Conv_7/Sigmoid";
        outNames[1] = "feature_fusion/concat_3";
        detector.forward(outs, outNames);

        blob.release(); // free the blob data from memory

        cv::Mat scores = outs[0];
        cv::Mat geometry = outs[1];

        // Decode predicted bounding boxes.
        std::vector<cv::RotatedRect> boxes;
        std::vector<float> confidences;
        decodeBoundingBoxes(scores, geometry, score_thresh, boxes, confidences);

        // Apply non-maximum suppression procedure.
        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, score_thresh, 0.8, indices);

        cv::Point2f ratio((float)color.cols / (blob_w * 32), (float)color.rows / (blob_h * 32));
        
        cv::Mat text_mask(color.size(), CV_8UC1, cv::Scalar(0));

        // Generate mask of text
        for (size_t i = 0; i < indices.size(); ++i)
        {
            cv::RotatedRect& box = boxes[indices[i]];

            cv::Point2f vertices2f[4];
            box.points(vertices2f);

            // rescale 
            for (int j = 0; j < 4; ++j)
            {
                vertices2f[j].x *= ratio.x;
                vertices2f[j].y *= ratio.y;
            }

            cv::Point vertices[4];    
            for(int i = 0; i < 4; ++i){
                vertices[i] = vertices2f[i];
            }

            cv::fillConvexPoly(text_mask, vertices,4, cv::Scalar(255));
        }

        // Get text regions from the mask
        cv::Mat text_regions;
        cv::Mat close_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(blob_w,blob_w)); // blob_w is nice size for the kernel ?
        cv::morphologyEx(text_mask, text_regions, cv::MORPH_CLOSE, close_kernel);

        close_kernel.release();
        text_mask.release();
        
        std::vector<std::vector<cv::Point>> contours;

        cv::findContours(text_regions, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        text_regions.release();

        std::vector<struct TextBlock> text_blocks;

        tesseract::TessBaseAPI ocr;

        ocr.Init(NULL, lang, tesseract::OEM_LSTM_ONLY /* Use the deep learning engine instead of the legacy one */); 
        ocr.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK); // assume single block of text

        for (auto &contour : contours) {
            struct TextBlock block;
            
            block.bounding_box = cv::boundingRect(contour);

            cv::Mat txt_im = color(block.bounding_box);

            ocr.SetImage(txt_im.data, txt_im.cols, txt_im.rows, txt_im.channels(), txt_im.step); // Load the image
            ocr.SetSourceResolution(ppi); // Set the ppi
            
            block.text = ocr.GetUTF8Text(); // get the text

            text_blocks.push_back(block);

            txt_im.release();
        }

        return text_blocks;
    }
}