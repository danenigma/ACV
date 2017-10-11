
 
#include "sweetsCounter.h"
 

Mat inputImage;


int view;


int main() {

   int  end_of_file;
   bool debug = true;
   char filename[MAX_FILENAME_LENGTH];
   int  smartiesCount;
   

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

		 
		 smartiesCount = count_sweets();
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
