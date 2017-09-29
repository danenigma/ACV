
 
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

int main() {
      
   int end_of_file;
   bool debug = true;
   char filename[MAX_FILENAME_LENGTH];
   Mat inputImage;

   FILE *fp_in, *fp_out;
   
   if ((fp_in = fopen("../data/input.txt","r")) == 0) {
	  printf("Error can't open input imageAcquisitionInput.txt\n");
     prompt_and_exit(1);
   }



   
   do {

      end_of_file = fscanf(fp_in, "%s", filename);
      
      if (end_of_file != EOF) { 
         printf("\nDisplaying image from image file %s \n",filename);
		 count_smarties_with_morphology(filename);
	  }
   } while (end_of_file != EOF);


   fclose(fp_in);
   /* fclose(fp_out); */
   // (if we don't the window process hangs when you try to click and drag)
   getchar(); // flush the buffer from the keyboard hit

  
   return 0;
}
