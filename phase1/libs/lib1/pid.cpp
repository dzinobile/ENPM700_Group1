#include "pid.hpp"

#include <algorithm>
#include <chrono>
#include <memory>

class PIDController {
 public:
  PIDController(double dt, // Constructor for PIDController
                double max, 
                double min, 
                double Kp, 
                double Kd,
                double Ki);
  ~PIDController(); // Destructor for PIDController
  [[nodiscard]] double compute(double setpoint, double pv); // Method to compute response

 private:
  std::chrono::duration<double> _dt; // Duration type, safer way to handle time units
  double _max;
  double _min;
  double _Kp;
  double _Kd;
  double _Ki;
  double _pre_error;
  double _integral;
  double _derivative;
};

PID::PID( double dt, // Constructor for front end PID class, "Pimpl pattern"
          double max, 
          double min, 
          double Kp, 
          double Kd, 
          double Ki) {
  pimpl = std::make_unique<PIDController>(dt, // Pointer to PIDController class
                                          max, 
                                          min, 
                                          Kp, 
                                          Kd, 
                                          Ki); 
}

double PID::compute(double setpoint, double pv) { // Compute method, points to implementation class
  return pimpl->compute(setpoint, pv);
}

PID::~PID() {} // Destructor for PID class


// Loading values from PID class into PIDController class
PIDController::PIDController(double dt, double max, double min, double Kp, 
                             double Kd, double Ki)
    :

      _dt(dt),
      _max(max),
      _min(min),
      _Kp(Kp),
      _Kd(Kd),
      _Ki(Ki),
      _pre_error(0),
      _integral(0),
      _derivative(0)

{}

//Defining compute method
double PIDController::compute(double setpoint, double pv) 
{ 
	// Calculate error 
	double error = setpoint - pv; 
	// Proportional term 
	double Pout = _Kp * error; 
	// Integral term 
	_integral += error * _dt.count(); 
	double Iout = _Ki * _integral; 
	// Derivative term 
	double derivative = (error - _pre_error) / _dt.count(); 
	double Dout = _Kd * derivative; 
	// Calculate total output 
	double output = Pout + Iout + Dout; 
	// Restrict to max/min 
	if( output > _max ) output = _max; 
	else if( output < _min ) output = _min; 
	// Save error to previous error 
	_pre_error = error; 
	return output;
}

PIDController::~PIDController() {} // Destructor for PIDController
