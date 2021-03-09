// Includes
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

using namespace std;
using namespace cv;

int backSub(VideoCapture capture){
    Mat inputFrame;
    capture >> inputFrame;
        if (inputFrame.empty())
        {
            return -1;
        }
    Ptr<BackgroundSubtractor> backS = createBackgroundSubtractorMOG2();
    Mat outFrame;
    backS-> apply(inputFrame, outFrame);
    
    //get the frame number and write it on the current frame
    rectangle(inputFrame, Point(10, 2), Point(100,20),Scalar(255,255,255), -1);
    stringstream current;
    current << capture.get(CAP_PROP_POS_FRAMES);
    string frameCount = current.str();
    putText(inputFrame, frameCount.c_str(), Point(15, 15),FONT_HERSHEY_SIMPLEX, 0.5 , Scalar(0,0,0));

     //show the current frame and the fg masks
    imshow("Frame", inputFrame);
    imshow("Mask", outFrame);

    // //get the input from the keyboard
    // int keyboard = waitKey(1);
    // if (keyboard == 'q' || keyboard == 27)
    // {
    //     return -1;
    // }
        
    return 0;        
}  

//main
int main(int argc, char** argv){
    
    VideoCapture cap("trafficvideo.mp4");

    if(!cap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    return -1;
    }

    
    
    while(1)
    {
        backSub(cap);

        char c = (char)waitKey(1); 
        if(c==27)
        {
            break;
        }
                   
    }

    cap.release();

    destroyAllWindows();
    return 0;
}

