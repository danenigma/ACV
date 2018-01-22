/* 
  Example use of openCV to perform image segmentation with the grabCut algorithm
  ------------------------------------------------------------------------------
  Interface file

  For a more sophisticated example, see http://docs.opencv.org/trunk/de/dd0/grabcut_8cpp-example.html

  David Vernon
  18 September 2017
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <algorithm>
//opencv
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

#define TRUE  1
#define FALSE 0
#define MAX_STRING_LENGTH   80
#define MAX_FILENAME_LENGTH 80
#define HAAR_FACE_CASCADE_INDEX 0
using namespace std;
using namespace cv;

/* function prototypes go here */

void performGrabCut(int, void*);  
void getControlPoints( int event, int x, int y, int, void*);
void prompt_and_exit(int status);
void prompt_and_continue();
void flood_fill(Mat filteredImage);