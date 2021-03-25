// Includes
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

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

// main
int main(int argc, char** argv)
{
    // Command line argument validation
    if (argc != 2)
    {
        cout << "Command format: ./output filename\nfor empty image: ./output empty.jpg\nfor traffic image: ./output traffic.jpg" << endl;
        return -1;
    }
    
    // Variable for extracting image file name from command line argument
    String img_name = argv[1]; 
    
    // Read image from file 
    Mat img = imread(img_name);
    
    // If fail to read the image
    if ( img.empty() ) 
    { 
        cout << "Error loading the image" << endl;
        return -1; 
    }

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
    warpPerspective(img, cropped, h1, cropped.size() );

    // Display and save images
   
    // name of the file to be saved
    img_name = "transformed_" + img_name;
    // Variable to check whether image is saved properly or not
    bool check = imwrite(img_name, im_out); 
  
    // if the image is not saved 
    if (check == false) { 
        cout << "Couldn't save the image." << endl;
        return -1; 
    } 

    // name of the file to be saved
    img_name = "cropped_" + img_name;
    // Variable to check whether image is saved properly or not
    check = imwrite(img_name, cropped); 
  
    // if the image is not saved 
    if (check == false) { 
        cout << "Couldn't save the image." << endl;
        return -1; 
    } 
    
    namedWindow("Transformed Image", 0);
    resizeWindow("Transformed Image", 1024, 720);
    imshow("Transformed Image", im_out);
    waitKey(0);
    destroyWindow("Transformed Image");
    namedWindow("Cropped Image", 0);
    resizeWindow("Cropped Image", 1024, 720);
    imshow("Cropped Image", cropped);
    waitKey(0);
    
//    for(int i=0; i < pts_src.size(); i++)
//    {
//        cout << pts_src.at(i) << ' ';
//    }
   


    
    return 0;

}
