/**
 * @file camera_model.hpp
 * @brief Header file for CameraModel class
 * @author Daniel Zinobile 
 * @date 30-Oct-2025
 */
#pragma once
#include "config_class.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

/**
 * @class CameraModel
 * @brief Class stores intrinsic camera parameters, either loaded from csv file or calculated from video, and extrinsic parameters loaded from csv file. Class inherits intrinsic and extrinsic filepaths from ConfigClass.
 */
class CameraModel : public ConfigClass {

  public:
  /**
   * @brief Constructor for CameraModel class
   */
    CameraModel();
    /**
     * @brief Intrinsic camera matrix, 3x3 OpenCV matrix of floats
     */
    cv::Mat K_mat = cv::Mat::zeros(3,3,CV_32F);
    /**
     * @brief Distortion coefficients, 1x5 OpenCV matrix of floats
     */
    cv::Mat D_mat = cv::Mat::zeros(1,5,CV_32F);
    /**
     * @brief Extrinsic camera matrix, 3x4 OpenCV matrix of floats
     */
    cv::Mat E_mat = cv::Mat::zeros(3,4,CV_32F);

    /**
     * @brief Function to load intrinsic camera matrix and distortion coefficients from csv file
     */
    void loadInFromFile();

    /**
     * @brief Function to load extrinsic camera matrix from csv file
     */
    void loadExFromFile();

    /**
     * @brief Function to calculate intrinsic camera matrix and distortion coefficients from .mp4 or .MOV video file
     */
    void calibrateFromFile();

    /**
     * @brief Function to undistort a provided image using the intrinsic camera matrix and distortion coefficients
     * @param img A provided image
     */
    cv::Mat undistort(cv::Mat img);


  private:



};




