#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define CL_RED Scalar(0,0,255)
#define CL_GREEN Scalar(0,255,0)
#define CL_YELLOW Scalar(0,255,255    )

int smoothSize = 5;
int numBins = 8;
int borderType = BORDER_REPLICATE;
std::string winName = "Histograms";
Mat grayImg;
Mat splitHsv[3];
//HELPER FUNCTIONS
struct histSerie {
    Mat hist;
    std::string Title;
    Scalar lineColor;
    int lineWidth;
    bool drawLocalMax;
    histSerie(std::string _Title = "", Mat _hist = Mat(),
        Scalar _lineColor = Scalar(255, 0, 0), bool _drawLocalMax = false, int _lineWidth = 1) :
        Title(_Title), lineColor(_lineColor), lineWidth(_lineWidth), 
        drawLocalMax(_drawLocalMax)  {_hist.copyTo(hist); }
};
void PlotHistSeries(const vector<histSerie> &histSeries, Mat &plotResult,
    Scalar bgColor = Scalar(0, 0, 0));
void onTrackBar(int, void*);
// MAIN
int main(int argc, char* argv[])
{
    Mat src,hsvImage;
    src = imread("../data/assignment2B.JPG");
    if (!src.data) return -1;
    cvtColor(src, hsvImage, CV_BGR2HSV);
	
	split(hsvImage,splitHsv);


	imshow("Src image", splitHsv[0]);
    namedWindow(winName);
    createTrackbar("n.bins 2^", winName, &numBins, 8, onTrackBar);
    createTrackbar("Smooth Size", winName, &smoothSize, 35, onTrackBar);
    createTrackbar("Border", winName, &borderType, BORDER_REPLICATE, onTrackBar);
    onTrackBar(0, 0);
    waitKey(0);
    return 0;
}

void onTrackBar(int, void*)
{
    Mat hist, histSmoothed, plotResult;
    vector<histSerie> histSeries;
    //enable this if you want to draw over srcImage
    //cvtColor(grayImg, plotResult, CV_GRAY2BGR);

    //Create the histogram
    float range[] = { 0, 256 };
    const float* histRange = { range };
    int histSize = numBins*numBins;
    calcHist(&(splitHsv[0]), 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);
    histSeries.push_back(histSerie("Histogram bins=" + histSize, 
            hist, CL_RED, false, 2));

    //smooth the histogram
    if ((smoothSize % 2) == 0) smoothSize += 1;  //use odd value
    Size sz(smoothSize, smoothSize);
    GaussianBlur(hist, histSmoothed, sz, 0, 0, borderType);
    histSeries.push_back(histSerie("Histogram Smoothed w=" + smoothSize,
        histSmoothed, CL_YELLOW, true));
    //Plot and show the result
    PlotHistSeries(histSeries, plotResult);
    imshow(winName, plotResult);
}

void PlotHistSeries(const vector<histSerie> &histSeries, Mat &plotResult, Scalar bgColor)
{
    double maxOfMax = 0;
    int histSize = 0;
    for (int s = 0; s < histSeries.size(); s++) {
        histSize = max(histSize, histSeries[s].hist.size().height);
        double minVal, maxVal;
        minMaxLoc(histSeries[s].hist, &minVal, &maxVal);
        maxOfMax = max(maxOfMax, maxVal);
    }
    //create the plot image
    if (plotResult.empty())
        plotResult = Mat(256, 512, CV_8UC3, bgColor);
    //calculate y scale
    double yScale = (double)plotResult.size().height / maxOfMax;
    double xScale = (double)plotResult.size().width / histSize;
    // Draw all histograms
    for (int s = 0; s < histSeries.size(); s++) {
        Mat data = histSeries[s].hist;
        Point pt0, pt1;
        int x0, x1, y0, y1;
        float v0, v1, v2;
        // for each bins
        for (int i = 0; i < histSize; i++) {
            v1 = data.at<float>(i);
            if (i == 0) v0 = v1;
            else v0 = data.at<float>(i-1);
            x0 = cvRound((i - 1) * xScale);
            x1 = cvRound(i * xScale);
            y0 = cvRound(plotResult.size().height - v0*yScale);
            y1 = cvRound(plotResult.size().height - v1*yScale);
            pt0 = Point(x0, y0);
            pt1 = Point(x1, y1);
            //draw localMax 1st
            if (histSeries[s].drawLocalMax)
            {
                // check max on border
                if (i < histSize - 1) v2 = data.at<float>(i + 1); 
                else v2 = v1 - 1;
                if (i == 0) v0 = v1 - 1;
                //if localmax
                if ((v1 > v0) && (v1 > v2))
                    circle(plotResult, pt1, 5, CL_GREEN, 2);
            }
            //hist segment
            line(plotResult, pt0, pt1, histSeries[s].lineColor, histSeries[s].lineWidth, 8, 0);
        }
        //legend
        pt0.x = 10; pt1.x = pt0.x + 20;
        pt0.y = pt1.y = 20 * (s + 1);
        line(plotResult, pt0, pt1, histSeries[s].lineColor, histSeries[s].lineWidth, 8, 0);
        pt1.x += 5;
        cv::putText(plotResult, histSeries[s].Title, pt1,
            CV_FONT_HERSHEY_PLAIN, 1, histSeries[s].lineColor);
    }
}