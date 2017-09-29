

#include "morphologySmartiesCounter.h"


void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal\n");
   getchar();
   exit(status);
}

void print_message_to_file(FILE *fp, char message[]) {
   fprintf(fp,"The message is: %s\n", message);
}
void count_smarties_with_morphology(char* fileName ){
	Mat inputImage,threshImage,grayImage;
	inputImage = imread(fileName);
	imshow("Input Image",inputImage);
	Mat element = getStructuringElement(MORPH_ELLIPSE,Size(9,9));
	//imshow("structuring element",element);
	GaussianBlur(inputImage,inputImage,Size(11,11),0);
	remove_white_bg(inputImage);//remove white and shadow
	cvtColor(inputImage,grayImage, CV_BGR2GRAY);
    threshold(grayImage,threshImage,1,255,CV_THRESH_BINARY);
	morphologyEx(threshImage,threshImage,MORPH_CLOSE,element,Point(0,0),2);
	//Mat distanceImage;
    //distanceTransform(threshImage, distanceImage, CV_DIST_L2, 3);
	//normalize(distanceImage, distanceImage, 0, 1., NORM_MINMAX);
	//threshold(distanceImage, distanceImage, .4, 1., CV_THRESH_BINARY);
	morphologyEx(threshImage,threshImage,MORPH_ERODE,element,Point(-1,-1),4);//open to smooth the edges
	imshow("Eroded Image",threshImage);

	
	do{
		waitKey(30);                                  // Must call this to allow openCV to display the images
     }while (!_kbhit());                             // We call it repeatedly to allow the user to move the windows
                                                    // (if we don't the window process hangs when you try to click and drag)
     getchar(); // flush the buffer from the keyboard hit
	 destroyAllWindows();

}


void remove_white_bg(Mat img){
int row, col;
for (row=0; row < img.rows; row++) {
		   for (col=0; col < img.cols; col++) {

		    /*assign colors on the achromatic axis(near) zero value(removing gray and white pixels)*/
            if ((img.at<Vec3b>(row,col)[0] >= 210) &&
                (img.at<Vec3b>(row,col)[1] >= 210) &&
                (img.at<Vec3b>(row,col)[2] >= 210)) {//if white

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

int get_contours_count(Mat src){

   /*this fun return the numebr of first hierarchy contours given an BGR image */
   bool DEBUG = false;
   int contourCount = 0;
   Mat src_gray,threshImage;

   cvtColor(src, src_gray, CV_BGR2GRAY);//convert to gray scale
   threshold(src_gray,threshImage,1,255,CV_THRESH_BINARY);// threshold the image
   vector<vector<Point>> contours;
   vector<Vec4i> hierarchy;
   findContours( threshImage.clone(), contours, hierarchy,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );//get the contours
   Mat drawing = Mat::zeros( threshImage.size(), CV_8UC3 );//to draw the contours
   RNG rng(12345);//for random colors when drawing the contours
   //parameter that needs tuning
   float contourThreshold = 0.001;//if the max contour area in the image is less than this value no smarties(too small)
   
   int max_contour_area = get_max_contour_size(contours);//get the max contour area among the contours
   int imageArea = src.cols*src.rows;//area of the original image
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
		imshow("Contours", drawing );
		cout<<"max contour area: "<<max_contour_area<<endl;
		cout<<"contour to image area : "<<(max_contour_area*1./imageArea)<<endl;
		cout<<"is it big enough: "<<((max_contour_area*1./imageArea)>contourThreshold)<<endl;
   
   }
   return contourCount;
}