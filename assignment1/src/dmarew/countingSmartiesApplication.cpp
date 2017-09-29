
 
#include "countingSmarties.h"
 


int main() {
      
   int end_of_file;
   bool debug = true;
   char filename[MAX_FILENAME_LENGTH];

   FILE *fp_in, *fp_out;
   
   if ((fp_in = fopen("../data/input.txt","r")) == 0) {
	  printf("Error can't open input imageAcquisitionInput.txt\n");
     prompt_and_exit(1);
   }


   printf("Example of how to use openCV to acquire and display images\n");
   
   do {

      end_of_file = fscanf(fp_in, "%s", filename);
      
      if (end_of_file != EOF) { 
         printf("\nDisplaying image from image file %s \n",filename);
		 printf("\n The number of smarties is : %i \n",get_smarties_count_and_display(filename,0));
	  }
   } while (end_of_file != EOF);


   fclose(fp_in);
   /* fclose(fp_out); */
   // (if we don't the window process hangs when you try to click and drag)
   getchar(); // flush the buffer from the keyboard hit

  
   return 0;
}
