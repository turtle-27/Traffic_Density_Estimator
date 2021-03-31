// Includes
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <ctime>
#include <time.h>
#include <chrono>
#include <pthread.h>


using namespace cv;
using namespace std;

Mat homography(Mat img, int X, int Y)
{
    // Array to store points selected by the user thorugh mouse clicks
    vector<Point2f> pts_frame;
    
    // Four points of the image of the destination image used to wrap perspective
    pts_frame.push_back(Point2f(961,213));
    pts_frame.push_back(Point2f(596,1066));
    pts_frame.push_back(Point2f(1560,1055));
    pts_frame.push_back(Point2f(1284,217));
    
    // Array to store points of the destination image
    vector<Point2f> pts_dst; 
    
    // Four points of the image of the destination image used to wrap perspective
    pts_dst.push_back(Point2f(472,52));
    pts_dst.push_back(Point2f(472,830));
    pts_dst.push_back(Point2f(800,830));
    pts_dst.push_back(Point2f(800,52));
    
    // Calculate Homography
    Mat h = findHomography(pts_frame, pts_dst);

    // Output image
    Mat im_out;
    
    // Warp source image to destination based on homography
    warpPerspective(img, im_out, h, {1366 ,748}); 
    Mat cropped = Mat::zeros(500, 200, CV_8UC3);
    
    // Cropping
    // Array to store points of the destination image used to crop the image
    vector<Point2f> pts_dst2; 
    pts_dst2.push_back(Point2f(0,0));
    pts_dst2.push_back(Point2f(0,500));
    pts_dst2.push_back(Point2f(200,500));
    pts_dst2.push_back(Point2f(200,0));
    Mat h1 = findHomography(pts_frame, pts_dst2);
    warpPerspective(img, cropped, h1, cropped.size());
    
    // Resizing 
    resize(cropped, cropped, Size(X, Y));

    return cropped;
}

int method_1_2(string filename, int x)
{
    Ptr<BackgroundSubtractor> pBackSub;
    pBackSub = createBackgroundSubtractorMOG2();
    
    VideoCapture capture(filename);
    if (!capture.isOpened())
    {
        //error in opening the video input
        cout << "Unable to open" << endl;
        return -1.0;
    }

    Mat frame; 
    Mat split;
    Mat fgMask;

    capture >> frame;

    while (true)
    {
        if (frame.empty())
        {
            break;
        }
        
        frame = homography(frame, 200, 500);

        int width = frame.cols;
        int height = frame.rows;
        int split_size = height / x ;
        int qd = 0.0;

        for (int y = 0; y < height - split_size; y += split_size-1) 
        {
            // split from y to y+split_size
            if (y <= height - 2*split_size)
            {
                // split of width (width - 0) and height (split_size-1)
                Rect grid_rect(0, y, width, split_size-1);
                split = frame(grid_rect);
            }
            // last split from y to height
            else
            {
                // split of width (width - 0) and height (height - y)
                Rect grid_rect(0, y, width, height - y);  
                split = frame(grid_rect); 
            } 
        }

        pBackSub->apply(frame, fgMask, 0);

        qd = countNonZero(fgMask);

        capture >> frame;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    // if command line arguments are not passed in proper format
    if (argc != 2)
    {
        cout << "Format: ./output videofilename" << endl; 
        return -1;
    }

    // filename
    string filename = argv[1];

    cout << "Loading..." << endl;
    
    int util = 0;

    // running method for different values of parameter
    for (int i = 1; i <= 10; i++)
    {
        auto begin = chrono::high_resolution_clock::now();
        util = method_1_2(filename, i);
        auto finish = chrono::high_resolution_clock::now();
        if (util == -1.0)
        {
            return -1;
        }
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(finish - begin);
        cout << elapsed.count()/1000.0 << endl;
    }
    
    cout << "Done." << endl;
    

    return 0;
}

