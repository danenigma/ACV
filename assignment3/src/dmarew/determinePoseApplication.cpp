/* 
  Example use of openCV to use the Hough Transform
  ------------------------------------------------
  Application file (i.e. main)

 
  David Vernon
  14 Feburary 2017

*/
 
#include "determinePose.h"

// Global variables to allow access by the display window callback functions




int main() {
    
   Point center;
   int orientation;
   int end_of_file;
   bool debug = true;
   char filename[MAX_FILENAME_LENGTH];

   FILE *fp_in;
   FILE *fp_out;



   if ((fp_in = fopen("../data/input.txt","r")) == 0) {
	  printf("Error can't open input file houghTransformLinesInput.txt\n");
      prompt_and_exit(1);
   }

   
   if ((fp_out = fopen("../data/output.txt","w")) == 0) {
	  printf("Error can't open output file houghTransformLinesOutput.txt\n");
      prompt_and_exit(1);
   }
  

   do {

      end_of_file = fscanf(fp_in, "%s", filename);
      
      if (end_of_file != EOF) {
		 get_pose_of_cube(filename,center,orientation);
  		 stringstream ss;
		 ss << filename <<": "<<center.x<<" "<<center.y<<" "<<orientation;        
         printf("Press any key to continue ...\n");
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


