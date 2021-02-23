#include <opencv2/opencv.hpp>
using namespace cv;

int main( int argc, char** argv )
{
 	Mat image;			                     //mat object for storing data
 	image = imread( "traffic.jpg", IMREAD_COLOR );	         //read file
 	
 	Mat gray_image;
 	cvtColor( image, gray_image, COLOR_BGR2GRAY );             //convert image from grey to color
 	imwrite( "/home/turtle-27/Desktop/COP290_Traffic_Density/greyTraffic.jpg", gray_image );
 	// namedWindow( "ImageFile", 2 );	
 	// namedWindow( "Gray image", 2 );
 	// imshow( "ImageFile", image );	                   //show window containing images
 	// imshow( "Gray image", gray_image );
 	// waitKey(0);	     //to exit
 	return 0;
}