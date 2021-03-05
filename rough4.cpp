// Includes
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <fstream>

using namespace cv;
using namespace std;


// GLOBAL VARIABLES
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

Mat homography(Mat img)
{
    // Array to store points of the destination image
    vector<Point2f> pts_dst; 
    // Four points of the image of the destination image used to wrap perspective
    pts_dst.push_back(Point2f(472,52));
    pts_dst.push_back(Point2f(472,830));
    pts_dst.push_back(Point2f(800,830));
    pts_dst.push_back(Point2f(800,52));
    
    // Calculate Homography
    Mat h = findHomography(::pts_src, pts_dst);

    // Output image test
    Mat im_out;
    // Warp source image to destination based on homography
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
    warpPerspective(img, cropped, h1, cropped.size());

    return cropped;
}

int queueDensity(VideoCapture capture)
{
    Ptr<BackgroundSubtractor> pBackSub;
    pBackSub = createBackgroundSubtractorMOG2();

    ofstream qDensity("queue_Density.csv");
    qDensity <<"Frame Count, Queue Density" << endl;

    Mat frame, fgMask;
    int white_density = 0;
    int frame_count = 0; 
    while (true) 
    {
        capture >> frame;
        frame_count ++;
        if (frame.empty())
        {
            break;
        }
        
        // Angle correction and cropping
        frame = homography(frame);
        
        //update the background model
        pBackSub->apply(frame, fgMask, 0);
        
        white_density = countNonZero(fgMask);

        qDensity << to_string(frame_count) + ", " + to_string(white_density) << endl;

        //show the current frame and the fg masks
        imshow("Frame", frame);
        imshow("FG Mask", fgMask);
        //get the input from the keyboard
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
        {
            break;
        }    
    }
    
    qDensity.close();

    return 0;
}

int main(int argc, char* argv[])
{
    VideoCapture capture("trafficvideo.mp4");
    if (!capture.isOpened())
    {
        //error in opening the video input
        cout << "Unable to open" << endl;
        return -1;
    }
    
    Mat first_frame;
    capture >> first_frame;
    if (first_frame.empty())
    {
        return -1;
    }

    // Variable to keep track of mouse clicks 
    int count = 0;

    // Create a window
    namedWindow("Original Image", 0);
    // resize the window 
    resizeWindow("Original Image", 1024, 720);
    // Set the callback function for any mouse event
    setMouseCallback("Original Image", CallBackFunc, NULL);
    // Show image
    imshow("Original Image", first_frame);  
    waitKey(0);

    queueDensity(capture);
}