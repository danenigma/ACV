/* 
  Example use of openCV to use the Hough Transform
  ------------------------------------------------
  Application file (i.e. main)

 
  David Vernon
  14 Feburary 2017

*/
 
#include "houghTransformLines.h"

// Global variables to allow access by the display window callback functions

Mat src;
Mat src_blur;
Mat src_gray;
Mat detected_edges;
Mat hough; 
int cannyThreshold             = 120;         // low threshold for Canny edge detector
char* canny_window_name        = "Canny Edge Map";

int houghThreshold             = 44;//used to be 50;         // number of intersections
char* hough_window_name        = "Hough Lines";

char* input_window_name        = "Input Image";

int view;

int main() {
         
   int end_of_file;
   bool debug = true;
   char filename[MAX_FILENAME_LENGTH];

   int const max_cannyThreshold   = 200;  // max low threshold for Canny
   int const max_houghThreshold   = 100;    

   FILE *fp_in;
   FILE *fp_out;

   printf("Example of how to use openCV to perform the Hough transform.\n\n");

   if ((fp_in = fopen("../data/houghTransformLinesInput.txt","r")) == 0) {
	  printf("Error can't open input file houghTransformLinesInput.txt\n");
     prompt_and_exit(1);
   }

   /*
   if ((fp_out = fopen("../data/houghTransformLinesOutput.txt","w")) == 0) {
	  printf("Error can't open output file houghTransformLinesOutput.txt\n");
     prompt_and_exit(1);
   }
   */

   do {

      end_of_file = fscanf(fp_in, "%s", filename);
      
      if (end_of_file != EOF) {
         //if (debug) printf ("%s\n",filename);

         src = imread(filename, CV_LOAD_IMAGE_COLOR);
         if(src.empty()) {
            cout << "can not open " << filename << endl;
            return -1;
         }
          
         printf("Press any key to continue ...\n");

         // Create a window for input and display it
         namedWindow(input_window_name, CV_WINDOW_AUTOSIZE );
         imshow(input_window_name, src);
 
         // Create a window
         namedWindow(canny_window_name, CV_WINDOW_AUTOSIZE );
         resizeWindow(canny_window_name,0,0); // this forces the trackbar to be as small as possible (and to fit in the window)

         createTrackbar( "Threshold:", canny_window_name, &cannyThreshold, max_cannyThreshold, CannyThreshold );

         // Create a window
         namedWindow(hough_window_name, CV_WINDOW_AUTOSIZE );
         resizeWindow(hough_window_name,0,0); // this forces the trackbar to be as small as possible (and to fit in the window)

         createTrackbar( "Threshold:", hough_window_name, &houghThreshold, max_houghThreshold, HoughThreshold );

         // Show the image
         CannyThreshold(0, 0);
         HoughThreshold(0, 0);
 
         //waitKey(0);  
         
         do{
            waitKey(30);                                  // Must call this to allow openCV to display the images
         } while (!_kbhit());                             // We call it repeatedly to allow the user to move the windows
                                                          // (if we don't the window process hangs when you try to click and drag

         getchar(); // flush the buffer from the keyboard hit

         destroyWindow(input_window_name);  
         destroyWindow(canny_window_name); 
         destroyWindow(hough_window_name); 

      }
   } while (end_of_file != EOF);

   fclose(fp_in);
   // fclose(fp_out);
    
   return 0;
}




/* Original version with option to compile the probabilistic Hough transform 
  
  This software is based on the openCV tutorial here
  http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html#hough-lines
*/

/*
int main(int argc, char** argv)
{
 const char* filename = argc >= 2 ? argv[1] : "../data/cube1.png";

 Mat src = imread(filename, 0);
 if(src.empty())
 {
     help();
     cout << "can not open " << filename << endl;
     return -1;
 }

 Mat dst, cdst;
 Canny(src, dst, 50, 200, 3);
 cvtColor(dst, cdst, CV_GRAY2BGR);

 #if TRUE
  vector<Vec2f> lines;
  HoughLines(dst, lines, 1, CV_PI/180, 100, 0, 0 );

  for( size_t i = 0; i < lines.size(); i++ )
  {
     float rho = lines[i][0], theta = lines[i][1];
     Point pt1, pt2;
     double a = cos(theta), b = sin(theta);
     double x0 = a*rho, y0 = b*rho;
     pt1.x = cvRound(x0 + 1000*(-b));
     pt1.y = cvRound(y0 + 1000*(a));
     pt2.x = cvRound(x0 - 1000*(-b));
     pt2.y = cvRound(y0 - 1000*(a));
     line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
  }
 #else
  vector<Vec4i> lines;
  HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];
    line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
  }
 #endif
 imshow("source", src);
 imshow("detected lines", cdst);

 waitKey();

 return 0;

 }
 */

