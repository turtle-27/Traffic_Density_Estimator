#include "opencv2/opencv.hpp"
#include <iostream>

int backSub(Mat inputFrame){
    Ptr<BackgroundSubtractor> backS = createBackgroundSubtractorMOG2();
    Mat outFrame;
    backS-> apply(inputFrame, outFrame);
    
    //get the frame number and write it on the current frame
    rectangle(inputFrame, cv::Point(10, 2), cv::Point(100,20),cv::Scalar(255,255,255), -1);
    stringstream current;
    current << capture.get(CAP_PROP_POS_FRAMES);
    string frameCount = current.str();
    putText(frame, frameNumberString.c_str(), cv::Point(15, 15),FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));

     //show the current frame and the fg masks
    imshow("Frame", inputFrame);
    imshow("Mask", outFrame);

    //get the input from the keyboard
    int keyboard = waitKey(30);
    if (keyboard == 'q' || keyboard == 27)
        break;
    return 0;        
}   