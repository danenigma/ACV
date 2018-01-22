
#include "determinePose.h"



void get_pose_of_cube(char* fileName,Point& center,int& bestLineOrientation){

	Mat src;
    Mat src_gray;
    Mat src_blur;
    Mat detected_edges;
	Mat hough;


    float rho;
    float theta;   
    Point pt1, pt2;
    double a, b;
    double x0, y0;
    double rho_resolution        = 1;    // pixels 
    double theta_resolution      = 2;    // degrees
    int cannyThreshold = 120; 
	double sigma = 2.0;
    int ratio = 3;
    int kernel_size = 3;
    
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;	
	Rect min_bounding_box;
	Point centerFromContour;
	int largestContourIndex;

	vector<Vec2f> bestLines;


	char* canny_window_name = "Canny Edge Map"; 
    char* hough_window_name = "Hough Lines";

	//read image
	src = imread(fileName, CV_LOAD_IMAGE_COLOR);
	if(src.empty()) {
            cout << "can not open " << fileName << endl;
            return;
    }
          
	//convert to gray scale
    cvtColor(src, src_gray, CV_BGR2GRAY);
	//gaussian blur
    GaussianBlur(src_gray, src_blur, Size(31,31), sigma);
	//extract edge image using canny
    Canny( src_blur, detected_edges, cannyThreshold, cannyThreshold*ratio, kernel_size );
    //extract contours from the edge image
	findContours(detected_edges.clone(), contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );
	//determine the index of the contour with the largest area
	largestContourIndex = get_max_contour_index(contours);
	//min bpuonding box for that contour
	min_bounding_box = boundingRect(contours[largestContourIndex]);
	//center of the min bounding box == center of cube
	centerFromContour.x = min_bounding_box.br().x - min_bounding_box.width/2;
	centerFromContour.y = min_bounding_box.br().y - min_bounding_box.height/2;

	//get the best lines
	bestLines = get_best_lines(detected_edges.clone());
	//compute the orientation of one of the best lines == orientaion of cube
    bestLineOrientation = bestLines[0][1]*180/CV_PI;
	if(bestLineOrientation==0)bestLineOrientation = 90;
	else if(bestLineOrientation>90)bestLineOrientation-=90;
	cout<<"bestLineOrientation: "<<bestLineOrientation<<endl;

    src.copyTo(hough);
	//draw the best lines
    for (size_t i = 0; i < bestLines.size(); i++ ) {

	  rho = bestLines[i][0];
      theta = bestLines[i][1];
      a = cos(theta);
      b = sin(theta);
      x0 = a*rho;
      y0 = b*rho;
      pt1.x = cvRound(x0 + 1000*(-b));
      pt1.y = cvRound(y0 + 1000*(a));
      pt2.x = cvRound(x0 - 1000*(-b));
      pt2.y = cvRound(y0 - 1000*(a));
      line( hough, pt1, pt2, Scalar(0,0,255), 1, CV_AA);  

   }
	//draw circle in the center if the cube
	circle(hough,centerFromContour,10,Scalar(0,0,255),-1);
	Point arrowEndPoint;
	center.x = centerFromContour.x;
	center.y = centerFromContour.y;
	//draw an arrow in the direction of the cubes' orientation
	arrowEndPoint.x = centerFromContour.x+100*cos(CV_PI*bestLineOrientation/180);
	arrowEndPoint.y = centerFromContour.y+100*sin(CV_PI*bestLineOrientation/180);
	arrowedLine(hough,centerFromContour,arrowEndPoint,Scalar(0,255,50),5);
	//display all the images 
    imshow( canny_window_name, detected_edges );
    imshow (hough_window_name, hough);
} 

void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal ... \n");
   getchar();
   exit(status);
}
int get_max_contour_index(vector<vector<Point> > contours){
  /********************this function returns the index of maximum area contour*************************************/
  /*loop through all the contours and compare their area with the current max area if greater update max area*/
  /*to the area of current contour																			 */
    int largest_area=0;
	int largestContourIndex;
	for( int i = 0; i< contours.size(); i++ ) // iterate through each contour. 
      {
       double a=contourArea( contours[i],false);  //  Find the area of contour
	  
       if(a>largest_area){
       largest_area=a;
	   largestContourIndex = i;
       }
      }
return largestContourIndex;
}
vector<Vec2f> get_best_lines(Mat detected_edges){
	/*this function return the best lines*/

	vector<Vec2f> lines; 
	double rho_resolution        = 1;    // pixels 
	double theta_resolution      = 2;    // degrees
	int  houghThreshold = 30;
	do{

		HoughLines(detected_edges, lines, rho_resolution, theta_resolution * CV_PI/180, houghThreshold++, 0, 0 );
		if (lines.size()==0){
			HoughLines(detected_edges, lines, rho_resolution, theta_resolution * CV_PI/180, houghThreshold-2, 0, 0 );
			break;
		}
		//cout<<"lines size: "<<lines.size()<<" hough thesh: "<<houghThreshold<<endl;
	}while(lines.size()!=1 && houghThreshold<100);

	if(houghThreshold==100){
	
		//cout<<"we are in trouble!!!<<"<<endl;
		HoughLines(detected_edges, lines, rho_resolution, theta_resolution * CV_PI/180, 44, 0, 0 );
	}
		
	//cout<<"best line: "<<lines[0]<<" size : "<<lines.size()<<" hough thesh: "<<houghThreshold<<endl;
	return lines;
}