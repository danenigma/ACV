
 
#include "morphologySmartiesCounter.h"
 
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
int iteration = 1;
int dtThreshold = 5;
int dtThresholdIt = 165;

char* inputWindowName     = "Input Image";
char* binaryWindowName    = "Binary Image";
char* processedWindowName = "Eroded Image";
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

   /*
   Mat image1,image2,out;
   image1 = imread("../data/assignment1B.jpg");
   resize(image1,image1,Size(600,300));//resize image
   image2 = imread("../data/assignment1A.jpg");
   resize(image2,image2,Size(600,300));
   hconcat(image1,image2,out);
   imwrite("../data/joindSmarties.jpg",out);
	*/
   FILE *fp_in;

   if ((fp_in = fopen("../data/input.txt","r")) == 0) {
	  printf("Error can't open input file input.txt\n");
      prompt_and_exit(1);
   }
   do {

      end_of_file = fscanf(fp_in, "%s", filename);
      
      if (end_of_file != EOF) {
         inputImage = imread(filename, CV_LOAD_IMAGE_UNCHANGED);
         if(inputImage.empty()) {
            cout << "can not open " << filename << endl;
            prompt_and_exit(-1);
         }
		 cout<<inputImage.size()<<endl;
         printf("Press any key to continue ...\n");
         namedWindow(inputWindowName, CV_WINDOW_AUTOSIZE);
		 
         // Create a window
         namedWindow(binaryWindowName,    CV_WINDOW_AUTOSIZE );
         namedWindow(processedWindowName, CV_WINDOW_AUTOSIZE );
         resizeWindow(processedWindowName,0,0); // this forces the trackbar to be as small as possible (and to fit in the window)

         createTrackbar( "Size/2",processedWindowName, &halfStructuringElementSize,
			 maxHalfStructuringElementSize,count_smarties_with_morphology); // same callback
		 createTrackbar( "Iteration",processedWindowName, &iteration,
			 maxIteration,count_smarties_with_morphology); // same callback
		 createTrackbar( "DtThreshold",processedWindowName, &dtThreshold,
			 maxDtThreshold,count_smarties_with_morphology); // same callback
		 createTrackbar( "DtThresholdIt",dtThreshWindowName, &dtThresholdIt,
			 maxDtThresholdIt,count_smarties_with_morphology); // same callback
		 // Show the image
         count_smarties_with_morphology(0, 0);
		 
         do{
            waitKey(30);                                  // Must call this to allow openCV to display the images
         } while (!_kbhit());                             // We call it repeatedly to allow the user to move the windows
                                                          // (if we don't the window process hangs when you try to click and drag

         getchar(); // flush the buffer from the keyboard hit

		 destroyAllWindows();

      }
   } while (end_of_file != EOF);

   fclose(fp_in);

   return 0;

}
