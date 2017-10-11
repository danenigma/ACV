

#include "sweetsCounter.h"




int count_sweets(int backgroundThreshold/*=190*/, int localMaximaSize/*=21*/,double overlapParam/*=0.75*/){

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
   char* DTOutputWindowName = "Distance Transform Output";
   char* DTLocalMaximaWindowName = "Distance Transform Local Maxima";
   char* DTLocalMaximaDilatedWindowName = "Dilated Local Maxima";
   char* DTBinaryWindowName = "Distance Transform Thresholded(Input for Component Analysis)";

   /*init windows*/
   namedWindow(inputWindowName, CV_WINDOW_AUTOSIZE);
   namedWindow(inputBgRemovedWindowName, CV_WINDOW_AUTOSIZE );
   namedWindow(DTInputWindowName, CV_WINDOW_AUTOSIZE );
   namedWindow(DTOutputWindowName, CV_WINDOW_AUTOSIZE );
   namedWindow(DTLocalMaximaWindowName, CV_WINDOW_AUTOSIZE );
   namedWindow(DTLocalMaximaDilatedWindowName, CV_WINDOW_AUTOSIZE );
   namedWindow(DTBinaryWindowName, CV_WINDOW_AUTOSIZE );
   
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
   int distTransformThreshold;
   int smartiesCount;
   int crossHairSize = 10;

   vector<Point2f> smartyCenters;
   

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
	Mat dilateElement = getStructuringElement(MORPH_ELLIPSE,Size(localMaximaSize+2,localMaximaSize+2));
	Mat closeElement  = getStructuringElement(MORPH_ELLIPSE,Size(structuringElementSize ,structuringElementSize ));
	Mat erodeElement  = getStructuringElement(MORPH_ELLIPSE,Size(structuringElementSize ,structuringElementSize ));
	
	
	/*====================================================================*/
	/*       Thresholding image and applying morphological operations     */
	/*====================================================================*/
	//threshold the image
	threshold(grayscaleImage,filteredImage,1,255,CV_THRESH_BINARY);
	//closing to fill holes
	morphologyEx(filteredImage,filteredImage, MORPH_CLOSE,closeElement);
	//errode to remove noise especially at the edges of the sweets 
	morphologyEx(filteredImage,dtInputImage,  MORPH_ERODE,erodeElement,Point(-1,-1),2);

	/*====================================================================*/
	/*       Compute distance transform and normalize it                  */
	/*====================================================================*/
	//compute L2 distance transform
	distanceTransform(dtInputImage,distTransform,CV_DIST_L2, 5);
	//normalize it between 0-1
	normalize(distTransform	, distTransform, 0, 1, NORM_MINMAX);
	//convert it to uint8
    distTransformUint =  convert_32bit_image_for_display(distTransform);
	distTransformUint.copyTo(distTransformLocalMaximaImage);

	/*====================================================================*/
	/*       Compute local maxima and dilate them                         */
	/*====================================================================*/
	//compute the local maxima
	get_local_maxima(distTransformLocalMaximaImage,localMaximaSize);
	//dilate it so that local maxima that are very close to one another join up and become a sigle maxima
	dilate(distTransformLocalMaximaImage,distTransformDilatedImage,dilateElement);
	
    /*====================================================================*/
	/*       Compute the appropriate threshold for the distance transform */
	/*		 local maxima and threshold it                                */
	/*====================================================================*/
	//compute the right threshold based on the mean intensity of the distance transform
	distTransformThreshold = get_threshold_from_image_stat(distTransformDilatedImage, overlapParam);
	//threshold the image using the threshold found above
	threshold(distTransformDilatedImage,distTransformBinaryImage,distTransformThreshold,255,CV_THRESH_BINARY);

	/*====================================================================*/
	/*       Connected Component Analysis to count the sweets             */
	/*====================================================================*/

	//get the centers of the sweets by finding the contours and moments 
	smartyCenters = get_contours_count(distTransformBinaryImage);
	//the number of sweets is the size of the vector containing the centers of the sweets
	smartiesCount = smartyCenters.size();

	/*====================================================================*/
	/*   Draw crosshairs on the original image at the center of the sweets*/
	/*====================================================================*/
	int x,y;
	for (int i=0;i<smartyCenters.size();i++){

	    x = smartyCenters[i].x;//x component
		y = smartyCenters[i].y;//y component 
		line(inputImage,Point(x-crossHairSize/2,y),Point(x+crossHairSize/2,y),(255,0,255),2);//draw vertical line
		line(inputImage,Point(x,y-crossHairSize/2),Point(x,y+crossHairSize/2),(255,0,255),2);//draw horizontal line	
	}
	//put the number of sweets in a stringstream
	stringstream ss;
	ss << "Number of smarties: "<<smartiesCount;

	//draw the number of smarties on the original image
	putText(inputImage,ss.str(),Point(30,30), 
    FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250), 1, CV_AA);
	//display all the images at different stages of processing
	imshow(inputWindowName,inputImage);
	imshow(inputBgRemovedWindowName,blurImage);
	imshow(DTInputWindowName,dtInputImage);
	imshow(DTOutputWindowName,distTransformUint);
	imshow(DTBinaryWindowName,distTransformBinaryImage);
	imshow(DTLocalMaximaWindowName,distTransformLocalMaximaImage);
	imshow(DTLocalMaximaDilatedWindowName,distTransformDilatedImage);

	Mat imageList[] = {dtInputImage,distTransformUint,distTransformLocalMaximaImage,
		distTransformDilatedImage,distTransformBinaryImage,inputImage};
	//display different stages of processing on a single window
	displayMultilpleImages(imageList,6);

return smartiesCount;//return the number of sweets
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

void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal\n");
   getchar();
   exit(status);
}

void print_message_to_file(FILE *fp, char message[]) {
   fprintf(fp,"The message is: %s\n", message);
}