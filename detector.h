#ifndef DETECTOR_H
#define DETECTOR_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<QString>
using namespace cv;
using namespace std;
extern QString causeColor;
extern QString causeColor_1_white;
extern QString causeColor_2_white;
extern QString causeColor_3_white;
extern QString causeColor_1_black;
extern QString causeColor_2_black;
extern QString causeColor_gray;
extern Mat Mresult;
extern Mat Mresult_1_white;
extern Mat Mresult_2_white;
extern Mat Mresult_3_white;
extern Mat Mresult_1_black;
extern Mat Mresult_2_black;
extern Mat Mresult_gray;
extern Mat M_white_1;
extern Mat M_biankuang;
extern Mat M_black_1;
extern Mat M_louguang_1;
extern Mat M_abshow_white;

extern Mat M_white_2;
extern Mat M_white_3;
extern Mat M_black_2;
extern Mat M_louguang_2;

extern Mat M_L_1;
extern Mat M_L_1_E;
extern Mat M_L_2;
extern Mat M_R_1;
extern Mat M_R_1_E;
extern Mat M_R_2;

extern Mat main_mask;
extern Mat left_mask;
extern Mat right_mask;
extern Mat front_mask;
extern Mat back_mask;
extern Mat color_mask;
extern Mat louguang_mask;

extern Mat MURA1;
extern Mat MURA2;
extern bool isCeshi;
extern bool isFrontBack;
extern bool isArea_1 ;
extern bool isArea_2 ;
extern bool isColor ;

Mat Gabor9(Mat img_1);
Mat Gabor7(Mat img_1);
//bool light_point7(Mat image_black,Mat *mresult,QString *causecolor);
bool light_point7(Mat image_black, Mat image_white, Mat *mresult, QString *causecolor);
bool new_sample_light_line(Mat src_black,Mat *mresult,QString *causecolor);
bool roi_white(Mat src_white,int border_white,int border_black,int border_lougunag,Mat *Mwhite,Mat *Mblack,Mat *Mlouguang,Mat *Mabshow,int ID);
bool roi_white_color(Mat src_white,int border_white,int border_black,int border_lougunag,Mat *Mwhite,Mat *Mblack,Mat *Mlouguang,int ID);//白色底色下提取黑色ROI
bool roi_ceshi(Mat src_white, int border_white, Mat *Mwhite);
bool roi_front_back(Mat src_white, int border_white, Mat *Mwhite);//白屏图片前后相机找透视变换的映射关系
Mat toushi_white(Mat image, Mat M, int border, int length, int width);
bool white_defect(Mat white,Mat whiteleft,Mat whiteright,Mat whitecolor,Mat ceguang,Mat back,Mat ceguangback,Mat front,Mat ceguangfront,Mat ceguangleft,Mat ceguangright,int num,Mat white_abshow1,Mat CeL1_Enlarge, Mat CeR1_Enlarge,Mat white_biankuang,Mat ceguangleft_enlarge,Mat ceguangright_enlarge);
bool black_defect(Mat img_black,Mat img_black1,Mat img_black2,Mat white,int border,int num);
bool gray_detect(Mat gray,Mat gray2,Mat gray3,Mat cg_main,Mat cg_left,Mat cg_right,Mat grayColor1,Mat grayColor2);
bool white_point(Mat white_yiwu ,Mat ceguang,Mat *mresult ,QString *causecolor);
bool WhiteDot_BackSide(Mat white_yiwu ,Mat ceguang, Mat *mresult ,QString *causecolor);
bool WhiteDotLeft(Mat white_yiwu, Mat ceguang, Mat Original, Mat *mresult, QString *causecolor);
bool GrayDotLeft(Mat white_yiwu, Mat ceguang, Mat *mresult, QString *causecolor);
bool Frame(Mat src_img, Mat *mresult, String *causecolor);
bool LessMaterial(Mat src_img, Mat *mresult, String *causecolor);
bool Shifting(Mat white, Mat *mresult, QString *causecolor);
bool Mura_Decter(Mat imageGray, Mat *mresult, QString *causecolor);
Mat fanzhuan(Mat src, int width, int height, int direction);
bool seban(Mat image_white_src,Mat *mresult,QString *causecolor);
bool redBlueSpot(Mat image_black1,Mat *mresult,QString *causecolor);
bool louye7(Mat grayImg,Mat *mresult ,QString *causecolor);
bool lack_line(Mat white_yiwu, Mat ceguang, Mat *mresult, QString *causecolor);//少线检测
bool Leakage(Mat image_black, Mat *mresult, QString *causecolor);//黑屏下大量漏光检测
bool Scratch(Mat white, Mat ceguang, Mat *mresult, QString *causecolor);
Mat StickerLocate(Mat ceguang);
bool TransverseLine(Mat white, Mat *mresult, QString *causecolor);
bool ColorLine(Mat black, Mat *mresult, QString *causecolor);
bool LampEye(Mat white, int topBottomEdgeWidth,int rightEdgeWidth,double regionWidth, int step,int gradSpan , double graTh,  int fliterWidth, double intervalBetweenExtremePointsTh,double gradBetweenExtremePointsTh1, double gradBetweenExtremePointsTh2, double intervalSimilarityTh, int numIntervalTh, int numSlopeTh,Mat *mresult, QString *causecolor);
bool RoiWhite_Arcangle(Mat src_white, int border_white, int border_black, int border_lougunag, Mat *Mwhite, Mat *Mblack, Mat *Mlouguang, int ID);
bool RoiSide_Arcangle(Mat src_white, int border_white, Mat *Mwhite,Mat *Mwhite_enlarge);
Point2f getCrossPoint(Vec4i LineA, Vec4i LineB);
bool RoiColor_Arcangle(Mat src_white, int border_white, int border_black, int border_lougunag, Mat *Mwhite, Mat *Mblack, Mat *Mlouguang, int ID);
bool RoiWhite_Arcangle_Abshow(Mat src_white, int border_white, int border_black, int border_lougunag, Mat *Mwhite, Mat *Mblack, Mat *Mlouguang, int ID);//白色底色下提取黑色ROI//主黑白相机白底显示异常检测
bool Abshow_White_Detect(Mat white1, Mat *mresult, QString *causecolor);
bool Stripe(Mat white, Mat *mresult, QString *causecolor);
bool SunSpots(Mat image_white_src, Mat *mresult, QString *causecolor);
bool ForeignBody(Mat white_yiwu, Mat ceguang, Mat Original, Mat *mresult, QString *causecolor);
bool ScreenFlash(Mat grayColor1,Mat grayColor2,Mat *mresult, QString *causecolor);
Mat strong(Mat input, double k,Size window1,Size window2);
int slidingWnd(Mat& src, vector<Mat>& wnd, Size& wndSize, double x_percent, double y_percent);
bool boom_light(Mat white, Mat ceguang,Mat *mresult, QString *causecolor);
bool Dead_light(Mat white, Mat ceguang, Mat *mresult, QString *causecolor);
bool CrossLine(Mat white, Mat *mresult, QString *causecolor);
bool Brightedge(Mat src_white,Mat Ceguang, Mat* Mwhite,QString* causecolor,int Flag_L_R);
void adaptiveThresholdCustom(const cv::Mat &src, cv::Mat &dst, double maxValue, int method, int type, int blockSize, double delta, double ratio);
void adaptiveThresholdCustom_whitedot(const cv::Mat &src, cv::Mat &dst, double maxValue, int method, int type, int blockSize, double delta, double ratio);
bool f_MainCam_PersTransMatCal(InputArray _src, int border_white, int border_biankuang, Mat *Mwhite, Mat *Mbiankuang, Mat *M_white_abshow, int ID, QString ScreenType_Flag);
Point2f getPointSlopeCrossPoint(Vec4f LineA, Vec4f LineB);
bool f_LeftRightCam_PersTransMatCal(InputArray _src, Mat *Mwhite,Mat *M_R_1_E, QString ScreenType_Flag,int border_white);
bool f_LeftRightCam_PersTransMatCal1(InputArray _src, Mat *Mwhite, QString ScreenType_Flag);
bool Light_leakage(Mat white, Mat ceguang, Mat* mresult, QString* causecolor);
Mat fitL(Mat left);
Mat fitR(Mat right);
Mat Ployfit_Col7_1(Mat img_col, int poly_n, bool isSaveOrNot, double Scoral);
bool white_defect1(Mat white,Mat mainfilter,Mat leftfilter,Mat rightfilter,Mat ceguang,Mat ceguangleft,Mat ceguangright,int num,Mat biankuangfilter);
bool white_defect2(Mat white,Mat mainfilter,Mat ceguang,Mat CeL1_Enlarge_Fliter,Mat ceguangleft_enlarge,Mat CeR1_Enlarge_Fliter,Mat ceguangright_enlarge, int num);
bool white_defect3(Mat white_abshow1,Mat mainfilter,Mat leftfilter,Mat rightfilter,Mat ceguangleft,Mat ceguangright,int num);
void delay(int MSecs);
void delay_msec(int msec);
bool display_defect(Mat src_white);
#endif // DETECTOR_H
