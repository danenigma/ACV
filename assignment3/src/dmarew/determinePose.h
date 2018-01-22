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
void get_pose_of_cube(char* fileName,Point& center,int& orientation);
void CannyThreshold(int, void*);
void HoughThreshold(int, void*);
void prompt_and_exit(int status);
void get_orientation_and_center(vector<Vec2f> lines);
void get_center(vector<Vec2f> lines);
void create_histogram_of_lines(vector<float> lineOrientation);
int find_perpendicular(int indexOfCurrent,vector<Vec2f> lines,int parLineIndex,int distance);
float computeDistanceBetweenLines(Vec2f line1,Vec2f line2);
void get_the_four_main_lines(vector<Vec2f> lines,int mainLineIndices[]);
void get_center_and_orientation(vector<Vec2i> bestLines,int& pos,Point& center,Point intersectionPoints[]);
Point get_intersection_point(Vec2i line1,Vec2i line2);
bool is_valid(float a);
int get_max_contour_index(vector<vector<Point> > contours);
vector<Vec2f> get_best_lines(Mat detected_edges);