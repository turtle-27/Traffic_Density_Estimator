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


// Structure defined for use in method 3 and 4
struct  helper_struct
{
    Mat frame;
    double qd;
    int method;
    Ptr<BackgroundSubtractor> pBackSub_eachThread;
};


// Function to calculate homography take in img and returns homographed image of X*Y resolution
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

// Function to be run after creating each thread
void* thread_function(void* arg)
{  
    // arg_struct has 4 fields => frame, qd, method, pBackSub_eachThread
    struct helper_struct *arg_struct = (struct helper_struct*) arg;
     
    // initializations
    Mat frame1 = arg_struct->frame;
    Mat fgMask;
    double temp_queue_density = 0.0;

    // homography needs to be calculated for method 3 and not for method 4
    if (arg_struct->method == 4)
    {
        // Angle correction and cropping
        frame1 = homography(frame1, 200, 500);
    }
    
    //update the background model
    arg_struct->pBackSub_eachThread->apply(frame1, fgMask, 0);

    // density values 
    temp_queue_density = countNonZero(fgMask);

    // queue density value written in qd.
    arg_struct->qd = temp_queue_density;

    pthread_exit(0);
}

// Function for calculating queue density according to method 4 and method 3
double method_3_4(string filename, int method, int x)
{

    // opens "Baseline.txt" having queue density values of baseline
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
    string baseline; 
    double error = 0.0;
    double queue_density = 0.0;
     
    double total = double(200*500); // No. of pixels in the image on which bgSub is applied

    // First frame captured
    capture >> frame;

    // helper_struct array defined
    struct helper_struct args[x];
   
    // thread array created and attributes initialized
    pthread_t threadId[x];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    // represents first access. Used in method 3
    bool first_array[x];
     

    // method 4
    if (method == 4)
    {
        // bgSub object crated and initialised
        Ptr<BackgroundSubtractor> pBackSub;
        pBackSub = createBackgroundSubtractorMOG2();
    
        // computation of queue density for each frame
        while(true)
        {    
            // break if next frame is empty
            if (frame.empty())
            {
                break;
            }
            
            // for single thread
            if (x == 1)
            {
                // args fields assigned to be passed to thread_function
                args[0].frame = frame;
                args[0].method = method;
                args[0].pBackSub_eachThread = pBackSub;
                
                // Thread created
                pthread_create(&threadId[0], &attr, thread_function, &args[0]);
                
                // wait for thread to finish
                pthread_join(threadId[0], NULL);
                
                // next frame
                capture >> frame;
            }
            else
            {   
                
                // creates x threads, x > 1 
                for (int i = 0; i < x; i++)
                {
                    // break if next frame is empty 
                    if (frame.empty())
                    {
                        x = i;
                        break;
                    }
                    
                    // args fields assigned to be passed to thread_function
                    args[i].frame = frame;
                    args[i].method = method;
                    args[i].pBackSub_eachThread = pBackSub;

                    // Thread created
                    pthread_create(&threadId[i], &attr, thread_function, &args[i]);
                    
                    // next frame
                    capture >> frame;
                }

                // wait for all the threads to finish
                for (int i = 0; i < x; i++)
                {
                    pthread_join(threadId[i], NULL);
                    
                    // calculate error
                    infile >> baseline;
                    cout << "QD: " << args[i].qd/total << " BD: " << baseline << endl;
                    error += abs((args[i].qd/total) - stod(baseline));
                }
            }
            
        }
    }
    // method 3 
    else if (method == 3)
    {
        
        // bgSub array created of size x for x threads 
        Ptr<BackgroundSubtractor> pBackSub[x];
        
        // computation of queue density for each frame
        while(true)
        {  
            queue_density = 0.0;  

            // break if next frame is empty 
            if (frame.empty())
            {
                break;
            }

            // Angle correction and cropping
            frame = homography(frame, 200, 500);          

            // initializing dimensions and split size 
            int width = frame.cols;
            int height = frame.rows;
            int split_size = height / x ;
            
            // to keep track of no. of threads created
            int count = 0;

            // for single thread 
            if (x == 1)
            {
                // initialises bgSub object
                if (first_array[0])
                {
                    pBackSub[0] = createBackgroundSubtractorMOG2();
                    first_array[0] = false;
                }
                
                // args fields assigned to be passed to thread_function
                args[count].frame = frame;
                args[count].method = method;
                args[count].pBackSub_eachThread = pBackSub[0];
                
                // Thread created
                pthread_create(&threadId[count], &attr, thread_function, &args[count]);
                
                // wait for thread to finish
                pthread_join(threadId[count], NULL);
                
                // queue density updated
                queue_density += args[count].qd;
                
                // error calculation
                infile >> baseline;
                error += abs((queue_density/total) - stod(baseline));

            }
            else
            {
                // creates x threads, x > 1
                // splits frame in x parts to be proccesed by each thread
                for (int y = 0; y < height - split_size; y += split_size-1) 
                {
                    // initialises bgSub object
                    if (!first_array[count])
                    {
                        pBackSub[count] = createBackgroundSubtractorMOG2();
                        first_array[count] = true;
                    }
                    
                    // split from y to y+split_size
                    if (y <= height - 2*split_size)
                    {
                        // split of width (width - 0) and height (split_size-1)
                        Rect grid_rect(0, y, width, split_size-1);
                        args[count].frame = frame(grid_rect); 
                    }
                    // last split from y to height
                    else
                    {
                        // split of width (width - 0) and height (height - y)
                        Rect grid_rect(0, y, width, height - y);
                        args[count].frame = frame(grid_rect);   
                        
                        // rectangle(frame, grid_rect, Scalar(0, 255, 0), 1);
                        // imshow(to_string(count), frame(grid_rect));
                        // waitKey(0);
                    }

                    // args fields assigned to be passed to thread_function
                    args[count].method = method;
                    args[count].pBackSub_eachThread = pBackSub[count]; 
                    
                    // Thread created
                    pthread_create(&threadId[count], &attr, thread_function, &args[count]);
                    
                    // count updated
                    count++; 
                }

                // wait for all the threads to finish
                for (int i = 0; i < count; i++)
                {
                    pthread_join(threadId[i], NULL);
                    
                    // queue density updated
                    queue_density += args[i].qd;  
                }
                
                // error calculation
                infile >> baseline;
                error += abs((queue_density/total) - stod(baseline));
            }
            
            // next frame
            capture >> frame;
        }
    }
            
    // mean error 
    error = error / 5736.0; 
    
    // file closed
    infile.close();
    return error;
}

// Function to calulate queue density for method 1 and 2 
double method_1_2(string filename, int method, int x, int reso_X, int reso_Y)
{
    // Objects defined for background subtraction model
    Ptr<BackgroundSubtractor> pBackSub;
    pBackSub = createBackgroundSubtractorMOG2();

    // opens "Baseline.txt" having queue density values of baseline
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
    double total; 
    string baseline; 
    double error = 0.0;

    // First Frame
    capture >> frame;

    // computation of queue density for each frame
    while (true) 
    {
        
        // if next frame is empty break
        if (frame.empty())
        {
            break;
        }  
        
        // homography returns frame of X*Y resolution for method 2
        if (method == 2)
        {
            // Angle correction and cropping
            frame = homography(frame, reso_X, reso_Y);
        }
        // simple homography for method 1
        else 
        {
            // Angle correction and cropping
            frame = homography(frame, 200, 500);
        }
        
        
        // total no. of pixels in the homographed frame
        total = double(frame.total());

        //update the backgroundSubtraction model
        pBackSub->apply(frame, fgMask, 0);

        // queue density calculated 
        queue_density  = countNonZero(fgMask)/total; 
        
        // method 1
        if (method == 1)
        {
            // gets the next x frames
            for (int i = 0; i < x; i++)
            {
                capture >> frame;
            }  

            // error calculation
            for (int i = 0; i < x; i++)
            {
                infile >> baseline;
                error += abs(queue_density - stod(baseline)); 
            }
        
        }

        // method 2
        if (method == 2)
        {
            // next frame
            capture >> frame;

            // error calculation   
            infile >> baseline;
            error += abs(queue_density - stod(baseline));
        }
           
    }
    

    // mean error
    error = error / 5736.0; 
    
    // file closed
    infile.close();
    return error;
}

// MAIN
int main(int argc, char* argv[])
{
    // if command line arguments are not passed in proper format
    if (argc != 3)
    {
        cout << "Format: ./output videofilename method" << endl; 
        return -1;
    }

    // filename
    string filename = argv[1];
    
    // method no.
    int method = stoi(argv[2]);
    
    // if method not in range
    if (method < 1 || method > 4)
    {
        cout << "INVALID METHOD. METHOD SHOULD BE FROM {1,2,3,4}" << endl;
        return -1;
    }
    
    // open appropriate file where runtime and utility have to be written
    ofstream utility;
    utility.open("utility_method" + to_string(method) + ".dat");
    
    // utility value
    double util = 0.0;

    // method 1
    if (method == 1)
    {   
        cout << "Loading..." << endl;
        
        // running method for different values of parameter
        for (int i = 1; i <= 10; i++)
        {
            auto begin = chrono::high_resolution_clock::now();
            util = method_1_2(filename, method, i, 200, 500);
            auto finish = chrono::high_resolution_clock::now();
            if (util == -1.0)
            {
                return -1;
            }
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(finish - begin);
            utility << elapsed.count()/1000.0 << "\t" + to_string(util) << endl;
        }
        
        cout << "Done." << endl;
    }
    // method 2
    else if (method == 2)
    {
         cout << "Loading..." << endl;
        int ini_reso_x = 200;
        int ini_reso_y = 500;
        
        // running method for different values of parameter
        for (int i = 0; i < 7; i++)
        {
            auto begin = chrono::high_resolution_clock::now();
            util = method_1_2(filename, method, i, ini_reso_x, ini_reso_y);
            auto finish = chrono::high_resolution_clock::now();
            if (util == -1.0)
            {
                return -1;
            }
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(finish - begin);
            utility << elapsed.count()/1000.0 <<  "\t" + to_string(util) << endl;

            ini_reso_x = ini_reso_x/2;    
            ini_reso_y = ini_reso_y/2;
        }
        
        cout << "Done." << endl;
    }
    // method 3
    else if (method == 3)
    {
        cout << "Loading..." << endl;   
        
        // running method for different values of parameter
        for (int i = 1; i <= 10; i++)
        {
            auto begin = chrono::high_resolution_clock::now();
            util = method_3_4(filename, method, i);
            auto finish = chrono::high_resolution_clock::now();
            if (util == -1.0)
            {
                return -1;
            }
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(finish - begin);
            utility << elapsed.count()/1000.0 << "\t" + to_string(util) << endl;
        }
        cout << "Done." << endl;
    }
    // method 4
    else if (method == 4)
    {
        cout << "Loading..." << endl;   
        
        // running method for different values of parameter
        for (int i = 2; i <= 10; i++)
        {
            auto begin = chrono::high_resolution_clock::now();
            util = method_3_4(filename, method, i);
            auto finish = chrono::high_resolution_clock::now();
            if (util == -1.0)
            {
                return -1;
            }
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(finish - begin);
            utility << elapsed.count()/1000.0 << "\t" + to_string(util) << endl;
        }
        
        cout << "Done." << endl;
    }
    
    // file closed
    utility.close();
    
    return 0;
}