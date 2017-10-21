#include "histogram.h"
void count_defective_sweets(Mat inputImage,int &total_defective_count,int &totalNumberOfColors,vector<colorTypes> &defectivePerColor);
void remove_white_bg(Mat img,int backgroundThreshold);
void flood_fill(Mat img);
vector<colorTypes>  get_individual_sweets(Mat filteredImage,Mat inputImage,int& defect_count);
bool is_defective(vector<Vec4i> defect,int defectDepthThreshold);
vector<colorTypes> computeHueHistogramMaxima(Mat inputImage);
int find_minimum_distance_match(vector<colorTypes> inputImageColorTypes,vector<colorTypes> currentContourColorType);
vector<colorTypes>  combine_red_smarties(vector <colorTypes> finalColorTypes);
vector<colorTypes> get_local_maxima(Mat img,int size);