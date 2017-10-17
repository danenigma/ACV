/*



Summary of Testing 
	limitation:
	- Can not handle too much overlaping
	- Works only for white background images

    Assumptions:

	- White background
	- The smarties have the relativly similar size. 


   sweetsCounter.h - interface file for a program that counts sweets in an image
   =============================================================================

   This program is for course 04-801 Applied Computer Vision, Assignment No. 1 
   The goal of this assignment is to count the number of sweets on an a image that has white background
   ==========================================================================================================
   Summary of the algorithm
   ==========================================================================================================
   -> Read file name from input.txt in the ../data/ folder.
   -> Read image ../data/filename (check if empty) 
   -> Call count_sweets() function on the image
	
		Step-1 BlurImage with a Gaussian kernel to remove noise(extract large structures)
		Step-2 Remove white and gray(shadow) background.
			 -We remove colors near and on the achromatic axis
		Step-3 Convert to gray scale image (if not grayscale already)
		Step-4 Threshold the image (binarize)
		Step-5 Morphological closing to fill small holes ( 5,5 ellipse structuring element 1 iterations)
		Step-6 Morphological erosion to remove noise( 5,5 ellipse structuring element 2 iterations)
   		Step-7 Compute the distance transform i.e the L2 distance(interms of the number of pixels) from background pixels
   		Step-8 Normalize the distance transform and convert it to 0-255(uint8)
  		Step-9 Extract only the local maxima with in some window (21x21)(None Maximum Suppression)
			 - The local maxima represent regions that are the farthest from the background in this case the centers of the
			   sweets.
			 - problem:
					if the sweets are overlaping, the overlapping region would have large L2 distance as well, hence resulting
					a local maxima
			 - Solution(see Step-11): coming up with the right threshold that will differentiate overlapping regions from the center of 
						the sweets
  		Step-10 Dilate the local maxima.
			- We dilate the local maxima so that local maxima that are very close to one another will join up
			  and become one sigle maximum
			- Typically we expect one local maxima per one sweet
  		Step-11 Compute the mean intensity of the distance transform image(dilated)
			
			- The mean intensity is the mean distance from the background(for the local maxima)
  			- Since the sweets are of similar size, we would expect their corresponding local maxima to have similar values.
			  That means local maxima that differ from the mean intensity by a significant amount are not sweets.
			  Mostly these local maxima arise from overlapping sweets. Hence the the overlapping factor (overlapParam)
			  determins the amount of overlapping we are willing to tolerate.(For the test images we find that 0.75 to be a
			  good factor. i.e local maxima with intensity(distance from background) less than 3/4th of the mean intensity
			  will be filtered out.
			- Since this threshold depends on the mean intensity it is adaptive to different images(we compute different thresholds
				for different images).
		Step-12 Theshold the distance transform image with the threshold we got by computing the mean intesity
		Step-13 Finally do Component Anaylsis i.e find all the contours(the number of contours=the number of sweets).
Summary of Testing
	- First I tested the algorithm with the provided test images 
	- After finding the right values for the different parameters I tested the algorithm on different images
	  By varying the number of sweets, the degree of overlap , size and camera viewing angle

	-1
		test:
			two overlapping sweets
		result:
			Correct count(2)
	-2 
		test:
			three sweets with two overlaps
		result:
			Correct count(3)
	-3 
		test:
			three sweets with three light overlaps
		result:
			Correct count(3)
	-4
		test:
			three sweets with three large overlaps
		result:
			Wrong count(2)
	-5	test:
			five sweets with five overlaps
		result:
			Correct count(5)
	-6 test:
			31 sweets with some overlap and slight variation in the 
			size of the sweets introduced by of the viewing angle of 
			the camera
			Wrong count(30) it counts all but one (which was to small compared 
							the other sweets)

	-7  test:
			38 sweets with some overlap(edited the original test image(30) sweets)
		result:
			Correct count(38)

	-8  test:
			small sweets slightly touching
			Correct count(8)
		result:

	-9  test:
			larg none overlapping sweets
			
		result:
			Correct count(28)





	limitation:
	- Can not handle too much overlaping
	- Works only for white background images(needs modification to work with other color backgrounds)

    Assumptions:

	- White background
	- The smarties have relativly similar sizes. 	
	
	
	Main parameters that needed tuning:
		1. Overlap allowed(for getting the threshold for the distance transform)
		2. Local maxima window size (depends on the size of the sweets workes well for the test images)
		3. backgroundness parameter (Achromatic axis) (any thing with bgr values > 240 or b=g=r)


   Author
   ------

   Daniel Marew
   10/11/2017


   Audit Trail
   -----------


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

void remove_white_bg(Mat img,int backgroundThreshold);
void get_local_maxima(Mat img,int size);
void displayMultilpleImages(Mat* imageList,int numberOfImages);

int count_sweets(int backgroundThreshold=240, int localMaximaSize=21,double overlapParam=0.8);
int get_threshold_from_image_stat(Mat img,double overlapParam);

vector<Point2f> get_contours_count(Mat filteredImage);
Mat convert_32bit_image_for_display(Mat& passed_image, double zero_maps_to=0.0, double passed_scale_factor=-1.0 );


