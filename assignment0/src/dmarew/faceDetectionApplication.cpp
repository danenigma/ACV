 
#include "faceDetection.h"
 
int main(int argc, const char** argv) {

   int debug = TRUE;
   CvCapture* capture;
   VideoCapture cap(0); 
   Mat frame;
   String face_cascade_name = "../data/haarcascade_frontalface_alt.xml";
   String eyes_cascade_name = "../data/haarcascade_eye_tree_eyeglasses.xml";
   CascadeClassifier face_cascade;
   CascadeClassifier eyes_cascade;
   if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
   if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

   printf("cascade loading successful!!");
   if(!cap.isOpened())  // check if we succeeded
        return -1;
   
     while( true )
     {
		 cap>>frame;

       if( !frame.empty() )
	   { 
		 detectAndDisplay( frame,face_cascade,eyes_cascade );	    
	   }
       else
       { printf(" --(!) No captured frame -- Break!"); break; }

       int c = waitKey(10);
       if( (char)c == 'c' ) { break; }
      }
   
   return 0;
 
}
