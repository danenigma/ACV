/*



   defectiveSweetsCounter.h - interface file for a program that counts the different sweet colors and the number of defective sweets of each 
   color type in an image
   =============================================================================================================

   This program is for course 04-801 Applied Computer Vision, Assignment No. 2 
   The goal of this assignment is to count the different sweet color types and the number of defective sweets of each 
   color type in an image
   ==========================================================================================================
   Summary of the algorithm
   ==========================================================================================================
   -> Read file name from input.txt in the ../data/ folder.
   -> Read image ../data/filename (check if empty) 
   -> Call count_defective_sweets() function on the image
	
		Step-1 BlurImage with a Gaussian kernel(21x21 1 s.d) to remove noise(extract large structures)
		Step-2 Remove white and gray(shadow) background.
			 -We remove colors near and on the achromatic axis
		Step-3 Convert to gray scale image (if not grayscale already)
		Step-4 Threshold the image (binarize)
		Step-5 flood_fill to fill small holes (using a flood fill routine)
		Step-6 Morphological erosion to remove noise( 5,5 ellipse structuring element 1 iteration)
   		Step-7 call get_distinct_colors_including_defect() function on the filtered image
			   - Find all the valid contours(contour size >10)
			   - find the minimum area among the contours (used as a threshold for next step)
			   - call computeHueHistogramMaxima(inputImage)
					- extract the hue of the inputImage(min saturation = 30)
					- compute the hue histogram (bin size = 255)
					- gaussian blur to remove noise in the hue histogram(size 7x7 1 sd)
					- remove hue values with small frequencies( other wise they will be counted as local maxima later)
						Previous Implementation(has color domination problem)
						
								- compute the maximum frequency in the hue histogram
								- use that to estabilish a threshold for low frequency hue histogram(5% of the maximum was used)
								- remove hue values lower than 5% of the maximum hue histogram value
						Current Implementation
							    - take the threshold to be 5% of the minimum area found above (NB. contour area approx No. pixels in the contour).
					- compute the local maxima in the filtered hue histogram
						- call get_local_maxima() fun
						- it returns the local maxima in the hue histogram( as a struct that has the centers of the histogram maxima and defective count
							default value = 0)
				- For each contour (with size greater than a threhold (10) otherwise count as noise)
					- get the minimum bounding box, convexhull and convexity defects
					- using the minimum bounding box crop the region in the input image
					- compute its hue histogram by calling computeHueHistogramMaxima(inputImage[ROI])
					- call find_minimum_distance_match() on the hue histogram of the current sweet
						- compare the local maximum of the hue histogram of the current sweet and the local maxima of the histogram of the input image
						- return the index of the local maxima in the input image that is the closest to the local maximum of the current sweet.
					- check if the current sweet has defects by calling is_defective() fun
						- check if the depth of the defect in the contour greater than certain threshold (1000 was used)
						- if so we count it as a defect
					- If there is a defect, increment the number of defects in the color type of the current sweet and also the total defective count. 
		Step-8  Combining the red sweets by calling combine_red_smarties() fun we need to do this because red sweets
				typically will have to local maxima in the histogram hence will be counted as two.
				-loop through all the local maxima(color types) of the hue histogram of the input image
					- accumulate all the red sweets i.e increament number of defective count of red sweets whenever we find one.
						red sweets is a sweet with hue <5 or hue >255
		Step-9 Display the Hue Histogram at different stages of processing.


Summary of Testing
	- First I tested the algorithm with the provided test images 
	- After finding the right values for the different parameters I tested the algorithm on different images
	  By varying the number of sweets (starting from 0),number of defects, combination of color types, color domination and finally size.

	-1  test:
			0 sweets white background
		result:
			Correct Defective count , Correct Color type count
	-2  test:
			1 sweet not defective red color 
		result:
			Correct Defective count(0) , Correct Color type count(1),Correct defective count per color type (0)
	-3  test:
			2 sweets both defective 1 brown and 1 orange colors (most difficult to separate in the Hu Histogram)
		result:
			Correct Defective count(1) , Correct Color type count(1),Correct defective count per color type (1,1)
			
	-4  test:
			8 red sweets 3 defective (to test how well the final step of the algorithm i.e(combining red sweets)works)
		result:
			Correct Defective count(3) , Correct Color type count(1) , Correct defective count per color type (3)
			
	-5	test:
			20 sweets with 8 different colors 18 defective
		result:
			Correct Defective count(18) , Correct Color type count(8), (Can't definitly tell but looks correct)(compare the number of  green sweets with the next test)
			
	-6  test:
			20 sweets with 8 different colors 19 defective (what makes it different from the test above is all the greens are defective)
		result:
			Correct Defective count(19) , Correct Color type count(8), (Can't definitly tell but looks correct)(compare the number of greens with previous test)

	-7  test:
			8 red and 1 green sweets 3 of the reds and the green sweets are defective(to check for color domination)
		result:
			Correct Defective count(4) , Correct Color type count(2),Correct defective count per color type (3,1)
		

	-8  test:
		Previous Implementation of histogram thresholding
			16 red and 1 green sweets 5 of the reds and the green sweets are defective(to check for color domination)
			result:
				Correct Defective count(6), Wrong Color type count(1), Wrong defective count per color type (6) (because the reds will
				overwhelm the green sweet so much that it would be considered noise)
		Current Implementation of histogram thresholding
			result:
				Correct Defective count(6) , Correct Color type count(2),Correct defective count per color type (5,1)
	-9  test:
			30 larg overlapping sweets 8 different colors no defectives (to test the robustness of the color segmentation but
			we expect wrong defective count because we didn't do anything to split them up)
			
		result:
			 Wrong Defective count(as expected), Correct Color type count(8) , Wrong defective per color count(as expected)
			

	limitation:
	- Can not handle overlapping
	- Works only for white background images(needs modification to work with other color backgrounds)
    Assumptions:
	- White background
	
		
	
	Main parameters that needed tuning:
		1. defective depth treshold (1000)
		2. backgroundness parameter (Achromatic axis) (any thing with bgr values > 200 or b=g=r) 
		3. redness thresholds(<5 and >250)
		4. contour noise threshold (10)
		5. Minimum Histogram Thresholding Factor (5%)
		6. Histogram bluring (Gaussian kernel size = 7x7)



   Author
   ------

   Daniel Marew
   10/24/2017


   Audit Trail
   -----------


*/

#include "histogram.h"
void count_defective_sweets(Mat inputImage,int &total_defective_count,int &totalNumberOfColors,vector<colorTypes> &defectivePerColor);
void remove_white_bg(Mat img,int backgroundThreshold);
void flood_fill(Mat img);
vector<colorTypes>  get_distinct_colors_including_defect(Mat filteredImage,Mat inputImage,int& defect_count);
bool is_defective(vector<Vec4i> defect,int defectDepthThreshold);
vector<colorTypes> computeHueHistogramMaxima(Mat inputImage);
int find_minimum_distance_match(vector<colorTypes> inputImageColorTypes,vector<colorTypes> currentContourColorType);
vector<colorTypes>  combine_red_smarties(vector <colorTypes> finalColorTypes);
vector<colorTypes> get_local_maxima(Mat img,int size);
int get_min_contour_size(vector<vector<Point> > contours);
void displayMultilpleImages(Mat* imageList,int numberOfImages);