/*
Algorthim

	blurImage with a gaussian
	remove white and gray(shadow) background(remove colors near the a chromatic axis)
	convert to gray scale
	threshold the image (binarize)
	floodfill or morphological closing to fill holes
	morphological erosion to remove noise
	Compute the distance transform
	Normalize the distance transform and convert it to 0-255
	Extract only the local maximas with in some window(21x21)
	dilate the local maxima so that local maximas that are very close to one another will join up(to avoid double counting)
	theshold the distance transform image
	do component anaylsis i.e find all the contours(the number of contours).
	prameters that needed tuning:
		1. overlap allowed
		2. local maxima window size
		3. backgroundness parameter (a chromatic axis) 

	limitation:
	when the smatries form a ring the flood fill will fillout the space inside 
	the ring resulting in a single structure.


*/
















#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>
#include <conio.h>
//opencv
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
#define DEBUG 0	
#define TRUE 1
#define FALSE 0
#define MAX_MESSAGE_LENGTH 81
#define MAX_FILENAME_LENGTH 80

/* function prototypes go here */

void prompt_and_exit(int status);
void print_message_to_file(FILE *fp, char message[]);
int count_smarties(int backgroundThreshold=240, int localMaximaSize=21,double overlapParam=0.8);
void remove_white_bg(Mat img,int backgroundThreshold);
vector<Point2f> get_contours_count(Mat filteredImage);
int get_max_contour_size(vector<vector<Point> > contours);
Mat convert_32bit_image_for_display(Mat& passed_image, double zero_maps_to=0.0, double passed_scale_factor=-1.0 );
void get_local_maxima(Mat img,int size);
int get_threshold_from_image_stat(Mat img,double overlapParam);
void displayMultilpleImages(Mat* imageList,int numberOfImages);