/* 
  Example use of openCV to generate a histogram of an image
  ---------------------------------------------------------
  Implementation file

  David Vernon
  19 May 2017

*/
 
#include "histogram.h"
 
/************************************************************************************************************/

/*
 * This code is provided as part of "A Practical Introduction to Computer Vision with OpenCV"
 * by Kenneth Dawson-Howe © Wiley & Sons Inc. 2014.  All rights reserved.
 */

//#include "Utilities.h"

class Histogram
{
protected:
	Mat mImage;
	int mNumberChannels;
	int* mChannelNumbers;
	int* mNumberBins;
	float mChannelRange[2];
public:
	Histogram( Mat image, int number_of_bins )
	{
		mImage = image;
		mNumberChannels = mImage.channels();
		mChannelNumbers = new int[mNumberChannels];
		mNumberBins = new int[mNumberChannels];
		mChannelRange[0] = 0.0;
		mChannelRange[1] = 255.0;
		for (int count=0; count<mNumberChannels; count++)
		{
			mChannelNumbers[count] = count;
			mNumberBins[count] = number_of_bins;
		}
		//ComputeHistogram();
	}
	virtual void ComputeHistogram()=0;
	virtual void NormaliseHistogram()=0;
	static void Draw1DHistogram( MatND histograms[], int number_of_histograms, Mat& display_image )
	{
		int number_of_bins = histograms[0].size[0];
		double max_value=0, min_value=0;
		double channel_max_value=0, channel_min_value=0;
		for (int channel=0; (channel < number_of_histograms); channel++)
		{
			minMaxLoc(histograms[channel], &channel_min_value, &channel_max_value, 0, 0);
			max_value = ((max_value > channel_max_value) && (channel > 0)) ? max_value : channel_max_value;
			min_value = ((min_value < channel_min_value) && (channel > 0)) ? min_value : channel_min_value;
		}
		float scaling_factor = ((float)256.0)/((float)number_of_bins);
		
		Mat histogram_image((int)(((float)number_of_bins)*scaling_factor)+1,(int)(((float)number_of_bins)*scaling_factor)+1,CV_8UC3,Scalar(255,255,255));
		display_image = histogram_image;
		line(histogram_image,Point(0,0),Point(0,histogram_image.rows-1),Scalar(0,0,0));
		line(histogram_image,Point(histogram_image.cols-1,histogram_image.rows-1),Point(0,histogram_image.rows-1),Scalar(0,0,0));
		int highest_point = static_cast<int>(0.9*((float)number_of_bins)*scaling_factor);
		for (int channel=0; (channel < number_of_histograms); channel++)
		{
			int last_height;
			for( int h = 0; h < number_of_bins; h++ )
			{
				float value = histograms[channel].at<float>(h);
				int height = static_cast<int>(value*highest_point/max_value);
				int where = (int)(((float)h)*scaling_factor);
				if (h > 0)
					line(histogram_image,Point((int)(((float)(h-1))*scaling_factor)+1,(int)(((float)number_of_bins)*scaling_factor)-last_height),
								         Point((int)(((float)h)*scaling_factor)+1,(int)(((float)number_of_bins)*scaling_factor)-height),
							             Scalar(channel==0?255:0,channel==1?255:0,channel==2?255:0));
				last_height = height;
			}
		}
	}
};
class OneDHistogram : public Histogram
{
private:
	MatND mHistogram[3];
public:
	OneDHistogram( Mat image, int number_of_bins ) :
	  Histogram( image, number_of_bins )
	{
		ComputeHistogram( );
	}
	void ComputeHistogram( )
	{
		vector<Mat> image_planes(mNumberChannels);
		split(mImage, image_planes);
		for (int channel=0; (channel < mNumberChannels); channel++)
		{
			const float* channel_ranges = mChannelRange;
			int *mch = {0};
			calcHist(&(image_planes[channel]), 1, mChannelNumbers, Mat(), mHistogram[channel], 1 , mNumberBins, &channel_ranges);
		}
	}
	void SmoothHistogram( )
	{
		for (int channel=0; (channel < mNumberChannels); channel++)
		{
			MatND temp_histogram = mHistogram[channel].clone();
			for(int i = 1; i < mHistogram[channel].rows - 1; ++i)
			{
				mHistogram[channel].at<float>(i) = (temp_histogram.at<float>(i-1) + temp_histogram.at<float>(i) + temp_histogram.at<float>(i+1)) / 3;
			}
		}
	}
	MatND getHistogram(int index)
	{
		return mHistogram[index];
	}
	void NormaliseHistogram()
	{
		for (int channel=0; (channel < mNumberChannels); channel++)
		{
			normalize(mHistogram[channel],mHistogram[channel],1.0);
		}
	}
	Mat BackProject( Mat& image )
	{
		Mat& result = image.clone();
		if (mNumberChannels == 1)
		{
			const float* channel_ranges[] = { mChannelRange, mChannelRange, mChannelRange };
			for (int channel=0; (channel < mNumberChannels); channel++)
			{
				calcBackProject(&image,1,mChannelNumbers,*mHistogram,result,channel_ranges,255.0);
			}
		}
		else
		{
		}
		return result;
	}
	void Draw( Mat& display_image )
	{
		Draw1DHistogram( mHistogram, mNumberChannels, display_image );
	}
};


class ColourHistogram : public Histogram
{
private:
	MatND mHistogram;
public:
	ColourHistogram( Mat image, int number_of_bins ) :
	  Histogram( image, number_of_bins )
	{
		ComputeHistogram();
	}
	void ComputeHistogram()
	{
		const float* channel_ranges[] = { mChannelRange, mChannelRange, mChannelRange };
		calcHist(&mImage, 1, mChannelNumbers, Mat(), mHistogram, mNumberChannels, mNumberBins, channel_ranges);
	}
	void NormaliseHistogram()
	{
		normalize(mHistogram,mHistogram,1.0);
	}
	Mat BackProject( Mat& image )
	{
		Mat& result = image.clone();
		const float* channel_ranges[] = { mChannelRange, mChannelRange, mChannelRange };
		calcBackProject(&image,1,mChannelNumbers,mHistogram,result,channel_ranges,255.0);
		return result;
	}
	MatND getHistogram()
	{
		return mHistogram;
	}
};


class HueHistogram : public Histogram
{
private:
	MatND mHistogram;
	int mMinimumSaturation, mMinimumValue, mMaximumValue;
#define DEFAULT_MIN_SATURATION 25
#define DEFAULT_MIN_VALUE 25
#define DEFAULT_MAX_VALUE 230
public:
	HueHistogram( Mat image, int number_of_bins, int min_saturation=DEFAULT_MIN_SATURATION, int min_value=DEFAULT_MIN_VALUE, int max_value=DEFAULT_MAX_VALUE ) :
	  Histogram( image, number_of_bins )
	{
		mMinimumSaturation = min_saturation;
		mMinimumValue = min_value;
		mMaximumValue = max_value;
		mChannelRange[1] = 180.0;
		ComputeHistogram();
	}
	void ComputeHistogram()
	{
		Mat hsv_image, hue_image, mask_image;
		cvtColor(mImage, hsv_image, CV_BGR2HSV);
		inRange( hsv_image, Scalar( 0, mMinimumSaturation, mMinimumValue ), Scalar( 180, 256, mMaximumValue ), mask_image );
		int channels[]={0,0};
		hue_image.create( mImage.size(), mImage.depth());
		mixChannels( &hsv_image, 1, &hue_image, 1, channels, 1 );
		const float* channel_ranges = mChannelRange;
		calcHist( &hue_image,1,0,mask_image,mHistogram,1,mNumberBins,&channel_ranges);
		
		cout<<mHistogram<<endl;
		Mat maskImage,res;
		double min,max;
		minMaxLoc(mHistogram,&min,&max);
		inRange(mHistogram, Scalar(max*0.25), Scalar(max),maskImage);
		maskImage.convertTo(maskImage,CV_32FC1);
		normalize(maskImage,maskImage,0,1,CV_MINMAX);
		multiply(mHistogram,maskImage,mHistogram);
		
		//Size sz(7,7);
		Size sz2(5,5);
		//Size sz3(3,3);
	
		//GaussianBlur(mHistogram, mHistogram,sz2,0,0);
		//GaussianBlur(mHistogram, mHistogram,sz2,0,0);
		//GaussianBlur(mHistogram, mHistogram,sz2,0,0);
		//GaussianBlur(mHistogram, mHistogram,sz3,0,0);
		//imshow("hist as a gray scale",mHistogram);

		
	}
	void NormaliseHistogram()
	{
		normalize(mHistogram,mHistogram,0,255,CV_MINMAX);

	}
	Mat BackProject( Mat& image )
	{
		Mat& result = image.clone();
		const float* channel_ranges = mChannelRange;
		calcBackProject(&image,1,mChannelNumbers,mHistogram,result,&channel_ranges,255.0);
		return result;
	}
	MatND getHistogram()
	{
		return mHistogram;
	}
	void Draw( Mat& display_image )
	{
		
		Draw1DHistogram( &mHistogram, 1, display_image );

	}
	vector<colorTypes> get_color_types(){

		vector<colorTypes>colorHistogramMaxima;
		colorHistogramMaxima = get_local_maxima(mHistogram.clone(),8);
		for (vector<colorTypes>::const_iterator i = colorHistogramMaxima.begin(); i != colorHistogramMaxima.end(); ++i){
			 cout<<"centers: "<<i->center<<endl;
		}
		return colorHistogramMaxima;
	}
};

/************************************************************************************************************/

/* Example Code */
/* David Vernon */





vector<colorTypes> computeHueHistogramMaxima(Mat inputImage){

	Mat display_image;
	HueHistogram hh(inputImage,255,30,5,255);
	hh.NormaliseHistogram();
	hh.Draw(display_image);
	imshow("Hue Histogram",display_image);
	
	return hh.get_color_types();
	
}
void generateHueHistogram(char *filename) {
  
   char inputWindowName[MAX_STRING_LENGTH]            = "Input Image";
   char histogramWindowName[MAX_STRING_LENGTH]        = "Histogram";
 
   Mat inputImage,histImage;
   namedWindow(inputWindowName,     CV_WINDOW_AUTOSIZE);  
   namedWindow(histogramWindowName, CV_WINDOW_AUTOSIZE);

   inputImage = imread(filename, CV_LOAD_IMAGE_UNCHANGED); // Read the file, either colour or greyscale

   if (!inputImage.data) {                            // Check for invalid input
      printf("Error: failed to read image %s\n",filename);
      prompt_and_exit(-1);
   }

   printf("Press any key to continue ...\n");        
  
   if (inputImage.type() == CV_8UC1) {     // greyscale image
	return;
   }
   else if (inputImage.type() == CV_8UC3) { // colour image
	HueHistogram hh(inputImage,255,30,5,255);
	hh.NormaliseHistogram();
	hh.Draw(histImage);
   } 
   	
   
   imshow(inputWindowName, inputImage );
   imshow(histogramWindowName, histImage );

   
	

   do{
      waitKey(30);                                  // Must call this to allow openCV to display the images
   } while (!_kbhit());                             // We call it repeatedly to allow the user to move the windows
                                                    // (if we don't the window process hangs when you try to click and drag

   getchar(); // flush the buffer from the keyboard hit

   destroyWindow(inputWindowName);  
   destroyWindow(histogramWindowName); 
}


void generateHistogram(char *filename) {
  
   char inputWindowName[MAX_STRING_LENGTH]            = "Input Image";
   char histogramWindowName[MAX_STRING_LENGTH]        = "Histogram";
 
   Mat inputImage;
   Mat histogramImage;

   const int* channel_numbers = { 0 };
   float channel_range[] = { 0.0, 255.0 };
   const float* channel_ranges = channel_range;
   int number_bins = 64;

   namedWindow(inputWindowName,     CV_WINDOW_AUTOSIZE);  
   namedWindow(histogramWindowName, CV_WINDOW_AUTOSIZE);

   inputImage = imread(filename, CV_LOAD_IMAGE_UNCHANGED); // Read the file, either colour or greyscale

   if (!inputImage.data) {                            // Check for invalid input
      printf("Error: failed to read image %s\n",filename);
      prompt_and_exit(-1);
   }

   printf("Press any key to continue ...\n");        
  
   if (inputImage.type() == CV_8UC1) {     // greyscale image
      MatND grey_histogram;

      calcHist( &inputImage, 1, channel_numbers, Mat(), grey_histogram, 1, &number_bins, &channel_ranges);
	   OneDHistogram::Draw1DHistogram( &grey_histogram, 1, histogramImage );
   }
   else if (inputImage.type() == CV_8UC3) { // colour image
      MatND* colour_histogram = new MatND[ inputImage.channels() ];  

	   vector<Mat> colour_channels( inputImage.channels() );
	   split(inputImage, colour_channels );
	 
      for (int chan=0; chan < inputImage.channels(); chan++) {
         calcHist( &(colour_channels[chan]), 1, channel_numbers, Mat(), colour_histogram[chan], 1, &number_bins, &channel_ranges);
      }

	   OneDHistogram::Draw1DHistogram( colour_histogram, inputImage.channels(), histogramImage );
   } 
   	
   
   imshow(inputWindowName, inputImage );
   imshow(histogramWindowName, histogramImage); 

	// This can also be done using the OneDHistogram class provided above:
	//Mat histogramImage2;
	//OneDHistogram histogram(inputImage,64);
	//histogram.Draw(histogramImage2);
	//imshow("Histogram 2", histogramImage2);


   do{
      waitKey(30);                                  // Must call this to allow openCV to display the images
   } while (!_kbhit());                             // We call it repeatedly to allow the user to move the windows
                                                    // (if we don't the window process hangs when you try to click and drag

   getchar(); // flush the buffer from the keyboard hit

   destroyWindow(inputWindowName);  
   destroyWindow(histogramWindowName); 
}
void generateSatHistogram(Mat inputImage) {
  
   char inputWindowName[MAX_STRING_LENGTH]            = "Input Image";
   char histogramWindowName[MAX_STRING_LENGTH]        = "Saturation Histogram";
 

   Mat histogramImage;

   const int* channel_numbers = { 0 };
   float channel_range[] = { 0.0, 255.0 };
   const float* channel_ranges = channel_range;
   int number_bins = 255;

   namedWindow(inputWindowName,     CV_WINDOW_AUTOSIZE);  
   namedWindow(histogramWindowName, CV_WINDOW_AUTOSIZE);



   printf("Press any key to continue ...\n");        
  
   if (inputImage.type() == CV_8UC1) {     // greyscale image
      MatND grey_histogram;

      calcHist( &inputImage, 1, channel_numbers, Mat(), grey_histogram, 1, &number_bins, &channel_ranges);
	   OneDHistogram::Draw1DHistogram( &grey_histogram, 1, histogramImage );
   }
   else if (inputImage.type() == CV_8UC3) { // colour image
      MatND* colour_histogram = new MatND[ inputImage.channels() ];  
	   Mat hsvImage,mask_image;
	   cvtColor(inputImage,hsvImage,CV_BGR2HSV);
	   inRange( hsvImage, Scalar( 0,5,0), Scalar( 180, 256,255 ), mask_image );
	   
	   vector<Mat> colour_channels( hsvImage.channels() );
	   split(hsvImage, colour_channels );


	   MatND grey_histogram;

	   calcHist( &colour_channels[1], 1, channel_numbers, mask_image, grey_histogram, 1, &number_bins, &channel_ranges);
	   //OneDHistogram::SmoothHistogram();
	   OneDHistogram::Draw1DHistogram( &grey_histogram, 1, histogramImage );


	 
      //for (int chan=0; chan < hsvImage.channels(); chan++) {
        // calcHist( &(colour_channels[chan]), 1, channel_numbers, Mat(), colour_histogram[chan], 1, &number_bins, &channel_ranges);
      //}

	 //  OneDHistogram::Draw1DHistogram( colour_histogram, hsvImage.channels(), histogramImage );
   } 
   	
   
   imshow(inputWindowName, inputImage );
   imshow(histogramWindowName, histogramImage); 



   do{
      waitKey(30);                                  // Must call this to allow openCV to display the images
   } while (!_kbhit());                             // We call it repeatedly to allow the user to move the windows
                                                    // (if we don't the window process hangs when you try to click and drag

   getchar(); // flush the buffer from the keyboard hit

   destroyWindow(inputWindowName);  
   destroyWindow(histogramWindowName); 
}
void count_defective_sweets(Mat inputImage){

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
	vector<colorTypes> sweetBBs = get_individual_sweets(filteredImage.clone(),inputImage.clone(),defectiveSmartiesCount);
	vector<colorTypes> finalColorTypesNoRedRedundance = combine_red_smarties(sweetBBs);
	for (vector<colorTypes>::const_iterator i = finalColorTypesNoRedRedundance.begin(); i != finalColorTypesNoRedRedundance.end(); ++i){

		cout<<"color type: "<<i->center	<<" defective count: "<<i->defective_count<<endl;
	}
	cout<<"total defective smarties count:"<<defectiveSmartiesCount<<endl;
	cout<<"total number of colors: "<<(int)finalColorTypesNoRedRedundance.size()<<endl;
	//errode to remove noise especially at the edges of the sweets 
	computeHueHistogramMaxima(inputImage);
	imshow(inputWindowName,inputImage);
	imshow(binaryWindowName,filteredImage);
	do{
       waitKey(30);                                  // Must call this to allow openCV to display the images
     } while (!_kbhit());                             // We call it repeatedly to allow the user to move the windows
                                                    // (if we don't the window process hangs when you try to click and drag

   getchar(); // flush the buffer from the keyboard hit

   destroyWindow(inputWindowName);  
   destroyWindow(binaryWindowName); 

} 

vector<colorTypes>  get_individual_sweets(Mat filteredImage,Mat inputImage,int& defect_count){
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
			cout<<"current smarty is of color: "<<inputImageColorTypes[currentSmartyColorIndex].center<<endl;
			if (is_defective(convexity_defects,1000)){
				inputImageColorTypes[currentSmartyColorIndex].defective_count =inputImageColorTypes[currentSmartyColorIndex].defective_count+1;
				total_defective_count++;
			}
		}
	}
    defect_count = total_defective_count;
	return inputImageColorTypes;

}
/*=======================================================*/
/* Utility functions to prompt user to continue          */ 
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

		if (contourDefects[defect_index][3]>defectDepthThreshold && contourDefects[defect_index][3]<2*defectDepthThreshold)
			return true;
				
		
	}

	return false;
}
int find_minimum_distance_match(vector<colorTypes> inputImageColorTypes,vector<colorTypes> currentContourColorType){
	
	int min_index = 0;
	int minim_distance = 1000;
	int distance;
	for (int color_number=0; (color_number<(int)inputImageColorTypes.size());color_number++) {

		
		distance  = abs(inputImageColorTypes[color_number].center-currentContourColorType[0].center);
		if(distance<minim_distance){
			min_index = color_number;
			minim_distance = distance;
		}

	
	}
		cout<<"current contour color: "<<currentContourColorType[0].center<<endl;
	return min_index;
}
vector<colorTypes> combine_red_smarties(vector <colorTypes> finalColorTypes){
	vector<colorTypes> finalColorTypeNoRedRedundance;

	int red_count = 0;
	int red_index = 0;
	
	for (int color_number=0; (color_number<(int)finalColorTypes.size());color_number++) {
		cout<<"i am working with: "<<finalColorTypes[color_number].center<<endl;
		if(finalColorTypes[color_number].center<3||finalColorTypes[color_number].center>252){
			if (red_count ==0){//first red
				
				finalColorTypeNoRedRedundance.push_back(finalColorTypes[color_number]);
				red_index = color_number;
				red_count ++;
				cout<<"first push red index: "<<red_index<<endl;
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
void chop_histogram(Mat mHistogram){
Mat maskImage,res;
double min,max;
minMaxLoc(mHistogram,&min,&max);
inRange(mHistogram, Scalar(max*0.25), Scalar(max),mHistogram);

/*maskImage.convertTo(maskImage,CV_32FC1);
cout<<"max value : "<<max<<endl;
cout<<"masked hist : "<<mHistogram<<endl;
cout<<"hist channel number: "<<mHistogram.channels()<<"type: "<<mHistogram.type()<<endl;
cout<<"mask image channel number: "<<maskImage.channels()<<"type: "<<maskImage.type()<<endl;

bitwise_and(mHistogram,maskImage,res);
cout<<"res : "<<res<<endl;
*/

}