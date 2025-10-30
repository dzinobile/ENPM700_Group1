#include <opencv2/opencv.hpp>
#include "yolo_class.hpp"
#include "camera_model.hpp"

int main(){
    cv::VideoCapture cap("test_media/test_pos.mp4");
    YoloClass yc;


    while (true){
        cv::Mat frame;
        cap >> frame;
        if(frame.empty()){
            break;
        }


        cv::Mat new_frame = yc.highlight_people(frame);

        cv::imshow("newframe", new_frame);
        cv::waitKey(500);


    }
    
    cv::destroyAllWindows();



}