#include <gtest/gtest.h>

#include <vector>
#include "config_class.hpp"
#include "camera_model.hpp"
#include "human_detector.hpp"
#include <opencv2/opencv.hpp>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <string>

// CameraModel class


TEST(camera_model_test, loads_From_csv_file) {



  CameraModel cm("media/test_cal.csv");
  std::vector<float> correct_kmat_values = {1148.941079262392,0,968.6603818232142,
                                      0,1157.532465485139,533.1086821168354,
                                      0,0,1};
  std::vector<float> correct_dmat_values = {
                                      -0.4518047618131186, 0.3228995925163289, 
                                      -0.001753905126417962, -0.0003906063544475221, 
                                      -0.1601718014427244};

  int i = 0;
  for (int r=0; r<3; r++) {
    for (int c=0; c<3; c++) {
      EXPECT_FLOAT_EQ(correct_kmat_values[i],cm.K_mat.at<float>(r,c));
      i++;
    }
  }

  for (int c=0; c<5; c++) {
    EXPECT_FLOAT_EQ(correct_dmat_values[c], cm.D_mat.at<float>(0,c));
  }

}

// TEST(camera_model_test, calibrates_from_file) {


//   CameraModel cm("media/test_cal.MOV");

//   std::vector<float> correct_kmat_values = {1148.941079262392,0,968.6603818232142,
//                                       0,1157.532465485139,533.1086821168354,
//                                       0,0,1};
//   std::vector<float> correct_dmat_values = {
//                                       -0.4518047618131186, 0.3228995925163289, 
//                                       -0.001753905126417962, -0.0003906063544475221, 
//                                       -0.1601718014427244};

//   int i = 0;
//   for (int r=0; r<3; r++) {
//     for (int c=0; c<3; c++) {
//       EXPECT_FLOAT_EQ(correct_kmat_values[i],cm.K_mat.at<float>(r,c));
//       i++;
//     }
//   }

//   for (int c=0; c<5; c++) {
//     EXPECT_FLOAT_EQ(correct_dmat_values[c], cm.D_mat.at<float>(0,c));
//   }
// }

TEST(camera_model_test, undistorts_image) {


  CameraModel cm("media/test_cal.csv");

  cv::Mat test_frame = cv::imread("media/test_frame.jpg");

  auto start = std::chrono::high_resolution_clock::now();
  cv::Mat undistorted_frame = cm.undistort(test_frame);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  // std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;

  EXPECT_FALSE(undistorted_frame.empty());
  EXPECT_EQ(undistorted_frame.size(),test_frame.size());
  EXPECT_EQ(undistorted_frame.type(),test_frame.type());
  EXPECT_LT(duration.count(),200);
  cv::imshow("test frame",test_frame);
  cv::waitKey(0);
  cv::imshow("undistorted frame",undistorted_frame);
  cv::waitKey(0);
  cv::destroyAllWindows();


}

// TEST(camera_model_test, undistorts_real_time) {
//   CameraModel cm("media/test_cal.csv");
//   cv::VideoCapture capture("media/test_cal.MOV");
//   cv::Mat frame;
//   while (true) {
//     capture >> frame;
//     if (frame.empty()) {break;}

//     cv::Mat new_frame = cm.undistort(frame);
//     cv::imshow("new frame", new_frame);
//     cv::waitKey(1);

//   }

//   cv::destroyAllWindows();

// }




TEST(camera_model_test, rejects_non_csv_file) {



  std::streambuf* originalCerrBuffer = std::cerr.rdbuf();
  std::stringstream capturedCerr;
  std::cerr.rdbuf(capturedCerr.rdbuf());

  CameraModel cm("media/test_cal.png");

  std::cerr.rdbuf(originalCerrBuffer);
  EXPECT_EQ(capturedCerr.str(), "Error: invalid file format\n");

}



TEST(config_test, loads_params){
  
  std::istringstream fakeInput("itest.csv\netest.jpg\nmtest.jpg\n1\n2\n3");

  std::streambuf* origCin = std::cin.rdbuf();
  std::cin.rdbuf(fakeInput.rdbuf());

  
  ConfigClass cfg_obj;
  std::cin.rdbuf(origCin);
  EXPECT_EQ("itest.csv",cfg_obj.intrinsicsPath);
  EXPECT_EQ("etest.jpg",cfg_obj.extrinsicsPath);
  EXPECT_EQ("mtest.jpg",cfg_obj.modelPath);
  EXPECT_EQ(1,cfg_obj.cameraHeight_m);
  EXPECT_EQ(2,cfg_obj.D_max_m);
  EXPECT_EQ(3,cfg_obj.D_close_m);
}

// Tests by anvesh

static std::string WriteTempIntrinsicsCSV(float fx, float fy, float cx, float cy) {
  // mkstemp needs a mutable template ending in XXXXXX
  char tmpl[] = "/tmp/intrinsics_XXXXXX";
  int fd = mkstemp(tmpl);   // creates & opens a unique file
  if (fd == -1) throw std::runtime_error("mkstemp failed");
  close(fd);                // we'll re-open with ofstream

  const std::string path = tmpl;  // no extension needed

  std::ofstream ofs(path);
  if (!ofs) throw std::runtime_error("Failed to open temp CSV: " + path);

  // Your loader expects 9 K values (row-major) then 5 distortion coeffs
  ofs << fx << ",0," << cx << ",\n"
      << "0," << fy << "," << cy << ",\n"
      << "0,0,1,\n"
      << "-0.10,0.01,0.0005,-0.0003,-0.001\n";
  ofs.close();
  return path;
}

TEST(HumanDetectorMath, PixelToGroundBasic) {
  const auto csv = WriteTempIntrinsicsCSV(800.f, 800.f, 640.f, 360.f);
  HumanDetector hd("unused", csv);

  // Use default camera_height_m = 1.2 (from Params)
  // Choose a pixel so that (v - cy) = 400 → Z = fy*h/(v-cy) = 800*1.2/400 = 2.4
  // Let (u - cx) = 120 → X = Z * 120 / 800 = 2.4 * 0.15 = 0.36
  const cv::Point2f uv(640.f + 120.f, 360.f + 400.f);
  const auto Xw = hd.pixelToGround(uv);
  EXPECT_NEAR(Xw.z, 2.4f, 1e-5f);
  EXPECT_NEAR(Xw.x, 0.36f, 1e-5f);
  EXPECT_NEAR(Xw.y, 0.0f, 1e-6f);
}

TEST(HumanDetectorMath, PixelToGroundSingularityThrows) {
  const auto csv = WriteTempIntrinsicsCSV(800.f, 800.f, 640.f, 360.f);
  HumanDetector hd("unused", csv);

  // v == cy → denominator ~ 0 → must throw
  const cv::Point2f uv(640.f, 360.f);
  EXPECT_THROW({
    (void)hd.pixelToGround(uv);
  }, std::runtime_error);
}

TEST(HumanDetectorParams, CameraHeightScalesDepth) {
  const auto csv = WriteTempIntrinsicsCSV(800.f, 800.f, 640.f, 360.f);
  HumanDetector hd("unused", csv);

  // Base height = 1.2 → with (v - cy) = 400, Z1 = 2.4
  cv::Point2f uv(640.f, 360.f + 400.f);
  auto P1 = hd.pixelToGround(uv);
  EXPECT_NEAR(P1.z, 2.4f, 1e-5f);

  // Double the height → depth must double
  hd.setCameraHeight(2.4f);
  auto P2 = hd.pixelToGround(uv);
  EXPECT_NEAR(P2.z, 4.8f, 1e-5f);
}