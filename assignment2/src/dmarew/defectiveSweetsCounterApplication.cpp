/* 
   defectiveSweetsCounterApplication.cpp - Application file for a program that counts the different sweet colors
   and the number of defective sweets of each color type in an image
  ---------------------------------------------------------
  Application file (i.e. main)

  Daniel Marew
  10/25/2017
  
*/
#include "defectiveSweetsCounter.h"


int main() {

   int end_of_file;
   bool debug = true;
   char filename[MAX_FILENAME_LENGTH];
   Mat inputImage;
   FILE *fp_in,*fp_out;
   
   
   if ((fp_in = fopen("../data/input.txt","r")) == 0) {
	  printf("Error can't open input histogramInput.txt\n");
     prompt_and_exit(1);
   }
      if ((fp_out = fopen("../data/output.txt","w")) == 0) {
	  printf("Error can't open output file output.txt\n");
     prompt_and_exit(1);
   }

   fprintf(fp_out,"dmarew\n");


   printf("counting the number of colors and the number of defective smarties\n\n");
   
   do {

      end_of_file = fscanf(fp_in, "%s", filename);
      
      if (end_of_file != EOF) {

		 inputImage = imread(filename, CV_LOAD_IMAGE_UNCHANGED);
         if(inputImage.empty()) {
            cout << "can not open here" << filename << endl;
            prompt_and_exit(-1);
         }
		 int totalDefectiveCount,totalNumberOfColors;
		 vector<colorTypes> defectivePerColor;
		 /*get the number of defective sweets by color*/
		 count_defective_sweets(inputImage,totalDefectiveCount,totalNumberOfColors,defectivePerColor);
		 int samplearray[3] = {1,2,3};
		 stringstream ss;
		 ss << filename <<" "<<totalDefectiveCount<<" defective sweets, "<<totalNumberOfColors<<" colours: ";
		 for (int color_number=0; (color_number<(int)defectivePerColor.size());color_number++)
			 ss<<((int)defectivePerColor[color_number].defective_count)<<" ";
		 ss<<"defects per colour";
		 /*write to file*/
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

