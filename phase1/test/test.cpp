#include <gtest/gtest.h>

#include <vector>

#include <opencv2/opencv.hpp>
#include <chrono>
#include "human_detector.hpp"
// CameraModel class

TEST(human_detector, human_detector_test) {
  HumanDetector hd("window");
  hd.bindWindow();
  cv::VideoCapture cap("test_media/dtest.mp4");

  while (true){

    cv::Mat frame;
    cap >> frame;
    

   
      hd.setFrame(frame);
      hd.redraw();
      // for (;;) {

      //   int key = cv::waitKey(20);
      //   if (key == 'r') {
      //   hd.redraw();
      //   if (key == 32) {
      //     break;
      //   }
      //   } 
      // }
      
    int key = cv::waitKey(0);


      for (;;) {

        // int key = cv::waitKey(20);
        if (key == 'r') {
        hd.redraw(); } 
        
        if (key == 32) {
          break;
        }
       
      }

  }



}
  


// TEST(camera_model_test, loads_From_csv_file) {



//   CameraModel cm("test_media/test_intrinsic.csv");
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

// TEST(camera_model_test, calibrates_from_file) {


//   CameraModel cm("test_media/test_cal.mp4");

//   std::vector<float> correct_kmat_values = {1154.8749,0,963.95282,
//                                       0,1164.9955,530.64508,
//                                       0,0,1};
//   std::vector<float> correct_dmat_values = {
//                                       -0.45195264, 0.31424072, -0.00096839585, 0.00036902804, -0.13790174};

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

// TEST(camera_model_test, undistorts_image) {


//   CameraModel cm("test_media/test_cal.csv");

//   cv::Mat test_frame = cv::imread("test_media/test_frame.jpg");

//   auto start = std::chrono::high_resolution_clock::now();
//   cv::Mat undistorted_frame = cm.undistort(test_frame);
//   auto end = std::chrono::high_resolution_clock::now();
//   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//   // std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;

//   EXPECT_FALSE(undistorted_frame.empty());
//   EXPECT_EQ(undistorted_frame.size(),test_frame.size());
//   EXPECT_EQ(undistorted_frame.type(),test_frame.type());
//   EXPECT_LT(duration.count(),200);
//   cv::imshow("test frame",test_frame);
//   cv::waitKey(0);
//   cv::imshow("undistorted frame",undistorted_frame);
//   cv::waitKey(0);
//   cv::destroyAllWindows();


// }






// TEST(camera_model_test, rejects_non_csv_file) {



//   std::streambuf* originalCerrBuffer = std::cerr.rdbuf();
//   std::stringstream capturedCerr;
//   std::cerr.rdbuf(capturedCerr.rdbuf());

//   CameraModel cm("test_media/test_cal.png");

//   std::cerr.rdbuf(originalCerrBuffer);
//   EXPECT_EQ(capturedCerr.str(), "Error: invalid file format\n");

// }



// TEST(config_test, loads_params){
  
//   std::istringstream fakeInput("itest.csv\netest.jpg\n2\n3");

//   std::streambuf* origCin = std::cin.rdbuf();
//   std::cin.rdbuf(fakeInput.rdbuf());

  
//   ConfigClass cfg_obj;
//   std::cin.rdbuf(origCin);
//   EXPECT_EQ("itest.csv",cfg_obj.intrinsicsPath);
//   EXPECT_EQ("etest.jpg",cfg_obj.extrinsicsPath);
//   EXPECT_EQ(2,cfg_obj.D_max_m);
//   EXPECT_EQ(3,cfg_obj.D_close_m);








// }





