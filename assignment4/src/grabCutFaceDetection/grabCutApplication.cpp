/* 
  Example use of openCV to perform image segmentation with the grabCut algorithm
  ------------------------------------------------------------------------------
  Application file

  For a more sophisticated example, see http://docs.opencv.org/trunk/de/dd0/grabcut_8cpp-example.html

  David Vernon
  18 September 2017
*/

#include "grabCut.h"


// Global variables to allow access by the display window callback functions

Mat inputImage;
Mat bgImage;
int numberOfIterations        = 1; // default number of iterations
int number_of_control_points  = 0;

char* input_window_name       = "Input Image";
char* grabcut_window_name     = "GrabCut Image";


int main() {
         
   int end_of_file;
   bool debug = true;
   char filename[MAX_FILENAME_LENGTH];
   int const max_iterations  = 5; 

   FILE *fp_in; 

   printf("Example use of openCV to perform image segmentation using the grabCut algorithm\n\n");

   if ((fp_in = fopen("../data/input.txt","r")) == 0) {
	  printf("Error can't open input file grabCutInput.txt\n");
     prompt_and_exit(1);
   }

   bgImage = imread("../data/cmubg.jpg", CV_LOAD_IMAGE_UNCHANGED);
   do {
	 
      end_of_file = fscanf(fp_in, "%s", filename);
      
      if (end_of_file != EOF) { 

         
		 inputImage = imread(filename, CV_LOAD_IMAGE_UNCHANGED);
		
		 if(inputImage.empty()) {
            cout << "can not open " << filename << endl;
            prompt_and_exit(-1);
         }
          
         printf("Press any key to continue ...\n");


         Mat blankImage(inputImage.size(),CV_8UC3,cv::Scalar(255,255,255));
         imshow(input_window_name,inputImage);  


         // process the image
         number_of_control_points = 0; // don't segment until the region in interest is specified
         performGrabCut(0, 0);

         do{
            waitKey(30);                
         } while (!_kbhit());                            

         getchar(); // flush the buffer from the keyboard hit

         destroyAllWindows();  

      }
   } while (end_of_file != EOF);

   fclose(fp_in);

   return 0;
}
