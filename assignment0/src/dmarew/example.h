/* 

   example.h - interface file for a program named example
   ======================================================

   This program is for course 04-801 Cognitive Robotics, Assignment No. 0 

   The assignment is defined as follows.
   
   Read a single-line message from an input file, write it to the terminal, and also write a version of it to an output file.
   The message to be written to file should be prepended with the following text:

   "The message is: "

   Use a function to prepend the text and write the message to the output file.

   Input data with a test case message is provided in an input file named input.txt.  
   This input file is located in the data directory. 
   Since this directory is a sibling directory of the bin directory where the example .exe file resides, 
   the filename used when opening the file is "../data/input.txt".

   Output for the test case is written to an output file "../data/output.txt"


   Input
   -----

   - A single line of text no longer than 80 characters.


   Output
   ------

   - A single line of text, beginning "The message is: "
 

   Sample Input
   ------------
   
   Hello World!


   Sample Output
   -------------

   The message is: Hello World!


   Solution Strategy
   -----------------

   This is just an example program so there is no real computational problem to solve.  In other cases, you would describe here
   the principle computational theories used in solving the problem, along with the main data structures and algorithms use.


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

 
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>

//opencv
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

using namespace cv;

#define TRUE 1
#define FALSE 0
#define MAX_MESSAGE_LENGTH 81

/* function prototypes go here */

void prompt_and_exit(int status);
void print_message_to_file(FILE *fp, char message[]);
