#include <gtest/gtest.h>

#include <vector>

#include "camera_model.hpp"
#include <opencv2/opencv.hpp>

// CameraModel class


TEST(camera_model_test, loads_From_csv_file) {

  char* argv[] = {
    const_cast<char*>("program_name"),
    const_cast<char*>("media/test_cal.csv"),
  };


  CameraModel cm(argv);
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

TEST(camera_model_test, calibrates_from_file) {
  char* argv[] = {
  const_cast<char*>("program_name"),
  const_cast<char*>("media/test_cal.MOV"),
  };

  CameraModel cm(argv);

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

TEST(camera_model_test, undistorts_image) {
  char* argv[] = {
  const_cast<char*>("program_name"),
  const_cast<char*>("media/test_cal.csv"),
  };
  CameraModel cm(argv);

  cv::Mat test_frame = cv::imread("media/test_frame.jpg");

  cv::Mat undistorted_frame = cm.undistort(test_frame);
  EXPECT_FALSE(undistorted_frame.empty());
  EXPECT_EQ(undistorted_frame.size(),test_frame.size());
  EXPECT_EQ(undistorted_frame.type(),test_frame.type());
  cv::imshow("test frame",test_frame);
  cv::waitKey(0);
  cv::imshow("undistorted frame",undistorted_frame);
  cv::waitKey(0);
  cv::destroyAllWindows();


}




TEST(camera_model_test, rejects_non_csv_file) {

  char* argv[] = {
    const_cast<char*>("program_name"),
    const_cast<char*>("media/test_cal.png"),
  };

  std::streambuf* originalCerrBuffer = std::cerr.rdbuf();
  std::stringstream capturedCerr;
  std::cerr.rdbuf(capturedCerr.rdbuf());

  CameraModel cm(argv);

  std::cerr.rdbuf(originalCerrBuffer);
  EXPECT_EQ(capturedCerr.str(), "Error: invalid file format\n");

}

TEST(camera_model_test, undistorts_frame) {

}





