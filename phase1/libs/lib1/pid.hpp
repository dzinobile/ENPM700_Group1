/**
 * @file pid.hpp
 * @brief Header file for the PID class
 * @author Daniel Zinobile (Driver), Anvesh Som (Navigator)
 * @date 2025-10-08
 */
#pragma once
#include <algorithm>
#include <chrono>
#include <memory>

/**
 * @class PIDController
 * @brief Helper class owned internally by PID class
 */
class PIDController;

/**
 * @class PID
 * @brief Public interface class for PID control
 */
class PID {
 public:
  /**
   * @brief Constructor for PID class
   * @param dt Time step size [sec]
   * @param max Maximum correction allowed
   * @param min Minimum correction allowed
   * @param Kp Proportional gain
   * @param Kd Derivative gain
   * @param Ki Integral gain
   */

  PID(double dt, double max, double min, double Kp, double Kd, double Ki);

  /**
   * @brief Method for computing correction based on process variable
   * (Points to backend implementation in PIDController class)
   * @param setpoint Desired output value
   * @param pv Process variable, i.e. current state of system
   */
  [[nodiscard]] double compute(
      double setpoint,
      double pv);  // [[nodiscard]] Gives warning if return value is ignored

  /**
   * @brief Destructor for PID class
   */
  ~PID();

 private:
  /**
   * @brief Pointer to implementation
   */
  std::unique_ptr<PIDController> pimpl;
};
