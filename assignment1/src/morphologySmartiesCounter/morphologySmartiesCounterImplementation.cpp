

#include "morphologySmartiesCounter.h"


void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal\n");
   getchar();
   exit(status);
}

void print_message_to_file(FILE *fp, char message[]) {
   fprintf(fp,"The message is: %s\n", message);
}
void get_local_maxima(Mat img,int size){

	Mat maxed;
	Mat comp; 
	Mat kernel = getStructuringElement(MORPH_RECT,Size(size,size),Point(size/2,size/2));
	dilate(img,maxed,kernel,Point(2,2),1);
	compare(img,maxed,comp,CV_CMP_EQ);
	multiply(img,comp,img,1/255.);

}
void count_smarties_with_morphology(int, void*){

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

   if (halfStructuringElementSize<1)halfStructuringElementSize=1;
   int structuringElementSize;

   structuringElementSize = halfStructuringElementSize * 2 + 1;

   /* convert from colour to greyscale if necessary */

   if (inputImage.type() == CV_8UC3) { // colour image

	   GaussianBlur(inputImage,blurImage,Size(5,5),1);
	   remove_white_bg(blurImage);//remove white and shadow
	   cvtColor(blurImage.clone(), greyscaleImage, CV_BGR2GRAY);
	   
    } 
   else {
      greyscaleImage = inputImage.clone();
    }

    binaryImage.create(greyscaleImage.size(), CV_8UC1);
	Mat closeElement = getStructuringElement(MORPH_ELLIPSE,Size(31,31));
	Mat closeKernel  = getStructuringElement(MORPH_ELLIPSE,Size(5,5));
	Mat erodeElement = getStructuringElement(MORPH_ELLIPSE,Size(structuringElementSize,structuringElementSize));
	threshold(greyscaleImage.clone(),filteredImage,1,255,CV_THRESH_BINARY);
	morphologyEx(filteredImage,filteredImage,MORPH_CLOSE,closeKernel,Point(-1,-1),iteration);//open to smooth the edges
	morphologyEx(filteredImage,filteredImage,MORPH_ERODE,erodeElement,Point(-1,-1),iteration);//open to smooth the edges
	
	Mat distTransform;
	distanceTransform(filteredImage,distTransform,CV_DIST_L2, 5);
	normalize(distTransform	, distTransform, 0, 1, NORM_MINMAX);
	Mat origDistTransform;
	distTransform.copyTo(origDistTransform);
	double min,max;
	minMaxLoc(distTransform,&min,&max);
	Mat dtint =  convert_32bit_image_for_display(origDistTransform);
	get_local_maxima(dtint,21);
	dilate(dtint,dtint,closeElement);
	Mat dtintThresh;
	threshold(dtint,dtintThresh,dtThresholdIt,255,CV_THRESH_BINARY);
	threshold(distTransform,distTransform,(dtThreshold/10.)*max,1,0);
	cout<<get_contours_count(dtintThresh)<<endl;
	imshow("dtint",dtint);
	imshow(dtThreshWindowName,dtintThresh);
	imshow("Dt Image",origDistTransform);
	imshow(binaryWindowName,    filteredImage);
    imshow(processedWindowName, origDistTransform);
	imshow(inputWindowName,inputImage);

}


void remove_white_bg(Mat img){
int row, col;
for (row=0; row < img.rows; row++) {
		   for (col=0; col < img.cols; col++) {

		    /*assign colors on the achromatic axis(near) zero value(removing gray and white pixels)*/
            if ((img.at<Vec3b>(row,col)[0] >= 240) &&
                (img.at<Vec3b>(row,col)[1] >= 240) &&
                (img.at<Vec3b>(row,col)[2] >= 240)) {//if white

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

int get_contours_count(Mat filteredImage){

   /*this fun return the numebr of first hierarchy contours given an BGR image */
   bool DEBUG = false;
   int contourCount = 0;
   vector<vector<Point>> contours;
   vector<Vec4i> hierarchy;
   findContours( filteredImage.clone(), contours, hierarchy,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );//get the contours
   Mat drawing = Mat::zeros(filteredImage.size(), CV_8UC3 );//to draw the contours
   RNG rng(12345);//for random colors when drawing the contours
   //parameter that needs tuning
   float contourThreshold = 0.001;//if the max contour area in the image is less than this value no smarties(too small)
   
   int max_contour_area = get_max_contour_size(contours);//get the max contour area among the contours
   int imageArea = filteredImage.cols*filteredImage.rows;//area of the original image
   if ((max_contour_area*1./imageArea)>contourThreshold){
   for( size_t i = 0; i< contours.size(); i++ )
     {
	   if(contourArea( contours[i],false)>0.25*max_contour_area){////parameter that needs tuning count everything 1/4 the area of max 
	   contourCount++;
	   if (DEBUG){
			Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			drawContours( drawing, contours, (int)i, color, 2, 8, hierarchy, 0, Point() );
	   }
	 }
   }
   }
   if(DEBUG)
   {
		//imshow("Contours", drawing );
		cout<<"max contour area: "<<max_contour_area<<endl;
		cout<<"contour to image area : "<<(max_contour_area*1./imageArea)<<endl;
		cout<<"is it big enough: "<<((max_contour_area*1./imageArea)>contourThreshold)<<endl;
   
   }
   return contourCount;
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
