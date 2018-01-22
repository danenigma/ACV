/* 
   defeterminePose.h - interface file for a program that determines the Pose a cube 
   =============================================================================================================

   This program is for course 18799-RW Applied Computer Vision, Assignment No. 3 

   ==========================================================================================================
   Summary of the algorithm
   ==========================================================================================================
   -> Read file name from input.txt in the ../data/ folder.
   -> Call get_pose_of_cube(fileName)
		-> Read image ../data/filename (check if empty) 
		-> Convert to gray scale image
		-> Gaussian blurring with sigma =2 and kernel size = (31x31)
		-> Canny Edge detection with thresh1 = 120 and thresh2 = 3*thresh1
		-> Extract contours in the edge image
		-> From among the contours pick the biggest by calling get_max_contour_index()
		-> Find minimum bounding box for that contour
		-> get the center of the bounding box. that would be the center of the cube
		-> get the best lines by gradually incrementing the hough threshold (between 30-100)
		-> pick one of the best lines and the orientation if this line is taken as the orientation of the cube
	    ->return center and orientation(Pose)
 Summary of Testing
	- First I tested the algorithm with the provided test images 
	- After finding the right values for the different parameters I tested the algorithm on different images
	  By varying the pose of the cube and its size (see test images).
	- since we don't have the ground truth pose it is difficult to quantify the error the algorithm makes 
	  but by visual inspection it seems to be doing the right thing.


   Author
   ------

   Daniel Marew
   11/18/2017


   Audit Trail
   -----------
*/

 

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <cmath>

#include <algorithm> // for std::find
#include <iterator> // for std::begin, std::end
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
void prompt_and_exit(int status);
void get_pose_of_cube(char* fileName,Point& center,int& orientation);
int get_max_contour_index(vector<vector<Point> > contours);
vector<Vec2f> get_best_lines(Mat detected_edges);