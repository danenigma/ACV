
 
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>

//opencv
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

using namespace cv;

#define TRUE 1
#define FALSE 0

/* function prototypes go here */
void detectAndDisplay( Mat frame ,CascadeClassifier face_cascade,CascadeClassifier eyes_cascade);

