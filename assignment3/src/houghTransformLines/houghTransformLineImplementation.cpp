/* 
/* 
  Example use of openCV to use the Hough Transform
  ------------------------------------------------
  Implementation file

 
  David Vernon
  24 January 2017

*/

#include "houghTransformLines.h"


/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{  

   extern Mat src;
   extern Mat src_gray;
   extern Mat src_blur;
   extern Mat detected_edges;
   extern int cannyThreshold; 
   extern char* canny_window_name;

   double sigma = 2.0;
   int ratio = 3;
   int kernel_size = 3;

   cvtColor(src, src_gray, CV_BGR2GRAY);

   GaussianBlur(src_gray, src_blur, Size(31,31), sigma);

   Canny( src_blur, detected_edges, cannyThreshold, cannyThreshold*ratio, kernel_size );

   imshow( canny_window_name, detected_edges );

 }



/**
 * @function HoughThreshold
 * @brief Trackbar callback - Hough Transform threshold
 */

void HoughThreshold(int, void*) {

   extern Mat src;
   extern Mat src_gray;
   extern Mat src_blur;
   extern Mat detected_edges;
   extern Mat hough;
   extern int houghThreshold; 
   extern char* hough_window_name;

   vector<Vec2f> lines; 
   vector<Vec2f> linesRec; 

   float rho;
   float theta;   
   Point pt1, pt2;
   double a, b;
   double x0, y0;
   double rho_resolution        = 1;    // pixels 
   double theta_resolution      = 2;    // degrees

   src.copyTo(hough);
   cout<<"#################################*****************################"<<endl;
   HoughLines(detected_edges, lines, rho_resolution, theta_resolution * CV_PI/180, houghThreshold, 0, 0 );
   vector<Vec2f> validLines;
   int mainLineIndices[4];
   get_the_four_main_lines(lines,mainLineIndices);
   vector <Vec2i> bestLinesTemp;
   for(int i=0;i<4;i++){
	   cout<<"main line: "<<lines[mainLineIndices[i]]<<" index: "<<mainLineIndices[i]<<endl;
	   bestLinesTemp.push_back(Vec2i(lines[mainLineIndices[i]][0],lines[mainLineIndices[i]][1]*180/CV_PI));
   }
   for(int i=0;i<4;i++)cout<<"attempt: "<<bestLinesTemp[i]<<endl;
   bool exists;
   float m,intercept;
   vector <Vec2i> bestLines;
   for (size_t i = 0; i < lines.size(); i++ ) {
	  exists = find(begin(mainLineIndices), end(mainLineIndices), i) != end(mainLineIndices);
	  
      cout<<"exists: "<<exists<<" index : "<<i<<endl;
	  rho = lines[i][0];
      theta = lines[i][1];
	  cout<<"rho: "<<rho<<" theta : "<<theta*180/CV_PI<<" index : "<<i<<endl;
      a = cos(theta);
      b = sin(theta);
      x0 = a*rho;
      y0 = b*rho;
      pt1.x = cvRound(x0 + 500*(-b));
      pt1.y = cvRound(y0 + 500*(a));
      pt2.x = cvRound(x0 - 500*(-b));
      pt2.y = cvRound(y0 - 500*(a));
	  /*
	  if (pt1.x!=pt2.x){
		  m = float((pt2.y-pt1.y))/(pt2.x-pt1.x);
		  intercept = pt2.y-m*pt2.x;
		  linesRec.push_back(Vec2d(m,intercept));
		 // cout<<"slope: "<<m<<" intercept: "<<b<<endl;
	  }else{
		  intercept = pt1.x;
		  linesRec.push_back(Vec2d(1000,intercept));
		//cout<<"undefined slope line: "<<" x-intercept : "<<intercept<<endl;
	  }
	  */
	  //cout<<"theta: "<<theta<<endl;
	  
	  if (exists){
	  bestLines.push_back(Vec2i(rho,theta*180/CV_PI));
      line( hough, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
	  //cout<<"perp: "<<find_perpendicular(i,lines,0,200)<<endl;	  
	  }else{
	  //line( hough, pt1, pt2, Scalar(0,255,0), 1, CV_AA);
	  }
   }
   //get_orientation_and_center(lines);
   //get_the_four_main_lines(lines);

   for(int i=0;i<bestLinesTemp.size();i++)cout<<"Best lines : "<<bestLinesTemp[i]<<endl;
   float pose;
   Point center;
   Point intersectionPoints[4];
   get_center_and_pos(bestLinesTemp,pose,center,intersectionPoints);
   for(int i=0;i<4;i++)circle(hough,intersectionPoints[i],5,(255,0,0));
   circle(hough,center,5,(255,0,255));
   Point arrowEndPoint;
   cout<<"pose from here: "<<pose<<endl;
   arrowEndPoint.x = center.x+100*cos(CV_PI*pose/180);
   arrowEndPoint.y = center.y+100*sin(CV_PI*pose/180);
   arrowedLine(hough,center,arrowEndPoint,(255,255,0));

   
   imshow (hough_window_name, hough);
}
void get_orientation_and_center(vector<Vec2f> lines){
    float rho;
    float theta;  
	float thetaSum=0;
	float meanTheta;

	vector<float> lineOrientation;
	Mat thetaImg;
	MatND thetaHist;

	
	int validCount = 0;

	
	for (size_t i = 0; i < lines.size(); i++ ) {
      rho = lines[i][0];
      theta = lines[i][1];
	  if (((theta*180)/CV_PI)>0 && ((theta*180)/CV_PI)<=91){
		thetaSum+=theta;
		cout<<"valid: "<<((theta*180)/CV_PI)<<endl;
		validCount++;
	  }
	  
	}
	
	meanTheta = thetaSum/validCount;
	get_center(lines);
	cout<<"mean theta : "<<(meanTheta*180)/CV_PI<<endl;
	
	

}

void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal ... \n");
   getchar();
   exit(status);
}
void get_center(vector<Vec2f> lines){
   float rho;
   float theta;   
   float a,b,x0,y0;
    
   vector<Point> slopsAndIntercepts;
   float x1,y1,x2,y2,m,intercept;
   for (size_t i = 0; i < lines.size(); i++ ) {
      rho = lines[i][0];
      theta = lines[i][1];
	  if (theta!=0){
		a  = cos(theta);
		b  = sin(theta);
		x0 = a*rho;
		y0 = b*rho;
		x1 = x0 + 10*(-b);
		x2 = x0 + 10*(b);
		y1 = y0 + 10*(a);
		y2 = y0 + 10*(-a);
		m  = (y2-y1)/(x2-x1);
		intercept  = y2-m*x2;
		cout<<"m: "<<m<<" b: "<<b<<endl;
		slopsAndIntercepts.push_back(Point(m,intercept));


	  }
   }


}
void create_histogram_of_lines(vector<float> lineOrientation){
const float bucket_size = 0.05;
int number_of_buckets = (int)ceil(1 / bucket_size); 
std::vector<int> histogram(number_of_buckets);

for(int i=0;i<lineOrientation.size();i++){
	
	int bucket = (int)floor(lineOrientation[i]/bucket_size);
    histogram[bucket] += 1;
	
	}
for(int i=0;i<histogram.size();i++)cout<<"out: "<<histogram[i]<<endl;
}
void get_the_four_main_lines(vector<Vec2f> lines,int mainLineIndices[]){
	vector<Vec2f> validLines;
	//int mainLineIndices[4];
	mainLineIndices[0] = 0;
	validLines.push_back(lines[0]);
	float currentLineRho,currentLineTheta,rho,theta,a,b,x0,y0;
	mainLineIndices[1] = find_perpendicular(0,lines,0,150,validLines);
	//validLines.push_back(lines[line1]);
	mainLineIndices[2] = find_perpendicular(mainLineIndices[1],lines,0,150,validLines);
	//validLines.push_back(lines[line2]);
	mainLineIndices[3] = find_perpendicular(mainLineIndices[2],lines,mainLineIndices[1],150,validLines);
	//validLines.push_back(lines.at<Vec2f>(line3));
}
int find_perpendicular(int indexOfCurrent,vector<Vec2f> lines,int parLineIndex,int distance,vector<Vec2f>& validLines){
	float minSimilarity = 2;
	float similarity;
	int mostPerpendicularIndex = -1;
	float currentTheta,theta;
	currentTheta = lines[indexOfCurrent][1];
	
	//cout<<"i peaked : theta "<<currentTheta*180/CV_PI<<" r: "<<lines[currentTheta][0]<<endl;

	for (size_t i = 1; i < lines.size(); i++ ) {
		if(i!=indexOfCurrent && i!=parLineIndex){
			
			theta = lines[i][1];
			//cout<<"theta: "<<theta*180/CV_PI<<endl;
			similarity = abs(cos(abs(theta-currentTheta)));

			//cout<<"Similarity : "<<similarity<<endl;
			
		    //cout<<"distance : " <<abs(lines[parLineIndex][0]-lines[i][0])<<endl;
			if(similarity<minSimilarity){
			//cout<<"angle parallility: " <<180*abs(lines[parLineIndex][1]-lines[i][1])/CV_PI<<endl;
				if(abs(lines[parLineIndex][0]-lines[i][0])>50){
				
					minSimilarity = similarity;
					mostPerpendicularIndex = i;
					//cout<<"i am changing min similarity: "<<minSimilarity<<"index: "<<mostPerpendicularIndex<<endl;
				}
			}
		}
	}
	validLines.push_back(Vec2f(lines[mostPerpendicularIndex][1],lines[mostPerpendicularIndex][0]));
	cout<<"i peaked : "<<minSimilarity<<" degree : "<<180*lines[mostPerpendicularIndex][1]/CV_PI<<" "<<lines[mostPerpendicularIndex][0]<<endl;
return mostPerpendicularIndex;
}
float computeDistanceBetweenLines(Vec2f line1,Vec2f line2){


return 0.0;
}
void drawLines(Mat img,vector<Vec2f> lines,vector<int>validLines){
   float a,b,x0,y0,rho,theta;
   Point pt1,pt2;

   for (size_t i = 0; i < lines.size(); i++ ) {
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
      line(img, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
	}

}
void get_center_and_pos(vector<Vec2i> bestLines,float& pose,Point& center,Point intersectionPoints[]){
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
	cout<<"center estimate x: "<<cent.x/4<<" y: "<<cent.y/4<<endl;
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
	cout<<"Pose : "<<pose<<endl;
}
Point get_intersection_point(Vec2i line1,Vec2i line2){
Point intersectionPoint(0,0);
float mLine1,mLine2,bLine1,bLine2,x0,y0;
if(line1[1]!=0 && line2[1]!=0 ){
	mLine1 = -cos(CV_PI*line1[1]/180)/sin(CV_PI*line1[1]/180);
	mLine2 = -cos(CV_PI*line2[1]/180)/sin(CV_PI*line2[1]/180);
	bLine1 = line1[0]/sin(CV_PI*line1[1]/180);
	bLine2 = line2[0]/sin(CV_PI*line2[1]/180);
	cout<<"mline1 "<<mLine1<<" mline2 "<<mLine2<<" line1 angle "<<line1[1]<<" line2 angle "<<line2[1]<<endl;
	cout<<"bline1 "<<bLine1<<" bline2 "<<bLine2<<endl;
	x0 = (bLine2-bLine1) / (mLine1-mLine2);
	y0 = mLine1 * x0 + bLine1;
	intersectionPoint.x = x0;
	intersectionPoint.y = y0;
	cout<<"intersection point: "<<intersectionPoint<<endl;
}else if (line1[1]==0 && line2[1]!=0){
	
	mLine2 = -cos(CV_PI*line2[1]/180)/sin(CV_PI*line2[1]/180);
	bLine2 = line2[0]/sin(CV_PI*line2[1]/180);
	x0 = line1[0]/cos(CV_PI*line1[1]/180);
	y0 = mLine2 * x0 + bLine2;
	intersectionPoint.x = x0;
	intersectionPoint.y = y0;
	cout<<"first vertical"<<endl;
	cout<<"intersection point: "<<intersectionPoint<<endl;
}else if (line1[1]!=0 && line2[1]==0){
	
	mLine1 = -cos(CV_PI*line1[1]/180)/sin(CV_PI*line1[1]/180);
	bLine1 = line1[0]/sin(CV_PI*line1[1]/180);
	x0 = line2[0]/cos(CV_PI*line2[1]/180);;
	y0 = mLine1 * x0 + bLine1;
	intersectionPoint.x = x0;
	intersectionPoint.y = y0;
	cout<<"intersection point: "<<intersectionPoint<<endl;
}else{

cout<<"somthing is really wrong !!"<<endl;
}

return intersectionPoint;
}
bool is_valid(float a){
	if(a > 0 && a <= 90)return true;
	return false;
}