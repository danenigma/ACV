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
#define TRUE 1
#define FALSE 0
#define MAX_MESSAGE_LENGTH 81
#define MAX_FILENAME_LENGTH 80

/* function prototypes go here */

void prompt_and_exit(int status);
void print_message_to_file(FILE *fp, char message[]);
void count_smarties_with_morphology(int, void* );
void remove_white_bg(Mat img);
int get_contours_count(Mat src);
int get_max_contour_size(vector<vector<Point> > contours);
Mat convert_32bit_image_for_display(Mat& passed_image, double zero_maps_to=0.0, double passed_scale_factor=-1.0 );
void get_local_maxima(Mat img,int size);