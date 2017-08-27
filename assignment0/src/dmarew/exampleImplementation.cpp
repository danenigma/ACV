/* 

   exampleImplementation.cpp - implementation file for a program named example
   ============================================================================

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

Mat image; // dummy image variable to check that openCV is working

void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal\n");
   getchar();
   exit(status);
}

void print_message_to_file(FILE *fp, char message[]) {
 
   fprintf(fp,"The message is: %s\n", message);
}