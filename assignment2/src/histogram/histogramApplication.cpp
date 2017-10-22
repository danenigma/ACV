/* 
  Example use of openCV to generate a histogram of an image
  ---------------------------------------------------------
  Application file (i.e. main)

  David Vernon
  19 May 2017
  
*/
 
#include "histogram.h"

int main() {

   int end_of_file;
   bool debug = true;
   char filename[MAX_FILENAME_LENGTH];
   Mat inputImage;
   FILE *fp_in;
   //FILE *fp_out;
   
   if ((fp_in = fopen("../data/input.txt","r")) == 0) {
	  printf("Error can't open input histogramInput.txt\n");
     prompt_and_exit(1);
   }
   /*
   if ((fp_out = fopen("../data/histogramOutput.txt","w")) == 0) {
	  printf("Error can't open output logPolarTransformOutput.txt\n");
     prompt_and_exit(1);
   }
   */

   printf("Example of how to use openCV to generate a histogram of an image.\n\n");
   
   do {

      end_of_file = fscanf(fp_in, "%s", filename);
      
      if (end_of_file != EOF) {
         //if (debug) printf ("%s\n",filename);
         printf("\nGenerating histogram of image in %s \n",filename);
		 inputImage = imread(filename, CV_LOAD_IMAGE_UNCHANGED);
         if(inputImage.empty()) {
            cout << "can not open here" << filename << endl;
            prompt_and_exit(-1);
         }

		 //count_defective_sweets(inputImage);
		 computeHueHistogramMaxima(inputImage.clone());
		 generateSatHistogram(inputImage.clone());
         //generateHueHistogram(filename);
      }
   } while (end_of_file != EOF);


   fclose(fp_in);
   //fclose(fp_out);
   
   return 0;
}

