#include "defectiveSweetsCounter.h"
void count_defective_sweets(Mat inputImage,int &total_defective_count,int &totalNumberOfColors,vector<colorTypes> &defectivePerColor){
   bool debug =false;
   int structuringElementSize = 5; 
   int defectiveSmartiesCount;
   int backgroundThreshold = 200;
   int gaussianKernelSize = 21;
   int gaussianStd = 1;
   
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
	/*
	if(debug)
	for (vector<colorTypes>::const_iterator i = finalColorTypesNoRedRedundance.begin(); i != finalColorTypesNoRedRedundance.end(); ++i){

		cout<<"color type: "<<i->center	<<" defective count: "<<i->defective_count<<endl;
	}
	*/
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
	//store copy of original image
   Mat floodFillInvertedImage,floodFillImage;
   floodFillImage = filteredImage.clone();
   // floodfill the filtered image starting from the top left corner(white)
   // the only black pixels in the output image would be the holes inside the sweets
   floodFill(floodFillImage, cv::Point(0,0), Scalar(255));
   // Invert floodFilled image so that the holes would be white
   bitwise_not(floodFillImage,floodFillInvertedImage);  
   // if it is in either the in the inverted flood filled image(hole) or the original image = forground(255 white)
   filteredImage = (filteredImage | floodFillInvertedImage);
}
bool is_defective(vector<Vec4i> contourDefects,int defectDepthThreshold){
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
	//the goal of this function is to combine all the red smarties into one color type
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
	RNG rng(12345);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

       // convex cavities

	findContours(filteredImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

	vector<Rect>   min_bounding_rectangle(contours.size()); ;
	vector<vector<Point>> hulls(contours.size());                    // convex hulls
	vector<vector<int>>   hull_indices(contours.size());             // indices of convex hulls
	vector<Vec4i> convexity_defects(contours.size()); 

	vector<colorTypes> inputImageColorTypes,currentContourColorType;
	int histogramTheshold = 80;
	double minHistogramFactor = 0.05;
	if (!contours.empty())		
		histogramTheshold = get_min_contour_size(contours)*minHistogramFactor;

	inputImageColorTypes = computeHueHistogramMaxima(inputImage,histogramTheshold);
	//cout<<"Histogram Threshold: "<<histogramTheshold<<endl;
	Mat currentSmartyImage;
	int currentSmartyColorIndex;
	int total_defective_count = 0;
	int contour_noise_threshold = 10;
	for (int contour_number=0; (contour_number<(int)contours.size()); contour_number++) {
		if (contours[contour_number].size() > contour_noise_threshold) {

			min_bounding_rectangle[contour_number] = boundingRect(contours[contour_number]);
			convexHull(contours[contour_number], hulls[contour_number]);
			convexHull(contours[contour_number], hull_indices[contour_number]);
			convexityDefects( contours[contour_number], hull_indices[contour_number], convexity_defects);
			currentSmartyImage = inputImage(min_bounding_rectangle[contour_number]);
			currentContourColorType = computeHueHistogramMaxima(currentSmartyImage,histogramTheshold);
			currentSmartyColorIndex = find_minimum_distance_match(inputImageColorTypes,currentContourColorType);
			//cout<<"current smarty is of color: "<<inputImageColorTypes[currentSmartyColorIndex].center<<endl;
			if (is_defective(convexity_defects,1000)){
				inputImageColorTypes[currentSmartyColorIndex].defective_count =inputImageColorTypes[currentSmartyColorIndex].defective_count+1;
				total_defective_count++;
			}
		}
	}
    defect_count = total_defective_count;
	return inputImageColorTypes;

}
vector<colorTypes> get_local_maxima(Mat img,int size){
	/*This function extracts the local maxima in img with in a window of size (size x size)*/
	bool debug = false;
	vector<colorTypes> colors;
	Mat localMaximizedImage;
	Mat temp; 
	if(debug)cout<<"histsize"<<img.size()<<endl;
	//structuring element
	Mat dilationElement = getStructuringElement(MORPH_RECT,Size(1,size));
	if(debug)cout<<"kernel size"<<dilationElement.size()<<endl;
	//dilate the image with rectangular kernel
	//results in a image where each point will have the value of local maxima in the window
	dilate(img,localMaximizedImage,dilationElement);
	//compare localMaximizedImage and the original image
	//local maxima are pixels that have the same values in both images
	//problem the background pixels will have the same values in both images
	compare(img,localMaximizedImage,temp,CV_CMP_EQ);
    if(debug)imshow("temp",temp);
	//to remove the background pixels from local maxima image 
	//we multiply it with the original image(so that 1./255*bgpixel(0)=0)
	Mat prod;
	if(debug)cout<<"temp: "<<temp<<endl;
	if(debug)cout<<"img type: "<<img.type()<<endl;
	Mat imgcopy;
	img.convertTo(imgcopy,CV_8UC3);
	
	if(debug)cout<<imgcopy<<endl;
	multiply(imgcopy,temp,prod);
	//here we are left with only the local maxima.
	if(debug)imshow("product",prod);
	Mat nonZeroCoordinates;
	findNonZero(prod, nonZeroCoordinates);

    for (int i = 0; i < nonZeroCoordinates.total(); i++ ) {
        if(debug)cout << "Zero#" << i << ": " << nonZeroCoordinates.at<Point>(i).x << ", " << nonZeroCoordinates.at<Point>(i).y << endl;
		colorTypes c;
		c.center = nonZeroCoordinates.at<Point>(i).y;
		c.defective_count = 0;
		colors.push_back(c);
	}

	return colors;
}
int get_min_contour_size(vector<vector<Point> > contours){

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