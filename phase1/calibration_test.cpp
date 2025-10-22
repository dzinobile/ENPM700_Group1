#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>



int main(){

    cv::VideoCapture capture("media/checkerboard_clipped.mp4");
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
    
    cv::Size patternSize(8,6);
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);

    std::vector<cv::Point3f> objp;
    for (int i=0; i<patternSize.height;i++) {
        for (int j=0; j<patternSize.width;j++) {
            objp.push_back(cv::Point3f(j,i,0));
        }
    }
    
    std::vector<std::vector<cv::Point3f>> objpoints;
    std::vector<std::vector<cv::Point2f>> imgpoints;

    cv::imshow("frame",frame);
    cv::waitKey(0);
    cv::destroyAllWindows();

    while (true) {
        capture >> frame;
        if (frame.empty()) {
            break;
        }

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



    
    if (objpoints.empty() || imgpoints.empty()) {
    std::cerr << "No corners were found — calibration aborted." << std::endl;
    return -1;
    }

    std::cout << imgpoints[0].size()<<" "<<imgpoints[1].size() << std::endl;
    std::cout << objpoints[0].size()<<" "<<objpoints[1].size() << " "<<std::endl;

    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;
    
    double rms = cv::calibrateCamera(objpoints, imgpoints, cv::Size(image_w, image_h),cameraMatrix, distCoeffs, rvecs, tvecs);
    // double rms = cv::calibrateCamera(test, imgpoints, cv::Size(3, 3),cameraMatrix, distCoeffs, rvecs, tvecs);

    std::cout << "Camera Matrix:\n" <<  std::endl;
    std::cout << "Distortion coefficients:\n"  << std::endl;



    return 0;
}