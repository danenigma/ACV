

#include "defectiveSweetsCounter.h"




int count_defective_sweets(int backgroundThreshold/*=190*/, int localMaximaSize/*=21*/,double overlapParam/*=0.75*/){

	/*

	This function counts the number of sweets in an image[for the Algorithm consult the interface file sweetsCounter.h]
	===================================================================================================================
	Input
	----------------------
	backgroundThreshold : int 
	localMaximaSize :     int (determines the size of the window where we compute the local maximum)
	overlapParam:         double (determines the amount of overlapping we can tolerate the bigger
						  the overlapParam the smaller the overlap we tolerate)
    Output
	----------------------
	smartiesCount: int (the number of sweets in the image)
	
	*/



   extern Mat inputImage; 
   /*Window names*/
   char* inputWindowName = "Input Image";
   char* inputBgRemovedWindowName = "Input Image without background";
   char* DTInputWindowName = "Distance Transform Input";


   /*init windows*/
   namedWindow(inputWindowName, CV_WINDOW_AUTOSIZE);
   namedWindow(inputBgRemovedWindowName, CV_WINDOW_AUTOSIZE );
   namedWindow(DTInputWindowName, CV_WINDOW_AUTOSIZE );

   
   /* images at different stages of processing*/
   Mat grayscaleImage;
   Mat dtInputImage;
   Mat filteredImage;
   Mat blurImage;
   Mat distTransform;
   Mat distTransformUint;
   Mat distTransformBinaryImage;
   Mat distTransformDilatedImage;
   Mat distTransformLocalMaximaImage;

   int structuringElementSize = 5; 
   int defectiveSmartiesCount;


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
	Mat erodeElement  = getStructuringElement(MORPH_ELLIPSE,Size(structuringElementSize ,structuringElementSize ));
	
	
	/*====================================================================*/
	/*       Thresholding image and applying morphological operations     */
	/*====================================================================*/
	//threshold the image
	threshold(grayscaleImage,filteredImage,1,255,CV_THRESH_BINARY);
	//closing to fill holes
	flood_fill(filteredImage);
	//errode to remove noise especially at the edges of the sweets 
	morphologyEx(filteredImage,filteredImage,  MORPH_ERODE,erodeElement,Point(-1,-1),2);


	
	/*====================================================================*/
	/*       Connected Component Analysis to count defective sweets   */
	/*====================================================================*/

	defectiveSmartiesCount = count_defective_sweets(filteredImage);
	displayHist(inputImage);
	imshow(inputWindowName,inputImage);
	imshow(inputBgRemovedWindowName,blurImage);
	imshow(DTInputWindowName,filteredImage);


	//Mat imageList[] = {dtInputImage,distTransformUint,distTransformLocalMaximaImage,
	//display different stages of processing on a single window
	//displayMultilpleImages(imageList,6);

return defectiveSmartiesCount;//smartiesCount;//return the number of sweets
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
	Mat DispImage = Mat::zeros(Size(imageList[0].cols + space,imageList[0].rows + space), CV_8UC3);
	int xScale = 3;
	int yScale = 2;
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
		xStart=xCounter*currentImg.cols/xScale;
		Rect ROI(xStart,yStart,(int)(currentImg.cols/xScale),(int)(currentImg.rows/yScale));
		resize(currentImg,temp, Size(ROI.width, ROI.height));
		temp.copyTo(DispImage(ROI));
		xCounter++;
		
		}
	
	
		imshow("Processing Steps",DispImage);//show processing steps
	
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
int count_defective_sweets(Mat binaryImage){
	vector<vector<Point>> contours;
	vector<Vec4i>         hierarchy;

	findContours(binaryImage.clone(),contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_NONE);

   /* extract features from the contours */

	vector<RotatedRect>   min_bounding_rectangle(contours.size());   // bounding rectangles
	vector<vector<Point>> hulls(contours.size());                    // convex hulls
	vector<vector<int>>   hull_indices(contours.size());             // indices of convex hulls
	vector<vector<Vec4i>> convexity_defects(contours.size());        // convex cavities
	

	for (int contour_number=0; (contour_number<(int)contours.size()); contour_number++) {
		if (contours[contour_number].size() > 10) {

			convexHull(contours[contour_number], hull_indices[contour_number]);
			convexityDefects( contours[contour_number], hull_indices[contour_number], convexity_defects[contour_number]);
		
		}
	}
	
	int defective_sweet_count = 0;

	for (int contour_number=0; (contour_number<(int)contours.size()); contour_number++) {
		for(int defect_index=0;defect_index<convexity_defects.size();defect_index++){

				if (convexity_defects[contour_number][defect_index][3]>1000 && convexity_defects[contour_number][defect_index][3]<2000){
					defective_sweet_count++;
				
				}
			}
	}
	return defective_sweet_count;
}
vector<Point2f> get_contours_count(Mat filteredImage){

   /*
   This function returns the centers of sweets by computing first hierarchy contours 
     and then computing moments of those contours. 

   =================================================================================
   Input
   --------------------------------
   filteredImage: binary image (Mat object)

   Output
   --------------------------------
   smartyCenters: a vector of points containing the centers of the sweets
   */

   vector<vector<Point>> contours;
   vector<Vec4i> hierarchy;
   /*get the contours(i.e sweets) */
   findContours(filteredImage.clone(), contours, hierarchy,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));//get the contours
   /*get the centers of the contours(sweets) by computing contour moments*/
   vector<Moments> mu(contours.size() );
   for( int i = 0; i < contours.size(); i++ ){//iterate over all the contours
	   mu[i] = moments( contours[i], false ); //get moments of for each contour
   }
   vector<Point2f> smartyCenters( contours.size() );
   for( int i = 0; i < contours.size(); i++ ){//iterate over all the contours
		 smartyCenters[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); //get the center of each contour
   }

   return smartyCenters;//return a vector of points containing the centers of each sweet
}

Mat convert_32bit_image_for_display(Mat& passed_image, double zero_maps_to/*=0.0*/, double passed_scale_factor/*=-1.0*/ )
{   /*
	Taken from the sample codes.
	It returns uint8 image for a 32 bit image  
	
	*/
	Mat display_image;
	double scale_factor = passed_scale_factor;
	if (passed_scale_factor == -1.0)
	{
		double minimum,maximum;
		minMaxLoc(passed_image,&minimum,&maximum);
		scale_factor = (255.0-zero_maps_to)/max(-minimum,maximum);
	}
	passed_image.convertTo(display_image, CV_8U, scale_factor, zero_maps_to);
	return display_image;
}


int get_threshold_from_image_stat(Mat img,double overlapParam){

	/*
	This function generates a threshold for a distance transform image by computing the 
	mean intensity(discounting the background) and multiplying it with the overlapParam.
	===================================================================================
	Input
	-----------------------
	img: distance transform image

	overlapParam : determines the amount of overlap we tolerate it is a value between 0-1
	
	Output
	-----------------------
	threshold: threshold for the distance transform image
	*/

	int pixelSum = 0;//pixel intensity accumulator
	int validPixelsCount = 0;// counts pixels of the foreground
	int meanImageIntensity;
	int threshold;
	for (int row=0; row < img.rows; row++) {
			uchar* value = img.ptr<uchar>(row);
			for (int column=0; column < img.cols; column++)
				if(*value++ != 0){//ignore background pixels
					pixelSum+=*value;
					validPixelsCount++;//count if foreground
				}
	}
	meanImageIntensity = pixelSum/(validPixelsCount);//compute the mean intensity

    if(DEBUG)cout<<"mean intensity of image: "<<meanImageIntensity<<endl;
	threshold = (int)(overlapParam*meanImageIntensity);//return threshold for the distance transform image
	return threshold;
}
void get_local_maxima(Mat img,int size){
	/*This function extracts the local maxima in img with in a window of size (size x size)*/

	Mat localMaximizedImage;
	Mat temp; 
	//structuring element
	Mat dilationElement = getStructuringElement(MORPH_RECT,Size(size,size),Point(size/2,size/2));
	//dilate the image with rectangular kernel
	//results in a image where each point will have the value of local maxima in the window
	dilate(img,localMaximizedImage,dilationElement,Point(2,2),1);
	//compare localMaximizedImage and the original image
	//local maxima are pixels that have the same values in both images
	//problem the background pixels will have the same values in both images
	compare(img,localMaximizedImage,temp,CV_CMP_EQ);
    //to remove the background pixels from local maxima image 
	//we multiply it with the original image(so that 1./255*bgpixel(0)=0)
	multiply(img,temp,img,1/255.);
	//here we are left with only the local maxima.

}
void displayHist(Mat src){
  vector<Mat> bgr_planes;
  split( src, bgr_planes );

  /// Establish the number of bins
  int histSize = 256;

  /// Set the ranges ( for B,G,R) )
  float range[] = { 0, 256 } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = false;

  Mat b_hist, g_hist, r_hist;

  /// Compute the histograms:
  calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

  // Draw the histograms for B, G and R
  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/histSize );

  Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

  /// Normalize the result to [ 0, histImage.rows ]
  normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

  /// Draw for each channel
  for( int i = 1; i < histSize; i++ )
  {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                       Scalar( 0, 255, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                       Scalar( 0, 0, 255), 2, 8, 0  );
  }

  /// Display
  namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
  imshow("calcHist Demo", histImage );

}
void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal\n");
   getchar();
   exit(status);
}

void print_message_to_file(FILE *fp, char message[]) {
   fprintf(fp,"The message is: %s\n", message);
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