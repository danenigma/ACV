/* 
  Example use of openCV to perform colour segmentation
   
  The user must interactively select the colour sample that will 
  form the basis of the segmentation. The user can also adjust the
  hue and saturation tolerances on that sample
  ------------------------------------------------------------
  Interface file

  David Vernon
  1 June 2017
 
*/
 

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>
#include <iostream>
#include <string>
#include <conio.h> 
#include <sys/types.h> 
#include <sys/timeb.h>

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

/* function prototypes go here */

void colourSegmentation(int, void*);
void getSamplePoint( int event, int x, int y, int, void*);
void prompt_and_exit(int status);
void prompt_and_continue();
void pause(int milliseconds);
int processImage(Mat img);
void remove_white_bg(Mat img);
int get_count_for_hue(Mat img, int hue,int hueRange);
int get_total_count(Mat segmentedImage,int colorArray[],int hueRange,int colorArraySize=8);
int get_max_contour_size(vector<vector<Point> > contours);
int get_contours(Mat threshImage);
void disp_hist(Mat hslImage);
void get_kmeans_clustor(Mat src);