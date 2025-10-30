#pragma once
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

// using namespace cv;
// using namespace dnn;
// using namespace std;
// cv::dnn::Net net
class YoloClass {

    public:
        YoloClass();
        cv::Mat highlight_people(cv::Mat frame);
        std::string modelConfiguration;
        std::string modelWeights;
        std::string classesFile;
        std::vector<std::string> classes;
        cv::dnn::Net net;
        std::string videoPath;
        std::vector<std::string> outNames;


    private:

};