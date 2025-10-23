#include <gtest/gtest.h>

#include <vector>

#include "camera_model.hpp"

// CameraModel class

char* argv[] = {
    const_cast<char*>("program_name"),
    const_cast<char*>("cm_dc.csv"),
};


CameraModel cm1(argv);
TEST(camera_model_test, loads_From_file) {
  EXPECT_EQ(cm1.loadFromFile(),true);

}







// // Compute function
// PID pidtest1(0.1, 100, -100, 0.1, 0.01, 0.5);
// TEST(compute_function, this_will_fail) {
//   EXPECT_EQ(pidtest1.compute(0, 20), -5);
// }

// // Min bound
// PID pidtest2(0.1, 1, -1, 0.1, 0.01, 0.5);
// TEST(min_bounds, this_will_fail) { EXPECT_EQ(pidtest2.compute(0, 200), -1); }
// // Max bound
// TEST(max_bounds, this_will_fail) { EXPECT_EQ(pidtest2.compute(0, -200), 1); }

// // Output should be zero if all gains are zero
// PID pidtest3(0.1, 100, -100, 0, 0, 0);
// TEST(gains_zero, this_will_fail) { EXPECT_EQ(pidtest3.compute(0, 20), 0); }

// // Value actually approaches desired value over time
// TEST(value_approaches_desired, this_will_fail) {
//   double val = 20;
//   for (int i = 0; i < 100; i++) {
//     double inc = pidtest1.compute(0, val);
//     val += inc;
//   }
//   EXPECT_NEAR(val, 0, 0.3);
// }

// // Rate of approach decreases as it gets closer to target
// TEST(approach_rate_decreases, this_will_fail) {
//   std::vector<double> values;
//   double val = 20;
//   for (int i = 0; i < 100; i++) {
//     double inc = pidtest1.compute(0, val);
//     val += inc;
//     if (i == 0) values.emplace_back(val);
//     if (i == 5) values.emplace_back(val);
//     if (i == 94) values.emplace_back(val);
//     if (i == 99) values.emplace_back(val);
//   }
//   EXPECT_GT(abs(values[0] - values[1]), abs(values[2] - values[3]));
// }
