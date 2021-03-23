// Includes
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <fstream>
#include <time.h>


using namespace cv;
using namespace std;


// GLOBAL VARIABLES

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
int density(string filename, int x)
{
    // Objects defined for background subtraction model
    Ptr<BackgroundSubtractor> pBackSub;
    Ptr<BackgroundSubtractor> pBackSub1;
    pBackSub = createBackgroundSubtractorMOG2();
    pBackSub1 = createBackgroundSubtractorMOG2();

    // opens file names "Density.txt" and sets the header
    ofstream utility;
    utility.open("utility_method1.dat", std::ios_base::app);

    ifstream infile; 
    infile.open("Baseline.txt");

    
    // open the videofile passed in command line argument
    VideoCapture capture(filename);
    if (!capture.isOpened())
    {
        //error in opening the video input
        cout << "Unable to open" << endl;
        return -1;
    }
    
    
    // initializations
    double runtime = 0.0;
    clock_t start, end;

    
    Mat frame, fgMask, fgMask1;
    double queue_density = 0.0;
    double dynamic_density = 0.0;
    double total; 
    double difference = 0.0;
    string baseline_difference; 
    double error = 0.0;
    int frame_count = 0; 
    
    capture >> frame;
    infile >> baseline_difference;

    // computation of density for each frame
    while (true) 
    {
        
        start = clock();
        // frame_count incremented
        frame_count += x;
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
 
        
        // gets the next x frames
        for (int i = 0; i < x; i++)
        {
            capture >> frame;
        }
        
        end = clock();

        runtime += (end - start); 
        
        difference = queue_density - dynamic_density;
        for (int i = 0; i < x; i++)
        {
            infile >> baseline_difference;
            error += abs(difference - stod(baseline_difference)); 
        }
        
        //get the input from the keyboard
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
        {
            break;
        }   
    }
    
    runtime = runtime / CLOCKS_PER_SEC;
    error = error / 5736.0; 

    utility << to_string(runtime) + "\t" + to_string(error) << endl;
    

    // close the file "Density.txt"
    utility.close();
    infile.close();
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

    string filename = argv[1];

    ofstream utility;
    utility.open("utility_method1.dat", std::ofstream::out | std::ofstream::trunc);
    utility.close();


    for (int i = 2; i <= 50; i++)
    {
        density(filename, i);
    }
    
    
    return 0;
}