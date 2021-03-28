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

int main(int argc, char* argv[])
{
    Mat src = imread("dog.png");

        int x = stoi(argv[1]);
        
        int width = src.cols;
        int height = src.rows;
        int GRID_SIZE = height/x;

        vector<Rect> mCells;

        if (x == 1)
        {
            imshow("src", src);
            waitKey(0);
        }
        while (true)
        {
            cout << "HI" << endl;
        }
        
        for (int y = 0; y < height - GRID_SIZE; y += GRID_SIZE) 
        {
            int k = width*y + width;
            if (y < height - 2*GRID_SIZE)
            {
                Rect grid_rect(0, y, width-1, GRID_SIZE);
                cout << grid_rect << endl;
                mCells.push_back(grid_rect);
                rectangle(src, grid_rect, Scalar(0, 255, 0), 1);
                imshow("src", src);
                imshow(format("grid%d",k), src(grid_rect));
                waitKey(0);
            }
            else
            {
                Rect grid_rect(0, y, width-1, height -1 - y);   
                cout << grid_rect << endl;
                mCells.push_back(grid_rect);
                rectangle(src, grid_rect, Scalar(0, 255, 0), 1);
                imshow("src", src);
                imshow(format("grid%d",k), src(grid_rect));
                waitKey(0);
            }
            
            
        }
}