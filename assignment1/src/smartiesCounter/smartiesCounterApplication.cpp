
 
#include "smartiesCounter.h"
 
/*
    Assumptions:

	- all the color types of Smarties appear on the image
	- size of the Smarties is uniform and don't differ from(may be slightly) the test images  
	- white background
	- speed is not much of a concern (kmeans takes time)
	Limitations[so far]
	- if all the Smarty types are not represented in the image ,since we have fixed number
	  of clusters on the kmeans setup, we might count some of the smarties more than once

*/
Mat inputImage;
int halfStructuringElementSize  = 1;   // default size of structuring element divided by two: structuring element size = value * 2 + 1
int dtThreshold = 5;
int dtThresholdIt = 165;

char* inputWindowName     = "Input Image";
char* binaryWindowName    = "Flooded Image";
char* processedWindowName = "Final output Image";
char* dtThreshWindowName  = "distance transform Thresh Image";
int view;


int main() {

   int end_of_file;
   bool debug = true;
   char filename[MAX_FILENAME_LENGTH];
   
   int const maxHalfStructuringElementSize  =   11;   
   int const maxIteration    =   10;   
   int const maxDtThreshold  =   10;   
   int const maxDtThresholdIt  =   255;   
   
   FILE *fp_in,*fp_out;

   if ((fp_in = fopen("../data/input.txt","r")) == 0) {
	  printf("Error can't open input file input.txt\n");
      prompt_and_exit(1);
   }
   if ((fp_out = fopen("../data/output.txt","w")) == 0) {
	  printf("Error can't open output file output.txt\n");
     prompt_and_exit(1);
   }

   fprintf(fp_out,"dmarew\n");
   do {

      end_of_file = fscanf(fp_in, "%s", filename);
      
      if (end_of_file != EOF) {
         inputImage = imread(filename, CV_LOAD_IMAGE_UNCHANGED);
         if(inputImage.empty()) {
            cout << "can not open " << filename << endl;
            prompt_and_exit(-1);
         }

         printf("Press any key to continue ...\n");
         namedWindow(inputWindowName, CV_WINDOW_AUTOSIZE);
		 
         // Create a window
         namedWindow(binaryWindowName,    CV_WINDOW_AUTOSIZE );
         namedWindow(processedWindowName, CV_WINDOW_AUTOSIZE );
         resizeWindow(processedWindowName,0,0); // this forces the trackbar to be as small as possible (and to fit in the window)
		 int smartiesCount;
		 smartiesCount = count_smarties();
		 stringstream ss;
		 ss << filename <<": "<<smartiesCount<<" sweets";
		 fprintf(fp_out,"%s\n",ss.str());
		 cout<<ss.str()<<endl;

         do{
            waitKey(30);                                  // Must call this to allow openCV to display the images
         } while (!_kbhit());                             // We call it repeatedly to allow the user to move the windows
                                                          // (if we don't the window process hangs when you try to click and drag
         getchar(); // flush the buffer from the keyboard hit

		 destroyAllWindows();

      }
   } while (end_of_file != EOF);

   fclose(fp_in);
   fclose(fp_out);
   return 0;

}
