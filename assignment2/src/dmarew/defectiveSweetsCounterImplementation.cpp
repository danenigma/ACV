#include "defectiveSweetsCounter.h"
void count_defective_sweets(Mat inputImage,int &total_defective_count,int &totalNumberOfColors,vector<colorTypes> &defectivePerColor){
   bool debug =false;
   int structuringElementSize = 5; 
   int defectiveSmartiesCount;
   int backgroundThreshold = 200;
   
   char inputWindowName[MAX_STRING_LENGTH]            = "Input Image";
   char histogramWindowName[MAX_STRING_LENGTH]        = "Histogram";
   char binaryWindowName[MAX_STRING_LENGTH]        = "binary Image";
   
   namedWindow(inputWindowName,     CV_WINDOW_AUTOSIZE);  
   namedWindow(binaryWindowName,     CV_WINDOW_AUTOSIZE);  

   //namedWindow(histogramWindowName, CV_WINDOW_AUTOSIZE);
   Mat blurImage,grayscaleImage,filteredImage;
   /* convert from colour to greyscale if necessary */
   //check number of channels
   if (inputImage.type() == CV_8UC3) { // colour image
	   //blur image to remove noise
	   GaussianBlur(inputImage,blurImage,Size(21,21),1);	 
	   //remove white and shadow
	   remove_white_bg(blurImage,backgroundThreshold);
	   //convert to grayscale image
	   cvtColor(blurImage.clone(), grayscaleImage, CV_BGR2GRAY);
	   
    } 
   else {
	   //else use as is
        grayscaleImage = inputImage.clone();
    }
    
   /*Structuring elements*/
	Mat openElement  = getStructuringElement(MORPH_ELLIPSE,Size(structuringElementSize ,structuringElementSize ));
	
	
	
	/*====================================================================*/
	/*       Thresholding image and applying morphological operations     */
	/*====================================================================*/
	//threshold the image
	threshold(grayscaleImage,filteredImage,1,255,CV_THRESH_BINARY);
	//closing to fill holes
	flood_fill(filteredImage);
	morphologyEx(filteredImage,filteredImage,  MORPH_ERODE,openElement,Point(structuringElementSize/2,structuringElementSize/2),1);
	vector<colorTypes> distinctColorsIncludingDefect = get_distinct_colors_including_defect(filteredImage.clone(),inputImage.clone(),defectiveSmartiesCount);
	vector<colorTypes> finalColorTypesNoRedRedundance = combine_red_smarties(distinctColorsIncludingDefect);
	if(debug)
	for (vector<colorTypes>::const_iterator i = finalColorTypesNoRedRedundance.begin(); i != finalColorTypesNoRedRedundance.end(); ++i){

		cout<<"color type: "<<i->center	<<" defective count: "<<i->defective_count<<endl;
	}
	//cout<<"total defective smarties count:"<<defectiveSmartiesCount<<endl;
	//cout<<"total number of colors improved: "<<(int)finalColorTypesNoRedRedundance.size()<<endl;
	//errode to remove noise especially at the edges of the sweets 
	totalNumberOfColors = (int)finalColorTypesNoRedRedundance.size();
	total_defective_count = defectiveSmartiesCount;
	defectivePerColor = finalColorTypesNoRedRedundance;


	//only for displaying the histogram 
	computeHueHistogramMaxima(inputImage);

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
void flood_fill(Mat img){
   Mat im_floodfill = img.clone();
   floodFill(im_floodfill, cv::Point(0,0), Scalar(255));
    // Invert floodfilled image
   Mat im_floodfill_inv;
   bitwise_not(im_floodfill, im_floodfill_inv);  
    // Combine the two images to get the foreground.
   img = (img | im_floodfill_inv);
}
bool is_defective(vector<Vec4i> contourDefects,int defectDepthThreshold){
	for(int defect_index=0;defect_index<contourDefects.size();defect_index++){
		//cout<<"defective depth:"<<contourDefects[defect_index][3]<<endl;
		if (contourDefects[defect_index][3]>defectDepthThreshold && contourDefects[defect_index][3]<4*defectDepthThreshold){
				
				return true;
		}
				
		
	}

	return false;
}
int find_minimum_distance_match(vector<colorTypes> inputImageColorTypes,vector<colorTypes> currentContourColorType){
	
	int min_index = 0;
	int minim_distance = 1000;
	int distance;
	for (int color_number=0; (color_number<(int)inputImageColorTypes.size());color_number++) {

		//cout<<"centers: "<<inputImageColorTypes[color_number].center<<endl;
		distance  = abs(inputImageColorTypes[color_number].center-currentContourColorType[0].center);
		if(distance<minim_distance){
			min_index = color_number;
			minim_distance = distance;
		}

	
	}
		//cout<<"current contour color: "<<currentContourColorType[0].center<<endl;
	return min_index;
}
vector<colorTypes> combine_red_smarties(vector <colorTypes> finalColorTypes){
	vector<colorTypes> finalColorTypeNoRedRedundance;

	int red_count = 0;
	int red_index = 0;
	
	for (int color_number=0; (color_number<(int)finalColorTypes.size());color_number++) {
		if(finalColorTypes[color_number].center<5||finalColorTypes[color_number].center>250){
			if (red_count ==0){//first red
				
				finalColorTypeNoRedRedundance.push_back(finalColorTypes[color_number]);
				red_index = color_number;
				red_count ++;
			
			}
			else{
				finalColorTypeNoRedRedundance[red_index].defective_count = finalColorTypeNoRedRedundance[red_index].defective_count+
																   finalColorTypes[color_number].defective_count ;
				
			}
		}
		else{
			finalColorTypeNoRedRedundance.push_back(finalColorTypes[color_number]);
		}
	}
	
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

	inputImageColorTypes = computeHueHistogramMaxima(inputImage);

	Mat currentSmartyImage;
	int currentSmartyColorIndex;
	int total_defective_count = 0;
	for (int contour_number=0; (contour_number<(int)contours.size()); contour_number++) {
		if (contours[contour_number].size() > 10) {

			min_bounding_rectangle[contour_number] = boundingRect(contours[contour_number]);
			convexHull(contours[contour_number], hulls[contour_number]);
			convexHull(contours[contour_number], hull_indices[contour_number]);
			convexityDefects( contours[contour_number], hull_indices[contour_number], convexity_defects);
			currentSmartyImage = inputImage(min_bounding_rectangle[contour_number]);
			currentContourColorType = computeHueHistogramMaxima(currentSmartyImage);
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

