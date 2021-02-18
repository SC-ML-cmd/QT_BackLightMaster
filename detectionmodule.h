
#ifndef DETECTIONMODULE_H
#define DETECTIONMODULE_H
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<QString>
using namespace cv;
using namespace std;
extern string causeColor;
string& replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value);
Mat RemoveLittleHoles(Mat Img);

void unevenLightCompensate(Mat &image, int blockSize=32);
Mat interceptScreenWithoutOutside(Mat minimumCircumscribedRectangle);
Mat mkKernel(int ks, double sig, double th, double lm, double ps);
Mat Process(cv::Mat src_f, int sigma, int pos_lmda, int pos_thta, int psi);
Mat New_Gabor(Mat img_1);
Mat Gaborfilter(cv::Mat image, int sigma, int lmda,  int psi);
Mat Vec2Mat(vector<vector<uchar>> array, int row, int col);
vector<vector<int>> Read_Mat(vector<vector<int>> array, Mat d_Img, int nRows, int nCols);
Mat Ployfit_Row(Mat img_row, int poly_n, bool isSaveOrNot, double Percentage);
Mat Ployfit_Col(Mat img_col, int poly_n, bool isSaveOrNot, int Scoral);
Mat RemoveLittleHoles(Mat Img);
void unevenLightCompensate(Mat &image, int blockSize);
void sum_rgb(IplImage* src, IplImage* dst);
CvRect interceptScreen_new(string name );
Mat interceptScreen(string name , int mode = 0);
Mat interceptScreen(Mat img, int mode = 0);
Mat blackOutOfScreen(string name);
bool liantongyuLable(Mat img,int grayTh,int areaTh);
bool liantongyuLable(Mat img,  int areaTh);
void MyGammaCorrection(Mat& src, Mat& dst, float fGamma);
void gammaCorrection(Mat& img, Mat& dst,float fGamma);
bool blackDetection(string name);
bool blackDetection_new(Mat minimumCircumscribedRectangleImg,String name,CvRect rect,string save_name,int signalcheck[8]);
bool whiteCenterScreenDetection(string name);
bool whiteEdgeScreenDetection(string name);
Mat dustRemoval(Mat beforeDustRemoval, Mat afterDustRemoval);
Mat dataPredict(Mat dataPrevious, int mode);
Mat matShift(Mat beforeShift);
Mat detectionBaiYin(Mat centerScreen);
Mat testImageRectification(cv::Mat &image_original);
bool slantDetection(string name);
bool whiteDetection_new(Mat minimumCircumscribedRectangle,string save_name,int signalcheck[8]);
bool whitecheck_TaiYang(Mat minimumCircumscribedRectangle,string save_name,int signalcheck[8]);
bool yiwu(string save_name,Mat white_yiwu ,Mat ceguang);
Mat Remove_WaterMaker(Mat image_white);//去水印
Rect roi_white(string name);//新的ROI提取算法
Mat warpPerspective( Mat image);//透视变换
bool detect_new(string name,int signalcheck[8]);
void showresult(string name);
void showMessagebox();
bool jiance(int signalcheck[8]);
Mat gamma_new(Mat src);
Mat Remove_WaterMaker1(Mat image_white);
bool whiteDetectionBasedScreen(string name);
bool whitecheck_loseline(Mat minimumCircumscribedRectangle,string save_name,int signalcheck[8]);
extern string result_lujing;
// bool LouGuangFlag;
//bool BaiDianFlag;
 //bool BanFlag;
// bool QueXianFlag;
// bool BeiGuangFlag;
//bool BaiYinFlag;
Mat gamma_new(Mat src);
Mat roi_white_yiwu(Mat src_white);
Mat toushi(Mat image,Mat M);
Rect watermark_locationRed(Mat src);
Mat pre_process_white(Mat src_white,Mat M);
Mat ROI(Mat src_white);
bool light_point(Mat image_black);
//extern bool open_close_flage=true;
#endif // DETECTIONMODULE_H
