#include "camera_model.hpp"
#include "config_class.hpp"
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>

// CameraModel Constructor
CameraModel::CameraModel(){

  // If camera extrinsics filepath ends in .csv, run loadExFromFile function

  std::string Efiletype = extrinsicsPath.substr(extrinsicsPath.length()-3);
  if (Efiletype == "csv"){
    loadExFromFile();
  }

  // If other filetype, return error
  else {
    std::cerr << "Error: invalid file format for extrinsics" << std::endl;
  }

  // If camera intrinsics filepath ends in .csv, run loadFromFile function
  std::string Ifiletype = intrinsicsPath.substr(intrinsicsPath.length()-3);
  if (Ifiletype == "csv"){
    loadInFromFile();
  }

  // If camera intrinsics filepath ends in .mp4 or .MOV, run calibrateFromFile function
  else if (Ifiletype == "mp4" || Ifiletype == "MOV"){
    calibrateFromFile();
  }

  // If other filetype, return error
  else {
    std::cerr << "Error: invalid file format for intrinsics" << std::endl;
  }


}

void CameraModel::loadExFromFile(){
  std::cout << "loading extrinsics from file" << std::endl;

  // Read from file at ConfigClass intrinsic path
  std::ifstream file(extrinsicsPath);

  //Confirm file opens
  if (!file.is_open()) {
    std::cerr << "Error opening" << extrinsicsPath << std::endl;
    return;
  }

  std::string line; // String for each line in file

  std::vector<double> values; // Vector of values 


  // Read values and load into values vector
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::stringstream ss(line);
    std::string cell;
    while (std::getline(ss,cell,',')) {
      values.push_back(std::stod(cell));
    }

  }

  // Load values into E_mat
  int i=0;
  for(int r=0; r<3; r++){
    for(int c=0; c<4; c++) {
      E_mat.at<float>(r,c) = values[i];
      i++;
    }

  }

  return;


}


// Calibrate from video file function
void CameraModel::calibrateFromFile(){
  std::cout << "Calibrating from file" << std::endl;

  int checkerboard_samples = 150; // Max number of frames to take from provided video
  int calibrate_samples = 30; // Max number of frames with detectable checkerboard corners to use for calibration (too many makes it run slowly)
  cv::Size patternSize(6,8); // Checkerboard pattern size

  cv::VideoCapture capture(intrinsicsPath); // Load video from ConfigClass intrinsic class

  // Confirm file was successfully opened
  if (!capture.isOpened()) {
      std::cerr << "Error: could not open video file" << std::endl;
      return;
  }

  // Load first frame 
  cv::Mat frame;
  capture >> frame;

  // Confirm first frame loaded succesfully 
  if (frame.empty()) {
      std::cerr << "Error: could not read first frame" << std::endl;
      return;
  }

  // Initialize image height and width
  int image_w = frame.cols;
  int image_h = frame.rows;
  
  // Load calibration criteria
  cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);

  // Load objp vector with correct number of zeros
  std::vector<cv::Point3f> objp;
  for (int i=0; i<patternSize.height;i++) {
      for (int j=0; j<patternSize.width;j++) {
          objp.push_back(cv::Point3f(j,i,0));
      }
  }


  std::vector<std::vector<cv::Point3f>> objpoints; // 3D coordinates of the checkerboard corners for every frame
  std::vector<std::vector<cv::Point2f>> imgpoints; // 2D coordinates of checkerboard corners on image for every frame

  // Ensure a max of 150 images evenly spaced across video are chosen 
  int frame_count = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_COUNT));
  int checkerboard_step = frame_count/checkerboard_samples;
  if (checkerboard_step < 1) {checkerboard_step = 1;}

  // Find corners and load objpoints and imgpoints from video
  int i = 0;
  while (true) {
      capture >> frame;

      if (frame.empty()) {
          break;
      }

      if (i%checkerboard_step==0){ // Limiting number of frames taken for faster processing
          cv::Mat gray;
          cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);


          std::vector<cv::Point2f> corners;
          bool found = cv::findChessboardCorners(gray, patternSize, corners, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE); //Find checkerboard corners
          if (found) {

              cv::cornerSubPix(gray, corners, cv::Size(5,5), cv::Size(-1,-1), criteria); // Find more exact corner positions
              objpoints.push_back(objp);
              imgpoints.push_back(corners);
          }
      }
      i++;

    }
  
  // Confirm information was loaded into objpoints and imgpoints
  if (objpoints.empty() || imgpoints.empty()) {
  std::cerr << "No corners were found — calibration aborted." << std::endl;
  return;
  }
  
  // Confirm objpoints and imgpoints are the same size
  if (objpoints.size() != imgpoints.size()) {
      std::cerr << "Mismatch: objpoints=" << objpoints.size()
              << ", imgpoints=" << imgpoints.size() << std::endl;
      return;
  }

  // Confirm the size of each object in objpoints is the same as each object in imgpoints
  for (size_t i = 0; i < objpoints.size(); ++i) {
      if (objpoints[i].size() != imgpoints[i].size()) {
          std::cerr << "Frame " << i << " has mismatched point counts ("
                  << objpoints[i].size() << " vs " << imgpoints[i].size() << ")" << std::endl;
          return;
      }
  }


  // Rotation and translation vectors for each frame
  std::vector<cv::Mat> rvecs, tvecs;

  // Sample 30 of the valid frames evenly spaced across the whole video
  // More than 30 frames runs slowly and with diminishing returns
  std::vector<std::vector<cv::Point3f>> objpoints_sampled;
  std::vector<std::vector<cv::Point2f>> imgpoints_sampled;
  int step = objpoints.size()/calibrate_samples;
  if (step < 1){step = 1;}
  for (int i=0; i<objpoints.size(); i+= step){
      objpoints_sampled.push_back(objpoints[i]);
      imgpoints_sampled.push_back(imgpoints[i]);

  }

  // Calibrate camera per sampled objpoints and imgpoints
  // Load camera intrinsic values into K_mat and D_mat
  cv::calibrateCamera(objpoints_sampled, imgpoints_sampled, cv::Size(image_w, image_h),K_mat, D_mat, rvecs, tvecs);

  // Convert K_mat and D_mat to floats instead of double
  K_mat.convertTo(K_mat, CV_32F);
  D_mat.convertTo(D_mat,CV_32F);

  return;

}

// Function to load K_mat and D_mat with information from file
void CameraModel::loadInFromFile(){
  std::cout << "loading intrinsics from file" << std::endl;

  // Read from file at ConfigClass intrinsic path
  std::ifstream file(intrinsicsPath);

  //Confirm file opens
  if (!file.is_open()) {
    std::cerr << "Error opening" << intrinsicsPath << std::endl;
    return;
  }

  std::string line; // String for each line in file

  std::vector<double> values; // Vector of values 

  // Bool for whether reading camera matrix info or distortion coefficients
  bool readingCamera = false;
  bool readingDist = false;

  // Read values and load into values vector
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::stringstream ss(line);
    std::string cell;
    while (std::getline(ss,cell,',')) {
      values.push_back(std::stod(cell));
    }

  }

  //Split values vector into one for camera matrix and one for distortion coefficients
  std::vector<double> cmatrix_values;
  std::vector<double> dcoeff_values;

  for (int i=0; i<9; i++) {
    cmatrix_values.push_back(values[i]);

  }

  for (int i=9; i<14; i++ ){
    dcoeff_values.push_back(values[i]);
  }

  // Load values into K_mat and D_mat
  int i=0;
  for(int r=0; r<3; r++){
    for(int c=0; c<3; c++) {
      K_mat.at<float>(r,c) = cmatrix_values[i];
      i++;
    }

  }

  for (int i = 0; i<5; i++) {
    D_mat.at<float>(0,i) = dcoeff_values[i];
  }
  return;
  
}

// Undistort a provided image per K_mat and D_mat
cv::Mat CameraModel::undistort(cv::Mat img) {

  int image_w = img.cols;
  int image_h = img.rows;

  cv::Mat newCameraMatrix = cv::getOptimalNewCameraMatrix(K_mat, D_mat, cv::Size(image_h,image_w),0); // Calculate optimal camera matrix
  cv::Mat dst;
  cv::undistort(img, dst, K_mat, D_mat, newCameraMatrix); // Undistort image

  return dst;

}







