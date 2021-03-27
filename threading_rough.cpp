#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <fstream>
#include <time.h>
#include <pthread.h>



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
        for (int i = 1; i <= 20; i++)
        {
            start = clock();
            util = density(filename, method, i, 1920, 1080);
            end = clock();
            if (util == -1.0)
            {
                return -1;
            }
            runtime = (end - start) / CLOCKS_PER_SEC;
            // runtime = runtime / CLOCKS_PER_SEC;
            utility << to_string(runtime) + "\t" + to_string(util) << endl;
        }
    }
    else if (method == 2)
    {
        cout << "loading..." << endl;
        int ini_reso_x = 400;
        int ini_reso_y = 1000;
        
        for (int i = 0; i < 8; i++)
        {
            ini_reso_x = ini_reso_x/2;    
            ini_reso_y = ini_reso_y/2;
            
            start = clock();
            util = density(filename, method, i, ini_reso_x, ini_reso_y);
            end = clock();
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
        for (int i = 5; i <= 12; i++)
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