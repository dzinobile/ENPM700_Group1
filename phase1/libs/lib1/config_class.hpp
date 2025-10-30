/**
 * @file config_class.hpp
 * @brief Header file for ConfigClass
 * @author Daniel Zinobile
 * @date 30-Oct-2025
 */
#pragma once
#include <string>

/**
 * @class ConfigClass
 * @brief Class stores filepaths for camera intrinsic and extrinsic parameters, and values for min and max detection distance, from user inputs
 */
class ConfigClass {

    public:
    /**
     * @brief Constructor for ConfigClass
     */
    ConfigClass();
    /**
     * @brief String for filepath of camera intrinsic parameters, either a .csv file for known paramters or a .mp4 or .MOV for calibrating from a video
     */
    std::string intrinsicsPath;
    /**
     * @brief String for filepath of camera extrinsic matrix, always a .csv file
     */
    std::string extrinsicsPath;
    /**
     * @brief Maximum detection distance
     */
    double D_max_m;
    /**
     * @brief Minimum safety distance for safety feature
     */
    double D_close_m;

    private:


};

