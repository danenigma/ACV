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
int count_smarties(int, void* );
void remove_white_bg(Mat img);
vector<Point2f> get_contours_count(Mat filteredImage);
int get_max_contour_size(vector<vector<Point> > contours);
Mat convert_32bit_image_for_display(Mat& passed_image, double zero_maps_to=0.0, double passed_scale_factor=-1.0 );
void get_local_maxima(Mat img,int size);
int get_threshold_from_image_stat(Mat img);
void flood_fill(Mat img);