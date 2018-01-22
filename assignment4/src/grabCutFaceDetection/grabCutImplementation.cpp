/* 
  Example use of openCV to perform image segmentation with the grabCut algorithm
  ------------------------------------------------------------------------------
  Implementation file

  For a more sophisticated example, see http://docs.opencv.org/trunk/de/dd0/grabcut_8cpp-example.html

  David Vernon
  18 September 201
*/
 
#include "grabCut.h"

// Global variables to allow access by the display window callback functions
// specifically the mouse callback function to acquire the control point coordinates and draw cross-hairs

Point2i control_points[4];
Rect    rect;
bool    rectState = false; // true => draw the rectangle as the mouse moves and flag the fact that the rectangle is defined

/*
 * function grabCut
 * Trackbar callback - number of iterations user input
*/

void performGrabCut(int, void*) {  

    extern Mat   inputImage;
	extern Mat   bgImage;

    Mat gray;
    vector<Rect> faces;
    int   numberOfIterations = 2; 
    char* grabcut_window_name = "grab cut window";
    Mat result;          // segmentation result 
    Mat bgModel,fgModel; // the models (hard constraints)

    char* file_location = "../data/";

	vector<CascadeClassifier> cascades;
	char* cascade_files[] = { 
		"haarcascades/haarcascade_frontalface_alt.xml" };
	int number_of_cascades = sizeof(cascade_files)/sizeof(cascade_files[0]);
	for (int cascade_file_no=0; (cascade_file_no < number_of_cascades); cascade_file_no++)
	{
		CascadeClassifier cascade;
		string filename(file_location);
		filename.append(cascade_files[cascade_file_no]);
		if( !cascade.load( filename ) )
		{
			cout << "Cannot load cascade file: " << filename << endl;
			return ;
		}
		else cascades.push_back(cascade);
	}

   	cvtColor(inputImage, gray, CV_BGR2GRAY );
	equalizeHist(gray, gray );
	cascades[HAAR_FACE_CASCADE_INDEX].detectMultiScale( gray, faces, 1.1, 2, CV_HAAR_SCALE_IMAGE, Size(30, 30) );
	//for (int count = 0; count < (int)faces.size(); count++ )
	//	rectangle(inputImage, faces[count], cv::Scalar(255,0,0), 2);
	Rect grabCutRect;
	int facePaddingSize;

	if(faces.size()>0){
	
		facePaddingSize = (int)(0.2*max(faces[0].width,faces[0].height));
		grabCutRect.x = faces[0].x-facePaddingSize;
		grabCutRect.y = faces[0].y-facePaddingSize;
		grabCutRect.width  = faces[0].width + 2*facePaddingSize;
		grabCutRect.height = faces[0].height + 2*facePaddingSize;
		cout<<"face padding size : "<<facePaddingSize<<endl;

		grabCut(inputImage.clone(),         // input image
					result,             // segmentation result (4 values); can also be used as an input mask providing constraints
					grabCutRect,               // rectangle containing foreground 
					bgModel,fgModel,    // for internal use ... allows continuation of iterative solution on subsequent calls
					numberOfIterations, // number of iterations
					GC_INIT_WITH_RECT); // use rectangle
    
			/* Get the pixels marked as likely foreground */
		compare(result,GC_PR_FGD,result,CMP_EQ);

			/* Generate output image */

		Mat foreground(inputImage.size(),CV_8UC3,cv::Scalar(0,0,0));
		inputImage.copyTo(foreground,result); // use result to mask out the background pixels 
		Mat faceImage(Scalar(faces[0].width,faces[0].height));
		Rect faceRect;
		faceRect.x = faces[0].x + faces[0].width/3;
		faceRect.y = faces[0].y + faces[0].height/3;
		faceRect.width  = faces[0].width/3;
		faceRect.height = faces[0].height/3;
		Mat noseAreaImage;
		noseAreaImage  = inputImage(faceRect);
		faceImage = inputImage(faces[0]);
		Mat faceImageHsv,noseAreaImageHsv;
		Mat noseAreaImageHsvSplit[3];
		
		cvtColor(faceImage,faceImageHsv,CV_BGR2HSV);
		cvtColor(noseAreaImage,noseAreaImageHsv,CV_BGR2HSV);
		
		split(noseAreaImageHsv,noseAreaImageHsvSplit);
		double hmax,hmin,smax,smin,vmax,vmin;
		minMaxLoc(noseAreaImageHsvSplit[0],&hmin,&hmax);
		minMaxLoc(noseAreaImageHsvSplit[1],&smin,&smax);
		minMaxLoc(noseAreaImageHsvSplit[2],&vmin,&vmax);
		Mat faceColorMask;
		inRange(faceImageHsv, Scalar(hmin, smin, vmin), Scalar(hmax, smax,vmax),faceColorMask); //Threshold the image
		//flood_fill(faceColorMask);
		Mat faceImageProcessed; 
		bitwise_and(faceImage,faceImage,faceImageProcessed,faceColorMask);

		imshow("face image  ",faceImageProcessed);

		//blending fg and bg
		 
		 Mat finalImage;

		 


		 resize(bgImage,bgImage,Size(480,600));
		 Mat faceFgImage(bgImage.size(),CV_8UC3,Scalar(0,0,0));
		 resize(faceImageProcessed,faceImageProcessed,Size(100,100));
		 faceImageProcessed.copyTo(faceFgImage(Rect(50,50,100,100)));
		 double beta,alpha = 0.75;
		 beta = 1-alpha;
		 addWeighted(faceFgImage, alpha, bgImage, beta, 0.0, finalImage);
		 imshow("final image",finalImage);

		 Mat foregroundGray,foregroundMask;

		 cvtColor(foreground,foregroundGray,CV_BGR2GRAY);

		 threshold(foregroundGray.clone(),foregroundMask,50,255,CV_THRESH_BINARY);
		 imshow("grabCut threshold image",foregroundMask);
		 imshow("grabcut gray",foregroundGray);
		 imshow(grabcut_window_name, foreground);
	}
}

                                                                                  

void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal ... \n");
   getchar();
   exit(status);
} 
void flood_fill(Mat filteredImage){
	/*
	
	This function is used for filling holes in the sweets 
	(idea taken from online discussion forums(stackoverflow and opencv answers))
	==========================================================
	input:
		filteredImage : (Mat) preprocessed binary image
	output:
		it modifies the input image
	*/

   //store a copy of original image
   Mat floodFillInvertedImage,floodFillImage;
   floodFillImage = filteredImage.clone();
   // floodfill the filtered image starting from the top left corner(white)
   // the only black pixels in the output image would be the holes inside the sweets
   floodFill(floodFillImage, cv::Point(0,0), Scalar(255));
   // Invert floodFilled image so that the holes would be white
   bitwise_not(floodFillImage,floodFillInvertedImage);  
   // if it is in either the in the inverted flood filled image(hole) or the original image = foreground(255 white)
   filteredImage = (filteredImage | floodFillInvertedImage);
}