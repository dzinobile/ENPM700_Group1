#include "yolo_class.hpp"
#include <fstream>

YoloClass::YoloClass(){
    // Paths to YOLO files
    modelConfiguration = "/home/dzinobile/yolo/yolov3-tiny.cfg";
    modelWeights = "/home/dzinobile/yolo/yolov3-tiny.weights";
    classesFile = "/home/dzinobile/yolo/coco.names";

    // Load class names
    std::ifstream ifs(classesFile.c_str());
    std::string line;
    while (getline(ifs, line)) classes.push_back(line);

    // Load YOLO network
    net = cv::dnn::readNetFromDarknet(modelConfiguration, modelWeights);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

    // Open video
    videoPath = "/home/dzinobile/test_media/test_pos.mp4"; 
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video file.\n";
        
    }

    // Get output layer names
    outNames = net.getUnconnectedOutLayersNames();


}

cv::Mat YoloClass::highlight_people(cv::Mat frame) {


    cv::Mat blob;
    cv::dnn::blobFromImage(frame, blob, 1/255.0, cv::Size(416, 416), cv::Scalar(), true, false);
    net.setInput(blob);

 
    std::vector<cv::Mat> outs;
    net.forward(outs, outNames);


    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;


    for (auto &output : outs) {
        for (int i = 0; i < output.rows; ++i) {
            float *data = (float *)output.ptr(i);

            // Check if the row has at least 5 elements (x, y, w, h, conf)
            if (output.cols < 5) continue;

            float confidence = data[4];
            if (confidence > 0.5) {
                // Check that we have class scores
                if (output.cols <= 5) continue;

                cv::Mat scores = output.row(i).colRange(5, output.cols);
                cv::Point classIdPoint;
                double maxClassScore; 
                minMaxLoc(scores, 0, &maxClassScore, 0, &classIdPoint);

                // Prevent class index out-of-range
                if (classIdPoint.x < 0 || classIdPoint.x >= (int)classes.size())
                    continue;

                if (classes[classIdPoint.x] == "person" && maxClassScore > 0.4) {
                    int centerX = (int)(data[0] * frame.cols);
                    int centerY = (int)(data[1] * frame.rows);
                    int width   = (int)(data[2] * frame.cols);
                    int height  = (int)(data[3] * frame.rows);
                    int left    = centerX - width / 2;
                    int top     = centerY - height / 2;

                    // Sanity check for valid box
                    if (width > 0 && height > 0 &&
                        left >= 0 && top >= 0 &&
                        left + width <= frame.cols &&
                        top + height <= frame.rows) {
                        boxes.push_back(cv::Rect(left, top, width, height));
                        confidences.push_back((float)maxClassScore);
                        classIds.push_back(classIdPoint.x);
                    }
                }
            }
        }
    }


    // Non-maximum suppression to reduce duplicate boxes
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, 0.4, 0.3, indices);

    for (int idx : indices) {
        cv::Rect box = boxes[idx];
        cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2);
        cv::putText(frame, "Person", cv::Point(box.x, box.y - 10),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
    }


    return frame;

}