#include <iostream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
using namespace cv;
using namespace std;
// const char* params
//     = "{ help h         |           | Print usage }"
//       "{ input          | vtest.avi | Path to a video or a sequence of image }"
//       "{ algo           | MOG2      | Background subtraction method (KNN, MOG2) }";


// Array to store points selected by the user thorugh mouse clicks
vector<Point2f> pts_src;
// Variable to keep track of mouse clicks 
int count = 0;

// Opencv function for receiving mouse click and mapping coordinates to variables x and y 
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    // If left mouse is clicked
    if  ( event == EVENT_LBUTTONDOWN )
    {
        ::pts_src.push_back(Point2f(x, y)); ::count++;// Count incremented 
    }
    // If count = 4 stop accepting more mouse clicks and close the window 
    if ( ::count == 4)
    {
        destroyWindow("Original Image");
        return;
    }
}


int main(int argc, char* argv[])
{
    
    // Create a window
    namedWindow("Original Image", 0);
    // resize the window 
    resizeWindow("Original Image", 1024, 720);
    // Set the callback function for any mouse event
    setMouseCallback("Original Image", CallBackFunc, NULL);
    // Show image
    imshow("Original Image", img);  
    waitKey(0);

    // Array to store points of the destination image
    vector<Point2f> pts_dst; 
    // Four points of the image of the destination image used to wrap perspective
    pts_dst.push_back(Point2f(472,52));
    pts_dst.push_back(Point2f(472,830));
    pts_dst.push_back(Point2f(800,830));
    pts_dst.push_back(Point2f(800,52));
    
    // Calculate Homography
    Mat h = findHomography(::pts_src, pts_dst);

    warpPerspective(img, im_out, h, {1366 ,748}); 
    Mat cropped = Mat::zeros(500,200, CV_8UC3);
    
    // Cropping
    // Array to store points of the destination image used to crop the image
    vector<Point2f> pts_dst2; 
    pts_dst2.push_back(Point2f(0,0));
    pts_dst2.push_back(Point2f(0,500));
    pts_dst2.push_back(Point2f(200,500));
    pts_dst2.push_back(Point2f(200,0));
    Mat h1 = findHomography(::pts_src, pts_dst2);
    warpPerspective(img, cropped, h1, cropped.size() );
    
    
    
    // CommandLineParser parser(argc, argv, params);
    // parser.about( "This program shows how to use background subtraction methods provided by "
    //               " OpenCV. You can process both videos and images.\n" );
    // if (parser.has("help"))
    // {
    //     //print help information
    //     parser.printMessage();
    // }
    //create Background Subtractor objects
    Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorMOG2();
    // if (parser.get<String>("algo") == "MOG2")
    //     pBackSub = createBackgroundSubtractorMOG2();
    // else
    //     pBackSub = createBackgroundSubtractorKNN();
    VideoCapture capture("trafficvideo.mp4");
    if (!capture.isOpened()){
        //error in opening the video input
        cout << "Unable to open" << endl;
        return 0;
    }
    Mat frame, fgMask;
    Mat temp_frame = imread("empty1.jpg");
    pBackSub->apply(temp_frame, fgMask, 0);
    while (true) {
        capture >> frame;
        if (frame.empty())
            break;
        //update the background model
        pBackSub->apply(frame, fgMask, 0);
        //get the frame number and write it on the current frame
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
                  cv::Scalar(255,255,255), -1);
        stringstream ss;
        ss << capture.get(CAP_PROP_POS_FRAMES);
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        //show the current frame and the fg masks
        imshow("Frame", frame);
        imshow("FG Mask", fgMask);
        //get the input from the keyboard
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
    }
    return 0;
}