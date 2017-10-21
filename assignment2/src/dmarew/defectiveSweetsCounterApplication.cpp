/* 
  Example use of openCV to generate a histogram of an image
  ---------------------------------------------------------
  Application file (i.e. main)

  David Vernon
  19 May 2017
  
*/
#include "defectiveSweetsCounter.h"


int main() {

   int end_of_file;
   bool debug = true;
   char filename[MAX_FILENAME_LENGTH];
   Mat inputImage;
   FILE *fp_in,*fp_out;
   //FILE *fp_out;
   
   if ((fp_in = fopen("../data/input.txt","r")) == 0) {
	  printf("Error can't open input histogramInput.txt\n");
     prompt_and_exit(1);
   }
      if ((fp_out = fopen("../data/output.txt","w")) == 0) {
	  printf("Error can't open output file output.txt\n");
     prompt_and_exit(1);
   }

   fprintf(fp_out,"dmarew\n");


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
		 int totalDefectiveCount,totalNumberOfColors;
		 vector<colorTypes> defectivePerColor;
		 count_defective_sweets(inputImage,totalDefectiveCount,totalNumberOfColors,defectivePerColor);
		 int samplearray[3] = {1,2,3};
		 stringstream ss;
		 ss << filename <<" "<<totalDefectiveCount<<" defective sweets, "<<totalNumberOfColors<<" colours, ";
		 for (int color_number=0; (color_number<(int)defectivePerColor.size());color_number++)
			 ss<<((int)defectivePerColor[color_number].defective_count)<<" ";
		 ss<<"defects per colour";
		 fprintf(fp_out,"%s\n",ss.str());
		 //generateHueHistogram(filename);
      }
   } while (end_of_file != EOF);


   fclose(fp_in);
   fclose(fp_out);
   
   return 0;
}

