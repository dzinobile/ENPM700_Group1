#include "camera_model.hpp"
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
CameraModel::CameraModel(char* argv[]){
  filepath = argv[1];
  std::string filetype = filepath.substr(filepath.length()-3);
  if (filetype == "csv"){
    loadFromFile();
  }

  else if (filetype == "mp4" || filetype == "MOV"){
    calibrateFromFile();
  }

  else {
    std::cerr << "Error: invalid file format" << std::endl;
  }


}


void CameraModel::calibrateFromFile(){
  std::cout << "Calibrating from file" << std::endl;

  int checkerboard_samples = 150;
  int calibrate_samples = 30;
  cv::Size patternSize(6,8);

  cv::VideoCapture capture(filepath);

  if (!capture.isOpened()) {
      std::cerr << "Error: could not open video file" << std::endl;
      return;
  }

  cv::Mat frame;
  capture >> frame;
  if (frame.empty()) {
      std::cerr << "Error: could not read first frame" << std::endl;
      return;
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

  if (objpoints.empty() || imgpoints.empty()) {
  std::cerr << "No corners were found — calibration aborted." << std::endl;
  return;
  }



  if (objpoints.empty() || imgpoints.empty()) {
      std::cerr << "No corners were found — calibration aborted." << std::endl;
      return;
  }

  if (objpoints.size() != imgpoints.size()) {
      std::cerr << "Mismatch: objpoints=" << objpoints.size()
              << ", imgpoints=" << imgpoints.size() << std::endl;
      return;
  }

  for (size_t i = 0; i < objpoints.size(); ++i) {
      if (objpoints[i].size() != imgpoints[i].size()) {
          std::cerr << "Frame " << i << " has mismatched point counts ("
                  << objpoints[i].size() << " vs " << imgpoints[i].size() << ")" << std::endl;
          return;
      }
  }



  // cv::Mat cameraMatrix, distCoeffs;
  // std::vector<cv::Mat> rvecs, tvecs;

  std::vector<std::vector<cv::Point3f>> objpoints_sampled;
  std::vector<std::vector<cv::Point2f>> imgpoints_sampled;
  int step = objpoints.size()/calibrate_samples;
  if (step < 1){step = 1;}
  for (int i=0; i<objpoints.size(); i+= step){
      objpoints_sampled.push_back(objpoints[i]);
      imgpoints_sampled.push_back(imgpoints[i]);

  }

  cv::calibrateCamera(objpoints_sampled, imgpoints_sampled, cv::Size(image_w, image_h),K_mat, D_mat, rvecs, tvecs);
  K_mat.convertTo(K_mat, CV_32F);
  D_mat.convertTo(D_mat,CV_32F);

  return;

}


void CameraModel::loadFromFile(){
  std::cout << "loading from file" << std::endl;

  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "Error opening" << filepath << std::endl;
    return;
  }



  std::string line;

  std::vector<double> values;

  bool readingCamera = false;
  bool readingDist = false;

  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::stringstream ss(line);
    std::string cell;
    while (std::getline(ss,cell,',')) {
      values.push_back(std::stod(cell));
    }

  }


  std::vector<double> cmatrix_values;
  std::vector<double> dcoeff_values;

  for (int i=0; i<9; i++) {
    cmatrix_values.push_back(values[i]);

  }


  for (int i=9; i<14; i++ ){
    dcoeff_values.push_back(values[i]);
  }


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

cv::Mat CameraModel::undistort(cv::Mat img) {

  int image_w = img.cols;
  int image_h = img.rows;

  cv::Mat newCameraMatrix = cv::getOptimalNewCameraMatrix(K_mat, D_mat, cv::Size(image_h,image_w),0);
  cv::Mat dst;
  cv::undistort(img, dst, K_mat, D_mat, newCameraMatrix);

  return dst;

}









// #include "pid.hpp"

// #include <algorithm>
// #include <chrono>
// #include <memory>

// class PIDController {
//  public:
//   PIDController(double dt, // Constructor for PIDController
//                 double max, 
//                 double min, 
//                 double Kp, 
//                 double Kd,
//                 double Ki);
//   ~PIDController(); // Destructor for PIDController
//   [[nodiscard]] double compute(double setpoint, double pv); // Method to compute response

//  private:
//   std::chrono::duration<double> _dt; // Duration type, safer way to handle time units
//   double _max;
//   double _min;
//   double _Kp;
//   double _Kd;
//   double _Ki;
//   double _pre_error;
//   double _integral;
//   double _derivative;
// };

// PID::PID( double dt, // Constructor for front end PID class, "Pimpl pattern"
//           double max, 
//           double min, 
//           double Kp, 
//           double Kd, 
//           double Ki) {
//   pimpl = std::make_unique<PIDController>(dt, // Pointer to PIDController class
//                                           max, 
//                                           min, 
//                                           Kp, 
//                                           Kd, 
//                                           Ki); 
// }

// double PID::compute(double setpoint, double pv) { // Compute method, points to implementation class
//   return pimpl->compute(setpoint, pv);
// }

// PID::~PID() {} // Destructor for PID class


// // Loading values from PID class into PIDController class
// PIDController::PIDController(double dt, double max, double min, double Kp, 
//                              double Kd, double Ki)
//     :

//       _dt(dt),
//       _max(max),
//       _min(min),
//       _Kp(Kp),
//       _Kd(Kd),
//       _Ki(Ki),
//       _pre_error(0),
//       _integral(0),
//       _derivative(0)

// {}

// //Defining compute method
// double PIDController::compute(double setpoint, double pv) 
// { 
// 	// Calculate error 
// 	double error = setpoint - pv; 
// 	// Proportional term 
// 	double Pout = _Kp * error; 
// 	// Integral term 
// 	_integral += error * _dt.count(); 
// 	double Iout = _Ki * _integral; 
// 	// Derivative term 
// 	double derivative = (error - _pre_error) / _dt.count(); 
// 	double Dout = _Kd * derivative; 
// 	// Calculate total output 
// 	double output = Pout + Iout + Dout; 
// 	// Restrict to max/min 
// 	if( output > _max ) output = _max; 
// 	else if( output < _min ) output = _min; 
// 	// Save error to previous error 
// 	_pre_error = error; 
// 	return output;
// }

// PIDController::~PIDController() {} // Destructor for PIDController
