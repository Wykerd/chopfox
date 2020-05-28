cmake_minimum_required(VERSION 3.10)

project(chopfox)

add_executable(chopfox-cli src/main.cc)

add_library(chopfox SHARED src/extract.cc src/text_detect.cc src/simple.cc)

include_directories(/usr/include/opencv4)

target_link_libraries(chopfox-cli PUBLIC chopfox opencv_imgcodecs)

target_link_libraries(chopfox PUBLIC opencv_core opencv_imgproc opencv_dnn tesseract tinyxml)