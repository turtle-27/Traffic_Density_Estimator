#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

vector<Point2f> pts_src;
int count = 0;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    
    if  ( event == EVENT_LBUTTONDOWN )
    {
        ::pts_src.push_back(Point2f(x, y)); ::count++;
    }
    if ( ::count == 4)
    {
        return;
    }
}

int main(int argc, char** argv)
{
     // Read image from file 
     Mat img = imread("empty.jpg");
    cout << img.size() << endl;
     //if fail to read the image
     if ( img.empty() ) 
     { 
          cout << "Error loading the image" << endl;
          return -1; 
     }

     //Create a window
     namedWindow("My Window", 2);
     //set the callback function for any mouse event
     setMouseCallback("My Window", CallBackFunc, NULL);
     imshow("My Window", img);
     waitKey(0);
     // Four corners of the book in destination image.
    vector<Point2f> pts_dst; 
    pts_dst.push_back(Point2f(0,1079));
    pts_dst.push_back(Point2f(1919,1079));
    pts_dst.push_back(Point2f(1919,0));
    pts_dst.push_back(Point2f(0,0));
    
    

    // Calculate Homography
    Mat h = findHomography(::pts_src, pts_dst);

    // Output image
    Mat im_out;
    // Warp source image to destination based on homography
    warpPerspective(img, im_out, h, img.size());

    // Display images
    namedWindow("Source Image", 2);
    imshow("Source Image", img);
    waitKey(0);
    namedWindow("Warped Source Image" ,2);
    imshow("Warped Source Image", im_out);

    waitKey(0);

    return 0;

}