/* 

   exampleApplication.cpp - application file for a program named example
   =====================================================================

   This program is for course 04-801 Cognitive Robotics, Assignment No. 0 

   Please refer to the interface file, example.h, for details of the functionality of this program


   File organization
   -----------------

   example.h                  interface file:      contains the declarations required to use the functions that implement the solution to this problem
                                                   typically, these will include the definitions of the abstract data types used in the implementation

   exampleImplementation.cpp  implementation file: contains the definitions of the functions that implement the algorithms used in the implementation
 
   exampleApplication.cpp     application file:    contains the code that instantiates the abstract data types and calls the associated functions
                                                   in order to effect the required functionality for this application


   Author
   ------

   David Vernon
   14/12/2016


   Audit Trail
   -----------

   - Added prompt_and_exit() to allow users to read the terminal messages before it is closed.  David Vernon, 29/12/2016.

*/
 
#include "example.h"
 
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
