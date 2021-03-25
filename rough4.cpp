// Includes
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <fstream>
#include <time.h>
#include <pthread.h>

using namespace cv;
using namespace std;


struct method4_struct
{
    Mat frame;
    double queue_density;
};

// GLOBAL VARIABLES
double runtime = 0.0;
Ptr<BackgroundSubtractor> pBackSub_method4;

// Function to calculate homography
Mat homography(Mat img, int X, int Y)
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
    Mat cropped = Mat::zeros(500, 200, CV_8UC3);
    
    // Cropping
    // Array to store points of the destination image used to crop the image
    vector<Point2f> pts_dst2; 
    pts_dst2.push_back(Point2f(0,0));
    pts_dst2.push_back(Point2f(0,500));
    pts_dst2.push_back(Point2f(200,500));
    pts_dst2.push_back(Point2f(200,0));
    Mat h1 = findHomography(pts_src, pts_dst2);
    warpPerspective(img, cropped, h1, cropped.size());
    
    resize(cropped, cropped, Size(X, Y));

    return cropped;
}

void* temp4(void* arg)
{
    struct method4_struct *arg_struct = (struct method4_struct*) arg;
    
    double total; 
    Mat frame = arg_struct->frame;
    Mat fgMask;
    double temp_queue_density = 0.0;

    frame = homography(frame, 200, 500);
    
    // total no. of pixels in the frame
    total = double(frame.total());

    //update the background model
    pBackSub_method4->apply(frame, fgMask, 0);
    
    // density values 
    temp_queue_density  = countNonZero(fgMask)/total;

    arg_struct->queue_density = temp_queue_density; 
    pthread_exit(0);
}

double method4(string filename, int method, int x)
{
    
    pBackSub_method4 = createBackgroundSubtractorMOG2();

    ifstream infile; 
    infile.open("Baseline.txt");

    // open the videofile passed in command line argument
    VideoCapture capture(filename);
    if (!capture.isOpened())
    {
        //error in opening the video input
        cout << "Unable to open" << endl;
        return -1.0;
    }

    // initializations
    Mat frame, fgMask; 
    string baseline_difference; 
    double error = 0.0;
    double utility = 0.0; 
    bool check = false;
    
    capture >> frame;

    pBackSub_method4->apply(frame, fgMask, 0);

    struct method4_struct args[x];
   
    while(true)
    {
        // if next frame is empty break
        if (frame.empty())
        {
            break;
        }  
        
        pthread_t threadId[x];
        for (int i = 0; i < x; i++)
        {
            if (frame.empty())
            {
                break;
                check = true;
            }
            args[i].frame = frame;
            pthread_create(&threadId[i], 0, temp4, &args[i]);
            capture >> frame;
        }

        if (check == true)
        {
            break;
        }

        for (int i = 0; i < x; i++)
        {
            pthread_join(threadId[i], NULL);
            infile >> baseline_difference;
            // cout << "Qd: " << args[i].queue_density << " Bd: " << baseline_difference << endl;
            // error += abs(args[i].queue_density - stod(baseline_difference));
        }

        //get the input from the keyboard
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
        {
            break;
        } 
    }

    error = error / 5736.0; 
    // utility = 1 / error;

    infile.close();
    return error;
}

// Function to calulate Density
double density(string filename, int method, int x, int reso_X, int reso_Y)
{
    // Objects defined for background subtraction model
    Ptr<BackgroundSubtractor> pBackSub;
    Ptr<BackgroundSubtractor> pBackSub1;
    pBackSub = createBackgroundSubtractorMOG2();
    pBackSub1 = createBackgroundSubtractorMOG2();

    // opens file names "Density.txt" and sets the header
    // ofstream utility;
    // utility.open("utility_method1.dat", std::ios_base::app);

    ifstream infile; 
    infile.open("Baseline.txt");

    
    // open the videofile passed in command line argument
    VideoCapture capture(filename);
    if (!capture.isOpened())
    {
        //error in opening the video input
        cout << "Unable to open" << endl;
        return -1.0;
    }
    
    // initializations
    Mat frame, fgMask, fgMask1;
    double queue_density = 0.0;
    double dynamic_density = 0.0;
    double total; 
    string baseline_difference; 
    double error = 0.0;
    double utility = 0.0;
    int frame_count = 0; 
    
    clock_t start, end;
    runtime = 0.0;

    capture >> frame;

    // computation of density for each frame
    while (true) 
    {
        
        // if next frame is empty break
        if (frame.empty())
        {
            break;
        }  
        
        start = clock();

        if (method == 2)
        {
            // Angle correction and cropping
            frame = homography(frame, reso_X, reso_Y);
        }
        else 
        {
            // Angle correction and cropping
            frame = homography(frame, 200, 500);
        }
        
        

        // total no. of pixels in the frame
        total = double(frame.total());

        //update the background model
        pBackSub->apply(frame, fgMask, 0);
        // pBackSub1->apply(frame, fgMask1, -1);

        // density values 
        queue_density  = countNonZero(fgMask)/total;
        // dynamic_density = countNonZero(fgMask1)/total;

        if (method == 1)
        {
            // gets the next x frames
            for (int i = 0; i < x; i++)
            {
                capture >> frame;
            }  

            for (int i = 0; i < x; i++)
            {
                infile >> baseline_difference;
                error += abs(queue_density - stod(baseline_difference)); 
            }
        
        }

        if (method == 2)
        {
            capture >> frame;   
            infile >> baseline_difference;
            error += abs(queue_density - stod(baseline_difference));
        }
        
        
        end = clock();
        
        runtime += (end - start);

        //get the input from the keyboard
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
        {
            break;
        }   
    }
    

    error = error / 5736.0; 
    utility = 1 / error;

    infile.close();
    return utility;
}

int main(int argc, char* argv[])
{
    
    // if command line arguments is not passed 
    if (argc != 3)
    {
        cout << "Format: ./output videofilename method" << endl; 
        return -1;
    }

    string filename = argv[1];
    int method = stoi(argv[2]);
    if (method < 1 || method > 4)
    {
        cout << "INVALID METHOD. METHOD SHOULD BE FROM {1,2,3,4}" << endl;
        return -1;
    }
    
    ofstream utility;
    utility.open("utility_method" + to_string(method) + ".dat");
    

    
    clock_t start, end;
    double util = 0.0;

    if (method == 1)
    {   
        for (int i = 2; i <= 20; i++)
        {
            start = clock();
            util = density(filename, method, i, 1920, 1080);
            end = clock();
            if (util == -1.0)
            {
                return -1;
            }
            runtime = (end - start) / CLOCKS_PER_SEC;
            utility << to_string(runtime) + "\t" + to_string(util) << endl;
        }
    }
    else if (method == 2)
    {
        cout << "loading..." << endl;
        int ini_reso_x = 200;
        int ini_reso_y = 500;
        
        for (int i = 2; i <= 10; i++)
        {
            ini_reso_x = ini_reso_x/2;    
            ini_reso_y = ini_reso_y/2;
            
            // start = clock();
            util = density(filename, method, i, ini_reso_x, ini_reso_y);
            // end = clock();
            if (util == -1.0)
            {
                return -1;
            }
            // runtime = (end - start) / CLOCKS_PER_SEC;
            runtime = runtime / CLOCKS_PER_SEC;
            utility << to_string(runtime) + "\t" + to_string(util) << endl;
        }
        cout << "DONE" << endl;
    }
    else if (method == 3)
    {
        return -1;
    }
    else if (method == 4)
    {
        for (int i = 1; i <= 5; i++)
        {
            start = clock();
            util = method4(filename, method, i);
            end = clock();
            if (util == -1.0)
            {
                return -1;
            }
            runtime = (end - start) / CLOCKS_PER_SEC;
            utility << to_string(runtime) + "\t" + to_string(util) << endl;
        }
    }
    
    utility.close();
    
    return 0;
}