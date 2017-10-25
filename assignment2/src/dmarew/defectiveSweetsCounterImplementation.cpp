/*
   Author
   ------

   Daniel Marew
   10/25/2017

*/

#include "defectiveSweetsCounter.h"



/*Implementation file for defectiveSweetsCounter.h */

void count_defective_sweets(Mat inputImage,int &total_defective_count,int &totalNumberOfColors,vector<colorTypes> &defectivePerColor){
	/*

	This function counts the different sweet color types and the number of defective sweets of each 
    color type in an image[for the Algorithm consult the interface file defectiveSweetsCounter.h]
	===================================================================================================================
	Input
	----------------------
	inputImage : Mat 
	total_defective_count : int& for output
	totalNumberOfColors : int& for output
	defectivePerColor:vector<colorTypes> &
	----------------------
	
	*/
   bool debug =false;
   int structuringElementSize = 5; 
   int defectiveSmartiesCount;
   int backgroundThreshold = 200;
   int gaussianKernelSize = 21;
   int gaussianStd = 1;
   /*window names */
   char inputWindowName[MAX_STRING_LENGTH]         = "Input Image";
   char histogramWindowName[MAX_STRING_LENGTH]     = "Histogram";
   char binaryWindowName[MAX_STRING_LENGTH]        = "binary Image";
   
   namedWindow(inputWindowName,     CV_WINDOW_AUTOSIZE);  
   namedWindow(binaryWindowName,     CV_WINDOW_AUTOSIZE);  


   Mat blurImage,grayscaleImage,filteredImage;
   /* convert from colour to greyscale if necessary */
   //check number of channels
   if (inputImage.type() == CV_8UC3) { // colour image
	   //blur image to remove noise
	   GaussianBlur(inputImage,blurImage,Size(gaussianKernelSize,gaussianKernelSize),gaussianStd);	 
	   //remove white and shadow
	   remove_white_bg(blurImage,backgroundThreshold);
	   //convert to grayscale image
	   cvtColor(blurImage.clone(), grayscaleImage, CV_BGR2GRAY);
	   
    } 
   else {
	   //only works for color images
       return;
    }
    
   /*Structuring elements*/
	Mat errodElement  = getStructuringElement(MORPH_ELLIPSE,Size(structuringElementSize ,structuringElementSize ));
	
	
	
	/*=========================================================================================*/
	/*                 Thresholding image and applying morphological operations                */
	/*=========================================================================================*/
	//threshold the image
	threshold(grayscaleImage,filteredImage,1,255,CV_THRESH_BINARY);
	//flood filling to fill holes
	flood_fill(filteredImage);
	// errosion to remove small noise 
	morphologyEx(filteredImage,filteredImage,  MORPH_ERODE,errodElement,Point(structuringElementSize/2,structuringElementSize/2),1);
	/*========================================================================================*/
	/* get distinct colors including their corrosponding defective count with red redundance  */
	/*========================================================================================*/	
	
	vector<colorTypes> distinctColorsIncludingDefect  = get_distinct_colors_including_defect(filteredImage.clone(),inputImage.clone(),defectiveSmartiesCount);
	/*========================================================================================*/
	/*                                 remove red redundance                                  */
	/*========================================================================================*/	
	vector<colorTypes> finalColorTypesNoRedRedundance = combine_red_smarties(distinctColorsIncludingDefect);

	if(debug)//shows colours are listed in increasing hue value.
	for (vector<colorTypes>::const_iterator i = finalColorTypesNoRedRedundance.begin(); i != finalColorTypesNoRedRedundance.end(); ++i){

		cout<<"color type: "<<i->center	<<" defective count: "<<i->defective_count<<endl;
	}
	
	//returning the total number of colors and there corrosponding defective count to be displayed in the application file
	totalNumberOfColors = (int)finalColorTypesNoRedRedundance.size();
	total_defective_count = defectiveSmartiesCount;
	defectivePerColor = finalColorTypesNoRedRedundance;


	//only for displaying the histograms 
	computeHueHistogramMaxima(inputImage,80);
	//display input image
	imshow(inputWindowName,inputImage);
	imshow(binaryWindowName,filteredImage);


}
void remove_white_bg(Mat img,int backgroundThreshold){
/*
	This function removes pixels with values on or close to the Achromatic axis 
	===========================================================================
	Input
	------------------------
	img: image (BGR image )
	backgroundThreshold: int (threshold for measure of whiteness)
*/

	int row, col;
	for (row=0; row < img.rows; row++) {
			   for (col=0; col < img.cols; col++) {

			    /*assign colors on the achromatic axis(near) zero value(removing gray and white pixels)*/
	            if (((img.at<Vec3b>(row,col)[0] >= backgroundThreshold) &&
	                (img.at<Vec3b>(row,col)[1]  >= backgroundThreshold) &&
	                (img.at<Vec3b>(row,col)[2]  >= backgroundThreshold))//all b,g and r > threshold
					|//or on the achromatic axis
					((img.at<Vec3b>(row,col)[0]==img.at<Vec3b>(row,col)[1])&&
					 (img.at<Vec3b>(row,col)[1]==img.at<Vec3b>(row,col)[2]))) {//if white
                   //assign 0 to background pixels
	               img.at<Vec3b>(row,col)[0] =0; 
	               img.at<Vec3b>(row,col)[1] =0; 
	               img.at<Vec3b>(row,col)[2] =0; 
	            }

	         }
	      }

}
void flood_fill(Mat filteredImage){
	/*
	
	This function is used for filling holes in the sweets 
	(idea taken from online discussion forums(stackoverflow and opencv answers))
	==========================================================
	input:
		filteredImage : (Mat) preprocessed binary image
	output:
		it modifies the input image
	*/

   //store a copy of original image
   Mat floodFillInvertedImage,floodFillImage;
   floodFillImage = filteredImage.clone();
   // floodfill the filtered image starting from the top left corner(white)
   // the only black pixels in the output image would be the holes inside the sweets
   floodFill(floodFillImage, cv::Point(0,0), Scalar(255));
   // Invert floodFilled image so that the holes would be white
   bitwise_not(floodFillImage,floodFillInvertedImage);  
   // if it is in either the in the inverted flood filled image(hole) or the original image = foreground(255 white)
   filteredImage = (filteredImage | floodFillInvertedImage);
}
bool is_defective(vector<Vec4i> contourDefects,int defectDepthThreshold){
	/*
	
	this function checks whether or not a sweet is defective
	========================================================
	input
	-----
		contourDefects : (vector<Vec4i>) a vector of defects of a single sweet
		defectDepthThreshold:(int) sweet with one or more  defect depth greater than this value is going to be cosidered defective
	output
	-----
		(boolean) true if defective false otherwise
	
	*/
	//for each defect in the current contour
	for(int defect_index=0;defect_index<contourDefects.size();defect_index++){
		//when the defect depth of the current contour is greater than the threshold stop iterating say it is defective
		if (contourDefects[defect_index][3]>defectDepthThreshold /*&& contourDefects[defect_index][3]<10*defectDepthThreshold*/){
				
				return true;
		}
					
	}
	//not defective otherwise
	return false;
}
int find_minimum_distance_match(vector<colorTypes> inputImageColorTypes,vector<colorTypes> currentContourColorType){

	/*

	this function returns the index of input image local maxima (in the hue histogram) that is the closest 
	to the current sweet contour color type (put diffently it finds the colortype of the current sweet)
	=================================================================================================
	input
	-----
		
		inputImageColorTypes: (vector<colorTypes>) holds the centers of the local maxima of the input image hue histogram(color template)
		currentContourColorType:(vector<colorTypes>) holds the center of the maximum of the current sweet image hue histogram
	output
	------
		index(int) index in the vector inputImageColorTypes(color type)
	
	*/


	//given the local maxima for the inputImage(color template) find the color template in the input histogram
	//that is the closest to the current color type local maximum
	int min_index = 0;
	int minim_distance = 1000;//some large number actually min dist <255
	int distance;
	//loop through all the color types
	for (int color_number=0; (color_number<(int)inputImageColorTypes.size());color_number++) {

		//compute L1 distance between the two centers
		distance  = abs(inputImageColorTypes[color_number].center-currentContourColorType[0].center);
		if(distance<minim_distance){// if less than current min distance update min distance
			min_index = color_number;
			minim_distance = distance;
		}

	
	}
	//return the index of the color type (from input color type templates)that is the closest to
	//the color of the current contour
	return min_index;
}
vector<colorTypes> combine_red_smarties(vector <colorTypes> finalColorTypes/*with red redundance*/){
	/*
	the goal of this function is to combine all the red smarties into one color type
	================================================================================
	input
	-----
		finalColorTypes:	(vector <colorTypes>) color types with red redundance (two reds typically)
	output
	-----
		finalColorTypeNoRedRedundance : (vector <colorTypes>) color types without red duplicates
	*/
	vector<colorTypes> finalColorTypeNoRedRedundance;

	int red_count = 0;
	int red_index = 0;
	//loop through all the color types
	for (int color_number=0; (color_number<(int)finalColorTypes.size());color_number++) {
		//if it is red
		if(finalColorTypes[color_number].center<5||finalColorTypes[color_number].center>250){
			if (red_count ==0){//first red
				//push to final output vector
				finalColorTypeNoRedRedundance.push_back(finalColorTypes[color_number]);
				red_index = color_number;//remember the index of red in the color type list 
				red_count ++;//increment red count
			
			}
			else{
				//because we remember the index of the red smarties
				//increament defective count
				finalColorTypeNoRedRedundance[red_index].defective_count = finalColorTypeNoRedRedundance[red_index].defective_count+
																   finalColorTypes[color_number].defective_count ;
				
			}
		}
		else{//if it is not read just push to the output vector
			finalColorTypeNoRedRedundance.push_back(finalColorTypes[color_number]);
		}
	}
//return colorType array with no red redundance	
return finalColorTypeNoRedRedundance;
}
vector<colorTypes> get_distinct_colors_including_defect(Mat filteredImage,Mat inputImage,int& defect_count){
	
	/*
	
	This function extracts the different color types and their defective count
	input
	-----
		filteredImage : (Mat) preprocessed binary image
		defect_count  : (int&) is going to hold the total defective count
	output
	-----
		(vector<colorTypes>) the different color types and their defective count
	
	*/










	Mat currentSmartyImage;
	
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	/*find the contours*/
	findContours(filteredImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

	vector<Rect>   min_bounding_rectangle(contours.size()); ;
	vector<vector<Point>> hulls(contours.size());                    // convex hulls
	vector<vector<int>>   hull_indices(contours.size());             // indices of convex hulls
	vector<Vec4i> convexity_defects(contours.size()); 

	vector<colorTypes> inputImageColorTypes,currentContourColorType;
	
	int histogramTheshold = 80;
	int currentSmartyColorIndex;
	int total_defective_count = 0;
	int contour_noise_threshold = 10;
	int isDefectiveThreshold = 1000;
	double minHistogramFactor = 0.05;//5% of the smallest area contour
	/*if contour is not empty get the area of the smallest contour for histogram threshold selection*/
	if (!contours.empty())		
		histogramTheshold = get_min_contour_size(contours)*minHistogramFactor;
	/*compute the hue histogram of the input image (color template)*/
	inputImageColorTypes = computeHueHistogramMaxima(inputImage,histogramTheshold);
	
	
	/*for each contour with size >threshold */
	for (int contour_number=0; (contour_number<(int)contours.size()); contour_number++) {
		if (contours[contour_number].size() > contour_noise_threshold) {
			/*get the minimum bounding box*/
			min_bounding_rectangle[contour_number] = boundingRect(contours[contour_number]);
			/*get the convex hull*/
			convexHull(contours[contour_number], hulls[contour_number]);
			convexHull(contours[contour_number], hull_indices[contour_number]);
			/*get the convexity defects for the current contour*/
			convexityDefects( contours[contour_number], hull_indices[contour_number], convexity_defects);
			/*creat an ROI image for the current sweet*/
			currentSmartyImage = inputImage(min_bounding_rectangle[contour_number]);
			/*compute its hue histogram*/
			currentContourColorType = computeHueHistogramMaxima(currentSmartyImage,histogramTheshold);
			/*compare with the template and get the right color type of the current sweet*/
			currentSmartyColorIndex = find_minimum_distance_match(inputImageColorTypes,currentContourColorType);
			/*check if it is defective*/
			if (is_defective(convexity_defects,isDefectiveThreshold)){
				/*if defective increment the number of defects in the colortype of the current sweet*/
				inputImageColorTypes[currentSmartyColorIndex].defective_count =inputImageColorTypes[currentSmartyColorIndex].defective_count+1;
				/*increment the total defective count*/
				total_defective_count++;
			}
		}
	}
    defect_count = total_defective_count;
	return inputImageColorTypes;

}
vector<colorTypes> get_local_maxima(Mat histogram,int size){
	/*
	
	This function extracts the local maxima in a histogram with in a window of size (size)
	====================================================================================
	input
	-----
		histogram: (Mat)
		size: (int) size of window we are going to look for a local maxima
	output
	-----
		(vector<colorTypes>) vector of the color type centers(local maxima)
	
	
	
	*/
	bool debug = false;
	vector<colorTypes> colors;
	Mat localMaximizedImage;
	Mat temp; 
	Mat product;
	Mat histogramCopy;
	Mat nonZeroCoordinates;
	//structuring element
	Mat dilationElement = getStructuringElement(MORPH_RECT,Size(1,size));
	//dilate the image with rectangular kernel
	//results in a image where each point will have the value of local maxima in the window
	dilate(histogram,localMaximizedImage,dilationElement);
	//compare localMaximizedImage and the original image
	//local maxima are pixels that have the same values in both images
	//problem the background pixels will have the same values in both images
	compare(histogram,localMaximizedImage,temp,CV_CMP_EQ);
    //to remove the background pixels from local maxima image 
	//we multiply it with the original image(so that 1./255*bgpixel(0)=0)

	histogram.convertTo(histogramCopy,CV_8UC3);//we don't to modify the original hist
	
	multiply(histogramCopy,temp,product);
	//here we are left with only the local maxima.
	//find non zero pixels(local maxima(color type modes in the histogram))
	findNonZero(product, nonZeroCoordinates);
	//push each local maxima to color type vector
    for (int i = 0; i < nonZeroCoordinates.total(); i++ ) {
        if(debug)cout << "Zero#" << i << ": " << nonZeroCoordinates.at<Point>(i).x << ", " << nonZeroCoordinates.at<Point>(i).y << endl;
		colorTypes c;
		c.center = nonZeroCoordinates.at<Point>(i).y;
		c.defective_count = 0;//default value
		colors.push_back(c);
	}

	return colors;
}
int get_min_contour_size(vector<vector<Point> > contours){
 /*
 
 this function returns the area(approx number of pixels. since opencv uses greens theorem 
 they are not equal but good enough approximation)

 input
 -----
	contours: (vector<vector<Point> > ) a list of contours 
 output
 -----
	(int) the area of the smallest contour 
 
 
 
 */

  int smallest_contour_area=contourArea( contours[0],false);
  double current_contour_area;

  for( int i = 1; i< contours.size(); i++ ) // iterate through each contour. 
      {
       current_contour_area = contourArea( contours[i],false);  //  Find the area of contour
	  
       if(current_contour_area < smallest_contour_area){
       smallest_contour_area = current_contour_area;

       }
      }
return smallest_contour_area;
}

void displayMultilpleImages(Mat* imageList,int numberOfImages){
	/*
	This function displays upto 6 images on the same window
	=======================================================
	Input
	---------------------
	imageList: list of images(Mat objects)
	numberOfImages: the number of images in the imageList
	
	
	*/
	int space = 10;
	//large image for putting all the images as ROIs
	Mat DispImage = Mat::zeros(Size(3*imageList[0].cols + 2*space,imageList[0].rows), CV_8UC3);
	double xScale = 1;
	double yScale = 1;
	int xStart = 0;
	int xCounter = 0;
	int yStart = 0;
	for(int i=0;i<numberOfImages;i++){

		Mat currentImg;
		if (imageList[i].type()==CV_8UC1) {
				  //input image is grayscale
			cvtColor(imageList[i], currentImg, CV_GRAY2RGB);
		}else{//input image is color

			currentImg = imageList[i];
		}

		Mat temp;
		if(i==3){//go to the second row
			yStart = (int)(currentImg.rows/yScale) + space;
			xCounter=0;
		}
		xStart=xCounter*(currentImg.cols/xScale + space);
		Rect ROI(xStart,yStart,(int)(currentImg.cols/xScale),(int)(currentImg.rows/yScale));
		resize(currentImg,temp, Size(ROI.width, ROI.height));
		temp.copyTo(DispImage(ROI));
		xCounter++;
		
		}
	
		resize(DispImage,DispImage,Size(1000,300));
		imshow("Hue Histogram At Different Processing Steps",DispImage);//show processing steps
	
}