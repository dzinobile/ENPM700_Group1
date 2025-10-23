#include "camera_model.hpp"
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
CameraModel::CameraModel(char* argv[]){
  filepath = argv[1];

}

bool CameraModel::loadFromFile(){
  
  if (filepath.back() == 'v'){
    read_from_csv(filepath);
    return true;
  }
  else {
    return false;
  }
  
}



void CameraModel::read_from_csv(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error opening" << filename << std::endl;
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
