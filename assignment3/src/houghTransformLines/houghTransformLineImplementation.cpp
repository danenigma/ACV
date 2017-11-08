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
   validLines = get_the_four_main_lines(lines);
   float m,intercept;
   for (size_t i = 0; i < validLines.size(); i++ ) {
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
	  //cout<<"theta: "<<theta<<endl;
      line( hough, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
	  //cout<<"perp: "<<find_perpendicular(i,lines,0,200)<<endl;	  
   }
   //get_orientation_and_center(lines);
   //get_the_four_main_lines(lines);
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
vector<Vec2f> get_the_four_main_lines(vector<Vec2f> lines){
	vector<Vec2f> validLines;
	validLines.push_back(lines[0]);
	float currentLineRho,currentLineTheta,rho,theta,a,b,x0,y0;
	int line1 = find_perpendicular(0,lines,0,150,validLines);
	//validLines.push_back(lines[line1]);
	int line2 = find_perpendicular(line1,lines,0,150,validLines);
	//validLines.push_back(lines[line2]);
	int line3 = find_perpendicular(line2,lines,line1,150,validLines);
	//validLines.push_back(lines.at<Vec2f>(line3));
	/*
	cout<<"main lines "<<0<<" "<<line1<<" "<<line2<<" "<<line3<<endl;
	cout<<"####################################################################"<<endl;
	cout<<"line 1 : r = "<<validLines[0][0]<<" theta = "<<validLines[0][1]*180/CV_PI<<endl;
	cout<<"line 2 : r = "<<validLines[line1][0]<<" theta = "<<validLines[line1][1]*180/CV_PI<<endl;
	cout<<"line 3 : r = "<<validLines[line2][0]<<" theta = "<<validLines[line2][1]*180/CV_PI<<endl;
	cout<<"line 4 : r = "<<validLines[line3][0]<<" theta = "<<validLines[line3][1]*180/CV_PI<<endl;
	*/
return validLines;
}
int find_perpendicular(int indexOfCurrent,vector<Vec2f> lines,int parLineIndex,int distance,vector<Vec2f>& validLines){
	float minSimilarity = 2;
	float similarity;
	int mostPerpendicularIndex = -1;
	float currentTheta,theta;
	currentTheta = lines[indexOfCurrent][1];
	cout<<"current theta : "<<currentTheta*180/CV_PI<<"r: "<<lines[currentTheta][0]<<endl;

	for (size_t i = 0; i < lines.size(); i++ ) {
		if(i!=indexOfCurrent && i!=parLineIndex){
			
			theta = lines[i][1];
			cout<<"theta: "<<theta*180/CV_PI<<endl;
			similarity = abs(cos(abs(theta-currentTheta)));

			cout<<"Similarity : "<<similarity<<endl;
			
			if(similarity<minSimilarity){
					minSimilarity = similarity;
					mostPerpendicularIndex = i;
					cout<<"i am changing min similarity: "<<minSimilarity<<"index: "<<mostPerpendicularIndex<<endl;
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