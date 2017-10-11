

#include "sweetsCounter.h"




int count_smarties(int backgroundThreshold/*=190*/, int localMaximaSize/*=21*/,double overlapParam/*=0.75*/){

   extern Mat inputImage; 
   
   char* inputWindowName = "Input Image";
   char* inputBgRemovedWindowName = "Input Image without background";
   char* DTInputWindowName = "Distance Transform Input";
   char* DTOutputWindowName = "Distance Transform Output";
   char* DTLocalMaximaWindowName = "Distance Transform Local Maxima";
   char* DTLocalMaximaDilatedWindowName = "Dilated Local Maxima";
   char* DTBinaryWindowName = "Distance Transform Thresholded(Input for Component Analysis)";


   namedWindow(inputWindowName, CV_WINDOW_AUTOSIZE);
   namedWindow(inputBgRemovedWindowName, CV_WINDOW_AUTOSIZE );
   namedWindow(DTInputWindowName, CV_WINDOW_AUTOSIZE );
   namedWindow(DTOutputWindowName, CV_WINDOW_AUTOSIZE );
   namedWindow(DTLocalMaximaWindowName, CV_WINDOW_AUTOSIZE );
   namedWindow(DTLocalMaximaDilatedWindowName, CV_WINDOW_AUTOSIZE );
   namedWindow(DTBinaryWindowName, CV_WINDOW_AUTOSIZE );
   

   Mat greyscaleImage;
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
   
   if (inputImage.type() == CV_8UC3) { // colour image
	   GaussianBlur(inputImage,blurImage,Size(21,21),1);
	   
	   remove_white_bg(blurImage,backgroundThreshold);//remove white and shadow
	   cvtColor(blurImage.clone(), greyscaleImage, CV_BGR2GRAY);
    } 
   else {
      greyscaleImage = inputImage.clone();
    }
    

	Mat dilateElement = getStructuringElement(MORPH_ELLIPSE,Size(23,23));
	Mat closeElement  = getStructuringElement(MORPH_ELLIPSE,Size(structuringElementSize ,structuringElementSize ));
	Mat erodeElement  = getStructuringElement(MORPH_ELLIPSE,Size(structuringElementSize ,structuringElementSize ));

	threshold(greyscaleImage,filteredImage,1,255,CV_THRESH_BINARY/*|CV_THRESH_OTSU*/);
	morphologyEx(filteredImage,filteredImage, MORPH_CLOSE,closeElement);
	morphologyEx(filteredImage,dtInputImage, MORPH_ERODE,erodeElement,Point(-1,-1),2);

	
	distanceTransform(dtInputImage,distTransform,CV_DIST_L2, 5);
	normalize(distTransform	, distTransform, 0, 1, NORM_MINMAX);
    distTransformUint =  convert_32bit_image_for_display(distTransform);
	distTransformUint.copyTo(distTransformLocalMaximaImage);


	get_local_maxima(distTransformLocalMaximaImage,localMaximaSize);
	

	dilate(distTransformLocalMaximaImage,distTransformDilatedImage,dilateElement);
	

	distTransformThreshold = get_threshold_from_image_stat(distTransformDilatedImage, overlapParam); 
	threshold(distTransformDilatedImage,distTransformBinaryImage,distTransformThreshold,255,CV_THRESH_BINARY);


	smartyCenters = get_contours_count(distTransformBinaryImage);
	smartiesCount = smartyCenters.size();
	int x,y;
	for (int i=0;i<smartyCenters.size();i++){

	    x = smartyCenters[i].x;
		y = smartyCenters[i].y;
		line(inputImage,Point(x-crossHairSize/2,y),Point(x+crossHairSize/2,y),(255,0,255),2);
		line(inputImage,Point(x,y-crossHairSize/2),Point(x,y+crossHairSize/2),(255,0,255),2);	
	}

	stringstream ss;
	ss << "Number of smarties: "<<smartiesCount;
	putText(inputImage,ss.str(),Point(30,30), 
    FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250), 1, CV_AA);

	imshow(inputWindowName,inputImage);
	imshow(inputBgRemovedWindowName,blurImage);
	imshow(DTInputWindowName,dtInputImage);
	imshow(DTOutputWindowName,distTransformUint);
	imshow(DTBinaryWindowName,distTransformBinaryImage);
	imshow(DTLocalMaximaWindowName,distTransformLocalMaximaImage);
	imshow(DTLocalMaximaDilatedWindowName,distTransformDilatedImage);
	Mat imageList[] = {dtInputImage,distTransformUint,distTransformLocalMaximaImage,
		distTransformDilatedImage,distTransformBinaryImage,inputImage};
	displayMultilpleImages(imageList,6);
return smartiesCount;
}

void displayMultilpleImages(Mat* imageList,int numberOfImages){

	int space = 10;
	
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

		}else{
	
			currentImg = imageList[i];
	
		}

		Mat temp;
		if(i==3){

			yStart = (int)(currentImg.rows/yScale) + space;
			xCounter=0;
		}
		xStart=xCounter*currentImg.cols/xScale;
		Rect ROI(xStart,yStart,(int)(currentImg.cols/xScale),(int)(currentImg.rows/yScale));
		resize(currentImg,temp, Size(ROI.width, ROI.height));
		temp.copyTo(DispImage(ROI));
		xCounter++;
		
		}
	
	
		imshow("Processing Steps",DispImage);
	
}
void remove_white_bg(Mat img,int backgroundThreshold){
	int row, col;
	for (row=0; row < img.rows; row++) {
			   for (col=0; col < img.cols; col++) {

			    /*assign colors on the achromatic axis(near) zero value(removing gray and white pixels)*/
	            if (((img.at<Vec3b>(row,col)[0] >= backgroundThreshold) &&
	                (img.at<Vec3b>(row,col)[1]  >= backgroundThreshold) &&
	                (img.at<Vec3b>(row,col)[2]  >= backgroundThreshold))
					|
					((img.at<Vec3b>(row,col)[0]==img.at<Vec3b>(row,col)[1])&&
					 (img.at<Vec3b>(row,col)[1]==img.at<Vec3b>(row,col)[2]))) {//if white

	               img.at<Vec3b>(row,col)[0] =0; 
	               img.at<Vec3b>(row,col)[1] =0; 
	               img.at<Vec3b>(row,col)[2] =0; 
	            }

	         }
	      }

}
int get_max_contour_size(vector<vector<Point> > contours){
  /********************this function returns the maximum area of contours*************************************/
  /*loop through all the contours and compare their area with the current max area if greater update max area*/
  /*to the area of current contour																			 */
  int largest_area=0;
	for( int i = 0; i< contours.size(); i++ ) // iterate through each contour. 
      {
       double a=contourArea( contours[i],false);  //  Find the area of contour
	  
       if(a>largest_area){
       largest_area=a;

       }
      }
return largest_area;
}

vector<Point2f> get_contours_count(Mat filteredImage){

   /*this fun return the number of first hierarchy contours given an BGR image */

   vector<vector<Point>> contours;
   vector<Vec4i> hierarchy;
   findContours(filteredImage.clone(), contours, hierarchy,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));//get the contours
   int max_contour_area = get_max_contour_size(contours);//get the max contour area among the contours
   int imageArea = filteredImage.cols*filteredImage.rows;//area of the original image
   vector<Moments> mu(contours.size() );
   for( int i = 0; i < contours.size(); i++ ){
	   mu[i] = moments( contours[i], false ); 
   }

  ///  Get the mass centers:
  vector<Point2f> smartyCenters( contours.size() );
  for( int i = 0; i < contours.size(); i++ ){
		 smartyCenters[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); 
  }

   return smartyCenters;
}

Mat convert_32bit_image_for_display(Mat& passed_image, double zero_maps_to/*=0.0*/, double passed_scale_factor/*=-1.0*/ )
{
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

	double min,max;
	minMaxLoc(img,&min,&max);

	int pixelSum = 0;
	int validPixelsCount = 0;
	for (int row=0; row < img.rows; row++) {
			uchar* value = img.ptr<uchar>(row);
			for (int column=0; column < img.cols; column++)
				if(*value++ != 0){
					pixelSum+=*value;
					validPixelsCount++;
				}
	}
	int meanImageIntensity = pixelSum/(validPixelsCount);

    if(DEBUG)cout<<"mean intensity of image: "<<meanImageIntensity<<endl;

	return (int)(overlapParam*meanImageIntensity);
}
void get_local_maxima(Mat img,int size){

	Mat maxed;
	Mat comp; 
	Mat kernel = getStructuringElement(MORPH_RECT,Size(size,size),Point(size/2,size/2));
	dilate(img,maxed,kernel,Point(2,2),1);
	compare(img,maxed,comp,CV_CMP_EQ);
	multiply(img,comp,img,1/255.);

}
void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal\n");
   getchar();
   exit(status);
}

void print_message_to_file(FILE *fp, char message[]) {
   fprintf(fp,"The message is: %s\n", message);
}