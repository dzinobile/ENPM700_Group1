#pragma once
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>


class YoloClass {

    public:
        YoloClass();
        std::vector<cv::Rect> highlight_people(cv::Mat frame);
        std::string modelConfiguration;
        std::string modelWeights;
        std::string classesFile;
        std::vector<std::string> classes;
        cv::dnn::Net net;
        std::string videoPath;
        std::vector<std::string> outNames;


    private:

};