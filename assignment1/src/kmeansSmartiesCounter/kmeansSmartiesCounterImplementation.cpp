

#include "kmeansSmartiesCounter.h"


void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal\n");
   getchar();
   exit(status);
}

void print_message_to_file(FILE *fp, char message[]) {
   fprintf(fp,"The message is: %s\n", message);
}


void segment_colors_with_kmeans(char* fileName ,int numberOfColors){
	/* this function uses kmeans to segment the image by colors (8 colors and 1 background) the extracts
	info about the image(in this case get the number of smarties)
	*/
	Mat labImage,inputImage,result_image; 

    int iterations;
    iterations = 5;
    inputImage = imread(fileName);
	if (!inputImage.data) {                               // Check for invalid input
		printf("Error: failed to read image\n");
		prompt_and_exit(-1);
	}
	bool DEBUG = false;
	imshow("Input Image",inputImage);
    Mat element = getStructuringElement(MORPH_ELLIPSE,Size(9,9));//blur bright spots
	remove_white_bg(inputImage);//remove white and shadow
	morphologyEx(inputImage,inputImage,MORPH_OPEN,element);//open to smooth the edges
	if(DEBUG)imshow("after shadow removal",inputImage);
	cvtColor(inputImage,labImage,COLOR_RGB2Lab);//convert color to L*a*b space
    CV_Assert( labImage.type() == CV_8UC3 );
	// Populate an n*3 array of float for each of the n pixels in the image


	//***************************************Kmeans Segmentation***********************************
	//*****************************constructing the sample points for kmeans***********************
	//samples have two chax a and b(from lab color space).the number of samples = number of pixels in the image
	//size of samples  = number of pixels x 2 
	//*********************************************************************************************
	Mat samples(labImage.rows*labImage.cols,labImage.channels()-1,CV_32F);
	float* sample = samples.ptr<float>(0);
	for(int row=0; row<labImage.rows; row++)
		for(int col=0; col<labImage.cols; col++)
			for (int channel=1; channel < labImage.channels(); channel++)
				samples.at<float>(row*labImage.cols+col,channel-1) = 
				(uchar) labImage.at<Vec3b>(row,col)[channel];

	Mat labels;//labels array for kmeans output
	Mat centres;// centroids 
	kmeans(samples,numberOfColors, labels, TermCriteria(KMEANS_PP_CENTERS, 1, 0.0001),
		iterations, KMEANS_PP_CENTERS, centres );//perform kmeans

	int cluster;
	int smartiesCount = 0;
	int smartiesInCurrentCluster = 0;
	Mat colorSegments[9];// holds all the segments as separate images 
	Mat src_gray,threshImage;// 
	/*****************************reconsruct image based on the output of kmeans*******************************/
	/* loop through all the pixels check cluster label from and cluster number match                          */
	/* if they match assign it the pixel from the orignal image                                               */
	/**********************************************************************************************************/
	for (cluster = 0;cluster<numberOfColors;cluster++){
		Mat segment  = Mat(inputImage.size(), inputImage.type());
		for(int row=0; row<inputImage.rows; row++)
			for(int col=0; col<inputImage.cols; col++)
				if (*(labels.ptr<int>(row*inputImage.cols+col))!= cluster)
					for (int channel=0;channel<=inputImage.channels();channel++)
						segment.at<Vec3b>(row,col)[channel] = (uchar)0;
				else
					for (int channel=0;channel<=inputImage.channels();channel++)
						segment.at<Vec3b>(row,col)[channel] = (uchar)inputImage.at<Vec3b>(row,col)[channel];
		
		segment.copyTo(colorSegments[cluster]);//copy current segment 
		segment.release();
		morphologyEx(colorSegments[cluster],colorSegments[cluster],MORPH_ERODE,element);///erode 
		smartiesInCurrentCluster=get_contours_count(colorSegments[cluster]);//get the number of valid contours 
	    smartiesCount+=smartiesInCurrentCluster;

		if (DEBUG){
			imshow("colorSegment "+cluster,colorSegments[cluster]);
			waitKey(100);
			cout<<"in cluster " << cluster<<" there are "<<smartiesInCurrentCluster<<" Smarties"<<endl;
		}
    }
	cout<<"On this image there are "<<smartiesCount<<" Smarties"<<endl;
	
	

	if (DEBUG){

		cout<<"centers channel "<<centres.size()<<"\nlabels number of channels "<<labels.size()<<endl;
	    cout<<"samples: "<<samples.size()<<endl;
		cout<<"image size: "<<inputImage.size()<<endl;
		cout<<"labels reshaped: "<<labels.size()<<endl;
	    cout<<"centers:\n "<<centres<<endl;
	}
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