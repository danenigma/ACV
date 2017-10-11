

#include "countingSmarties.h"


void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal\n");
   getchar();
   exit(status);
}

void print_message_to_file(FILE *fp, char message[]) {
   fprintf(fp,"The message is: %s\n", message);
}
int get_contours(Mat threshImage){
   RNG rng(12345);
   vector<vector<Point>> contours;
   vector<Vec4i> hierarchy;
   findContours( threshImage.clone(), contours, hierarchy,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );
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
void flood_fill(Mat img){
   Mat im_floodfill = img.clone();
   floodFill(im_floodfill, cv::Point(0,0), Scalar(255));
    // Invert floodfilled image
   Mat im_floodfill_inv;
   bitwise_not(im_floodfill, im_floodfill_inv);  
    // Combine the two images to get the foreground.
   img = (img | im_floodfill_inv);
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
int  get_smarties_count_and_display(char *filename,void*){

   string inputWindowName       = "Input Image";
   string erodeWindowName       = "Eroded Image";
   
   namedWindow(inputWindowName, CV_WINDOW_AUTOSIZE); 
   
   Mat image,src_gray;
   Mat src_blur;
   Mat detected_edges,threshImage,output,im_out;
   
   int cannyThreshold =50; 
   int gaussian_std_dev = 1; 

   double sigma = 2.0;
   int ratio = 3;
   int kernel_size = 3;
   int filter_size;
   int element_size = 1;
   filter_size = gaussian_std_dev * 4 + 1;  
   image = imread(filename, CV_LOAD_IMAGE_COLOR);   // Read the file

   if (!image.data) {                               // Check for invalid input
      printf("Error: failed to read image\n");
      prompt_and_exit(-1);
   }
   cvtColor(image, src_gray, CV_BGR2GRAY);
   threshold(src_gray,threshImage,230,255,1);
   GaussianBlur(src_gray, src_blur, Size(filter_size,filter_size), gaussian_std_dev);
   Canny( src_blur, detected_edges, cannyThreshold, cannyThreshold*ratio, kernel_size );
   Mat element = getStructuringElement( MORPH_RECT,
                                       Size( 2*element_size + 1, 2*element_size+1 ),
                                       Point(element_size, element_size));
   imshow("canny", detected_edges );
   subtract(threshImage,detected_edges,output);
   erode(output,output,element);
   morphologyEx(output,output,MORPH_CLOSE,element);
   im_out = output.clone();
   flood_fill(im_out);
   imshow("flood",im_out);
   imshow(inputWindowName,image );  
   int number_of_smarties=get_contours(im_out);
   //imshow("contours",);
   do{
      waitKey(30);                                  // Must call this to allow openCV to display the images
   } while (!_kbhit());                             // We call it repeatedly to allow the user to move the windows
                                                    // (if we don't the window process hangs when you try to click and drag)
   getchar(); // flush the buffer from the keyboard hit
   
return number_of_smarties;
}

