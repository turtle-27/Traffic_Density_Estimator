// Includes
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <fstream>

using namespace cv;
using namespace std;


// GLOBAL VARIABLES
// Variable to keep track of mouse clicks 
int count = 0;

// Function to calculate homography
Mat homography(Mat img)
{
    // Array to store points selected by the user thorugh mouse clicks
    vector<Point2f> pts_src;
    // Four points of the image of the destination image used to wrap perspective
    pts_src.push_back(Point2f(961,213));
    pts_src.push_back(Point2f(596,1066));
    pts_src.push_back(Point2f(1560,1055));
    pts_src.push_back(Point2f(1284,217));
    
    // Array to store points of the destination image
    vector<Point2f> pts_dst; 
    // Four points of the image of the destination image used to wrap perspective
    pts_dst.push_back(Point2f(472,52));
    pts_dst.push_back(Point2f(472,830));
    pts_dst.push_back(Point2f(800,830));
    pts_dst.push_back(Point2f(800,52));
    
    // Calculate Homography
    Mat h = findHomography(pts_src, pts_dst);

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
    Mat h1 = findHomography(pts_src, pts_dst2);
    warpPerspective(img, cropped, h1, cropped.size());

    return cropped;
}

// Function to calulate Density
int density(VideoCapture capture)
{
    // Objects defined for background subtraction model
    Ptr<BackgroundSubtractor> pBackSub;
    Ptr<BackgroundSubtractor> pBackSub1;
    pBackSub = createBackgroundSubtractorMOG2();
    pBackSub1 = createBackgroundSubtractorMOG2();

    // opens file names "Density.csv" and sets the header
    ofstream Density("Density.csv");
    Density <<"Frame Count, Queue Density, Dynamic Density" << endl;


    // initializations
    Mat frame, fgMask, fgMask1;
    double queue_density = 0.0;
    double dynamic_density = 0.0;
    double total; 
    int frame_count = 0; 
    // computation of density for each frame
    while (true) 
    {
        // gets the next frame
        capture >> frame;
        // frame_count incremented
        frame_count += 1;
        // if next frame is empty break
        if (frame.empty())
        {
            break;
        }  
        
        // Angle correction and cropping
        frame = homography(frame);

        // total no. of pixels in the frame
        total = double(frame.total());

        //update the background model
        pBackSub->apply(frame, fgMask, 0);
        pBackSub1->apply(frame, fgMask1, -1);

        // density values 
        queue_density   = countNonZero(fgMask)/total;
        dynamic_density = countNonZero(fgMask1)/total;

        // density values written in the file "Density.csv" and output shown on terminal as well
        Density << to_string(frame_count) + ", " + to_string(queue_density) + ", " + to_string(dynamic_density) << endl;
        cout << to_string(frame_count) + ", " + to_string(queue_density) + ", " + to_string(dynamic_density) << endl;

        //get the input from the keyboard
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
        {
            break;
        }   
    }
    
    // close the file "Density.csv"
    Density.close();

    return 0;
}

int main(int argc, char* argv[])
{
    // if command line arguments is not passed 
    if (argc != 2)
    {
        cout << "Format: ./output videofilename" << endl; 
        return -1;
    }
    
    // open the videofile passed in command line argument
    VideoCapture capture(argv[1]);
    if (!capture.isOpened())
    {
        //error in opening the video input
        cout << "Unable to open" << endl;
        return -1;
    }

    // Variable to keep track of mouse clicks 
    int count = 0;

    // calculates and outputs the density on terminal while also writing a file "Density.csv"
    density(capture);
}