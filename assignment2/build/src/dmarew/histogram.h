/* 
  Example use of openCV to generate a histogram of an image
  ---------------------------------------------------------
  Interface file

  David Vernon
  19 May 2017
 
*/
 

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>
#include <iostream>
#include <string>
#include <conio.h>

//opencv
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
 

#define TRUE  1
#define FALSE 0
#define MAX_STRING_LENGTH 80
#define MAX_FILENAME_LENGTH 80

using namespace std;
using namespace cv;

struct colorTypes{

	int center;
	int defective_count;
};




/* function prototypes go here */
void generateHueHistogram(char *filename);
void generateHistogram(char *filename);
void prompt_and_exit(int status);
void prompt_and_continue();
vector<int> compute_local_maxima(Mat hist);
Mat convert_32bit_image_for_display(Mat& passed_image, double zero_maps_to/*=0.0*/, double passed_scale_factor/*=-1.0*/ );
void generateSatHistogram(Mat saturation);
vector<colorTypes> get_local_maxima(Mat img,int size);