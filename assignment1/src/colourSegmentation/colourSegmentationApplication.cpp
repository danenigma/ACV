/* 
  Example use of openCV to perform colour segmentation
   
  The user must interactively select the colour sample that will 
  form the basis of the segmentation. The user can also adjust the
  hue and saturation tolerances on that sample
  ------------------------------------------------------------
  Application file

  David Vernon
  1 June 2017
 
*/
 
#include "colourSegmentation.h"
#include "colorHistogram.h"

// Global variables to allow access by the display window callback functions

Mat inputBGRImage;
Mat inputHLSImage;
int hueRange            =4; // default range
int saturationRange     = 128; // default range
Point2f sample_point; 
int number_of_sample_points;

char* input_window_name       = "Input Image";
char* segmented_window_name   = "Segmented Image";


int main() {

   int end_of_file;
   bool debug = false;
   char filename[MAX_FILENAME_LENGTH];
   int max_hue_range = 180;
   int max_saturation_range = 128;
   int colorArray[] = {171,14,6,155,36,27,103,123};
   Mat outputImage;
   ColorHistogram ch;
   FILE *fp_in;
   //FILE *fp_out;
   
   if ((fp_in = fopen("../data/colourSegmentationInput.txt","r")) == 0) {
	  printf("Error can't open input colourSegmentationInput.txt\n");
     prompt_and_exit(1);
   }
   /*
   if ((fp_out = fopen("../data/colourSegmentationOutput.txt","w")) == 0) {
	  printf("Error can't open output colourSegmentationOutput.txt\n");
     prompt_and_exit(1);
   }
   */

   printf("Example of how to use openCV to perform colour segmentation.\n\n");
   
   do {

      end_of_file = fscanf(fp_in, "%s", filename);
      
      if (end_of_file != EOF) {
         //if (debug) printf ("%s\n",filename);

         inputBGRImage = imread(filename, CV_LOAD_IMAGE_UNCHANGED);
         if(inputBGRImage.empty()) {
            cout << "can not open " << filename << endl;
            prompt_and_exit(-1);
         }

         CV_Assert(inputBGRImage.type() == CV_8UC3 ); // make sure we are dealing with a colour image

         printf("Click on a sample point in the input image.\n");
         printf("When finished with this image, press any key to continue ...\n");

         // Create a window for input and display it
         namedWindow(input_window_name, CV_WINDOW_AUTOSIZE );
         setMouseCallback(input_window_name, getSamplePoint);    // use this callback to get the colour components of the sample point
         //GaussianBlur(inputBGRImage,inputBGRImage,Size(11,11),0);
		 remove_white_bg(inputBGRImage);
		 int histSize = 361;
		 float range[] = { 0, 361 } ;
		 const float* histRange = { range };
		 bool uniform = true; bool accumulate = false;
		

		 cout<<get_total_count(inputBGRImage,colorArray,5)<<endl;

		 imwrite("new.jpg",inputBGRImage);
		 imshow(input_window_name, inputBGRImage);
		 cvtColor(inputBGRImage, inputHLSImage, CV_BGR2HLS);
		 //get_kmeans_clustor(inputBGRImage);
         //namedWindow(segmented_window_name, CV_WINDOW_AUTOSIZE );
         resizeWindow(segmented_window_name,0,0); // this forces the trackbar to be as small as possible (and to fit in the window)

         createTrackbar( "Hue Range", segmented_window_name, &hueRange,        max_hue_range,        colourSegmentation);
         createTrackbar( "Sat Range", segmented_window_name, &saturationRange, max_saturation_range, colourSegmentation);

         // display a zero output 
 
         outputImage = Mat::zeros(inputBGRImage.rows, inputBGRImage.cols, inputBGRImage.type()); 
         imshow(segmented_window_name, outputImage);
		 
         waitKey(30); 

         // need a sample point to get started ... wait for user to click on input image
         
         // Show the image for default thresholds
         // colourSegmentation(0, 0);

         // now wait for user interaction - mouse click to change the colour sample or trackbar adjustment to change the thresholds
         
         number_of_sample_points = 0;

         do{
            waitKey(30);                             
         } while (!_kbhit());      

         
         if (debug) printf("kbhit ... \n");

         getchar(); // flush the buffer from the keyboard hit

         destroyWindow(input_window_name);  
         destroyWindow(segmented_window_name); 
      }
   } while (end_of_file != EOF);

   fclose(fp_in);
   //fclose(fp_out);
   
   return 0;
}

