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
void get_contours(Mat img,int& number_of_smarties);
int  get_smarties_count_and_display(char *filename,void*);
void prompt_and_exit(int status);
void print_message_to_file(FILE *fp, char message[]);
void CannyThreshold(Mat img);
void flood_fill(Mat img);
int get_max_contour_size(vector<vector<Point> > contours);

