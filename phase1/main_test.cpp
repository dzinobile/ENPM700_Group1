#include <opencv2/opencv.hpp>
#include "yolo_class.hpp"
#include "camera_model.hpp"
#include "human_detector.hpp"

int main(){
    cv::VideoCapture cap("/home/dzinobile/test_media/test_pos.mp4");
    CameraModel cm;
    YoloClass yc;
    HumanDetector hd("input");
    
    

    while (true){
        cv::Mat frame;
        cap >> frame;
        if(frame.empty()){
            break;
        }
        cv::Mat new_frame = hd.undistort(frame);
        // new_frame = hd.find_coords(yc.highlight_people(new_frame));
        cv::imshow("newframe", new_frame);
        cv::waitKey(1);


    }
    
    cv::destroyAllWindows();



}