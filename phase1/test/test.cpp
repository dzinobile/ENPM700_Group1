#include <gtest/gtest.h>

#include <vector>

#include <opencv2/opencv.hpp>
#include <chrono>
#include "human_detector.hpp"
// CameraModel class

// TEST(human_detector, human_detector_test) {
//   HumanDetector hd("window");
//   hd.bindWindow();
//   cv::VideoCapture cap("test_media/test_pos.mp4");
//   while (true) {
//     cv::Mat frame;
//     cap >> frame;
//     hd.setFrame(frame);
//     hd.redraw();
//     int key = cv::waitKey(0);
//     if (key == 'r'){
//       hd.reset();
//     }
//   }
// }
  

// CameraModel class testing ---------------------------------------------------
// Testing load from csv file functionality
TEST(camera_model_test, loads_From_csv_file) {

  // Fake user input to ConfigClass with valid csv file
  std::istringstream fakeInput("test_media/test_intrinsic.csv\ntest_media/test_extrinsic.csv\n10\n1");
  std::streambuf* origCin = std::cin.rdbuf();
  std::cin.rdbuf(fakeInput.rdbuf());

  // Create CameraModel object
  CameraModel cm;

  // Pass fake user input to ConfigClass
  std::cin.rdbuf(origCin);

  // Correct values in csv file, for comparison
  std::vector<float> correct_kmat_values = {1148.941079262392,0,968.6603818232142,
                                      0,1157.532465485139,533.1086821168354,
                                      0,0,1};
  std::vector<float> correct_dmat_values = {
                                      -0.4518047618131186, 0.3228995925163289, 
                                      -0.001753905126417962, -0.0003906063544475221, 
                                      -0.1601718014427244};
  
  // Compare loaded K_mat values with correct values
  int i = 0;
  for (int r=0; r<3; r++) {
    for (int c=0; c<3; c++) {
      EXPECT_FLOAT_EQ(correct_kmat_values[i],cm.K_mat.at<float>(r,c));
      i++;
    }
  }

  // Compare loaded D_mat values with correct values
  for (int c=0; c<5; c++) {
    EXPECT_FLOAT_EQ(correct_dmat_values[c], cm.D_mat.at<float>(0,c));
  }
}

// Testing calibrate from file functionality
TEST(camera_model_test, calibrates_from_file) {

  // Fake user input with valid calibration file
  std::istringstream fakeInput("test_media/test_cal.mp4\ntest_media/test_extrinsic.csv\n10\n1");
  std::streambuf* origCin = std::cin.rdbuf();
  std::cin.rdbuf(fakeInput.rdbuf());

  // Create CameraModel object
  CameraModel cm;

  // Pass fake input to ConfigClass
  std::cin.rdbuf(origCin);
  
  // Correct values that should result from known video 
  std::vector<float> correct_kmat_values = {1154.8749,0,963.95282,
                                      0,1164.9955,530.64508,
                                      0,0,1};
  std::vector<float> correct_dmat_values = {
                                      -0.45195264, 0.31424072, -0.00096839585, 0.00036902804, -0.13790174};
  
  // Compare loaded K_mat values with correct values
  int i = 0;
  for (int r=0; r<3; r++) {
    for (int c=0; c<3; c++) {
      EXPECT_FLOAT_EQ(correct_kmat_values[i],cm.K_mat.at<float>(r,c));
      i++;
    }
  }

  // Compare loaded D_mat values with correct values
  for (int c=0; c<5; c++) {
    EXPECT_FLOAT_EQ(correct_dmat_values[c], cm.D_mat.at<float>(0,c));
  }
}

// Testing load extrinsics from file functionality
TEST(camera_model_test, loads_extrinsics_from_file){

  // Fake user input for ConfigClass with valid extrinsic filepath
  std::istringstream fakeInput("test_media/test_intrinsic.csv\ntest_media/test_extrinsic_2.csv\n10\n1");
  std::streambuf* origCin = std::cin.rdbuf();
  std::cin.rdbuf(fakeInput.rdbuf());

  // Create CameraModel Object
  CameraModel cm;

  // Pass fake user input
  std::cin.rdbuf(origCin);

  // Correct values from csv file
  std::vector<float> correct_emat_values = {13,83,10,0,
                                      59,2,5,32,
                                      37,76,47,22};

  // Compare loaded E_mat values with correct values
  int i = 0;
  for (int r = 0; r < 3; r++){
    for (int c = 0; c< 4; c++){
      EXPECT_FLOAT_EQ(correct_emat_values[i], cm.E_mat.at<float>(r,c));
      i++;
    }
  }


}

// Test undistort image function for functionality
TEST(camera_model_test, undistorts_image) {
  
  // Fake user input for ConfigClass with valid inputs 
  std::istringstream fakeInput("test_media/test_intrinsic.csv\ntest_media/test_extrinsic.csv\n10\n1");
  std::streambuf* origCin = std::cin.rdbuf();
  std::cin.rdbuf(fakeInput.rdbuf());

  // Create CameraModel object
  CameraModel cm;

  // Pass fake user input
  std::cin.rdbuf(origCin);

  // Load test known test frame with distortion
  cv::Mat test_frame = cv::imread("test_media/test_frame.jpg");

  // Time how long to undistort image
  auto start = std::chrono::high_resolution_clock::now();
  cv::Mat undistorted_frame = cm.undistort(test_frame);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  // Frame should not be empty
  EXPECT_FALSE(undistorted_frame.empty());

  // Undistorted frame size should be the same as original frame size
  EXPECT_EQ(undistorted_frame.size(),test_frame.size());

  // Undistorted frame type should be the same as original frame type
  EXPECT_EQ(undistorted_frame.type(),test_frame.type());

  // Duration should be less than 83 ms for 12 fps
  EXPECT_LT(duration.count(),83);



}

// Test camera model rejects invalid intrinsics file type
TEST(camera_model_test, rejects_non_csv_file_intrinsics) {

  // Fake user input for ConfigClass with invalid intrinsic input
  std::istringstream fakeInput("test_media/test_intrinsic.jpg\ntest_media/test_extrinsic.csv\n10\n1");
  std::streambuf* origCin = std::cin.rdbuf();
  std::cin.rdbuf(fakeInput.rdbuf());

  // Set up cerr capture buffer
  std::streambuf* originalCerrBuffer = std::cerr.rdbuf();
  std::stringstream capturedCerr;
  std::cerr.rdbuf(capturedCerr.rdbuf());

  // Create CameraModel object
  CameraModel cm;

  // Pass fake user inputs
  std::cin.rdbuf(origCin);

  // Capture cerr output
  std::cerr.rdbuf(originalCerrBuffer);

  // Confirm correct error outputs
  EXPECT_EQ(capturedCerr.str(), "Error: invalid file format for intrinsics\n");

}

// Test CameraModel rejects invalid extrinsic input
TEST(camera_model_test, rejects_non_csv_file_extrinsics) {

  // Fake user input for ConfigClass with invalid extrinsic input
  std::istringstream fakeInput("test_media/test_intrinsic.csv\ntest_media/test_extrinsic.jpg\n10\n1");
  std::streambuf* origCin = std::cin.rdbuf();
  std::cin.rdbuf(fakeInput.rdbuf());

  // Set up cerr capture buffer
  std::streambuf* originalCerrBuffer = std::cerr.rdbuf();
  std::stringstream capturedCerr;
  std::cerr.rdbuf(capturedCerr.rdbuf());

  // Create CameraModel object
  CameraModel cm;

  // Pass fake user inputs
  std::cin.rdbuf(origCin);

  // Capture cerr output
  std::cerr.rdbuf(originalCerrBuffer);

  // Confirm correct error outputs
  EXPECT_EQ(capturedCerr.str(), "Error: invalid file format for extrinsics\n");

}
// End CameraModel testing ----------------------------------------------------

// ConfigClass testing --------------------------------------------------------

// Test that valid parameters can be loaded
TEST(config_test, loads_params){
  
  // Fake user input with valid distance inputs
  std::istringstream fakeInput("itest\netest\n3\n2");
  std::streambuf* origCin = std::cin.rdbuf();
  std::cin.rdbuf(fakeInput.rdbuf());

  // Create ConfigClass object  
  ConfigClass cfg_obj;

  // Pass fake user inputs
  std::cin.rdbuf(origCin);

  // Confirm parameters loaded with correct inputs
  EXPECT_EQ("itest",cfg_obj.intrinsicsPath);
  EXPECT_EQ("etest",cfg_obj.extrinsicsPath);
  EXPECT_EQ(3,cfg_obj.D_max_m);
  EXPECT_EQ(2,cfg_obj.D_close_m);
}

// Test ConfigClass rejects for D max <= D close
TEST(config_test, rejects_dmax_lessthan_dclose){
  // Fake user input with invalid distance inputs
  std::istringstream fakeInput("test\netest\n2\n3");
  std::streambuf* origCin = std::cin.rdbuf();
  std::cin.rdbuf(fakeInput.rdbuf());

  std::streambuf* originalCerrBuffer = std::cerr.rdbuf();
  std::stringstream capturedCerr;
  std::cerr.rdbuf(capturedCerr.rdbuf());

  // Create ConfigClass object  
  ConfigClass cfg_obj;

  // Pass fake user inputs
  std::cin.rdbuf(origCin);

  // Capture cerr output
  std::cerr.rdbuf(originalCerrBuffer);

  // Confirm correct error outputs
  EXPECT_EQ(capturedCerr.str(), "Error: D close must be smaller than D max\n");

}

// Test ConfigClass rejects for D max <=0
TEST(config_test, rejects_dmax){
  // Fake user input with invalid distance inputs
  std::istringstream fakeInput("test\netest\n0\n-1");
  std::streambuf* origCin = std::cin.rdbuf();
  std::cin.rdbuf(fakeInput.rdbuf());

  std::streambuf* originalCerrBuffer = std::cerr.rdbuf();
  std::stringstream capturedCerr;
  std::cerr.rdbuf(capturedCerr.rdbuf());

  // Create ConfigClass object  
  ConfigClass cfg_obj;

  // Pass fake user inputs
  std::cin.rdbuf(origCin);

  // Capture cerr output
  std::cerr.rdbuf(originalCerrBuffer);

  // Confirm correct error outputs
  EXPECT_EQ(capturedCerr.str(), "Error: distance inputs must be greater than zero\n");

}

// Test ConfigClass rejects for D close <=0
TEST(config_test, rejects_dclose){
  // Fake user input with invalid distance inputs
  std::istringstream fakeInput("test\netest\n3\n0");
  std::streambuf* origCin = std::cin.rdbuf();
  std::cin.rdbuf(fakeInput.rdbuf());

  std::streambuf* originalCerrBuffer = std::cerr.rdbuf();
  std::stringstream capturedCerr;
  std::cerr.rdbuf(capturedCerr.rdbuf());

  // Create ConfigClass object  
  ConfigClass cfg_obj;

  // Pass fake user inputs
  std::cin.rdbuf(origCin);

  // Capture cerr output
  std::cerr.rdbuf(originalCerrBuffer);

  // Confirm correct error outputs
  EXPECT_EQ(capturedCerr.str(), "Error: distance inputs must be greater than zero\n");

}




