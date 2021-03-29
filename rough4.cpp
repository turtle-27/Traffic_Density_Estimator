// Includes
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <fstream>
#include <time.h>
#include <chrono>
#include <pthread.h>


using namespace cv;
using namespace std;

struct  method4_struct
{
    Mat frame;
    double qd;
    int method;
    Ptr<BackgroundSubtractor> pBackSub_eachThread;
};

// GLOBAL VARIABLES


// Function to calculate homography
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
    Mat h1 = findHomography(pts_frame, pts_dst2);
    warpPerspective(img, cropped, h1, cropped.size());
    
    resize(cropped, cropped, Size(X, Y));

    return cropped;
}

void* temp4(void* arg)
{
    struct method4_struct *arg_struct = (struct method4_struct*) arg;
     
    Mat frame1 = arg_struct->frame;
    Mat fgMask;
    // Ptr<BackgroundSubtractor> pBackSub = arg_struct->pBackSub_eachThread;
    double temp_queue_density = 0.0;

    if (arg_struct->method == 4)
    {
        frame1 = homography(frame1, 200, 500);
    }
    
    //update the background model
    arg_struct->pBackSub_eachThread->apply(frame1, fgMask, 0);
    // imshow("f", fgMask);
    // waitKey(0);
    // density values 
    temp_queue_density = countNonZero(fgMask);

    arg_struct->qd = temp_queue_density;

    pthread_exit(0);
}

double method4(string filename, int method, int x)
{

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
    double queue_density = 0.0;
    double utility = 0.0; 
    double total = 0.0;

    capture >> frame;
    total = double(frame.total());

    struct method4_struct args[x];
   
    pthread_t threadId[x];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    

    bool first_time1 = true;
    bool first_array[x];
     
    if (method == 4)
    {
        Ptr<BackgroundSubtractor> pBackSub;
        pBackSub = createBackgroundSubtractorMOG2();

        while(true)
        {    
            if (frame.empty())
            {
                break;
            }


            if (x == 1)
            {
                // imshow("frame", frame);
                // waitKey(0);
                args[0].frame = frame;
                args[0].method = method;
                args[0].pBackSub_eachThread = pBackSub;
                pthread_create(&threadId[0], &attr, temp4, &args[0]);
                pthread_join(threadId[0], NULL);
                capture >> frame;
            }
            else
            {
                for (int i = 0; i < x; i++)
                {
                    if (frame.empty())
                    {
                        x = i;
                        break;
                    }
                    args[i].frame = frame;
                    args[i].method = method;
                    args[i].pBackSub_eachThread = pBackSub;
                    pthread_create(&threadId[i], &attr, temp4, &args[i]);
                    capture >> frame;
                }

                for (int i = 0; i < x; i++)
                {
                    pthread_join(threadId[i], NULL);
                    infile >> baseline_difference;
                    error += abs((args[i].qd/total) - stod(baseline_difference));
                }
            }
            
        }
    }
    else
    {
        
        Ptr<BackgroundSubtractor> pBackSub[x];
        
        while(true)
        {  
            queue_density = 0.0;  
            if (frame.empty())
            {
                break;
            }

            frame = homography(frame, 200, 500);          
        
            int width = frame.cols;
            int height = frame.rows;
            int GRID_SIZE = height / x ;
            int count = 0;

            if (x == 1)
            {
                // imshow("frame", frame);
                // waitKey(0);
                infile >> baseline_difference;
                if (first_time1)
                {
                    pBackSub[0] = createBackgroundSubtractorMOG2();
                    first_time1 = false;
                }
                args[count].frame = frame;
                args[count].method = method;
                args[count].pBackSub_eachThread = pBackSub[0];
                pthread_create(&threadId[count], &attr, temp4, &args[count]);
                pthread_join(threadId[count], NULL);
                queue_density += args[count].qd;
                error += abs((queue_density/total) - stod(baseline_difference));

            }
            else
            {
                for (int y = 0; y < height - GRID_SIZE; y += GRID_SIZE-1) 
                {
                     
                    // int k = width*y + width;
                    
                    if (!first_array[count])
                    {
                        pBackSub[count] = createBackgroundSubtractorMOG2();
                        first_array[count] = true;
                    }
                    
                    if (y <= height - 2*GRID_SIZE)
                    {
                        
                        Rect grid_rect(0, y, width-1, GRID_SIZE-2);
                        // rectangle(frame, grid_rect, Scalar(0, 255, 0), 1);
                        // imshow(to_string(count), frame(grid_rect));
                        // waitKey(0);
                        args[count].frame = frame(grid_rect);
                        args[count].method = method;
                    }
                    else
                    {
                        
                        Rect grid_rect(0, y, width-1, height - 1 - y);   
                        // rectangle(frame, grid_rect, Scalar(0, 255, 0), 1);
                        // imshow(to_string(count), frame(grid_rect));
                        // waitKey(0);
                        args[count].frame = frame(grid_rect);
                        args[count].method = method;
                    }

                    args[count].pBackSub_eachThread = pBackSub[count]; 
                    pthread_create(&threadId[count], &attr, temp4, &args[count]);
                    count++; 
                }
                infile >> baseline_difference;
                for (int i = 0; i < count; i++)
                {
                    pthread_join(threadId[i], NULL);
                    queue_density += args[i].qd;  
                }
                error += abs((queue_density/total) - stod(baseline_difference));
            }
            
            capture >> frame;
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

    capture >> frame;

    // computation of density for each frame
    while (true) 
    {
        
        // if next frame is empty break
        if (frame.empty())
        {
            break;
        }  
        
        

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
    
    double util = 0.0;

    if (method == 1)
    {   
        cout << "loading..." << endl;
        for (int i = 1; i <= 20; i++)
        {
            auto begin = chrono::high_resolution_clock::now();
            util = density(filename, method, i, 1920, 1080);
            auto finish = chrono::high_resolution_clock::now();
            if (util == -1.0)
            {
                return -1;
            }
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(finish - begin);
            utility << elapsed.count()/1000.0 << "\t" + to_string(util) << endl;
        }
        cout << "DONE" << endl;
    }
    else if (method == 2)
    {
        cout << "loading..." << endl;
        int ini_reso_x = 200;
        int ini_reso_y = 500;
        
        for (int i = 0; i < 10; i++)
        {
            auto begin = chrono::high_resolution_clock::now();
            util = density(filename, method, i, ini_reso_x, ini_reso_y);
            auto finish = chrono::high_resolution_clock::now();
            if (util == -1.0)
            {
                return -1;
            }
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(finish - begin);
            utility << elapsed.count()/1000.0 <<  "\t" + to_string(util) << endl;

            ini_reso_x = ini_reso_x/1.5;    
            ini_reso_y = ini_reso_y/1.5;
        }
        cout << "DONE" << endl;
    }
    else if (method == 3)
    {
        cout << "loading..." << endl;   
        for (int i = 1; i <= 20; i++)
        {
            auto begin = chrono::high_resolution_clock::now();
            util = method4(filename, method, i);
            auto finish = chrono::high_resolution_clock::now();
            if (util == -1.0)
            {
                return -1;
            }
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(finish - begin);
            utility << elapsed.count()/1000.0 << "\t" + to_string(util) << endl;
        }
        cout << "DONE" << endl;
    }
    else if (method == 4)
    {
        cout << "loading..." << endl;   
        for (int i = 1; i <= 20; i++)
        {
            auto begin = chrono::high_resolution_clock::now();
            util = method4(filename, method, i);
            auto finish = chrono::high_resolution_clock::now();
            if (util == -1.0)
            {
                return -1;
            }
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(finish - begin);
            utility << elapsed.count()/1000.0 << "\t" + to_string(util) << endl;
        }
        cout << "DONE" << endl;
    }
    
    utility.close();
    
    return 0;
}