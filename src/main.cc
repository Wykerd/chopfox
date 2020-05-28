#include "simple.hpp"
#include <opencv2/imgcodecs.hpp>
#include <algorithm>

using namespace chopfox;

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

int main (int argc, char** argv) {
    char* input_file = getCmdOption(argv, argv+argc, "--input");

    if (!input_file) {
        printf("Error: No input file specified\n");
        return 1;
    }

    SimpleProcessor* proc = simple_processor_init("../frozen_east_text_detection.pb", 2);
    
    cv::Mat img = cv::imread(input_file, cv::IMREAD_COLOR);

    SimpleComicData data;

    simple_process_panels(proc, img, &data);

    simple_process_chop(proc, img, &data);

    simple_process_text(proc, img, &data);

    char* debug_file = getCmdOption(argv, argv+argc, "--debug_file");

    if (debug_file) {
        cv::Mat im_debug = img.clone();

        simple_draw_bounding_boxes(&data, im_debug);

        cv::imwrite(debug_file, im_debug);
    }

    img.release();

    char* xml_file = getCmdOption(argv, argv+argc, "--info_file");

    if (xml_file) {
        simple_xml_info(&data, true).SaveFile(std::string(xml_file));
    }

    char* output_format = getCmdOption(argv, argv+argc, "--chop_output");

    if (output_format) {
        for (int i = 0; i < data.frames.size(); i++) {
            size_t length = snprintf(NULL, 0, output_format, i);
            std::string filename(length + 1, '\0'); // fill string with 0
            sprintf(&filename[0], output_format, i);
            cv::imwrite(filename.c_str(), data.frames[i]);
        }
    }

    /* cleanup */
    simple_processor_free(proc);
    simple_data_free(data);

    return 0;
}