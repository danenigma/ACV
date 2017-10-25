/* 
  Example use of openCV to generate a histogram of an image
  ---------------------------------------------------------
  Interface file

  David Vernon
  19 May 2017

  Modified By
  ------------
  Daniel Marew 
  10/25/2017

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
/*struct to hold the local maxima value (hue value) of a distinct color and its defective count*/
struct colorTypes{

	int center;
	int defective_count;
};




/* function prototypes go here */

void prompt_and_exit(int status);
void prompt_and_continue();


vector<colorTypes> computeHueHistogramMaxima(Mat inputImage,int min_histogram_threshold=80);
