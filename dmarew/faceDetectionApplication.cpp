
 
#include "faceDetection.h"
 
int main() {

   int debug = TRUE;
   char message[MAX_MESSAGE_LENGTH];

   FILE *fp_in, *fp_out;

   if ((fp_in = fopen("../data/input.txt","r")) == 0) {
	  printf("Error can't open input input.txt\n");
     prompt_and_exit(1);
   }

   if ((fp_out = fopen("../data/output.txt","w")) == 0) {
	  printf("Error can't open output output.txt\n");
     prompt_and_exit(1);
   }

   // read the message from the input file

   fgets(message, MAX_MESSAGE_LENGTH, fp_in);
 
   if (debug) printf ("%s\n",message);

   print_message_to_file(fp_out, message);

   fclose(fp_in);
   fclose(fp_out);

   prompt_and_exit(0);
}
