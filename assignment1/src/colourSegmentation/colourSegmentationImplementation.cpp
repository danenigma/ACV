/* 
  Example use of openCV to perform colour segmentation
   
  The user must interactively select the colour sample that will 
  form the basis of the segmentation. The user can also adjust the
  hue and saturation tolerances on that sample 
  ------------------------------------------------------------
  Implementation file

  David Vernon
  1 June 2017
 
*/
#include "colourSegmentation.h"

void colourSegmentation(int, void*) {
  
   extern Mat inputBGRImage;
   extern Mat inputHLSImage;
   extern int hueRange;
   extern int saturationRange;
   extern Point2f sample_point; 
   extern char* segmented_window_name;
   extern int number_of_sample_points;

   Mat segmentedImage; 
   int row, col;
 
   int hue;
   int saturation;

   bool debug = true;

   /* now get the sample point */

  if (number_of_sample_points == 1) {
  
      segmentedImage = inputBGRImage.clone();
    
      hue        = inputHLSImage.at<Vec3b>((int)sample_point.y,(int)sample_point.x)[0]; // note order of indices
      saturation = inputHLSImage.at<Vec3b>((int)sample_point.y,(int)sample_point.x)[2]; // note order of indices

      if (debug) { 
         printf("Sample point (%f, %f) Hue: %d  Saturation: %d\n", sample_point.y, sample_point.x, hue, saturation); // note order of indices
         printf("Hue range: %d  Saturation range: %d\n", hueRange, saturationRange); // note order of indices
      }
	  get_count_for_hue(segmentedImage,hue,hueRange);
}

   if (debug) printf("Leaving colourSegmentation() \n");

}
 

void getSamplePoint( int event, int x, int y, int, void* ) {
      
   extern char* input_window_name;
   extern Mat   inputBGRImage; 
   extern Point2f sample_point; 
   extern int number_of_sample_points;

   Mat inputImageCopy;

   int crossHairSize = 10;

   if (event != EVENT_LBUTTONDOWN) {
      return;
   }
   else {
  
      number_of_sample_points = 1;

      sample_point.x = (float) x;
      sample_point.y = (float) y;

      inputImageCopy= inputBGRImage.clone(); // we make a copy of the input image on which to draw the cross-hairs

      line(inputImageCopy,Point(x-crossHairSize/2,y),Point(x+crossHairSize/2,y),Scalar(0, 255, 0),1, CV_AA); // Green
      line(inputImageCopy,Point(x,y-crossHairSize/2),Point(x,y+crossHairSize/2),Scalar(0, 255, 0),1, CV_AA);

      imshow(input_window_name, inputImageCopy); // show the image with the cross-hairs
 
      // Show the segmented image for new colour sample and current thresholds
      colourSegmentation(0, 0);
   }
}


/*=======================================================*/
/* Utility functions                                     */ 
/*=======================================================*/

void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal ... \n");
   getchar();
   exit(status);
}

void prompt_and_continue() {
   printf("Press any key to continue ... \n");
   getchar();
}

void pause(int milliseconds) {

   _timeb tb;

   long int s1, s2;
   long int ms1, ms2;
   long elapsed;

   _ftime(&tb); 
   s1=(long) tb.time; 
   ms1=tb.millitm;

   do {
     _ftime(&tb); 
     s2=(long) tb.time; 
     ms2=tb.millitm; 
     elapsed =(s2*1000+ms2)-(s1*1000+ms1);
   } while (elapsed < milliseconds);
}

int processImage(Mat img){
	int element_size = 3;
	Mat threshImage, grayImage;
	cvtColor(img, grayImage, CV_BGR2GRAY);
	threshold(grayImage,threshImage,100,255,0);
	Mat element = getStructuringElement( MORPH_RECT,
                                   Size( 2*element_size + 1, 2*element_size+1 ),
                                       Point(element_size, element_size));
	morphologyEx(threshImage,threshImage,MORPH_CLOSE,element);
	erode(threshImage,threshImage,element);
	morphologyEx(threshImage,threshImage,MORPH_CLOSE,element);
	erode(threshImage,threshImage,element);

	//imshow("gray image",grayImage);
	//imshow("color thresh",threshImage);
	return get_contours(threshImage);
}
void remove_white_bg(Mat img){
int row, col;
for (row=0; row < img.rows; row++) {
		   for (col=0; col < img.cols; col++) {

            /* warning ... this will not work properly when hue limit is less than zero or greater than 360 */

            if ((img.at<Vec3b>(row,col)[0] >= 200) &&
                (img.at<Vec3b>(row,col)[1] >= 200) &&
                (img.at<Vec3b>(row,col)[2] >= 200)) {//if white

               img.at<Vec3b>(row,col)[0] =0; //inputBGRImage.at<Vec3b>(row,col)[0];
               img.at<Vec3b>(row,col)[1] =0; //inputBGRImage.at<Vec3b>(row,col)[1];
               img.at<Vec3b>(row,col)[2] =0; //inputBGRImage.at<Vec3b>(row,col)[2];
            }

         }
      }

}

int get_contours(Mat threshImage){
   RNG rng(12345);
   vector<vector<Point>> contours;
   vector<Vec4i> hierarchy;
   findContours( threshImage.clone(), contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
   Mat drawing = Mat::zeros( threshImage.size(), CV_8UC3 );
   int counter = 0;
   int max_contour_area = get_max_contour_size(contours);
   for( size_t i = 0; i< contours.size(); i++ )
     { 
	   if(contourArea( contours[i],false)>0.4*max_contour_area){counter++;
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, (int)i, color, 2, 8, hierarchy, 0, Point() );
	   }
   }
   imshow("Contours", drawing );
   return counter;
}
int get_max_contour_size(vector<vector<Point> > contours){
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

int get_count_for_hue(Mat segmentedImage,int hue,int hueRange){
	   int row,col;
	   Mat inputHLSImage;
	   cvtColor(segmentedImage, inputHLSImage, CV_BGR2HLS);
 	   for (row=0; row < segmentedImage.rows; row++) {
		   for (col=0; col < segmentedImage.cols; col++) {

            /* warning ... this will not work properly when hue limit is less than zero or greater than 360 */

            if ((inputHLSImage.at<Vec3b>(row,col)[0] >= hue - hueRange) &&
                (inputHLSImage.at<Vec3b>(row,col)[0] <= hue + hueRange)) {

               segmentedImage.at<Vec3b>(row,col)[0] =255; //inputBGRImage.at<Vec3b>(row,col)[0];
               segmentedImage.at<Vec3b>(row,col)[1] =255; //inputBGRImage.at<Vec3b>(row,col)[1];
               segmentedImage.at<Vec3b>(row,col)[2] =255; //inputBGRImage.at<Vec3b>(row,col)[2];
            }
            else {
               segmentedImage.at<Vec3b>(row,col)[0] = 0;
               segmentedImage.at<Vec3b>(row,col)[1] = 0;
               segmentedImage.at<Vec3b>(row,col)[2] = 0;
            }
         }
      }
	  int count  = processImage(segmentedImage);
	  cout<<count<<endl;
	  return count;
}
int get_total_count(Mat segmentedImage,int colorArray[],int hueRange,int colorArraySize){
	int total_count=0,i;
	for(i=0;i<colorArraySize;i++)
	{
		total_count+=get_count_for_hue(segmentedImage.clone(),colorArray[i],hueRange);
	}
return total_count;
}
void disp_hist(Mat src){
    Mat  hsv;
    
    cvtColor(src, hsv, CV_BGR2HSV);

    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    int hbins = 30, sbins = 32;
    int histSize[] = {hbins, sbins};
    // hue varies from 0 to 179, see cvtColor
    float hranges[] = { 0, 180 };
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };
    MatND hist;
    // we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0, 1};

    calcHist( &hsv, 1, channels, Mat(), // do not use mask
             hist, 2, histSize, ranges,
             true, // the histogram is uniform
             false );
    double maxVal=0;
    minMaxLoc(hist, 0, &maxVal, 0, 0);

    int scale = 10;
    Mat histImg = Mat::zeros(sbins*scale, hbins*10, CV_8UC3);

    for( int h = 0; h < hbins; h++ )
        for( int s = 0; s < sbins; s++ )
        {
            float binVal = hist.at<float>(h, s);
            int intensity = cvRound(binVal*255/maxVal);
            rectangle( histImg, Point(h*scale, s*scale),
                        Point( (h+1)*scale - 1, (s+1)*scale - 1),
                        Scalar::all(intensity),
                        CV_FILLED );
        }

    namedWindow( "Source", 1 );
    imshow( "Source", src );

    namedWindow( "H-S Histogram", 1 );
    imshow( "H-S Histogram", histImg );


}

void get_kmeans_clustor(Mat image){
 
   int k =4; 
   char* outputWindowName = "kmeans";
  
   Mat result_image; 

   int iterations;
   iterations = 5;

   if (k < 1)  // the trackbar has a lower value of 0 which is invalid
      k = 1;
	CV_Assert( image.type() == CV_8UC3 );
	// Populate an n*3 array of float for each of the n pixels in the image
	Mat samples(image.rows*image.cols, image.channels(), CV_32F);
	float* sample = samples.ptr<float>(0);
	for(int row=0; row<image.rows; row++)
		for(int col=0; col<image.cols; col++)
			for (int channel=0; channel < image.channels(); channel++)
				samples.at<float>(row*image.cols+col,channel) = 
								(uchar) image.at<Vec3b>(row,col)[channel];
	// Apply k-means clustering to cluster all the samples so that each sample
	// is given a label and each label corresponds to a cluster with a particular
	// centre.
	Mat labels;
	Mat centres;
	kmeans(samples, k, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 1, 0.0001),
		iterations, KMEANS_PP_CENTERS, centres );
	// Put the relevant cluster centre values into a result image
	result_image = Mat( image.size(), image.type() );
	for(int row=0; row<image.rows; row++)
		for(int col=0; col<image.cols; col++)
			for (int channel=0; channel < image.channels(); channel++)
				result_image.at<Vec3b>(row,col)[channel] = (uchar) centres.at<float>(*(labels.ptr<int>(row*image.cols+col)), channel); 
 
   /************************************************************************************************************/
        
   imshow(outputWindowName, result_image);  
}