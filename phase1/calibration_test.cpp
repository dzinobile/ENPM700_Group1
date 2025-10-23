#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>

void writeMatToCSV(const cv::Mat& cameramat, const cv::Mat& distmat, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening " << filename << std::endl;
        return;
    }

    file << "# Camera Matrix\n";
    for (int i = 0; i < cameramat.rows; ++i) {
        for (int j = 0; j < cameramat.cols; ++j) {
            file << cameramat.at<double>(i, j);
            if (j < cameramat.cols - 1)
                file << ",";
        }
        file << "\n";
    }

    file << "\n# Distortion Coefficients\n";
    for (int i = 0; i < distmat.rows; ++i) {
        for (int j = 0; j < distmat.cols; ++j) {
            file << distmat.at<double>(i, j);
            if (j < distmat.cols - 1)
                file << ",";
        }
        file << "\n";
    }

    file.close();
}



int main(){

    int checkerboard_samples = 150;
    int calibrate_samples = 30;
    cv::Size patternSize(6,8);


    cv::VideoCapture capture("media/checkerboard.MOV");

    if (!capture.isOpened()) {
        std::cerr << "Error: could not open video file" << std::endl;
        return -1;
    }

    cv::Mat frame;
    capture >> frame;
    if (frame.empty()) {
        std::cerr << "Error: could not read first frame" << std::endl;
        return -1;
    }

    int image_w = frame.cols;
    int image_h = frame.rows;
    
    
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);

    std::vector<cv::Point3f> objp;
    for (int i=0; i<patternSize.height;i++) {
        for (int j=0; j<patternSize.width;j++) {
            objp.push_back(cv::Point3f(j,i,0));
        }
    }
    
    std::vector<std::vector<cv::Point3f>> objpoints;
    std::vector<std::vector<cv::Point2f>> imgpoints;


    int frame_count = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_COUNT));
    int checkerboard_step = frame_count/checkerboard_samples;
    if (checkerboard_step < 1) {checkerboard_step = 1;}
    std::cout << "checkerboard step: " << checkerboard_step << std::endl;

    int i = 0;
    while (true) {
        capture >> frame;

        if (frame.empty()) {
            break;
        }

        if (i%checkerboard_step==0){ 
            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);


            std::vector<cv::Point2f> corners;
            bool found = cv::findChessboardCorners(gray, patternSize, corners, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
            if (found) {

                cv::cornerSubPix(gray, corners, cv::Size(5,5), cv::Size(-1,-1), criteria);
                objpoints.push_back(objp);
                imgpoints.push_back(corners);
            }
        }
        i++;

    }
    std::cout << "i: " << i << std::endl;


    
    if (objpoints.empty() || imgpoints.empty()) {
    std::cerr << "No corners were found — calibration aborted." << std::endl;
    return -1;
    }

    std::cout << "Detected " << objpoints.size() << " valid frames." << std::endl;

    if (objpoints.empty() || imgpoints.empty()) {
        std::cerr << "No corners were found — calibration aborted." << std::endl;
        return -1;
    }

    if (objpoints.size() != imgpoints.size()) {
        std::cerr << "Mismatch: objpoints=" << objpoints.size()
                << ", imgpoints=" << imgpoints.size() << std::endl;
        return -1;
    }

    for (size_t i = 0; i < objpoints.size(); ++i) {
        if (objpoints[i].size() != imgpoints[i].size()) {
            std::cerr << "Frame " << i << " has mismatched point counts ("
                    << objpoints[i].size() << " vs " << imgpoints[i].size() << ")" << std::endl;
            return -1;
        }
    }





    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;

    std::vector<std::vector<cv::Point3f>> objpoints_sampled;
    std::vector<std::vector<cv::Point2f>> imgpoints_sampled;
    int step = objpoints.size()/calibrate_samples;
    if (step < 1){step = 1;}
    for (int i=0; i<objpoints.size(); i+= step){
        objpoints_sampled.push_back(objpoints[i]);
        imgpoints_sampled.push_back(imgpoints[i]);

    }

    cv::calibrateCamera(objpoints_sampled, imgpoints_sampled, cv::Size(image_w, image_h),cameraMatrix, distCoeffs, rvecs, tvecs);
    

    std::cout << "Camera Matrix:\n" << cameraMatrix <<  std::endl;
    std::cout << "Distortion coefficients:\n"  << distCoeffs << std::endl;
    writeMatToCSV(cameraMatrix, distCoeffs, "media/cm_dc.csv");



    cv::VideoWriter undistorted("media/undistorted.mp4", cv::VideoWriter::fourcc('m','p','4','v'), 25.0, cv::Size(image_w, image_h));
    capture.set(cv::CAP_PROP_POS_FRAMES, 0);
    i = 0;
    while(true) {
        capture >> frame;
        if (frame.empty()) {break;}
        if (i%checkerboard_step==0) {

            cv::Mat newCameraMatrix = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, cv::Size(image_h,image_w),0);
            cv::Mat dst;
            cv::undistort(frame, dst, cameraMatrix, distCoeffs, newCameraMatrix);
            undistorted.write(dst);
        }
        i++;
    }

    capture.release();
    undistorted.release();

    return 0;
}