
#include "determinePose.h"



void get_pose_of_cube(char* fileName,Point& center,int& orientation){
    //[TODO] CHECK FOR THE SIZE OF THE NUMBER OF LINES
	Mat src;
    Mat src_gray;
    Mat src_blur;
    Mat detected_edges;
	Mat hough;

	vector<Vec2f> lines; 

    float rho;
    float theta;   
    Point pt1, pt2;
    double a, b;
    double x0, y0;
    double rho_resolution        = 1;    // pixels 
    double theta_resolution      = 2;    // degrees
    int cannyThreshold = 120; 
    char* canny_window_name = "Canny Edge Map";
    int houghThreshold = 44;//44 perfect 
    char* hough_window_name = "Hough Lines";

	src = imread(fileName, CV_LOAD_IMAGE_COLOR);
	if(src.empty()) {
            cout << "can not open " << fileName << endl;
            return;
    }
          
    double sigma = 2.0;
    int ratio = 3;
    int kernel_size = 3;
    cvtColor(src, src_gray, CV_BGR2GRAY);

    GaussianBlur(src_gray, src_blur, Size(31,31), sigma);

    Canny( src_blur, detected_edges, cannyThreshold, cannyThreshold*ratio, kernel_size );
    
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	
	findContours(detected_edges.clone(), contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );
	Rect min_bounding_box;
	Point centerFromContour;
	int contourIndex = get_max_contour_index(contours);
	min_bounding_box = boundingRect(contours[contourIndex]);
	centerFromContour.x = min_bounding_box.br().x - min_bounding_box.width/2;
	centerFromContour.y = min_bounding_box.br().y - min_bounding_box.height/2;



    imshow( canny_window_name, detected_edges );


    src.copyTo(hough);


    vector<Vec2f> validLines,bestLine;

	bestLine= get_best_lines(detected_edges.clone());

    HoughLines(detected_edges, lines, rho_resolution, theta_resolution * CV_PI/180, houghThreshold, 0, 0 );
    
	int simpleOrientation = lines[0][1]*180/CV_PI;
	if(simpleOrientation == 0)simpleOrientation = 90;
	else if(simpleOrientation>90)simpleOrientation-=90;
	cout<<"simple orientation: "<<simpleOrientation<<endl;

	int bestLineOrientation = bestLine[0][1]*180/CV_PI;
	if(bestLineOrientation==0)bestLineOrientation = 90;
	else if(bestLineOrientation>90)bestLineOrientation-=90;
	cout<<"bestLineOrientation: "<<bestLineOrientation<<endl;

    int mainLineIndices[4];
    get_the_four_main_lines(lines,mainLineIndices);
    vector <Vec2i> bestLinesTemp;
    for(int i=0;i<4;i++)
	bestLinesTemp.push_back(Vec2i(lines[mainLineIndices[i]][0],lines[mainLineIndices[i]][1]*180/CV_PI));
    
	bool exists;
	float m,intercept;
	vector <Vec2i> bestLines;

    for (size_t i = 0; i < bestLine.size(); i++ ) {

	  exists = find(begin(mainLineIndices), end(mainLineIndices), i) != end(mainLineIndices);
	  rho = lines[i][0];
      theta = lines[i][1];
      a = cos(theta);
      b = sin(theta);
      x0 = a*rho;
      y0 = b*rho;
      pt1.x = cvRound(x0 + 500*(-b));
      pt1.y = cvRound(y0 + 500*(a));
      pt2.x = cvRound(x0 - 500*(-b));
      pt2.y = cvRound(y0 - 500*(a));


	  if (exists){
	  bestLines.push_back(Vec2i(rho,theta*180/CV_PI));
      line( hough, pt1, pt2, Scalar(0,0,255), 1, CV_AA);  
	  }else{
	  line( hough, pt1, pt2, Scalar(0,255,0), 1, CV_AA);
	  }
   }

	Point intersectionPoints[4];
	get_center_and_orientation(bestLinesTemp,orientation,center,intersectionPoints);
	//comment out this part to go back previous implementation
	orientation = simpleOrientation;
	//for(int i=0;i<4;i++)circle(hough,intersectionPoints[i],5,Scalar(255,0,0),-1);
	circle(hough,centerFromContour,10,Scalar(0,0,255),-1);
	Point arrowEndPoint;
	center.x = centerFromContour.x;
	center.y = centerFromContour.y;
	arrowEndPoint.x = centerFromContour.x+100*cos(CV_PI*orientation/180);
	arrowEndPoint.y = centerFromContour.y+100*sin(CV_PI*orientation/180);
	arrowedLine(hough,centerFromContour,arrowEndPoint,Scalar(0,255,50),5);
    imshow (hough_window_name, hough);
} 

void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal ... \n");
   getchar();
   exit(status);
}
void get_the_four_main_lines(vector<Vec2f> lines,int mainLineIndices[]){
	float currentLineRho,currentLineTheta,rho,theta,a,b,x0,y0;

	mainLineIndices[0] = 0;
	mainLineIndices[1] = find_perpendicular(0,lines,0,50);
	mainLineIndices[2] = find_perpendicular(mainLineIndices[1],lines,0,50);
	mainLineIndices[3] = find_perpendicular(mainLineIndices[2],lines,mainLineIndices[1],50);
}
int find_perpendicular(int indexOfCurrent,vector<Vec2f> lines,int parLineIndex,int distance){
	float minSimilarity = 2;
	float similarity;
	int mostPerpendicularIndex = -1;
	float currentTheta,theta;
	currentTheta = lines[indexOfCurrent][1];

	for (size_t i = 1; i < lines.size(); i++ ) {
		if(i!=indexOfCurrent && i!=parLineIndex){
			theta = lines[i][1];
			similarity = abs(cos(abs(theta-currentTheta)));
			if(similarity<minSimilarity){
				if(abs(lines[parLineIndex][0]-lines[i][0])>distance){
					minSimilarity = similarity;
					mostPerpendicularIndex = i;
				}
			}
		}
	}
return mostPerpendicularIndex;
}
void get_center_and_orientation(vector<Vec2i> bestLines,int& pose,Point& center,Point intersectionPoints[]){
	float validLinePoseSum;
	int validPoseCount=0;
	//Point intersectionPoints[4];
	intersectionPoints[0] = get_intersection_point(bestLines[0],bestLines[1]);
	intersectionPoints[1] = get_intersection_point(bestLines[1],bestLines[2]);
	intersectionPoints[2] = get_intersection_point(bestLines[2],bestLines[3]);
	intersectionPoints[3] = get_intersection_point(bestLines[3],bestLines[0]);
	//for(int i=0;i<4;i++)cout<<"Intersection point: "<<i<<" "<<intersectionPoints[i]<<endl;
	Point cent;
	cent = (intersectionPoints[0]+intersectionPoints[1]+intersectionPoints[2]+intersectionPoints[3]);
	center.x  = cent.x/4;
	center.y  = cent.y/4;
	
	for(int i = 0;i<bestLines.size();i++){
		if(bestLines[i][1]>0 && bestLines[i][1]<=CV_PI/2){
			validLinePoseSum += bestLines[i][1];
			validPoseCount++;
		}
	}
	if(is_valid(bestLines[0][1]) && is_valid(bestLines[1][1])){
		pose = max(bestLines[0][1],bestLines[1][1]);
	}else if(is_valid(bestLines[0][1]) && ~is_valid(bestLines[1][1])){
		pose = bestLines[0][1];
	}else if(~is_valid(bestLines[0][1]) && is_valid(bestLines[1][1])){
		pose = bestLines[1][1];
	}else if(is_valid(bestLines[2][1]) && is_valid(bestLines[3][1])){
		pose = max(bestLines[0][1],bestLines[1][1]);
	}else if(is_valid(bestLines[2][1]) && ~is_valid(bestLines[3][1])){
		pose = bestLines[0][1];
	}else if(~is_valid(bestLines[2][1]) && is_valid(bestLines[3][1])){
		pose = bestLines[1][1];
	}else{

		cout<<"can't decide"<<endl;
	}

}
Point get_intersection_point(Vec2i line1,Vec2i line2){

	Point intersectionPoint(0,0);
	float mLine1,mLine2,bLine1,bLine2,x0,y0;
	if(line1[1]!=0 && line2[1]!=0 ){
		mLine1 = -cos(CV_PI*line1[1]/180)/sin(CV_PI*line1[1]/180);
		mLine2 = -cos(CV_PI*line2[1]/180)/sin(CV_PI*line2[1]/180);
		bLine1 = line1[0]/sin(CV_PI*line1[1]/180);
		bLine2 = line2[0]/sin(CV_PI*line2[1]/180);
		x0 = (bLine2-bLine1) / (mLine1-mLine2);
		y0 = mLine1 * x0 + bLine1;
		intersectionPoint.x = x0;
		intersectionPoint.y = y0;
	}else if (line1[1]==0 && line2[1]!=0){
	
		mLine2 = -cos(CV_PI*line2[1]/180)/sin(CV_PI*line2[1]/180);
		bLine2 = line2[0]/sin(CV_PI*line2[1]/180);
		x0 = line1[0]/cos(CV_PI*line1[1]/180);
		y0 = mLine2 * x0 + bLine2;
		intersectionPoint.x = x0;
		intersectionPoint.y = y0;
	}else if (line1[1]!=0 && line2[1]==0){
	
		mLine1 = -cos(CV_PI*line1[1]/180)/sin(CV_PI*line1[1]/180);
		bLine1 = line1[0]/sin(CV_PI*line1[1]/180);
		x0 = line2[0]/cos(CV_PI*line2[1]/180);;
		y0 = mLine1 * x0 + bLine1;
		intersectionPoint.x = x0;
		intersectionPoint.y = y0;
	}else{

	cout<<"somthing is really wrong !!"<<endl;
	}

return intersectionPoint;
}
bool is_valid(float a){
	if(a > 0 && a <= 90)return true;
	return false;
}
int get_max_contour_index(vector<vector<Point> > contours){
  /********************this function returns the maximum area of contours*************************************/
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
		cout<<"lines size: "<<lines.size()<<" hough thesh: "<<houghThreshold<<endl;
	}while(lines.size()!=1 && houghThreshold<100);

	if(houghThreshold==100){
	
		cout<<"we are in trouble!!!<<"<<endl;
		HoughLines(detected_edges, lines, rho_resolution, theta_resolution * CV_PI/180, 44, 0, 0 );
	}
		
	cout<<"best line: "<<lines[0]<<" size : "<<lines.size()<<" hough thesh: "<<houghThreshold<<endl;
	return lines;
}