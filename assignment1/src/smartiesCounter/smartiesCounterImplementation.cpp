

#include "smartiesCounter.h"




int count_smarties(int, void*){

   extern Mat inputImage; 
   extern int halfStructuringElementSize; 
   extern int iteration;
   extern int dtThreshold;
   extern int dtThresholdIt;
   extern char* binaryWindowName;
   extern char* processedWindowName;
   extern char* inputWindowName;
   extern char* dtThreshWindowName;
   Mat greyscaleImage;
   Mat binaryImage;
   Mat filteredImage;
   Mat blurImage;
   Mat distTransform;
   Mat distTransformUint;
   Mat distTransformThresh;

   if (halfStructuringElementSize<1)halfStructuringElementSize=1;
   int structuringElementSize;
   int distTransformThreshold;
   int smartiesCount;
   int crossHairSize = 10;
   vector<Point2f> smarties;
   


   structuringElementSize = halfStructuringElementSize * 2 + 1;

   /* convert from colour to greyscale if necessary */

   if (inputImage.type() == CV_8UC3) { // colour image

	   GaussianBlur(inputImage,blurImage,Size(5,5),1);
	   remove_white_bg(blurImage);//remove white and shadow
	   if(~DEBUG)imshow("bg removed image",blurImage);
	   cvtColor(blurImage.clone(), greyscaleImage, CV_BGR2GRAY);
	   
    } 
   else {
      greyscaleImage = inputImage.clone();
    }

    binaryImage.create(greyscaleImage.size(), CV_8UC1);
	//structuring elements

	Mat dilateElement = getStructuringElement(MORPH_ELLIPSE,Size(31,31));
	Mat closeElement  = getStructuringElement(MORPH_ELLIPSE,Size(5,5));
	Mat erodeElement  = getStructuringElement(MORPH_ELLIPSE,Size(structuringElementSize,structuringElementSize));

	threshold(greyscaleImage.clone(),filteredImage,1,255,CV_THRESH_BINARY);
	flood_fill(filteredImage);
	//morphologyEx(filteredImage,filteredImage,MORPH_CLOSE,closeElement);//close to fill any holes
	
	morphologyEx(filteredImage,filteredImage,MORPH_ERODE,erodeElement);//open to smooth the edges
	
	
	distanceTransform(filteredImage,distTransform,CV_DIST_L2, 5);
	normalize(distTransform	, distTransform, 0, 1, NORM_MINMAX);



	distTransformUint =  convert_32bit_image_for_display(distTransform);

	get_local_maxima(distTransformUint,25);

	dilate(distTransformUint,distTransformUint,dilateElement);
	
	distTransformThreshold = get_threshold_from_image_stat(distTransformUint); 
	threshold(distTransformUint,distTransformThresh,distTransformThreshold,255,CV_THRESH_BINARY);

	
	smarties = get_contours_count(distTransformThresh);
	smartiesCount = smarties.size();
	int x,y;
	for (int i=0;i<smarties.size();i++){

	    x = smarties[i].x;y=smarties[i].y;
		line(inputImage,Point(x-crossHairSize/2,y),Point(x+crossHairSize/2,y),(255,0,255),2);
		line(inputImage,Point(x,y-crossHairSize/2),Point(x,y+crossHairSize/2),(255,0,255),2);	
	}
	stringstream ss;
	ss << "Number of smarties: "<<smartiesCount;
	putText(inputImage,ss.str(),Point(30,30), 
    FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250), 1, CV_AA);

	imshow(dtThreshWindowName,distTransformThresh);
	imshow(binaryWindowName,    filteredImage);
	imshow(processedWindowName, distTransformUint);
	imshow(inputWindowName,inputImage);

	return smartiesCount;
}


void remove_white_bg(Mat img){
	int row, col;
	for (row=0; row < img.rows; row++) {
			   for (col=0; col < img.cols; col++) {

			    /*assign colors on the achromatic axis(near) zero value(removing gray and white pixels)*/
	            if ((img.at<Vec3b>(row,col)[0] >= 190) &&
	                (img.at<Vec3b>(row,col)[1] >= 190) &&
	                (img.at<Vec3b>(row,col)[2] >= 190)) {//if white

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

void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal\n");
   getchar();
   exit(status);
}

void print_message_to_file(FILE *fp, char message[]) {
   fprintf(fp,"The message is: %s\n", message);
}
int get_threshold_from_image_stat(Mat img){

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

	return (int)(0.9*meanImageIntensity);
}
void get_local_maxima(Mat img,int size){

	Mat maxed;
	Mat comp; 
	Mat kernel = getStructuringElement(MORPH_RECT,Size(size,size),Point(size/2,size/2));
	dilate(img,maxed,kernel,Point(2,2),1);
	compare(img,maxed,comp,CV_CMP_EQ);
	multiply(img,comp,img,1/255.);

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