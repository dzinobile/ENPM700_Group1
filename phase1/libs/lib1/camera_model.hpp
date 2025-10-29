
#pragma once
#include "config_class.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>


class CameraModel : public ConfigClass {

  public:
    CameraModel();
    cv::Mat K_mat = cv::Mat::zeros(3,3,CV_32F);
    cv::Mat D_mat = cv::Mat::zeros(1,5,CV_32F);
    cv::Mat I_mat = cv::Mat::zeros(3,4,CV_32F);

    void loadFromFile();
    void calibrateFromFile();
    cv::Mat undistort(cv::Mat img);


  private:



};




