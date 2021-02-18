#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<QDebug>
#include <iostream>
#include<vector>
#include<string>
#include <fstream>
#include<math.h>
#include<stdio.h>
#include <complex>
#include "Ployfit.h"
#include"parameters.h"
#include <opencv2/opencv.hpp>
//#include"detectionmodule.h"
//#include"detectionoffline.h"

using namespace cv;
using namespace std;
using namespace czy;

#define			    POLY_N				 3       //多项式拟合阶数
#define				SCOLAR_COL		     2		 //多项式列拟合阈值，阈值越大，白色区域越多
#define             POS_SIGMA		     3		 //Gabor滤波器的核大小
#define				POS_LMD				 400	 //Gabor滤波器的波长
#define				POS_TH				 0		 //Gabor滤波器的角度
#define				POS_PSI				 85		 //Gabor滤波器的PSI


double Screen_area=0 ;
//sum_rgb用
int INTERCEPTSCREENTHRESHOLD = 15;

//截屏用（去除的边界像素大小）
 int LEFTRIGHTEDGECOUNT = 30;//左右去除像素30
 int UPDOWNEDGECOUNT = 30;//上下去除像素35

//黑色底色时用
int static BLACKAVERGRAYVALUE = 10;//取出黑底屏幕后取的屏幕灰度平均值
int static Edge_leakage_threshold=200;//黑色底色下判断边缘漏光时二值化的阈值
double static Edge_leakage_Average_gray=2.0;//黑色底色下判断边缘漏光时平均灰度值
int static White_Point_threshold=80;//黑色底色下判断白点时二值化的阈值
int static  White_Point_Lower_limit_area=5;//黑色底色下判断白点时面积下限
int static  White_Point_Upper_limit_area=40000;//黑色底色下判断白点时面积上限
//白印检测参数
int static Baiyin_Brightness_correction=100;//白印亮度校正值
int static Baiyin_Blur_kernel_size=3;//白印均值滤波核大小
int static Baiyin_LEFTRIGHTEDGECOUNT=100;//白印边缘去除值
int static Baiyin_ploy_n=3;
double static Baiyin_ploy_percen=0.017;
int static  Baiyin_Lower_limit_area=100;//判断白印时面积下限
int static  Baiyin_Upper_limit_area=4000;//判断白印时面积上限
int static Baiyin_erode_kernel_size=12;
//黑点检测参数
int static Black_Point_adthreshold_kernel_size=25;//黑点自适应阈值核大小，越大越灵敏 21_25
int static Black_Point_adthreshold_constant=4;//黑点自适应阈值常数大小，越小越灵敏5_4
int static  Black_Point_Lower_limit_area=150;//判断白印时面积下限150
int static  Black_Point_Upper_limit_area=500;//判断白印时面积上限500


double GAMMAVALUE = 0.45;//0.4Gamma校正的参数值
 int BLACKOUTOFSCREENTHRESHOLD = 110;//漏光时选取的阈值
int BLACKOUTOFSCREENAREA = 500;//漏光时连通域面积阈值

int GAMMABLACKAVERGRAYVALUE=35;
 int BLACKSCREENTHRESHOLD =15;//15黑色屏幕区域的灰度阈值
 int BLACKSCREENAREA = 30;//黑色屏幕区域的面积阈值

//白色用Gabor参数
 int WHITEKERNELSIZE =21;
 int WHITEPOSSIGMA = 3;//3
 int WHITEPOSLM =400;//400 波长，越长衍射波纹越不明显
 int WHITEPOSTH = 0;
int WHITEPOSPSI = 85;//85 相位片异，70-90之间




 int HEGEAREA = 2500000;//白色时截到屏幕的面积下限4630000,gdi应该是4300000

 int UPPERLIMITERROR = 9;//白色屏幕区域Gabor滤波后全局平均灰度值上限，超过上限部分为缺陷
 int LOWERLIMITERROR = 12;//15白色屏幕区域Gabor滤波后全局平均灰度值下限，超过下限部分为缺陷
 int UPPERLOWERLIMITAREA = 30;//白色屏幕区域Gabor滤波后连通域面积阈值

 int WHITESCREENEDGECOUNT = 35;//白色边缘屏幕区域像素数目

 int DUSTTHRESHOLD = 10;//灰尘阈值
 int DUSTAREA = 10;//灰尘大小

 int DIFFZHENGXU = 3;//白色正序预测允许差值
 int DIFFDAOXU = 2;//白色倒序预测允许差值
 int PREDICTDISTANCE = 10;//预测的间隔
 int SHIFTDISTANCE = 20;//预测时使用的位移
int DIFFPREDICTZHENGXUTHRESHOLD = 0;//正序预测误差图二值化阈值
 int DIFFPREDICTDAOXUTHRESHOLD = 0;//倒序预测误差图二值化阈值
 int BAIYINEDGEDISTANCE = 30;//白印检测时忽略的边缘像素大小
 int BAIYINAREA = 40;//白印面积大小阈值
 int EDGEDETECTIONTHRESHOLD=70;
 int EDGEDIFFTHRESHOLD=15;
 int EDGEDIFFAREA=30;


 //红色参数
 int REDUPPERLIMITERROR = 9;//白色屏幕区域Gabor滤波后全局平均灰度值上限，超过上限部分为缺陷
 int REDLOWERLIMITERROR = 12;//15白色屏幕区域Gabor滤波后全局平均灰度值下限，超过下限部分为缺陷
 int REDUPPERLOWERLIMITAREA = 30;//白色屏幕区域Gabor滤波后连通域面积阈值

 //蓝色参数
 int BLUEUPPERLIMITERROR = 9;//白色屏幕区域Gabor滤波后全局平均灰度值上限，超过上限部分为缺陷
 int BLUELOWERLIMITERROR = 12;//15白色屏幕区域Gabor滤波后全局平均灰度值下限，超过下限部分为缺陷
 int BLUEUPPERLOWERLIMITAREA = 30;//白色屏幕区域Gabor滤波后连通域面积阈值

 //绿色参数
 int GREENUPPERLIMITERROR = 9;//白色屏幕区域Gabor滤波后全局平均灰度值上限，超过上限部分为缺陷
 int GREENLOWERLIMITERROR = 12;//15白色屏幕区域Gabor滤波后全局平均灰度值下限，超过下限部分为缺陷
 int GREENUPPERLOWERLIMITAREA = 30;//白色屏幕区域Gabor滤波后连通域面积阈值

string ImgName;
string causeColor;
bool bianyuanlouguangflag;
//string static save_name;

/*=========================================================
* 函 数 名: 白印检测
* 功能描述: 核函数的计算
=========================================================*/
//Mat image_1 = interceptScreen("E:\\day0211\\sample\\ImageNum_1_fn8798_white.bmp" , 0);
//cvtColor(image_1,image_1,CV_BGR2GRAY);
//Mat img_2=New_Gabor(image_1);
//unevenLightCompensate(img_2, 62);
//medianBlur(img_2, img_2, 21);
//Mat img_1(img_2, Rect(50, 50, img_2.cols - 100, img_2.rows - 100));
//Mat img_3=Ployfit_Row(img_1, 2, false, 2170);
//            namedWindow("img_masked", 0);
//            imshow("img_masked",img_3);
//            waitKey(0);
//            destroyAllWindows();

/*=========================================================
* 函 数 名: mkKernel
* 功能描述: 核函数的计算
=========================================================*/

Mat mkKernel(int ks, double sig, double th, double lm, double ps)
{
    int hks = (ks - 1) / 2;
    double theta = th*CV_PI / 180;
    double psi = ps*CV_PI / 180;
    double del = 2.0 / (ks - 1);
    double lmbd = lm;
    double sigma = sig / ks;
    double x_theta;
    double y_theta;
    Mat kernel(ks, ks, CV_32F);
    for (int y = -hks; y <= hks; y++)
    {
        for (int x = -hks; x <= hks; x++)
        {
            x_theta = x*del*cos(theta) + y*del*sin(theta);
            y_theta = -x*del*sin(theta) + y*del*cos(theta);
            kernel.at<float>(hks + y, hks + x) = (float)exp(-0.5*(pow(x_theta, 2) + pow(y_theta, 2)) / pow(sigma, 2))* cos(2 * CV_PI*x_theta / lmbd + psi);
        }
    }

    return kernel;
}

int  kernel_size = 21;

/*=========================================================
* 函 数 名: Process
* 功能描述: Gabor滤波器计算程序
=========================================================*/

Mat Process(cv::Mat src_f, int sigma, int pos_lmda, int pos_thta, int psi)

{
    Mat dest;

    double sig = sigma;
    double lm = 0.5 + pos_lmda / 100.0;
    double th = pos_thta;
    double ps = psi;

    Mat kernel = mkKernel(kernel_size, sig, th, lm, ps);
    filter2D(src_f, dest, CV_32F, kernel);

    return dest;
}

//新屏白色底色用
Mat Process0(cv::Mat src_f, int sigma, int pos_lmda, int pos_thta, int psi)

{
    Mat dest;

    double sig = sigma;
    double lm = 0.5 + pos_lmda / 100.0;
    double th = pos_thta;
    double ps = psi;

    Mat kernel = mkKernel(kernel_size, sig, th, lm, ps);
    filter2D(src_f, dest, CV_32F, kernel);
    cv::Mat Lkernel(kernel_size * 20, kernel_size * 20, CV_32F);
    cv::resize(kernel, Lkernel, Lkernel.size());
    Lkernel /= 2.;
    Lkernel += 0.5;
    cv::Mat mag;
    cv::pow(dest, 2.0, mag);
    return mag;
}

/*=========================================================
* 函 数 名: Gaborfilter
* 功能描述: 对灰度图进行Gabor滤波处理
* 输入变量: 1.灰度图像 2.sigma 3.波长 4.角度 5.psi
=========================================================*/

Mat Gaborfilter(cv::Mat image, int sigma, int lmda, int psi)
{
    Mat src_f;
    image.convertTo(src_f, CV_32F, 1.0 / 255, 0);

    if (!kernel_size % 2)
        kernel_size += 1;

    Mat result1 = Process(src_f, sigma, lmda, 0, psi);
    Mat result2 = Process(result1, sigma, lmda, 90, psi);

    Mat result3;
    result2.convertTo(result3, CV_8U, 255, 0);

    return result3;

}


Mat GaborfilterWhiteCenter(cv::Mat image, int sigma, int lmda, int thta, int psi)
{
    Mat src_f;
    image.convertTo(src_f, CV_32F, 1.0 / 255, 0);

    if (!kernel_size % 2)
        kernel_size += 1;

    Mat result1 = Process0(src_f, sigma, lmda, thta, psi);
    Mat result3;
    result1.convertTo(result3, CV_8U, 255, 0);

    return result3;

}
/*=========================Gaborfilter滤波================================*/
Mat New_Gabor(Mat img_1)
{
    Mat kernel1 = getGaborKernel(Size(5, 5), 1.5, CV_PI/2, 0.5, 5.0, 0, CV_32F );//求卷积核
   float sum=0.0;
    for (int i = 0; i < kernel1.rows; i++)
    {
        for (int j = 0; j < kernel1.cols; j++)

        {
           sum=sum+kernel1.ptr<float>(i)[j];
        }

    }

     Mat mmm= kernel1/sum;
     Mat kernel2 = getGaborKernel(Size(5, 5), 1.5, 0, 0.5, 5.0, 0, CV_32F );
    float sum2=0.0;
     for (int i = 0; i < kernel2.rows; i++)
     {
         for (int j = 0; j < kernel2.cols; j++)

         {
            sum2=sum2+kernel2.ptr<float>(i)[j];
         }

     }

      Mat mmm2= kernel2/sum2;


     Mat img_4,img_5;
     filter2D(img_1,img_4,CV_8UC3,mmm);//卷积运算
     filter2D(img_4,img_5,CV_8UC3,mmm2);
     imwrite("D:\\program\\ceguang_1.bmp", img_5);
     return img_5;

}

/*=========================================================
* 函 数 名: Vec2Mat
* 功能描述: 将Vector二维数组转换为Mat数据格式
* 输入变量: 1.二维数组Vector 2.图片像素行数 3.图片像素列数
=========================================================*/

Mat Vec2Mat(vector<vector<uchar>> array, int row, int col)
{
    Mat img(row, col, CV_8UC1);
    uchar *ptmp = NULL;

    for (int i = 0; i <row; i++)
    {
        vector<uchar> z_i = array[i];
        ptmp = img.ptr<uchar>(i);
        for (int j = 0; j < col; j++)
            ptmp[j] = z_i[j];
    }

    return img;
}

/*========================================================================
* 函 数 名: Read_Mat
* 功能描述: 将单通道图像像素读到Vector中
* 输入变量: 1.二维数组Vector 2.输出的图片 3.图片像素行数 4.图片的列数
==========================================================================*/

vector<vector<int>> Read_Mat(vector<vector<int>> array, Mat d_Img, int nRows, int nCols)
{
    for (int i = 0; i < nRows; i++)
    {
        uchar* p = d_Img.ptr<uchar>(i);
        for (int j = 0; j < nCols; j++)
            array[i][j] = p[j];
    }
    return array;
}


/*========================================================================================================
* 函 数 名: Ployfit_Row
* 功能描述: 对”行“数据进行多项式拟合，并返回拟合结果
* 输入变量: 1.输入图片 2.拟合的阶数（若poly_n=2，则y=a0+a1*x+a2*x^2 ）3.是否保存数据（FLASE）
4.图片行数 5.图片列数 6.行拟合阈值
==========================================================================================================*/

Mat Ployfit_Row(Mat img_row, int poly_n, bool isSaveOrNot, double Percentage)
{
    Fit fit1;
    int nRows=img_row.rows;
    int nCols=img_row.cols;
    int sigma = 0;

    vector<int> array_cols(nCols);
    vector<vector<int>> array(nRows);
    vector<vector<int>> array_row_diff(nRows);
    vector<vector<uchar>> array_clone1(nRows);      //二维数组的创建

    for (int i = 0; i<nRows; i++) {
        array_row_diff[i].resize(nCols);
        array_clone1[i].resize(nCols);
        array[i].resize(nCols);
    }

    for (int a = 1; a < nCols + 1; a++)
        array_cols[a - 1] = a;

    array = Read_Mat(array, img_row, nRows, nCols);
    for (int i = 0; i < nRows; i++)
    {
        fit1.polyfit(array_cols, array[i], poly_n, isSaveOrNot);
        for (int j = 0; j < nCols; j++)
          {
             if(poly_n==3)
             {array_row_diff[i][j] = abs(array[i][j] - (fit1.getFactor(0) + fit1.getFactor(1) * (j + 1) + fit1.getFactor(2)  * pow(j + 1, 2)+ fit1.getFactor(3)  * pow(j + 1, 3) ));//+ fit1.getFactor(3)  * pow(j + 1, 3);
             }
             else {
                 array_row_diff[i][j] = abs(array[i][j] - (fit1.getFactor(0) + fit1.getFactor(1) * (j + 1) + fit1.getFactor(2)  * pow(j + 1, 2) ));
             }

          array_row_diff[i][j]=pow(array_row_diff[i][j],3);

            if (array_row_diff[i][j] > sigma) {
                sigma = array_row_diff[i][j];
            }

            if (i == 87)
            {

            }
        }

    }

    /*对行拟合进行阈值的选择*/
    for (int i = 0; i < nRows; i++)
    for (int j = 0; j < nCols; j++)
        //if (array_row_diff[i][j] < sigma - 36) Scoral

    if (array_row_diff[i][j] < Percentage*sigma)//sigma - Scoral
        array_clone1[i][j] = (uchar)0;
    else
        array_clone1[i][j] = (uchar)255;
    Mat img2 = Vec2Mat(array_clone1, nRows, nCols);
    return img2;
}

/*========================================================================================================
* 函 数 名: Ployfit_Col
* 功能描述: 对”列“数据进行多项式拟合，并返回拟合结果
* 输入变量: 1.输入图片 2.拟合的阶数（若poly_n=2，则y=a0+a1*x+a2*x^2 ）3.是否保存数据（FLASE）
4.图片行数 5.图片列数 6.行拟合阈值
==========================================================================================================*/

Mat Ployfit_Col(Mat img_col, int poly_n, bool isSaveOrNot, int Scoral)
{
    Fit fit1;
    int nRows=img_col.rows;
    int nCols=img_col.cols;
    int sigma = 0;

    vector<vector<int>> array(nRows);
    vector<vector<int>> array_col_diff(nRows);
    vector<vector<uchar>> array_clone2(nRows);
    vector<int> array_clone_cols(nRows);
    vector<int> array_rows(nRows);

    for (int i = 0; i<nRows; i++) {
        array_col_diff[i].resize(nCols);
        array_clone2[i].resize(nCols);
        array[i].resize(nCols);
    }

    array = Read_Mat(array, img_col, nRows, nCols);  //将图像数据存入vector

    for (int b = 1; b < nRows + 1; b++)
        array_rows[b - 1] = b;

    for (int i = 0; i < nCols; i++)
    {
        for (int m = 0; m < nRows; m++)
            array_clone_cols[m] = array[m][i];

        fit1.polyfit(array_rows, array_clone_cols, poly_n, isSaveOrNot);

        for (int j = 0; j < nRows; j++)
        {
            array_col_diff[j][i] = array[j][i] - (fit1.getFactor(0) + fit1.getFactor(1) * (j + 1) + fit1.getFactor(2)  * pow(j + 1, 2) + fit1.getFactor(3)  * pow(j + 1, 3));;

            if (array_col_diff[j][i] > sigma) {
                sigma = array_col_diff[j][i];
            }

        }
    }

    /* 对列列拟合进行阈值选择 */
    for (int i = 0; i < nRows; i++)
    for (int j = 0; j < nCols; j++)
        //if (array_col_diff[i][j] < sigma1 - 2)
    if (array_col_diff[i][j] < sigma - Scoral)
        array_clone2[i][j] = 0;
    else
        array_clone2[i][j] = 255;

    Mat img = Vec2Mat(array_clone2, nRows, nCols);

    return img;
}

Mat Ployfit_Col2(Mat img_col, int poly_n, bool isSaveOrNot, int nRows, int nCols, int Scoral)
{
    Fit fit1;
    int sigma = 0;
    Mat_<int> img = img_col.clone();

    vector<int> array_clone_cols(nRows);
    vector<int> array_rows(nRows);



    //array = Read_Mat(array, img_col, nRows, nCols);  //将图像数据存入vector

    for (int b = 1; b < nRows + 1; b++)
        array_rows[b - 1] = b;

    for (int i = 0; i < nCols; i++)
    {
        for (int m = 0; m < nRows; m++)
            array_clone_cols[m] = img[m][i];

        fit1.polyfit(array_rows, array_clone_cols, poly_n, isSaveOrNot);

        for (int j = 0; j < nRows; j++)
        {
            img[j][i] = img[j][i] - (fit1.getFactor(0) + fit1.getFactor(1) * (j + 1) + fit1.getFactor(2)  * pow(j + 1, 2) + fit1.getFactor(3)  * pow(j + 1, 3));;

            if (img[j][i] > sigma) {
                sigma = img[j][i];
            }

        }
    }

    /* 对列列拟合进行阈值选择 */
    for (int i = 0; i < nRows; i++)
    for (int j = 0; j < nCols; j++)
        //if (array_col_diff[i][j] < sigma1 - 2)
    if (img[i][j] < sigma - Scoral)
        img[i][j] = 0;
    else
        img[i][j] = 255;

    //Mat img = Vec2Mat(array_clone2, nRows, nCols);

    return img;
}

/*========================================================================================================
* 函 数 名: RemoveLittleHoles
* 功能描述: 去除图像的小连通域，填充空洞
* 输入变量: 1.输入图片 2.拟合的阶数（若poly_n=2，则y=a0+a1*x+a2*x^2 ）3.是否保存数据（FLASE）
4.图片行数 5.图片列数 6.行拟合阈值
* 黑色时用的10，白印用
==========================================================================================================*/

Mat RemoveLittleHoles(Mat Img)
{
    Mat imgHSVMask(Img.size(), CV_8UC1);

    // 提取连通区域，并剔除小面积联通区域
    vector<vector<Point>> contours(100000);
    findContours(Img, contours,  CV_RETR_CCOMP, CHAIN_APPROX_NONE);//检测所有轮廓，两个等级关系，保存物体边界上的所有连续轮廓点到contours向量内
    contours.erase(remove_if(contours.begin(), contours.end(),
        [](const vector<Point>& c) {return contourArea(c) < 10; }), contours.end());

    // 显示图像并保存
    imgHSVMask.setTo(0);
    drawContours(imgHSVMask, contours, -1, Scalar(255), FILLED);//scalar将图像设置成纯色，-1代表画全部轮廓，filled代表填充

    return imgHSVMask;
}

Mat RemoveLittleHolesBaiYin(Mat Img)
{
    Mat imgHSVMask(Img.size(), CV_8UC1);

    // 提取连通区域，并剔除小面积联通区域
    vector<vector<Point>> contours(100000);
    findContours(Img, contours,  CV_RETR_CCOMP, CHAIN_APPROX_NONE);
    contours.erase(remove_if(contours.begin(), contours.end(),
        [](const vector<Point>& c) {return contourArea(c) < 25; }), contours.end());

    // 显示图像并保存
    imgHSVMask.setTo(0);
    drawContours(imgHSVMask, contours, -1, Scalar(255), FILLED);

    return imgHSVMask;
}

/*=========================================================
* 函 数 名: RemoveSmallRegion
* 功能描述: 去除小连通区域与孔洞
* CheckMode: 0代表去除黑区域，1代表去除白区域;
* NeihborMode：0代表4邻域，1代表8邻域;
=========================================================*/
void RemoveSmallRegion(Mat &Src, Mat &Dst, int AreaLimit, int CheckMode, int NeihborMode)
{
    int RemoveCount = 0;
    //新建一幅标签图像初始化为0像素点，为了记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查

    //初始化的图像全部为0，未检查
    Mat PointLabel = Mat::zeros(Src.size(), CV_8UC1);

    if (CheckMode == 1)//去除小连通区域的白色点
    {
        cout << "去除小连通域.";
        for (int i = 0; i < Src.rows; i++)
        {
            for (int j = 0; j < Src.cols; j++)
            {
                if (Src.at<uchar>(i, j) < 10)
                {
                    PointLabel.at<uchar>(i, j) = 3;//将背景黑色点标记为合格，像素为3
                }
            }
        }
    }
    else//去除孔洞，黑色点像素
    {
        cout << "去除孔洞";
        for (int i = 0; i < Src.rows; i++)
        {
            for (int j = 0; j < Src.cols; j++)
            {
                if (Src.at<uchar>(i, j) > 10)
                {
                    PointLabel.at<uchar>(i, j) = 3;//如果原图是白色区域，标记为合格，像素为3
                }
            }
        }
    }
    vector<Point2i>NeihborPos;//将邻域压进容器
    NeihborPos.push_back(Point2i(-1, 0));
    NeihborPos.push_back(Point2i(1, 0));
    NeihborPos.push_back(Point2i(0, -1));
    NeihborPos.push_back(Point2i(0, 1));
    if (NeihborMode == 1)
    {
        cout << "Neighbor mode: 8邻域." << endl;
        NeihborPos.push_back(Point2i(-1, -1));
        NeihborPos.push_back(Point2i(-1, 1));
        NeihborPos.push_back(Point2i(1, -1));
        NeihborPos.push_back(Point2i(1, 1));
    }
    else cout << "Neighbor mode: 4邻域." << endl;
    int NeihborCount = 4 + 4 * NeihborMode;
    int CurrX = 0, CurrY = 0;
    //开始检测
    for (int i = 0; i < Src.rows; i++)
    {
        for (int j = 0; j < Src.cols; j++)
        {
            if (PointLabel.at<uchar>(i, j) == 0)//标签图像像素点为0，表示还未检查的不合格点
            {   //开始检查
                vector<Point2i>GrowBuffer;//记录检查像素点的个数
                GrowBuffer.push_back(Point2i(j, i));
                PointLabel.at<uchar>(i, j) = 1;//标记为正在检查
                int CheckResult = 0;

                for (int z = 0; z < GrowBuffer.size(); z++)
                {
                    for (int q = 0; q < NeihborCount; q++)
                    {
                        CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
                        CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
                        if (CurrX >= 0 && CurrX<Src.cols&&CurrY >= 0 && CurrY<Src.rows)  //防止越界
                        {
                            if (PointLabel.at<uchar>(CurrY, CurrX) == 0)
                            {
                                GrowBuffer.push_back(Point2i(CurrX, CurrY));  //邻域点加入buffer
                                PointLabel.at<uchar>(CurrY, CurrX) = 1;           //更新邻域点的检查标签，避免重复检查
                            }
                        }
                    }
                }
                if (GrowBuffer.size() > AreaLimit)
                {
                    //判断结果（是否超出限定的大小），1为未超出，2为超出
                    CheckResult = 2;
                    cout << GrowBuffer.size() << endl;
                }
                else
                {
                    CheckResult = 1;
                    RemoveCount++;//记录有多少区域被去除
                    cout << GrowBuffer.size() << endl;
                }
                for (int z = 0; z < GrowBuffer.size(); z++)
                {
                    CurrX = GrowBuffer.at(z).x;
                    CurrY = GrowBuffer.at(z).y;
                    PointLabel.at<uchar>(CurrY, CurrX) += CheckResult;//标记不合格的像素点，像素值为2
                }
                //********结束该点处的检查**********
            }
        }
    }
    CheckMode = 255 * (1 - CheckMode);
    //开始反转面积过小的区域
    for (int i = 0; i < Src.rows; ++i)
    {
        for (int j = 0; j < Src.cols; ++j)
        {
            if (PointLabel.at<uchar>(i, j) == 2)
            {
                Dst.at<uchar>(i, j) = CheckMode;
            }
            else if (PointLabel.at<uchar>(i, j) == 3)
            {
                Dst.at<uchar>(i, j) = Src.at<uchar>(i, j);
            }
        }
    }
    cout << RemoveCount << " objects removed." << endl;
}


/*=========================================================
* 函 数 名: unevenLightCompensate
* 功能描述: 亮度补偿，减小图像中间过亮两边过暗的现象，默认以32个像素为单位进行处理
* Brightness compensation function.But there are some problems with this function.
* The black matrix on the outside or defects will affect the surrounding pixels.
* For example, the black matrix on the outside will cause serious white matrix phenomenon on the edges of the image.
* So can't keep the outside black matrix when intercepting the screen.
* Source:https://blog.csdn.net/hust_bochu_xuchao/article/details/54019994
* Paper title:《一种基于亮度均衡的图像阈值分割技术》 School:合肥工业大学
=========================================================*/

void unevenLightCompensate(Mat &image, int blockSize=32)
{
    if (image.channels() == 3) cvtColor(image, image, 7);
    double average = mean(image)[0];
    int rows_new = ceil(double(image.rows) / double(blockSize));
    int cols_new = ceil(double(image.cols) / double(blockSize));
    Mat blockImage;
    blockImage = Mat::zeros(rows_new, cols_new, CV_32FC1);
    for (int i = 0; i < rows_new; i++)
    {
        for (int j = 0; j < cols_new - 0; j++)
        {
            int rowmin = i*blockSize;
            int rowmax = (i + 1)*blockSize;
            if (rowmax > image.rows) rowmax = image.rows;
            int colmin = j*blockSize;
            int colmax = (j + 1)*blockSize;
            if (colmax > image.cols) colmax = image.cols;
            Mat imageROI = image(Range(rowmin, rowmax), Range(colmin, colmax));
            double temaver = mean(imageROI)[0];
            blockImage.at<float>(i, j) = temaver;
        }
    }
    blockImage = blockImage - average;
    Mat blockImage2;
    resize(blockImage, blockImage2, image.size(), (0, 0), (0, 0), INTER_CUBIC);
    Mat image2;
    image.convertTo(image2, CV_32FC1);
    Mat dst = image2 - blockImage2;
    dst.convertTo(image, CV_8UC1);
}

/*=========================================================
* 函 数 名: sum_rgb
* 功能描述: 将整图灰度化并二值化，为取屏做准备
* The function of this function is to convert source image to grayscale and then binarize
=========================================================*/

void sum_rgb(IplImage* src, IplImage* dst)
{
    IplImage* r = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);//8位无符号整型，1代表通道数
    IplImage* g = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage* b = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    //函数 cvSplit 分割多通道数组成分离的单通道数组d
    //Funtion cvSplit splits multiple channel numbers to form a separate single-channel array d.
    cvSplit(src, r, g, b, NULL);//将rgb三个通道的像素分别复制到三通道上
    IplImage* s = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    cvAddWeighted(r, 1. / 3, g, 1. / 3, 0.0, s);//赋权值
    cvAddWeighted(s, 2. / 3, b, 1. / 3, 0.0, s);
    //Set the threshold 20 for distinguishing between the screen and the outside. The value of threshold is variable
    cvThreshold(s, dst, INTERCEPTSCREENTHRESHOLD, 255, CV_THRESH_BINARY);//2值化，必须单通道图像
    cvReleaseImage(&r);
    cvReleaseImage(&g);
    cvReleaseImage(&b);
    cvReleaseImage(&s);
}

/*=========================================================
* 函 数 名: interceptScreenWithoutOutside
* 功能描述: 取出屏幕区域，去除边界部分。
* Intercept screen area, single image.mode 0:Screen area only; mode 1:minimum circumscribed rectangle of the screen
=========================================================*/
Mat interceptScreenWithoutOutside_new(Mat minimumCircumscribedRectangle,int value)
{
    Mat image_screen(minimumCircumscribedRectangle, Rect(value, value, minimumCircumscribedRectangle.cols - value * 2, minimumCircumscribedRectangle.rows - value*2));
    return image_screen;
}
Mat interceptScreenWithoutOutside(Mat minimumCircumscribedRectangle)
{
    Mat image_screen(minimumCircumscribedRectangle, Rect(LEFTRIGHTEDGECOUNT, UPDOWNEDGECOUNT, minimumCircumscribedRectangle.cols - LEFTRIGHTEDGECOUNT * 2, minimumCircumscribedRectangle.rows - UPDOWNEDGECOUNT*2));
    return image_screen;
}
/*=========================================================
* 函 数 名: 字符串替换
=========================================================*/
string&   replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value)
    {
        for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {
            if(   (pos=str.find(old_value,pos))!=string::npos   )
                str.replace(pos,old_value.length(),new_value);
            else   break;
        }
        return   str;
    }


/*=========================================================
* 函 数 名: interceptScreen
* 功能描述: 取屏。两种重载，一种通过名称截取，一种通过图像截取。
* mode：0 取出屏幕区域，去除边界部分。
* mode：1 取出最小外接矩形
* mode：2 取出屏幕区域的Mask图像
* Intercept screen area, single image.mode 0:Screen area only; mode 1:minimum circumscribed rectangle of the screen
=========================================================*/
CvRect rect;//基本数据类型，包括x,y,高和宽。通过定位矩形左上角坐标，确定矩形
CvRect interceptScreen_new(string name)
{
    int i=0;
    CvSeq* contour = NULL;//序列，类似于cector
    CvSeq* contourMax = NULL;
    CvMemStorage * storage = cvCreateMemStorage();//内存存储器，可以存储序列，轮廓，图像，子划分等
    const char* src_path = name.data();
    IplImage* src = cvLoadImage(src_path);
    if(src==NULL)
    {
        qDebug()<<"没读到图片";
    }
    IplImage* dst = cvCreateImage(cvGetSize(src), src->depth, 1);//取单通道
    IplImage* tempdst1 = cvCreateImage(cvGetSize(src), src->depth, 3);//取三通道
    IplImage *pCannyImg = cvCreateImage(cvGetSize(src), src->depth, 1);
    //Bilateral filtering
    bilateralFilter(cvarrToMat(src), cvarrToMat(tempdst1), 5, 100, 3);//对三通道图像双边滤波，去掉噪声同时保留好边缘信息
    //Convert to grayscale and then binarize
    sum_rgb(tempdst1, dst);//灰度图像二值化，三通道灰度化为单通道，黑色，彩色照片都可以
    //腐蚀，腐蚀操作可用于去除小尺度噪声点，或去除连通区域边缘上的毛刺
    //Corrosion operations can be used to remove small-scale noise spots or remove burrs on the edges of connected areas
    cvErode(dst, dst, 0, 3);//腐蚀，0形状，3膨胀大小
    Mat img_masked;
    Mat mask = cvarrToMat(dst);
    //Get the mask image.Here we get the mask image from bilateral filtering image, also we can get from src image.
    //cvarrToMat(tempdst1).copyTo(img_masked, mask);
    cvarrToMat(src).copyTo(img_masked, mask);
    cvCanny(dst, pCannyImg, 0, 10, 3);//canny边缘检测
    cvFindContours(pCannyImg, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);//srorage轮廓存储，contour第一个轮廓指针，ccomp所有轮廓
    cout<<"contour"<<contour;
    double  areaMax = cvContourArea(contour, CV_WHOLE_SEQ);//1、轮廓2、起点终点，默认整个轮廓面积
    for (; contour != NULL; contour = contour->h_next)
    {
        //轮廓的方向影响面积的符号。因此函数也许会返回负的结果。应用函数 fabs() 得到面积的绝对值。
        //The direction of the contour affects the sign of the area. So the funtion might return a negative result.
        //Apply the funtion fabs() to get the absolute value of the area.
        double area = cvContourArea(contour, CV_WHOLE_SEQ);//寻找最大的轮廓面积，即最外层
        //计算整个轮廓或部分轮廓的面积
        //Calculate the area of the entire or partial contour
        if (fabs(area) >= areaMax)
        {
            areaMax = area;
            contourMax = contour;
        }
    }
    rect = cvBoundingRect(contourMax, 0);//返回点集的最外层矩阵
    //image_screen2的目的去除四周的外界黑底，如果可以消除图像畸变和支撑架两端高度不一致的问题就不用使用image_screen2
    //The purpose of image_screen2 is to remove the surrounding black background. If you can eliminate the image distortion and the height of the support frame is not consistent, you don't need to use image_screen2.
    //Mat image_screen2(image_screen, Rect(LEFTRIGHTEDGECOUNT, UPDOWNEDGECOUNT, image_screen.cols - LEFTRIGHTEDGECOUNT * 2, image_screen.rows - UPDOWNEDGECOUNT*2));
    cvReleaseImage(&src);
    cvReleaseImage(&dst);
    cvReleaseImage(&pCannyImg);
    cvReleaseMemStorage(&storage);
    storage = NULL;
    return rect;
}

Mat interceptScreen(string name1 , int mode)
{
    string name=replace_all_distinct(name1,"/","\\");
    //CvSeq本身就是一个可增长的序列
    //CvSeq itself is a growing sequence

    int i=0;
    //cout<<BLACKAVERGRAYVALUE;
    CvSeq* contour = NULL;
    CvSeq* contourMax = NULL;

    //目前黑底时屏幕的获取方式是通过白底的位置来提取
    //At present, the way the screen is obtained at the black bottom is extracted by the position of the white background.
    string name_black="";
    if (name.find("black.bmp")!=name.npos)
    {
        name_black = name;
        name = name.substr(0, name.find("black.bmp")) + "white.bmp";
    }

    //CvMemStorage用来创建一个内存存储器，来统一管理各种动态对象的内存。函数返回一个新创建的内存存储器指针。
    //CvMemStorage is used to create a memory to manage the memory of various dynamic objects.
    //The function returns a newly created memory pointer.
    CvMemStorage * storage = cvCreateMemStorage();
    const char* src_path = name.data();


    qDebug()<<src_path;

    IplImage* src = cvLoadImage(src_path);
    if(src==NULL)
    {
        qDebug()<<"dao";
    }

    qDebug()<<src_path;

    IplImage* dst = cvCreateImage(cvGetSize(src), src->depth, 1);


    IplImage* tempdst1 = cvCreateImage(cvGetSize(src), src->depth, 3);
    IplImage *pCannyImg = cvCreateImage(cvGetSize(src), src->depth, 1);
    //Bilateral filtering
    bilateralFilter(cvarrToMat(src), cvarrToMat(tempdst1), 5, 100, 3);
    //Convert to grayscale and then binarize
    sum_rgb(tempdst1, dst);
    //腐蚀，腐蚀操作可用于去除小尺度噪声点，或去除连通区域边缘上的毛刺
    //Corrosion operations can be used to remove small-scale noise spots or remove burrs on the edges of connected areas
    cvErode(dst, dst, 0, 3);
    Mat img_masked;
    Mat mask = cvarrToMat(dst);
    //Get the mask image.Here we get the mask image from bilateral filtering image, also we can get from src image.
    //cvarrToMat(tempdst1).copyTo(img_masked, mask);
    cvarrToMat(src).copyTo(img_masked, mask);
    /*namedWindow("img_masked", 0);
    imshow("img_masked", img_masked);*/
    if (mode == 1)
    {
        Mat src_black = imread(name_black, 0);
        Mat blackMasked;
        src_black.copyTo(blackMasked, mask);
        cvReleaseImage(&src);
        cvReleaseImage(&dst);
        cvReleaseImage(&tempdst1);
        cvReleaseImage(&pCannyImg);
        cvReleaseMemStorage(&storage);
        storage = NULL;
        return blackMasked;
    }


    cvZero(tempdst1);
    int maxh = 0, maxw = 0;
    int minh = 100000, minw = 100000;
    //Take edges for contour extraction
    cvCanny(dst, pCannyImg, 0, 10, 3);
    //cvNamedWindow("pCannyImg", 0);
    //cvShowImage("pCannyImg", pCannyImg);

    //Contour extraction
    cvFindContours(pCannyImg, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    cvZero(tempdst1);
    double  areaMax = cvContourArea(contour, CV_WHOLE_SEQ);
    for (; contour != NULL; contour = contour->h_next)
    {
        //轮廓的方向影响面积的符号。因此函数也许会返回负的结果。应用函数 fabs() 得到面积的绝对值。
        //The direction of the contour affects the sign of the area. So the funtion might return a negative result.
        //Apply the funtion fabs() to get the absolute value of the area.
        double area = cvContourArea(contour, CV_WHOLE_SEQ);
        //计算整个轮廓或部分轮廓的面积
        //Calculate the area of the entire or partial contour
        if (fabs(area) >= areaMax)
        {
            areaMax = area;
            contourMax = contour;
        }
    }
    //将屏幕部分框出来（不需要实现）
    //Frame the screen section(not required)
    //CvScalar color = CV_RGB(255, 0, 0);
    //计算点集的最外面（up-right）矩形边界
    //Calculate the outermost (up-right) rectangular boundary of the point set
    rect = cvBoundingRect(contourMax, 0);
    //将屏幕部分在原图中标出
    //Mark the screen part in the original image
    //cvRectangle(tempdst1, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), CV_RGB(0, 0, 255), 3, 8, 0);
    //cvAddWeighted(src, 1, tempdst1, 0.9, 0, src);

    //image_screen即为屏幕的最小外接矩形
    //Image_screen is the minimum circumscribed rectangle of the screen

    //需要深拷贝，浅拷贝在离开作用域时会把数据delete掉。
    Mat mat_src = cvarrToMat(src).clone();
    Mat image_screen = mat_src(rect);

    //黑底返回屏幕
    //Return screen when background is black
    if (name_black.find("black.bmp") != name_black.npos)
    {
        Mat src_black = imread(name_black, 0);
        image_screen = src_black(rect);
        //Mat image_screen2(image_screen, Rect(LEFTRIGHTEDGECOUNT, UPDOWNEDGECOUNT, image_screen.cols - LEFTRIGHTEDGECOUNT * 2, image_screen.rows - UPDOWNEDGECOUNT*2));
        cvReleaseImage(&src);
        cvReleaseImage(&dst);
        cvReleaseImage(&tempdst1);
        cvReleaseImage(&pCannyImg);
        cvReleaseMemStorage(&storage);
        storage = NULL;

            return image_screen;

    }

    //image_screen2的目的去除四周的外界黑底，如果可以消除图像畸变和支撑架两端高度不一致的问题就不用使用image_screen2
    //The purpose of image_screen2 is to remove the surrounding black background. If you can eliminate the image distortion and the height of the support frame is not consistent, you don't need to use image_screen2.
    //Mat image_screen2(image_screen, Rect(LEFTRIGHTEDGECOUNT, UPDOWNEDGECOUNT, image_screen.cols - LEFTRIGHTEDGECOUNT * 2, image_screen.rows - UPDOWNEDGECOUNT*2));

    cvReleaseImage(&src);
    cvReleaseImage(&dst);
    cvReleaseImage(&tempdst1);
    cvReleaseImage(&pCannyImg);
    cvReleaseMemStorage(&storage);
    storage = NULL;

        return image_screen;

}

Mat interceptScreen(Mat img, int mode = 0)
{
    //CvSeq本身就是一个可增长的序列
    //CvSeq itself is a growing sequence
    int i=0;
    CvSeq* contour = NULL;
    CvSeq* contourMax = NULL;


    //CvMemStorage用来创建一个内存存储器，来统一管理各种动态对象的内存。函数返回一个新创建的内存存储器指针。
    //CvMemStorage is used to create a memory to manage the memory of various dynamic objects.
    //The function returns a newly created memory pointer.
    CvMemStorage * storage = cvCreateMemStorage();
    IplImage temp=(IplImage)img;
    IplImage* src = &temp;

    IplImage* dst = cvCreateImage(cvGetSize(src), src->depth, 1);
    IplImage* tempdst1 = cvCreateImage(cvGetSize(src), src->depth, 3);
    IplImage *pCannyImg = cvCreateImage(cvGetSize(src), src->depth, 1);
    //Bilateral filtering
    bilateralFilter(cvarrToMat(src), cvarrToMat(tempdst1), 5, 100, 3);
    //Convert to grayscale and then binarize
    sum_rgb(tempdst1, dst);
    //腐蚀，腐蚀操作可用于去除小尺度噪声点，或去除连通区域边缘上的毛刺
    //Corrosion operations can be used to remove small-scale noise spots or remove burrs on the edges of connected areas
    cvErode(dst, dst, 0, 3);
    Mat img_masked;
    Mat mask = cvarrToMat(dst);
    //Get the mask image.Here we get the mask image from bilateral filtering image, also we can get from src image.
    //cvarrToMat(tempdst1).copyTo(img_masked, mask);
    cvarrToMat(src).copyTo(img_masked, mask);
    /*namedWindow("img_masked", 0);
    imshow("img_masked", img_masked);*/

    cvZero(tempdst1);
    int maxh = 0, maxw = 0;
    int minh = 100000, minw = 100000;
    //Take edges for contour extraction
    cvCanny(dst, pCannyImg, 0, 10, 3);
    //cvNamedWindow("pCannyImg", 0);
    //cvShowImage("pCannyImg", pCannyImg);

    //Contour extraction
    cvFindContours(pCannyImg, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    cvZero(tempdst1);
    double  areaMax = cvContourArea(contour, CV_WHOLE_SEQ);
    for (; contour != NULL; contour = contour->h_next)
    {
        //轮廓的方向影响面积的符号。因此函数也许会返回负的结果。应用函数 fabs() 得到面积的绝对值。
        //The direction of the contour affects the sign of the area. So the funtion might return a negative result.
        //Apply the funtion fabs() to get the absolute value of the area.
        double area = cvContourArea(contour, CV_WHOLE_SEQ);
        //计算整个轮廓或部分轮廓的面积
        //Calculate the area of the entire or partial contour
        if (fabs(area) >= areaMax)
        {
            areaMax = area;
            contourMax = contour;
        }
    }
    //将屏幕部分框出来（不需要实现）
    //Frame the screen section(not required)
    //CvScalar color = CV_RGB(255, 0, 0);
    //计算点集的最外面（up-right）矩形边界
    //Calculate the outermost (up-right) rectangular boundary of the point set
    CvRect rect = cvBoundingRect(contourMax, 0);
    //将屏幕部分在原图中标出
    //Mark the screen part in the original image
    //cvRectangle(tempdst1, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), CV_RGB(0, 0, 255), 3, 8, 0);
    //cvAddWeighted(src, 1, tempdst1, 0.9, 0, src);

    //image_screen即为屏幕的最小外接矩形
    //Image_screen is the minimum circumscribed rectangle of the screen

    //需要深拷贝，浅拷贝在离开作用域时会把数据delete掉。
    Mat mat_src = cvarrToMat(src).clone();
    Mat image_screen = mat_src(rect);


    //image_screen2的目的去除四周的外界黑底，如果可以消除图像畸变和支撑架两端高度不一致的问题就不用使用image_screen2
    //The purpose of image_screen2 is to remove the surrounding black background. If you can eliminate the image distortion and the height of the support frame is not consistent, you don't need to use image_screen2.
    Mat image_screen2(image_screen, Rect(LEFTRIGHTEDGECOUNT, UPDOWNEDGECOUNT, image_screen.cols - LEFTRIGHTEDGECOUNT * 2, image_screen.rows - UPDOWNEDGECOUNT * 2));

    cvReleaseImage(&src);
    cvReleaseImage(&dst);
    cvReleaseImage(&tempdst1);
    cvReleaseImage(&pCannyImg);
    cvReleaseMemStorage(&storage);
    storage = NULL;
    if (mode == 1)
        return image_screen;

    return image_screen2;
}

/*=========================================================
* 函 数 名: blackOutOfScreen
* 功能描述: 取屏幕外界区域图像，用于检测外界缺陷，漏光等。
=========================================================*/
Mat blackOutOfScreen_new(CvRect rect,string name)
{
    Mat blackImg = imread(name,0);//0代表单通道读取，灰度图，1代表彩色图，三通道
    blackImg(rect)=uchar(0);
    int x=rect.x;
    int y=rect.y;
    int w=rect.width;
    int h=rect.height;

    Mat blackOutOfScr = blackImg(Rect(x-20,y-20,w+40,h+40));
    return blackOutOfScr;
}
Mat blackOutOfScreen(string name)
{
    Mat blackMasked = interceptScreen(name, 1);
    Mat blackImg = imread(name, 0);
    Mat blackOutOfScr = blackImg - blackMasked;
    return blackOutOfScr;
}


/*************************************************
//  Method:    convertTo3Channels
//  Description: 将单通道图像转为三通道图像
//  Returns:   cv::Mat
//  Parameter: binImg 单通道图像对象
*************************************************/
Mat convertTo3Channels(const Mat& binImg)
{
    Mat three_channel = Mat::zeros(binImg.rows,binImg.cols,CV_8UC3);
    vector<Mat> channels;
    for (int i=0;i<3;i++)
    {
        channels.push_back(binImg);
    }
    merge(channels,three_channel);
    return three_channel;
}


/*=========================================================
* 函 数 名: liantongyuLable
* 功能描述: 连通域标记，得出每个连通域的面积。两种重载，一种在函数内进行二值化，一种在函数外进行二值化
* int cv::connectedComponentsWithStats(
    InputArray  image, // 输入二值图像，黑色背景
    OutputArray     labels, // 输出的标记图像，背景index=0
    OutputArray     stats, // 统计信息，包括每个组件的位置、宽、高与面积
    OutputArray     centroids, // 每个组件的中心位置坐标cx, cy
    int     connectivity, // 寻找连通组件算法的连通域，默认是8连通
    int     ltype, // 输出的labels的Mat类型CV_32S
    int     ccltype // 连通组件算法
    )
其中stats包括以下枚举类型数据信息：
CC_STAT_LEFT
组件的左上角点像素点坐标的X位置.
CC_STAT_TOP
组件的左上角点像素点坐标的Y位置.
CC_STAT_WIDTH
组件外接矩形的宽度
CC_STAT_HEIGHT
组件外接矩形的高度.
CC_STAT_AREA
当前连通组件的面积（像素单位）
=========================================================*/

bool liantongyuLable(Mat img,int grayTh,int areaTh)
{
    Mat dst, Imglabels, Imgstats, Imgcentriods;
    threshold(img, dst, grayTh, 255, CV_THRESH_BINARY);
    int Imglabelnum = connectedComponentsWithStats(dst, Imglabels, Imgstats, Imgcentriods);
    int labelNum = Imgcentriods.rows;
    int area;
    for (int i = 0; i < labelNum; ++i)
    {
        area = Imgstats.at<int>(i, CC_STAT_AREA);
        if (area>areaTh)
            return true;
    }
    return false;
}
bool liantongyuLable(Mat img,  int areaTh)
{
    Mat  Imglabels, Imgstats, Imgcentriods;
    //threshold(img, dst, grayTh, 255, CV_THRESH_BINARY);
    int Imglabelnum = connectedComponentsWithStats(img, Imglabels, Imgstats, Imgcentriods);
    int labelNum = Imgcentriods.rows;
    int area;

    //******************************************************************
    //测试用
    for (int i = 0; i < labelNum; ++i)
    {
        area = Imgstats.at<int>(i, CC_STAT_AREA);
        cout << i << " " << area << endl;
    }
//    vector<cv::Vec3b> colors(Imglabelnum + 1);
//    colors[0] = Vec3b(0, 0, 0); // background pixels remain black.
//    for (int i = 1; i < Imglabelnum; i++) {
//        colors[i] = Vec3b(rand() % 256, rand() % 256, rand() % 256);
//        if (Imgstats.at<int>(i, cv::CC_STAT_AREA) < areaTh)
//            colors[i] = Vec3b(0, 0, 0); // small regions are painted with black too.
//    }
//    Mat img_color = Mat::zeros(img.size(), CV_8UC3);
//    for (int y = 0; y < img_color.rows; y++)
//    for (int x = 0; x < img_color.cols; x++)
//    {
//        int label = Imglabels.at<int>(y, x);
//        CV_Assert(0 <= label && label <= Imglabelnum);
//        img_color.at<cv::Vec3b>(y, x) = colors[label];
//    }

//    string imgname=ImgName.substr((ImgName.find_last_of("/"))!=ImgName.npos?ImgName.find_last_of("/"):ImgName.find_last_of("\\"));
//    string resultname="G:\\MVS\\Data\\resultImg\\"+imgname;
//    imwrite( resultname, img_color);
//    namedWindow("Labeled map", 0);
//    cv::imshow("Labeled map", img_color);
    //******************************************************************


    //位置0处为背景
    for (int i = 1; i < labelNum; ++i)
    {
        area = Imgstats.at<int>(i, CC_STAT_AREA);
        if (area>=areaTh)
        {
            Rect select;//声明矩形
            select.y = Imgstats.at<int>(i, CC_STAT_TOP);
            select.x = Imgstats.at<int>(i, CC_STAT_LEFT);
            select.width =Imgstats.at<int>(i, CC_STAT_WIDTH);
            select.height =Imgstats.at<int>(i, CC_STAT_HEIGHT);
            Mat resultImg(img.size(),CV_8UC3);
            resultImg=convertTo3Channels(img);
            rectangle(resultImg,select,Scalar(0,0,255),10,8,0);//用矩形画矩形窗
            string imgname=ImgName.substr((ImgName.find_last_of("/"))!=ImgName.npos?ImgName.find_last_of("/"):ImgName.find_last_of("\\"));
//            string resultname="G:\\MVS\\Data\\resultImg1.14\\"+imgname;
//            imwrite( resultname, resultImg);
//            namedWindow("Labeled map", 0);
//            cv::imshow("Labeled map", resultImg);
            return true;
        }
    }
    return false;
}

/*=========================================================
* 函 数 名: MyGammaCorrection
* 功能描述: Gamma校正。MyGammaCorrection是从网上download，gammaCorrection是根据matlab中的程序改的
=========================================================*/

void MyGammaCorrection(Mat& src, Mat& dst, float fGamma)
{
    CV_Assert(src.data);
    // accept only char type matrices
    CV_Assert(src.depth() != sizeof(uchar));
    // build look up table
    unsigned char lut[256];
    for (int i = 0; i < 256; i++)
    {
        lut[i] = saturate_cast<uchar>(pow((float)(i / 255.0), fGamma) * 255.0f);
    }
    dst = src.clone();
    const int channels = dst.channels();
    switch (channels)
    {
    case 1:
    {

              MatIterator_<uchar> it, end;
              for (it = dst.begin<uchar>(), end = dst.end<uchar>(); it != end; it++)
                  //*it = pow((float)(((*it))/255.0), fGamma) * 255.0;
                  *it = lut[(*it)];
              break;
    }
    case 3:
    {
              MatIterator_<Vec3b> it, end;
              for (it = dst.begin<Vec3b>(), end = dst.end<Vec3b>(); it != end; it++)
              {
                  //(*it)[0] = pow((float)(((*it)[0])/255.0), fGamma) * 255.0;
                  //(*it)[1] = pow((float)(((*it)[1])/255.0), fGamma) * 255.0;
                  //(*it)[2] = pow((float)(((*it)[2])/255.0), fGamma) * 255.0;
                  (*it)[0] = lut[((*it)[0])];
                  (*it)[1] = lut[((*it)[1])];
                  (*it)[2] = lut[((*it)[2])];
              }
              break;
    }
    }
    //string imgname=ImgName.substr((ImgName.find_last_of("/"))!=ImgName.npos?ImgName.find_last_of("/"):ImgName.find_last_of("\\"));
//    string resultname="G:\\MVS\\Data\\gamma\\"+ImgName;
//    imwrite( resultname, dst);
}


void gammaCorrection(Mat& img, Mat& dst,float fGamma)
{
    int imgRow = img.rows;
    int imgCol = img.cols;
    float *ptmp = NULL;
    uchar * p = NULL;
    for (int i = 0; i <imgRow; i++)
    {
        p = img.ptr<uchar>(i);
        ptmp = dst.ptr<float>(i);
        for (int j = 0; j < imgCol; j++)
        {
            ptmp[j] =(pow(p[j], fGamma));
        }
    }
//    double minVal; double maxVal;
//    minMaxLoc(dst, &minVal, &maxVal);
//    dst = dst / maxVal;
}


/*=========================================================
* 函 数 名: blackDetection
* 功能描述: 黑色底色下的整体检测模块。先判断屏幕面积，然后判断全局平均灰度值，再判断漏光，最后判断屏幕区域
=========================================================*/
bool blackDetection_new(Mat minimumCircumscribedRectangleImg,String name,CvRect rect,string save_name,int signalcheck[8])
{
    int height = minimumCircumscribedRectangleImg.rows;
    int width = minimumCircumscribedRectangleImg.cols;
    if (height*width<HEGEAREA)
    {   causeColor="屏幕面积过小";
        imwrite(save_name,minimumCircumscribedRectangleImg);
        return true;}
    CvScalar cs;
    IplImage temp1=IplImage( minimumCircumscribedRectangleImg);
    cs = cvAvg(&temp1);
    cout<<"全局灰度值为"<<cs.val[0]<<endl;
    if (cs.val[0] > 40)//BLACKAVERGRAYVALUE
    {   causeColor="黑色全局阈值过大";
        imwrite(save_name,minimumCircumscribedRectangleImg);
        return true;}

    Mat blackImg = imread(name);
    int x7=rect.x-15;
    int y7=rect.y-15;
    int w7=rect.width+30;
    int h7=rect.height+30;


    blackImg((Rect(x7,y7,w7,h7)))=uchar(0);
    int x=rect.x;
    int y=rect.y;
    int w=rect.width;
    int h=rect.height;
    int x1=x-50;
    if(x1<0){
        x1=0;
    }
    int y1=y-50;
    if(y1<0){
        y1=0;
    }
    int w1=w+100;
    if(x1+w1> blackImg.cols){
        w1=blackImg.cols-x1;
    }
    int h1=h+100;
    if(h1+y1> blackImg.rows){
        h1=blackImg.rows-y1;
    }

    Mat blackOutOfScr = blackImg(Rect(x1,y1,w1,h1));


//#####################边缘漏光################################
if(signalcheck[0]==1)
{
    Mat blackOutOfScr_gray;
    cvtColor(blackOutOfScr,blackOutOfScr_gray,CV_BGR2GRAY);//屏外区域灰度化
    Mat blackOutOfScr_brain;
    threshold(blackOutOfScr_gray,blackOutOfScr_brain,100, 255, CV_THRESH_BINARY);//屏外区域二值化
    vector<vector<Point>> contours_blackOutOfScr;
    findContours(blackOutOfScr_brain, contours_blackOutOfScr,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    int Num_of_point_blackOutOfScr=0;
    vector<Rect> boundRect_blackOutOfScr(contours_blackOutOfScr.size());
    for(vector<int>::size_type i = 0; i < contours_blackOutOfScr.size(); i++)
        {
          double area0 = contourArea(contours_blackOutOfScr[i]);
          //cout<<area0<<endl;
          if (area0>White_Point_Lower_limit_area and area0<White_Point_Upper_limit_area)//5->white_point_Lower_limit_area
          {   Num_of_point_blackOutOfScr=Num_of_point_blackOutOfScr+1;
              boundRect_blackOutOfScr[i] = boundingRect(Mat(contours_blackOutOfScr[i]));
              int X_1=boundRect_blackOutOfScr[i].tl().x-20;
              int Y_1=boundRect_blackOutOfScr[i].tl().y-20;
              int X_2=boundRect_blackOutOfScr[i].br().x+20;
              int Y_2=boundRect_blackOutOfScr[i].br().y+20;
              CvPoint top_lef = cvPoint(X_1, Y_1);
              CvPoint bottom_right = cvPoint(X_2, Y_2);
              rectangle(blackOutOfScr, top_lef, bottom_right, Scalar(0,0,255), 5, 8, 0);

             }
        }
    if(Num_of_point_blackOutOfScr>0)
    {
        causeColor="边缘漏光";
        imwrite(save_name,blackOutOfScr);

        return true;
    }

//#######################################################
   // imwrite(save_name,blackOutOfScr);
//    //cout<<"边缘漏光缺陷"<<endl;
  //  return true;


//    Mat outOfScreenMedianBlur;
//    medianBlur(outOfScreen, outOfScreenMedianBlur, 3);
  //  Mat outOfScreenMedianBlurBinarization;
   //threshold(blackOutOfScr, outOfScreenMedianBlurBinarization,Edge_leakage_threshold, 255, CV_THRESH_BINARY);// BLACKOUTOFSCREENTHRESHOLD
    Scalar tempVal = mean(blackOutOfScr );
   cout<<"漏光灰度平均值为"<<tempVal.val[0]<<endl;
    if (tempVal.val[0] > Edge_leakage_Average_gray)
    {   causeColor="边缘漏光";
        imwrite(save_name,blackOutOfScr);
        return true;}
}
//##################白点检测###################3
if(signalcheck[1]==1)
{
    Mat minimumCircumscribedRectangleImg_gray;
        Mat white_point_img=minimumCircumscribedRectangleImg.clone();
        cvtColor(white_point_img,minimumCircumscribedRectangleImg_gray,CV_BGR2GRAY);
        Mat Wp_gray_clone=minimumCircumscribedRectangleImg_gray.clone();
        cv::medianBlur(minimumCircumscribedRectangleImg_gray,minimumCircumscribedRectangleImg_gray,3);
        Scalar tempVal1 ;
        int row;
        int col;
        for( row=0;row<minimumCircumscribedRectangleImg_gray.rows-30;row=row+30)//滑动窗口分块检测
        {
            for( col=0;col<minimumCircumscribedRectangleImg_gray.cols-30;col=col+30)
            {
                Rect rect_wp(col,row,30,30);
                Mat temp_wp=minimumCircumscribedRectangleImg_gray(rect_wp);
                tempVal1 =mean( temp_wp );
               double matMean_wp = tempVal1.val[0];
                Mat th_wp;
                threshold(temp_wp, th_wp,matMean_wp+10, 255, CV_THRESH_BINARY);//10->20
                   th_wp.copyTo(Wp_gray_clone(rect_wp));
             }

          }
        for(int rows=0;rows<row;rows=rows+30)
        {
            Rect rect_x(col,rows,minimumCircumscribedRectangleImg_gray.cols-col,30);
            Mat temp_x=minimumCircumscribedRectangleImg_gray(rect_x);
            tempVal1=mean(temp_x);
            double matMean_x=tempVal1.val[0];
            Mat th_x;
            threshold(temp_x,th_x,matMean_x+10,255,CV_THRESH_BINARY);
            th_x.copyTo(Wp_gray_clone(rect_x));
        }
        for(int cols=0;cols<col;cols=cols+30)
        {
            Rect rect_y(cols,row,30,minimumCircumscribedRectangleImg_gray.rows-row);
            Mat temp_y=minimumCircumscribedRectangleImg_gray(rect_y);
            tempVal1=mean(temp_y);
            double matMean_y=tempVal1.val[0];
            Mat th_y;
            threshold(temp_y,th_y,matMean_y+10,255,CV_THRESH_BINARY);
            th_y.copyTo(Wp_gray_clone(rect_y));
        }
        if(minimumCircumscribedRectangleImg_gray.rows-row>0 and minimumCircumscribedRectangleImg_gray.cols-col>0)
        {
            Rect rect_yx(col,row,minimumCircumscribedRectangleImg_gray.cols-col,minimumCircumscribedRectangleImg_gray.rows-row);
            Mat temp_yx=minimumCircumscribedRectangleImg_gray(rect_yx);
            tempVal1=mean(temp_yx);
            double matMean_yx=tempVal1.val[0];
            Mat th_yx;
            threshold(temp_yx,th_yx,matMean_yx+10,255,CV_THRESH_BINARY);
            th_yx.copyTo(Wp_gray_clone(rect_yx));
        }
       // namedWindow("baidian",0);
      //  imshow("baidian",Wp_gray_clone);
        vector<vector<Point>> contours;
        findContours(Wp_gray_clone, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
      //画矩形轮廓
        int Num_of_point=0;
        vector<Rect> boundRect(contours.size());
        for(vector<int>::size_type i = 0; i < contours.size(); i++)
            {
              double area0 = contourArea(contours[i]);
                        if (area0>150 and area0<500)//10->150,500->5000
              {   Num_of_point=Num_of_point+1;
                  boundRect[i] = boundingRect(Mat(contours[i]));
                  int X_1=boundRect[i].tl().x-20;
                  int Y_1=boundRect[i].tl().y-20;
                  int X_2=boundRect[i].br().x+20;
                  int Y_2=boundRect[i].br().y+20;
                  CvPoint top_lef = cvPoint(X_1, Y_1);
                  CvPoint bottom_right = cvPoint(X_2, Y_2);
                  rectangle(white_point_img, top_lef, bottom_right, Scalar(0,0,255), 5, 8, 0);

                 }
           }

        if(Num_of_point>0)
        {
            causeColor="白点缺陷";
            imwrite(save_name,white_point_img);
            return true;
        }

}
    //########白点######################
   // Mat Img_gram= minimumCircumscribedRectangleImg.clone();
   // Mat minimumCircumscribedRectangleImg_gray;
   // Mat white_point_img=minimumCircumscribedRectangleImg.clone();
   // Rect rect2(40,40,white_point_img.cols-80,white_point_img.rows-80);
   // white_point_img=white_point_img(rect2);
   // cvtColor(white_point_img,minimumCircumscribedRectangleImg_gray,CV_BGR2GRAY);
  //  Mat Binarization_gray;
   // threshold(minimumCircumscribedRectangleImg_gray, Binarization_gray,50, 255, CV_THRESH_BINARY);
   // vector<vector<Point>> contours;
  //  findContours(Binarization_gray, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  //画矩形轮廓
   // int Num_of_point=0;
   // vector<Rect> boundRect(contours.size());
   // for(vector<int>::size_type i = 0; i < contours.size(); i++)
    //    {
    //      double area0 = contourArea(contours[i]);
     //     //cout<<area0<<endl;
     //     if (area0>White_Point_Lower_limit_area and area0<White_Point_Upper_limit_area)
     //     {   Num_of_point=Num_of_point+1;
     //         boundRect[i] = boundingRect(Mat(contours[i]));
     //         int X_1=boundRect[i].tl().x-20;
     //         int Y_1=boundRect[i].tl().y-20;
     //         int X_2=boundRect[i].br().x+20;
     //         int Y_2=boundRect[i].br().y+20;
     //         CvPoint top_lef = cvPoint(X_1, Y_1);
     //         CvPoint bottom_right = cvPoint(X_2, Y_2);
     //
     //        }
     //   }
   // if(Num_of_point>0)
   // {
    //    causeColor="白点缺陷";
     //   imwrite(save_name,white_point_img);
        //cout<<"存在"<<Num_of_point<<"个白点缺陷"<<endl;
      //  return true;
   // }

//############################红斑#######################################
if(signalcheck[2]==1)
{
    Mat src_white = minimumCircumscribedRectangleImg( Rect(40,40,minimumCircumscribedRectangleImg.cols-80,minimumCircumscribedRectangleImg.rows-80));
    Mat white_point_img_copy=src_white.clone();
//    int height = white_point_img.rows;
//    int width = white_point_img.cols;
    Mat hsv;
    Mat out;
    //cvtColor(src_white, hsv,COLOR_BayerBG2BGR);
     cvtColor(src_white, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(156, 43, 43), Scalar(180, 255, 255),out);
   // inRange(hsv, Scalar(26, 43, 43), Scalar(77, 255, 255),out);
    vector<vector<Point>> contours2;
    findContours(out, contours2,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  //画矩形轮廓
    int Num_of_point2=0;
    vector<Rect> boundRect2(contours2.size());
    for(vector<int>::size_type i = 0; i < contours2.size(); i++)
        {
          double area0 = contourArea(contours2[i]);
          if (area0>5 and area0<4000000)

          {
              Num_of_point2=Num_of_point2+1;
              boundRect2[i] = boundingRect(Mat(contours2[i]));
              int X_1=boundRect2[i].tl().x;
              int Y_1=boundRect2[i].tl().y;
              int X_2=boundRect2[i].br().x;
              int Y_2=boundRect2[i].br().y;
              CvPoint top_lef2 = cvPoint(X_1, Y_1);
              CvPoint bottom_right2 = cvPoint(X_2, Y_2);
              rectangle(white_point_img_copy , top_lef2, bottom_right2, Scalar(0,0,255), 5, 8, 0);

           }
        }
        if( Num_of_point2>0)
            {
             causeColor="红色斑点";
            imwrite(save_name,white_point_img_copy);

            return  true;
        }

        //############################黄斑#######################################

            Mat src_white2 = minimumCircumscribedRectangleImg(Rect(40,40,minimumCircumscribedRectangleImg.cols-80,minimumCircumscribedRectangleImg.rows-80));

            Mat white_point_img_copy2=src_white2.clone();

        //    int height = white_point_img.rows;
        //    int width = white_point_img.cols;

            Mat hsv2;
            Mat out2;

            cvtColor(src_white2, hsv2, COLOR_BGR2HSV);
            inRange(hsv2, Scalar(11, 43, 43), Scalar(99, 255, 255),out2);
           // inRange(hsv, Scalar(26, 43, 43), Scalar(77, 255, 255),out);
            vector<vector<Point>> contours3;
            findContours(out2, contours3,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
          //画矩形轮廓
            int Num_of_point3=0;
            vector<Rect> boundRect3(contours3.size());
            for(vector<int>::size_type i = 0; i < contours3.size(); i++)
                {
                  double area1 = contourArea(contours3[i]);
                  //cout<<area0<<endl;
                  if (area1>100 and area1<4000000)

                  {  //  cout<<area0<<endl;
                      Num_of_point3=Num_of_point3+1;
                      boundRect3[i] = boundingRect(Mat(contours3[i]));
                      int X_1=boundRect3[i].tl().x;
                      int Y_1=boundRect3[i].tl().y;
                      int X_2=boundRect3[i].br().x;
                      int Y_2=boundRect3[i].br().y;
                      CvPoint top_lef3 = cvPoint(X_1, Y_1);
                      CvPoint bottom_right3 = cvPoint(X_2, Y_2);
                      rectangle(white_point_img_copy2 , top_lef3, bottom_right3, Scalar(0,0,255), 5, 8, 0);

                   }
                }

                if( Num_of_point3>0)
                    {
                     causeColor="黄色斑点";
                    imwrite(save_name,white_point_img_copy2);

                    return  true;
                }

//##########################蓝色斑点#########################################

                Mat src_white3 = minimumCircumscribedRectangleImg(Rect(40,40,minimumCircumscribedRectangleImg.cols-80,minimumCircumscribedRectangleImg.rows-80));
                Mat Hsv_Blue;
                Mat Blue_Out;
                cvtColor(src_white3,Hsv_Blue, COLOR_BGR2HSV);
                vector<Mat> channels;
                split(src_white3, channels);
                Mat Blue_Point=channels.at(0);
                Mat Th_Blue_Point;
                adaptiveThreshold( Blue_Point,Th_Blue_Point, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 21, 5);
                medianBlur(Th_Blue_Point,Th_Blue_Point,3);
                inRange(Hsv_Blue, Scalar(110, 110, 45), Scalar(180,180, 255),Blue_Out);//蓝色颜色模板

               Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(5,5));
               Mat dilate1;
               dilate(Blue_Out, dilate1, structure_element1);
               Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(11,11));
               Mat  erode1;
               erode(dilate1, erode1, structure_element2);
               Mat Candidate_blue;
               bitwise_and(Th_Blue_Point,dilate1,Candidate_blue);
               Mat structure_element3 = getStructuringElement(MORPH_RECT, Size(15,15));
               Mat dilate2;
               dilate(Candidate_blue, dilate2, structure_element3);
               Mat structure_element4 = getStructuringElement(MORPH_RECT, Size(13,13));
               Mat  erode2;
               erode(dilate2, erode2, structure_element4);
               vector<vector<Point>> contours4;
               findContours( erode2, contours4,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
             //画矩形轮廓
               int Num_of_point4=0;
               vector<Rect> boundRect4(contours4.size());
               for(vector<int>::size_type i = 0; i < contours4.size(); i++)
                   {
                     double area4 = contourArea(contours4[i]);
                     if (area4>10 and area4<4000000)//1000->100

                     {
                         Num_of_point4=Num_of_point4+1;
                         boundRect4[i] = boundingRect(Mat(contours4[i]));
                         int X_1=boundRect4[i].tl().x;
                         int Y_1=boundRect4[i].tl().y;
                         int X_2=boundRect4[i].br().x;
                         int Y_2=boundRect4[i].br().y;
                         CvPoint top_lef4 = cvPoint(X_1, Y_1);
                         CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                         rectangle(src_white3 , top_lef4, bottom_right4, Scalar(0,0,255), 5, 8, 0);

                      }
                   }

                   if( Num_of_point4>0)
                       {
                        causeColor="蓝色斑点";
                       imwrite(save_name,src_white3);
                       return  true;
                   }

}

    return false;

}

bool blackDetection(string name)
{
    //ImgName=name;
    //string imgname=ImgName.substr((ImgName.find_last_of("/"))!=ImgName.npos?ImgName.find_last_of("/"):ImgName.find_last_of("\\"));
    //ImgName=name.substr(name.find_last_of("\\"));

    ImgName=name.substr((name.find_last_of("/"))!=name.npos?name.find_last_of("/"):name.find_last_of("\\"));
//    qDebug()<<ImgName.data();

    Mat minimumCircumscribedRectangleImg = interceptScreen(name,0);

    int height = minimumCircumscribedRectangleImg.rows;
    int width = minimumCircumscribedRectangleImg.cols;
    cout << "面积" << height*width<<endl;
    if (height*width<HEGEAREA)
    {   causeColor="屏幕面积";
        return true;}
    /*namedWindow("minimumCircumscribedRectangleImg", 0);
    imshow("minimumCircumscribedRectangleImg", minimumCircumscribedRectangleImg);*/

//    Mat screenImg=interceptScreenWithoutOutside(minimumCircumscribedRectangleImg);
    CvScalar cs;
    IplImage temp1=IplImage( minimumCircumscribedRectangleImg);
    cs = cvAvg(&temp1);
    cout<<cs.val[0]<<endl;
    if (cs.val[0] > BLACKAVERGRAYVALUE)
    {   causeColor="黑色全局阈值过大";
        return true;}
    Mat outOfScreen = blackOutOfScreen(name);
    //Mat outOfScreen = minimumCircumscribedRectangleImg;
    /*namedWindow("outOfScreen", 0);
    imshow("outOfScreen", outOfScreen);*/
    Mat outOfScreenMedianBlur;
    medianBlur(outOfScreen, outOfScreenMedianBlur, 3);
    Mat outOfScreenMedianBlurBinarization;
    threshold(outOfScreenMedianBlur, outOfScreenMedianBlurBinarization, BLACKOUTOFSCREENTHRESHOLD, 255, CV_THRESH_BINARY);
    /*namedWindow("outOfScreenMedianBlurBinarization", 0);
    imshow("outOfScreenMedianBlurBinarization", outOfScreenMedianBlurBinarization);*/

//    bool outOfBlackResult = liantongyuLable(outOfScreenMedianBlurBinarization, BLACKOUTOFSCREENAREA);
//    //测试屏幕部分时注释上
//    if (outOfBlackResult)
//        return true;
    Mat screenImg = interceptScreenWithoutOutside(minimumCircumscribedRectangleImg);



    //开gamma
    //**************************************
    Mat screenGamma(screenImg.size(),CV_32FC1) ;
    Mat screenGammaMedianBlur;

    MyGammaCorrection(screenImg, screenGamma, GAMMAVALUE);
//    temp1=IplImage( screenGamma);
//    cs = cvAvg(&temp1);
//    cout<<cs.val[0]<<endl;
    //screenGamma=screenImg;
    //namedWindow("screenGamma", 0);
    //imshow("screenGamma", screenGamma);
    //gammaCorrection(screenImg, screenGamma, 0.6);
    //screenGamma *= 255;
    //Mat screenGamma8U;
    screenGamma.convertTo(screenGamma, CV_8UC1);
    //imwrite("C:\\gamma.bmp",screenGamma8U);
    unevenLightCompensate(screenGamma);
    //imwrite("伽马0.6"+ImgName, screenGamma);
    //namedWindow("screenGamm2", 0);
    //imshow("screenGamma2", screenGamma);
    medianBlur(screenGamma, screenGammaMedianBlur, 3);
    IplImage temp2=IplImage(screenGammaMedianBlur);
    cs = cvAvg(&temp2);
    int th = cs.val[0];
    if(th>GAMMABLACKAVERGRAYVALUE)
    {   causeColor="伽马全局阈值过大";
        return true;}
    Mat screenGammaMedianBlurBinarization;
    //threshold(screenGammaMedianBlur, screenGammaMedianBlurBinarization, th+16, 255, CV_THRESH_BINARY);//16
    threshold(screenGammaMedianBlur, screenGammaMedianBlurBinarization, 60, 255, CV_THRESH_BINARY);//16





    //namedWindow("screenGammaMedianBlurBinarization", 0);
    //imshow("screenGammaMedianBlurBinarization", screenGammaMedianBlurBinarization);
    //*************************************************


    //不开gamma
    //***************
//    Mat screenMedianBlur;
//    medianBlur(screenImg, screenMedianBlur, 3);
//    Mat screenGammaMedianBlurBinarization;
//    threshold(screenMedianBlur, screenGammaMedianBlurBinarization, BLACKSCREENTHRESHOLD, 255, CV_THRESH_BINARY);//152530
    //**************


    Mat screenGammaMedianBlurBinarization2(screenGammaMedianBlurBinarization.size(),CV_8UC1);
    screenGammaMedianBlurBinarization2=RemoveLittleHoles(screenGammaMedianBlurBinarization);

    //namedWindow("screenGammaMedianBlurBinarization2",0);
    //imshow("screenGammaMedianBlurBinarization2",screenGammaMedianBlurBinarization2);
    //测试漏光时注释
    bool screenOfBlackResult = liantongyuLable(screenGammaMedianBlurBinarization, BLACKSCREENAREA);
    //bool screenOfBlackResult = liantongyuLable(screenBlur, BLACKSCREENTHRESHOLD, BLACKSCREENAREA);
    if (screenOfBlackResult)
    {   causeColor="黑色屏幕";
        return true;}
    return false;








    //*****************************************************
    //Gabor尝试操场
//    unevenLightCompensate(screenImg);

//    Mat centerScreenGabor = GaborfilterWhiteCenter(screenImg, 4, 63, 0, 61);
//    Mat centerScreenGabor1 = GaborfilterWhiteCenter(screenImg, 4, 63, 90, 61);
//    //imwrite("G:\\MVS\\Data\\new_batch1\\centerScreenGabor.bmp",centerScreenGabor);
//   // namedWindow("gabor",0);
//   // imshow("gabor", centerScreenGabor);

//    //Img_Gabor = centerScreenGabor.clone();


//    Mat centerScreenGaborLowerLimit, centerScreenGaborLowerLimit1;
//    threshold(centerScreenGabor, centerScreenGaborLowerLimit, 50, 255, CV_THRESH_BINARY);
//    threshold(centerScreenGabor1, centerScreenGaborLowerLimit1, 50, 255, CV_THRESH_BINARY);
//    centerScreenGaborLowerLimit = centerScreenGaborLowerLimit + centerScreenGaborLowerLimit1;

////    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
////    dilate(centerScreenGaborLowerLimit, centerScreenGaborLowerLimit, element);

//    //Mat centerScreenGaborLimit1 = centerScreenGaborLimit(Range(0, centerScreenGaborLimit.rows), Range(10, centerScreenGaborLimit.cols-170));
//    //namedWindow("二值图", 0);
//    //imshow("二值图", centerScreenGaborLowerLimit);

//    Mat Img222 = centerScreenGaborLowerLimit.clone();
//    RemoveSmallRegion(centerScreenGaborLowerLimit, Img222,55,1,1);

//    //namedWindow("去除小连通域", 0);
//    //imshow("去除小连通域", Img222.clone());

//    vector<Vec4i> hierarchy;
//    vector<vector<Point> > contours;
//    findContours(Img222, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

//    bool centerScreenGaborLimitResult = false;
//    if (contours.size())
//    {
//        Mat cnt_img = Mat::zeros(Img222.rows, Img222.cols, CV_8UC3);
//        int index = 0;
//        for ( ; index >= 0; index = hierarchy[index][0])
//        {
//            Scalar color(0, 0, 255);
//            drawContours(cnt_img, contours, index, color, 10, 8, hierarchy);
//        }
//        centerScreenGaborLimitResult = true;

//        string imgname=ImgName.substr((ImgName.find_last_of("/"))!=ImgName.npos?ImgName.find_last_of("/"):ImgName.find_last_of("\\"));
//        //string imgname=ImgName.substr((ImgName.find_last_of("/"))!=ImgName.npos?ImgName.find_last_of("/"):ImgName.find_last_of("\\"));
//        string resultname="G:\\MVS\\Data\\resultImg4\\"+imgname;
//        imwrite( resultname, cnt_img);
//        namedWindow("0",0);
//        imshow("0",cnt_img);
//    }


//    if (centerScreenGaborLimitResult)
//        return true;
//    else
//    {
//        return false;
//    }
    //************************************************************
}


/*=========================================================
* 函 数 名: whiteCenterScreenDetection
* 功能描述: 白色屏幕区域检测。
=========================================================*/

bool whiteCenterScreenDetection(Mat centerScreen)
{
    Mat CenterScreen =( centerScreen.clone());

    bool centerScreenGaborLimitResult = false;

    unevenLightCompensate(centerScreen);

    Mat centerScreenGabor = GaborfilterWhiteCenter(centerScreen, 5, 50, 90, 90);
    Mat centerScreenGabor1 = GaborfilterWhiteCenter(centerScreen, 5, 50, 0, 90);
   // imwrite("G:\\MVS\\Data\\1.10\\centerScreenGabor90.bmp",centerScreenGabor);
    //imwrite("G:\\MVS\\Data\\1.10\\centerScreenGabor0.bmp",centerScreenGabor1);
   // namedWindow("gabor",0);
   // imshow("gabor", centerScreenGabor);

    //Img_Gabor = centerScreenGabor.clone();

    CvScalar cs;
    IplImage temp = IplImage(centerScreenGabor);
    cs = cvAvg(&temp);
    Mat centerScreenGaborLowerLimit, centerScreenGaborLowerLimit1;
    threshold(centerScreenGabor, centerScreenGaborLowerLimit, 100, 255, CV_THRESH_BINARY);
    threshold(centerScreenGabor1, centerScreenGaborLowerLimit1, 100, 255, CV_THRESH_BINARY);
    centerScreenGaborLowerLimit = centerScreenGaborLowerLimit + centerScreenGaborLowerLimit1;

    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
    dilate(centerScreenGaborLowerLimit, centerScreenGaborLowerLimit, element);

    //Mat centerScreenGaborLimit1 = centerScreenGaborLimit(Range(0, centerScreenGaborLimit.rows), Range(10, centerScreenGaborLimit.cols-170));
    //namedWindow("二值图", 0);
    //imshow("二值图", centerScreenGaborLowerLimit);

    //Mat Img222 = centerScreenGaborLowerLimit.clone();
    //RemoveSmallRegion(centerScreenGaborLowerLimit, Img222,55,1,1);

    //namedWindow("去除小连通域", 0);
    //imshow("去除小连通域", Img222.clone());

    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(centerScreenGaborLowerLimit, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

    if (contours.size())
    {
        Mat cnt_img = Mat::zeros(CenterScreen.rows, CenterScreen.cols, CV_8UC3);
        cvtColor(CenterScreen,cnt_img,CV_GRAY2RGB);
        int index = 0;
        Scalar color(0, 0, 255);
        for (int i = 0; i < contours.size(); i++)
        {
            cout << i <<":   "<< contourArea(contours[i]) << endl;
            if (contourArea(contours[i]) > 55)
            {
                drawContours(cnt_img, contours, index, color, 5, 8, hierarchy);
                centerScreenGaborLimitResult = true;
            }
            index = hierarchy[index][0];
        }
        //string imgname=ImgName.substr((ImgName.find_last_of("/"))!=ImgName.npos?ImgName.find_last_of("/"):ImgName.find_last_of("\\"));
        //string resultname="G:\\MVS\\Data\\resultImg1.14\\"+imgname;
//        string resultname="G:\\MVS\\Data\\resultImg1.14\\"+ImgName;
 //       imwrite( resultname, cnt_img);
//        namedWindow("0",0);
//        imshow("0",CenterScreen);
    }



     return centerScreenGaborLimitResult;


}

bool whiteCenterScreenBrightDetection(Mat centerScreen)
{
    unevenLightCompensate(centerScreen);

    Mat centerScreenGabor = Gaborfilter(centerScreen, 1, 30, 0);//2,50,75
   // namedWindow("Gabor",0);
   // imshow("Gabor",centerScreenGabor);
    CvScalar cs;
    IplImage temp = IplImage(centerScreenGabor);
    cs = cvAvg(&temp);
    Mat centerScreenGaborUpperLimit,centerScreenGaborLowerLimit;
    threshold(centerScreenGabor, centerScreenGaborUpperLimit, cs.val[0]+10, 255, CV_THRESH_BINARY);
    //threshold(centerScreenGabor, centerScreenGaborLowerLimit, cs.val[0]-10, 255, CV_THRESH_BINARY);
  //  namedWindow("centerScreenGaborUpperLimit",0);
  //  imshow("centerScreenGaborUpperLimit",centerScreenGaborUpperLimit);
    //namedWindow("centerScreenGaborLowerLimit",0);
    //imshow("centerScreenGaborLowerLimit",centerScreenGaborLowerLimit);
    bool result=liantongyuLable(centerScreenGaborUpperLimit,30);
    if(result)
        return true;
    return false;
}


/*=========================================================
* 函 数 名: binaryFindFirstMoreNum
* 功能描述: 白色边缘区域检测。（拟合部分未完成）
=========================================================*/

int binaryFindFirstMoreNum(vector<int>& nums, int n)
{
    int left = 0;
    int right = nums.size() - 1;
    while(left < right)
    {
        int middle = (left + right) / 2;
        if(nums[middle] > n)
            right = middle;  //保留大于n的下标以防这是第一个
        else if(nums[middle] <= n)  //将小于改为小于等于
            left = middle + 1;
    }
    if(nums[left] > n)
        return left;
    else
        return -1;
}






/*========================================================================================================
* 函 数 名: Out_Screen_Detection
* 功能描述: 取出最小外接矩形前30行与后30行的数据,对漏光进行检测
* 输入变量: 1.单通道IMG 2.图片的行 3.图片的列 4.阈值
* 输出图像：输出上30行+后30行的合成图片
==========================================================================================================*/
Mat Out_Screen_Detection(Mat img, int Rows, int Cols , int Scale)
{
    //取出屏幕前30行
    Fit fit1;
    Mat img2,img3;
    Mat Img_OutScreen_Up = img(Range(0, WHITESCREENEDGECOUNT), Range(100, Cols-100));
    cv::resize(Img_OutScreen_Up, img2, cv::Size(Size(Img_OutScreen_Up.cols / 2, Img_OutScreen_Up.rows)), 0, 0 , INTER_AREA);

    //imshow("1", img2);
    Mat Img_OutScreen_Down = img(Range(Rows - WHITESCREENEDGECOUNT ,Rows), Range(100, Cols-100));
    cv::resize(Img_OutScreen_Down, img3, cv::Size(Size(Img_OutScreen_Down.cols / 2, Img_OutScreen_Down.rows)), 0, 0, INTER_AREA);

    Mat_<int> img1 = img2.clone();
    Mat_<int> img4 = img3.clone();
    //Mat_<int> img1 = img.clone();


    //ofstream f("G:\\PhoneScreenDetect\\1.Program\\拟合_滤波_去除小连通域\\OutTxt.txt");//以输出方式打开文件

    vector<vector<int>> array_Up;
    vector<vector<int>> array_Up_Right;
    vector<vector<int>> array_Up_Right_Diff;

    int Rows_ = img2.rows * 2;
    int array_right[Rows_];
    memset(array_right, 0, sizeof(array_right));

    for (int i = 0; i < Rows_; i++)
    {

        vector<int> array_up_0;
        vector<int> array_up_1;

        bool IsZero = false;
        bool IsZero1 = false;
        for (int j = 0; j < img2.cols; j++)
        {
            if (i <  WHITESCREENEDGECOUNT)
            {
                if (img1[i][j] >= 10)
                {
                    if (IsZero == false)
                    {
                        array_right[i] = j;

                    }
                    array_up_1.push_back(img1[i][j]);
                    IsZero = true;
                }
                else
                {
                    array_up_0.push_back(0);
                }
            }
            else
            {
                if (img4[i-WHITESCREENEDGECOUNT][j] >= 10)
                {
                    if (IsZero1 == false )
                    {
                        array_right[i] = j;

                    }
                    array_up_1.push_back(img4[i-WHITESCREENEDGECOUNT][j]);
                    IsZero1 = true;
                }
                else
                    array_up_0.push_back(0);
            }

        }
        array_Up.push_back(array_up_1);
        array_Up_Right.push_back(array_up_0);
        //cout << array_right[i] << endl;
    }

    array_Up_Right_Diff.assign(array_Up.begin(), array_Up.end());

    vector<vector<int>> array_rows_x;

    for (int i = 0; i < Rows_; i++)
    {
        vector<int> array_up_x;
        for (int b = 0; b < array_Up[i].size(); b++)
            array_up_x.push_back(b+1);
        array_rows_x.push_back(array_up_x);
    }

    //cout<<
    //array_Up = Read_Mat(array_Up, img2, img2.rows, img2.cols);

    for (int i = 0; i < Rows_; i++)
    {
        fit1.polyfit(array_rows_x[i], array_Up[i], 3, false);

        for (int j = 0; j < array_Up[i].size(); j++)
        {

            array_Up_Right_Diff[i][j] = array_Up[i][j] - (fit1.getFactor(0) + fit1.getFactor(1) * (j + 1) + fit1.getFactor(2)  * pow(j + 1, 2) + fit1.getFactor(3)  * pow(j + 1, 3));


            if (array_Up_Right_Diff[i][j] < -Scale)   //阈值一般选为25
                array_Up_Right_Diff[i][j] = 255;
            else
                array_Up_Right_Diff[i][j] = 0;
        }

        array_Up_Right[i].insert(array_Up_Right[i].end() - array_right[i], array_Up_Right_Diff[i].begin(), array_Up_Right_Diff[i].end());
    }

    Mat img5(Rows_, img1.cols, CV_8UC1);
    uchar *ptmp = NULL;
    for (int i = 0; i <Rows_; i++)
    {
        vector<int> z_i = array_Up_Right[i];
        ptmp = img5.ptr<uchar>(i);
        for (int j = 0; j < img1.cols; j++)
            ptmp[j] = (uchar)z_i[j];
    }
    //imshow("1", img5);
    return img5;
}

bool whiteEdgeScreenDetection(Mat minimumCircumscribedRectangle)
{
    Mat result3;
    int nRows=minimumCircumscribedRectangle.rows,nCols=minimumCircumscribedRectangle.cols;
    cvtColor(minimumCircumscribedRectangle,result3,CV_BGR2GRAY);
    Mat Img=Gaborfilter(result3,3,400,85);
    Mat Img3=Out_Screen_Detection(Img,nRows,nCols,15);
    Mat Img4=RemoveLittleHolesBaiYin(Img3);
    bool result=liantongyuLable(Img4,50);
    if(result)
        return true;
    else {
        return false;
    }
}

/*=========================================================
* 函 数 名: dustRemoval
* 功能描述: 除尘操作，若吹风机吹尘后不含灰尘则输出吹尘后图像，若灰尘还在屏上，则将吹尘后图像中灰尘粒所在区域像素替换成吹尘前的像素
=========================================================*/

Mat dustRemoval(Mat beforeDustRemoval, Mat afterDustRemoval)
{
    int rowBefore = beforeDustRemoval.rows;
    int colBefore = beforeDustRemoval.cols;
    int rowAfter = afterDustRemoval.rows;
    int colAfter = afterDustRemoval.cols;
    int rowNew, colNew;
    if (rowBefore > rowAfter)
        rowNew = rowAfter;
    if (colBefore > colAfter)
        colNew = colAfter;
    Mat beforeDustRemovalNew(beforeDustRemoval, Rect(0, 0, colNew, rowNew));
    Mat afterDustRemovalNew(afterDustRemoval, Rect(0, 0, colNew, rowNew));
    Mat dustImg = beforeDustRemovalNew - afterDustRemovalNew;
    threshold(dustImg, dustImg, DUSTTHRESHOLD, 255, CV_THRESH_BINARY);

    Mat  Imglabels, Imgstats, Imgcentriods;
    //threshold(img, dst, grayTh, 255, CV_THRESH_BINARY);
    int Imglabelnum = connectedComponentsWithStats(dustImg, Imglabels, Imgstats, Imgcentriods);
    int labelNum = Imgcentriods.rows;
    int area;
    //Mat afterDustRemovalNew
    for (int i = 0; i < labelNum; ++i)
    {
        area = Imgstats.at<int>(i, CC_STAT_AREA);
        if (area>DUSTAREA)
        {
            double leftX = Imgstats.at<int>(i, CC_STAT_LEFT);//连通域的boundingbox的X
            double topY = Imgstats.at<int>(i, CC_STAT_TOP);//连通域的boundingbox的Y
            double width = Imgstats.at<int>(i, CC_STAT_WIDTH);//连通域的宽
            double height =Imgstats.at<int>(i, CC_STAT_HEIGHT);//连通域的高
            Mat dustROI = beforeDustRemovalNew(Rect(leftX, topY, width, height));
            dustROI.copyTo(afterDustRemovalNew(Rect(leftX, topY, width, height)));
        }
    }
    return afterDustRemovalNew;
}


/*=========================================================
* 函 数 名: dataPredict
* 功能描述: 数据预测，0为正序预测，1为倒序预测。用于白印和划伤等白色缺陷
=========================================================*/

Mat dataPredict(Mat dataPrevious, int mode)
{
    Mat dataNew(dataPrevious.size(), CV_8UC1);
    dataNew= dataPrevious.clone();
    int row = dataPrevious.rows;
    int col = dataPrevious.cols;
    if (mode == 0)
    {
        for (int i = 0; i < row; ++i)
        {
            //Mat rowData = dataPrevious.row(i);
            int diffZhengXu = DIFFZHENGXU;
            uchar* rowData = dataPrevious.ptr<uchar>(i);
            uchar* rowDataNew = dataNew.ptr<uchar>(i);
            for (int j = 0; j < col - PREDICTDISTANCE * 2; ++j)
            {
                /*if ((rowData[j] + (rowData[j + PREDICTDISTANCE] - rowData[j]) * 2)>255)
                    cout << "超过255 ";*/
                //cout << (int)rowData[j] << " ";
                int diff = (rowData[j + PREDICTDISTANCE] - rowData[j])<0 ? 0 : rowData[j + PREDICTDISTANCE] - rowData[j];
                //rowDataNew[j + PREDICTDISTANCE * 2] = (rowData[j] + (rowData[j + PREDICTDISTANCE] - rowData[j]) * 2)>255 ? 255 : rowData[j] + (rowData[j + PREDICTDISTANCE] - rowData[j]) * 2;
                rowDataNew[j + PREDICTDISTANCE * 2] = (rowData[j] + diff * 2)>255 ? 255 : rowData[j] + (diff) * 2;


                if (rowData[j + PREDICTDISTANCE * 2] - rowDataNew[j + PREDICTDISTANCE * 2] <= DIFFZHENGXU)
                    rowDataNew[j + PREDICTDISTANCE * 2] = rowData[j + PREDICTDISTANCE * 2];
                //cout <<(int) rowDataNew[j] << " ";
            }
        }
        return dataNew;
    }
    else if (mode == 1)
    {
        for (int i = 0; i < row; ++i)
        {
            //Mat rowData = dataPrevious.row(i);
            int diffDaoXu = DIFFDAOXU;
            uchar* rowData = dataPrevious.ptr<uchar>(i);
            uchar* rowDataNew = dataNew.ptr<uchar>(i);
            for (int j = col-1; j >  PREDICTDISTANCE * 2; --j)
            {
                int diff = (rowData[j - PREDICTDISTANCE] - rowData[j])<0 ? 0 : rowData[j - PREDICTDISTANCE] - rowData[j];
                rowDataNew[j - PREDICTDISTANCE * 2] = (rowData[j] + diff * 2)>255 ? 255 : rowData[j] + (diff)* 2;
                if (rowData[j - PREDICTDISTANCE * 2] - rowDataNew[j - PREDICTDISTANCE * 2] <= DIFFDAOXU)
                    rowDataNew[j - PREDICTDISTANCE * 2] = rowData[j - PREDICTDISTANCE * 2];

            }
        }
        return dataNew;
    }
}


/*=========================================================
* 函 数 名: matShift
* 功能描述: 图像像素移动。本部分只实现了将图像像素向左位移
=========================================================*/
Mat matShift(Mat beforeShift)
{
    Mat afterShift(beforeShift.size(), CV_8UC1);
    afterShift = Scalar::all(0);
    Mat outRightImg = beforeShift(Range::all(), Range(0, SHIFTDISTANCE - 1));
    Mat outLeftImg = beforeShift(Range::all(), Range(SHIFTDISTANCE - 1, beforeShift.cols - 1));
    Mat afterRightImg = afterShift(Rect(SHIFTDISTANCE, 0, SHIFTDISTANCE, beforeShift.rows));
    Mat afterLeftImg = afterShift(Rect(0, 0, beforeShift.cols - SHIFTDISTANCE, beforeShift.rows));
    outLeftImg.copyTo(afterLeftImg);
    outRightImg.copyTo(afterRightImg);
    return afterShift;
}


/*=========================================================
* 函 数 名: detectionBaiYin
* 功能描述: 白印检测。centerScreen应该为屏幕原图进行Gabor滤波后的图像
=========================================================*/

Mat detectionBaiYin(Mat img)
{
//    Mat centerScreen(img.size(),CV_8UC1);
//    cvtColor(img,centerScreen,CV_RGB2GRAY);
//    Mat halfCenterScreen;
//    resize(img,halfCenterScreen,Size(img.cols / 2, img.rows/2), 0.5, 0.5, CV_INTER_AREA);
    Mat centerScreenGabor=New_Gabor(img);
    //namedWindow("Gabor",0);
    //imshow("Gabor",centerScreenGabor);
    Mat centerScreenMedianBlur(centerScreenGabor.size(),CV_8UC1);
    medianBlur(centerScreenGabor, centerScreenMedianBlur, 5);
    //imwrite("中值滤波.bmp", centerScreenMedianBlur);
    //Mat predictArea = centerScreenMedianBlur(Range(20, centerScreenMedianBlur.rows - 20), Range(20, centerScreenMedianBlur.cols - 20));
    Mat predictArea = centerScreenMedianBlur;

    Mat imgPredictZhengXu(predictArea.size(),CV_8UC1);
    imgPredictZhengXu = dataPredict(predictArea, 0);
    //imwrite("正序.bmp", imgPredictZhengXu);
    //namedWindow("imgPredictZhengXu", 0);
    //imshow("imgPredictZhengXu", imgPredictZhengXu);
    Mat imgPredictDaoXu;
    imgPredictDaoXu = dataPredict(predictArea, 1);
    //imwrite("倒序.bmp", imgPredictDaoXu);
    //namedWindow("imgPredictDaoXu", 0);
    //imshow("imgPredictDaoXu", imgPredictDaoXu);
    Mat diffPredictZhengXu = predictArea - imgPredictZhengXu;
    //cout << diffPredictZhengXu << endl;
    //namedWindow("diffPredictZhengXu", 0);
    //imshow("diffPredictZhengXu", diffPredictZhengXu);
    Mat diffPredictDaoXu = predictArea - imgPredictDaoXu;
    //namedWindow("diffPredictDaoXu", 0);
    //imshow("diffPredictDaoXu", diffPredictDaoXu);
    Mat diffPredictZhengXuBinarization, diffPredictDaoXuBinarization;
    threshold(diffPredictZhengXu, diffPredictZhengXuBinarization,DIFFPREDICTZHENGXUTHRESHOLD , 255, CV_THRESH_BINARY);
    //namedWindow("diffPredictZhengXuBinarization", 0);
    //imshow("diffPredictZhengXuBinarization", diffPredictZhengXuBinarization);
    threshold(diffPredictDaoXu, diffPredictDaoXuBinarization, DIFFPREDICTDAOXUTHRESHOLD, 255, CV_THRESH_BINARY);
    //namedWindow("diffPredictDaoXuBinarization", 0);
    //imshow("diffPredictDaoXuBinarization", diffPredictDaoXuBinarization);
    Mat ellipse = getStructuringElement(MORPH_ELLIPSE, Size(7, 7), Point(-1, -1));
    Mat ellipseZhengXu, ellipseDaoXu;
    dilate(diffPredictZhengXuBinarization, ellipseZhengXu, ellipse);
    //namedWindow("ellipseZhengXu", 0);
    //imshow("ellipseZhengXu", ellipseZhengXu);
    dilate(diffPredictDaoXuBinarization, ellipseDaoXu, ellipse);
    //namedWindow("ellipseDaoXu", 0);
    //imshow("ellipseDaoXu", ellipseDaoXu);
    Mat diffPredictDaoXuBinarizationShift = matShift(ellipseDaoXu);
    //namedWindow("diffPredictZhengXuBinarizationShift", 0);
    //imshow("diffPredictZhengXuBinarizationShift", diffPredictDaoXuBinarizationShift);
    Mat intersectMat = ellipseZhengXu & diffPredictDaoXuBinarizationShift;
    //namedWindow("intersectMat", 0);
    //imshow("intersectMat", intersectMat);
    Mat detectionMat = intersectMat(Range(BAIYINEDGEDISTANCE, intersectMat.rows - BAIYINEDGEDISTANCE), Range(BAIYINEDGEDISTANCE, intersectMat.cols - BAIYINEDGEDISTANCE));
    //namedWindow("detectionMat", 0);
    //imshow("detectionMat", detectionMat);

//    Mat detectionMatRemove=RemoveLittleHoles(detectionMat);
//    bool result = liantongyuLable(detectionMatRemove, BAIYINAREA);
//    if (result)
//        return true;
//    return false;
    return centerScreenGabor;
}


/*=========================================================
* 函 数 名: whiteDetection
* 功能描述: 白色底色检测。包括中心屏幕检测，边缘检测，白印检测。不含倾斜检测。
=========================================================*/
bool whiteDetection_new(Mat minimumCircumscribedRectangle,string save_name,int signalcheck[8])
{




    //###########################缺线######################
     //  Mat lose_line=minimumCircumscribedRectangle.clone();
    //   Mat lose_line_mark=minimumCircumscribedRectangle.clone();
    //   double m1 = mean(lose_line)[0];
    //   int Num_lose_line=0;
    //   for(int row_line=30;row_line<lose_line.rows-30;row_line=row_line+2)
    //   {
   //        Mat row = lose_line.rowRange(row_line,row_line+1).clone();
   //        cout<<row_line;
   //         double m = mean(row)[0];
   //         if(m<m1-10)//缺线20-》30
   //         {
   //             Num_lose_line=Num_lose_line+1;
   //             CvPoint top_lef2 = cvPoint(100,row_line);
   //             CvPoint bottom_right2 = cvPoint(600,row_line+1);
   //             rectangle(lose_line_mark, top_lef2, bottom_right2, Scalar(0,0,255), 1, 8, 0);



     //       }


    //   }
     //  if(Num_lose_line>0)
     //  {
    //       causeColor="缺线";
     //      imwrite(save_name,lose_line_mark);

     //      return true;
     //  }





    Mat img_gray=minimumCircumscribedRectangle.clone();
    //img_gray=gamma_new(img_gray);
    cvtColor(img_gray,img_gray,CV_BGR2GRAY);
    Mat Gabor_img=New_Gabor(img_gray);
    Mat YIWU_Gabor=Gabor_img.clone();

    //777777777777777背光异物检测7777777777777777777777777
//       YIWU_Gabor= interceptScreenWithoutOutside_new(YIWU_Gabor,20);
//        // Mat YIWU_Gabor_gray;
//        // cvtColor(YIWU_Gabor,YIWU_Gabor_gray,CV_BGR2GRAY);
//         Mat YIWU_Gabor_gray_blur;
//        // blur( YIWU_Gabor,YIWU_Gabor_gray_blur,Size(2, 2));
//         Mat th;
//         adaptiveThreshold(YIWU_Gabor, th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, Black_Point_adthreshold_kernel_size, Black_Point_adthreshold_constant);//21越大检测越灵敏，5越小越灵敏

////         imwrite(save_name,th);
////          return true;

//         vector<vector<Point>> contours2;
//         findContours(th, contours2,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//       //画矩形轮廓
//         int Num_of_point2=0;
//         vector<Rect> boundRect2(contours2.size());
//         for(vector<int>::size_type i = 0; i < contours2.size(); i++)
//             {
//               double area0 = contourArea(contours2[i]);
//               //cout<<area0<<endl;
//               if (area0>Black_Point_Lower_limit_area and area0<Black_Point_Upper_limit_area)

//               {  //  cout<<area0<<endl;
//                   Num_of_point2=Num_of_point2+1;
//                   boundRect2[i] = boundingRect(Mat(contours2[i]));
//                   int X_1=boundRect2[i].tl().x-20;
//                   int Y_1=boundRect2[i].tl().y-20;
//                   int X_2=boundRect2[i].br().x+20;
//                   int Y_2=boundRect2[i].br().y+20;
//                   CvPoint top_lef2 = cvPoint(X_1, Y_1);
//                   CvPoint bottom_right2 = cvPoint(X_2, Y_2);
//                   rectangle(YIWU_Gabor, top_lef2, bottom_right2, Scalar(0,0,255), 5, 8, 0);

//                  }
//             }
//         if(Num_of_point2>0)
//         {
//           //  causeColor="背光异物";
//            //imwrite(save_name,YIWU_Gabor);
//            // cout<<"存在"<<Num_of_point2<<"个黑点缺陷"<<endl;
//            // return true;
//         }
//########################异物#####################
//if(signalcheck[4]==1)
//{

 //        Mat yiwu;
//           yiwu=New_Gabor(img_gray);
        //  cvtColor(minimumCircumscribedRectangle,yiwu,CV_BGR2GRAY);
//          blur(yiwu,yiwu,Size(5,5));
//         Mat centerScreen_yiwu(yiwu, Rect(0, 0, Gabor_img.cols - 0 * 2, Gabor_img.rows - 0*2));
//         Mat centerScreen_copy_yiwu= centerScreen_yiwu.clone();
//         double minv_yiwu = 0.0, maxv_yiwu = 0.0;
//          double* minp_yiwu = &minv_yiwu;
//          double* maxp_yiwu = &maxv_yiwu;
//          double m_yiwu = 0;
//          double diff_yiwu=0;
//           vector<Mat> channels2_yiwu;
//           Mat M_yiwu(centerScreen_yiwu.size[0], centerScreen_yiwu.size[1], CV_8UC1, Scalar(0) );




//         for(int row=0;row<centerScreen_yiwu.rows-50;row=row+30)
//         {
//             for(int col=0;col<centerScreen_yiwu.cols-50;col=col+30)

//             {
//                 Rect rect1(col,row,50,50);
//                 Mat temp_yiwu=centerScreen_yiwu(rect1);

//                 m_yiwu = mean(temp_yiwu)[0];



//              minMaxIdx(temp_yiwu,minp_yiwu,maxp_yiwu);
//              diff_yiwu=maxv_yiwu-m_yiwu;


//            Mat th2_yiwu;
//            adaptiveThreshold(temp_yiwu, th2_yiwu, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, Black_Point_adthreshold_kernel_size, Black_Point_adthreshold_constant);//21越大检测越灵敏，5越小越灵敏


                          // threshold(temp_yiwu, th2_yiwu,m_yiwu+3, 255, CV_THRESH_BINARY);

//             th2_yiwu.copyTo(M_yiwu(rect1));


//              }


//             }
 //        vector<vector<Point>> contours_yiwu;
//         findContours(M_yiwu, contours_yiwu,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
       //画矩形轮廓
//         int Num_of_point_yiwu=0;
//         vector<Rect> boundRect_yiwu(contours_yiwu.size());
//         for(vector<int>::size_type i = 0; i < contours_yiwu.size(); i++)
//             {
//               double area_yiwu = contourArea(contours_yiwu[i]);
               //cout<<area0<<endl;
//               if (area_yiwu>50 and area_yiwu<500)//Black_Point_Lower_limit_area......Black_Point_Upper_limit_area\100~500

//               {  //  cout<<area0<<endl;
//                   Num_of_point_yiwu=Num_of_point_yiwu+1;
//                   boundRect_yiwu[i] = boundingRect(Mat(contours_yiwu[i]));
//                   int X_1_yiwu=boundRect_yiwu[i].tl().x-20;
//                   int Y_1_yiwu=boundRect_yiwu[i].tl().y-20;
//                   int X_2_yiwu=boundRect_yiwu[i].br().x+20;
//                   int Y_2_yiwu=boundRect_yiwu[i].br().y+20;
//                   CvPoint top_lef_yiwu = cvPoint(X_1_yiwu, Y_1_yiwu);
//                   CvPoint bottom_right_yiwu = cvPoint(X_2_yiwu, Y_2_yiwu);
//                   rectangle(centerScreen_copy_yiwu, top_lef_yiwu, bottom_right_yiwu, Scalar(0,0,255), 5, 8, 0);

//                  }
//             }
//         if(Num_of_point_yiwu>0)
 //        {
 //            causeColor="背光异物";
 //          imwrite(save_name,centerScreen_copy_yiwu);

 //            return true;
 //        }

//}
//#########################太阳斑##################

//77777777777777777777白印检测7777777777777777777
if((signalcheck[5]==1)&&false)
{



 //###############白印#################################
 Mat minimumCircumscribedRectangle_copy;
 //namedWindow("bb",0);
 //imshow("bb",minimumCircumscribedRectangle);
 Mat G=minimumCircumscribedRectangle.clone();
  cvtColor(minimumCircumscribedRectangle,minimumCircumscribedRectangle_copy,CV_BGR2GRAY);
// blur(minimumCircumscribedRectangle_copy,minimumCircumscribedRectangle_copy,Size(Baiyin_Blur_kernel_size,Baiyin_Blur_kernel_size));
 Mat centerScreen(minimumCircumscribedRectangle_copy, Rect(30, 30, Gabor_img.cols - 30 * 2, Gabor_img.rows - 30*2));
 Mat centerScreen_copy= centerScreen.clone();
 double minv = 0.0, maxv = 0.0;
  double* minp = &minv;
  double* maxp = &maxv;
  double m = 0;
  double diff=0;
   vector<Mat> channels2;
   Mat M(centerScreen.size[0], centerScreen.size[1], CV_8UC1, Scalar(0) );


int Num_baiyin=0;
int row;
int col;
 for(row=0;row<centerScreen.rows-20;row=row+20)//-50,+10
 {
     for(col=0;col<centerScreen.cols-20;col=col+20)

     {
         Rect rect1(col,row,20,20);//20
         Mat temp=centerScreen(rect1);

         m = mean(temp)[0];



      minMaxIdx(temp,minp,maxp);
     // diff=maxv-minv;
      diff=maxv-m;
      if(diff>13){  //    diff>7
                 Num_baiyin++;

                   Mat th2;

                   threshold(temp, th2,m+13, 255, CV_THRESH_BINARY);//m+7

                   th2.copyTo(M(rect1));


               }




      }


     }

 for(int rows=0;rows<row;rows=rows+20)
 {
     Rect rect_x(col,rows,centerScreen.cols-col,20);
    // Rect rect1(col,row,20,20);//20
     Mat temp=centerScreen(rect_x);

     m = mean(temp)[0];



  minMaxIdx(temp,minp,maxp);
 // diff=maxv-minv;
  diff=maxv-m;
  if(diff>13){  //    diff>7
             Num_baiyin++;

               Mat th2;

               threshold(temp, th2,m+13, 255, CV_THRESH_BINARY);//m+7

               th2.copyTo(M(rect_x));


           };
 }
 for(int cols=0;cols<col;cols=cols+20)
 {
     Rect rect_y(cols,row,20,centerScreen.rows-row);
    // Mat temp_y=minimumCircumscribedRectangleImg_gray(rect_y);
     Mat temp=centerScreen(rect_y);

     m = mean(temp)[0];



  minMaxIdx(temp,minp,maxp);
 // diff=maxv-minv;
  diff=maxv-m;
  if(diff>13){  //    diff>7
             Num_baiyin++;

               Mat th2;

               threshold(temp, th2,m+13, 255, CV_THRESH_BINARY);//m+7

               th2.copyTo(M(rect_y));


           };
 }
 if(centerScreen.rows-row>0 and centerScreen.cols-col>0)
 {
     Rect rect_yx(col,row,centerScreen.cols-col,centerScreen.rows-row);
     Mat temp=centerScreen(rect_yx);

     m = mean(temp)[0];



  minMaxIdx(temp,minp,maxp);
 // diff=maxv-minv;
  diff=maxv-m;
  if(diff>13){  //    diff>7
             Num_baiyin++;

               Mat th2;

               threshold(temp, th2,m+13, 255, CV_THRESH_BINARY);//m+7

               th2.copyTo(M(rect_yx));


           };
 }
 vector<vector<Point>> contours_baiyin;
 findContours(M, contours_baiyin,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//画矩形轮廓
 int Num_of_point_baiyin=0;
 vector<Rect> boundRect_baiyin(contours_baiyin.size());
 for(vector<int>::size_type i = 0; i < contours_baiyin.size(); i++)
     {
       double area_baiyin = contourArea(contours_baiyin[i]);
       //cout<<area0<<endl;
       if (area_baiyin>50 and area_baiyin<500)//Black_Point_Lower_limit_area......Black_Point_Upper_limit_area\100~500

       {  //  cout<<area0<<endl;
           Num_of_point_baiyin=Num_of_point_baiyin+1;
           boundRect_baiyin[i] = boundingRect(Mat(contours_baiyin[i]));
           int X_1_yiwu=boundRect_baiyin[i].tl().x-20;
           int Y_1_yiwu=boundRect_baiyin[i].tl().y-20;
           int X_2_yiwu=boundRect_baiyin[i].br().x+20;
           int Y_2_yiwu=boundRect_baiyin[i].br().y+20;
           CvPoint top_lef_yiwu = cvPoint(X_1_yiwu, Y_1_yiwu);
           CvPoint bottom_right_yiwu = cvPoint(X_2_yiwu, Y_2_yiwu);
           rectangle(G, top_lef_yiwu, bottom_right_yiwu, Scalar(0,0,255), 5, 8, 0);

          }
     }
 //namedWindow("白印",0);
 //imshow("白印",M);
 if(Num_baiyin>0&&Num_of_point_baiyin>0){
              causeColor="白印";
              imwrite(save_name,M);

             return  true;
 }

}














//    //77777777777777777777白印检测7777777777777777777
//   // unevenLightCompensate(Gabor_img,Baiyin_Brightness_correction);
//    //medianBlur(Gabor_img,Gabor_img,3);
//    blur(Gabor_img,Gabor_img,Size(Baiyin_Blur_kernel_size,Baiyin_Blur_kernel_size));
//   // Mat centerScreen=interceptScreenWithoutOutside_new(Gabor_img,Baiyin_LEFTRIGHTEDGECOUNT);

//    Mat centerScreen(Gabor_img, Rect(30, 30, Gabor_img.cols - 30 * 2, Gabor_img.rows - 30*2));

//    Mat result=Ployfit_Row(centerScreen,Baiyin_ploy_n,false,Baiyin_ploy_percen);
//    Mat structure_element = getStructuringElement(MORPH_RECT, Size(Baiyin_erode_kernel_size,Baiyin_erode_kernel_size));
//    erode(result, result, structure_element);
//    Mat result2=interceptScreenWithoutOutside_new(result,50);//腐蚀
//    vector<vector<Point>> contours;
//    findContours(result2, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//  //画矩形轮廓
//    int Num_of_point=0;
//    vector<Rect> boundRect(contours.size());
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//        {
//          double area0 = contourArea(contours[i]);
//          //cout<<area0<<endl;
//          if (area0>Baiyin_Lower_limit_area and area0<Baiyin_Upper_limit_area)
//          {   Num_of_point=Num_of_point+1;
//              boundRect[i] = boundingRect(Mat(contours[i]));
//              int X_1=boundRect[i].tl().x-20;
//              int Y_1=boundRect[i].tl().y-20;
//              int X_2=boundRect[i].br().x+20;
//              int Y_2=boundRect[i].br().y+20;
//              CvPoint top_lef = cvPoint(X_1, Y_1);
//              CvPoint bottom_right = cvPoint(X_2, Y_2);
//              rectangle(result2, top_lef, bottom_right, Scalar(255,255,255), 5, 8, 0);

//             }
//        }
//    if(Num_of_point>0)
//    {
//        causeColor="白印缺陷";
//        imwrite(save_name,result2);
//       // cout<<"存在"<<Num_of_point<<"个白印缺陷"<<endl;
//        return true;
//    }
//77777777777777777777777777777777777777777777777777777777777
//        namedWindow("img_masked", 0);
//        imshow("img_masked",result2);
//        waitKey(0);
//        destroyAllWindows();

//     namedWindow("img_masked2", 0);
//     imshow("img_masked2",YIWU_Gabor);
//     waitKey(0);
//     destroyAllWindows();


    return  false;
}



//7777777777777777777777777777777777777777777
bool whiteDetection(string name)
{
    //ImgName=name;
    ImgName=name.substr((name.find_last_of("/"))!=name.npos?name.find_last_of("/"):name.find_last_of("\\")+1);
    //ImgName=name.substr(name.find_last_of("\\")+1);

    Mat minimumCircumscribedRectangle=interceptScreen(name,0);
    //namedWindow("minimumCircumscribedRectangle",0);
    //imshow("minimumCircumscribedRectangle",minimumCircumscribedRectangle);

    int height = minimumCircumscribedRectangle.rows;
    int width = minimumCircumscribedRectangle.cols;
    cout << "面积" << height*width<<endl;
    if (height*width<HEGEAREA)
    {   causeColor="屏幕面积";
        return true;}

    Mat centerScreen=interceptScreenWithoutOutside(minimumCircumscribedRectangle);
    //namedWindow("centerScreen",0);
    //imshow("centerScreen",centerScreen);

    //Mat centerScreen2=RemoveLittleHoles(centerScreen);
    cout<<centerScreen.channels();
    bool centerResult=whiteCenterScreenDetection(centerScreen);
    if(centerResult)
    {   causeColor="白色屏幕中心区域";
        return true;}
//    bool centerBrightResult=whiteCenterScreenBrightDetection(centerScreen);
//    if(centerBrightResult)
//    {   causeColor="白色屏幕中心区域";
//        return true;}
//    bool edgeResult=whiteEdgeScreenDetection(minimumCircumscribedRectangle);
//    if(edgeResult)
//    {   causeColor="白色边缘";
//        return true;}

//    bool baiyinResult=detectionBaiYin(centerScreen);
//    if(baiyinResult)
//    {   causeColor="白印";
//        return true;}

    return  false;
}

bool whiteDetectionBasedScreen(string name)
{
    //ImgName=name;
    ImgName=name.substr(name.find_last_of("\\")+1);

    Mat whiteImg=imread(name,0);
    Mat minimumCircumscribedRectangle=whiteImg(rect);

    //namedWindow("minimumCircumscribedRectangle",0);
    //imshow("minimumCircumscribedRectangle",minimumCircumscribedRectangle);

    int height = minimumCircumscribedRectangle.rows;
    int width = minimumCircumscribedRectangle.cols;
    cout << "面积" << height*width<<endl;
    if (height*width<HEGEAREA)
    {   causeColor="屏幕面积";
        return true;}

    Mat centerScreen=interceptScreenWithoutOutside(minimumCircumscribedRectangle);
    //namedWindow("centerScreen",0);
    //imshow("centerScreen",centerScreen);

    //Mat centerScreen2=RemoveLittleHoles(centerScreen);
    cout<<centerScreen.channels();
    bool centerResult=whiteCenterScreenDetection(centerScreen);
    if(centerResult)
    {   causeColor="白色屏幕中心区域";
        return true;}

//    bool edgeResult=whiteEdgeScreenDetection(minimumCircumscribedRectangle);
//    if(edgeResult)
//    {   causeColor="白色边缘";
//        return true;}

//    bool baiyinResult=detectionBaiYin(centerScreen);
//    if(baiyinResult)
//    {   causeColor="白印";
//        return true;}

    return  false;
}

/*=========================================================
* 函 数 名: testImageRectification
* 功能描述: 透视变换，将侧视图转成主视图。直接download的
=========================================================*/

Mat warpPerspective( Mat image)

{
    Mat dst;
    cvtColor(image,dst,CV_BGR2GRAY);
    Mat Binarization_gray;
    threshold(dst, Binarization_gray,100, 255, CV_THRESH_BINARY);
    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(15,15));
    Mat  erode1;
    erode(Binarization_gray, erode1, structure_element2);

    Mat structure_element = getStructuringElement(MORPH_RECT, Size(19,19)); //设置膨胀/腐蚀的核为矩形，大小为3*3
    Mat dilate1;
    dilate(erode1, dilate1, structure_element);
    vector<vector<Point>> contours;
    findContours( dilate1, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    cv::Mat perspective;
    for(int i = 0; i < contours.size(); i++)
        {
        vector<cv::Point> approxCurve;
        double epsilon = 0.1*arcLength(contours[i], true);
        approxPolyDP(contours[i], approxCurve, epsilon, true);
//        vector<vector<cv::Point>> poolApproxCurve;
//        poolApproxCurve.push_back(approxCurve);





    cv::Point2f src_points[] = {

         approxCurve[0],

         approxCurve[1],

         approxCurve[2],
         approxCurve[3]

        //cv::Point2f(615, 125)
                                };



    cv::Point2f dst_points[] = {

        cv::Point2f(0, 0),

        cv::Point2f(0, 1400),

        cv::Point2f(3000, 1400),

        cv::Point2f(3000, 0) };

cv::Mat M = cv::getPerspectiveTransform(src_points, dst_points);



cv::warpPerspective(image, perspective, M, cv::Size(3000, 1400), cv::INTER_LINEAR);






}
return perspective;
}




Mat testImageRectification(cv::Mat &image_original)
{
    //CV_SHOW(image_original); // CV_SHOW是cv::imshow的一个自定义宏，忽略即可
    namedWindow("image_original", 0);
    imshow("image_original", image_original);
    cv::Mat &&image = image_original.clone();

    cv::Mat image_gray;
    cv::cvtColor(image, image_gray, cv::COLOR_BGR2GRAY);
    //cv::threshold(image_gray, image_gray, g_threshVal, g_threshMax, cv::THRESH_BINARY);
    cv::threshold(image_gray, image_gray, 15, 250, cv::THRESH_BINARY);

    std::vector< std::vector<cv::Point> > contours_list;
    {
        std::vector<cv::Vec4i> hierarchy;
        // Since opencv 3.2 source image is not modified by this function
        cv::findContours(image_gray, contours_list, hierarchy,
            cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_NONE);
    }
    cv::Mat warp;
    for (uint64 index = 0; index < contours_list.size(); ++index) {
        cv::RotatedRect &&rect = cv::minAreaRect(contours_list[index]);
        if (rect.size.area() > 4000000) {
            if (rect.angle != 0.) {
                // 此处可通过cv::warpAffine进行旋转矫正，本例不需要
            } //if

            cv::Mat &mask = image_gray;
            cv::drawContours(mask, contours_list, static_cast<int>(index), cv::Scalar(255), cv::FILLED);

            cv::Mat extracted(image_gray.rows, image_gray.cols, CV_8UC1, cv::Scalar(0));
            image.copyTo(extracted, mask);
            //CV_SHOW(extracted);
            namedWindow("extracted", 0);
            imshow("extracted", extracted);
            std::vector<cv::Point2f> poly;
            cv::approxPolyDP(contours_list[index], poly, 30, true); // 多边形逼近，精度(即最小边长)设为30是为了得到4个角点
            cv::Point2f pts_src[] =
            { // 此处顺序调整是为了和后面配对，仅作为示例
                poly[1],
                poly[0],
                poly[3],
                poly[2]
            };

            cv::Rect &&r = rect.boundingRect(); // 注意坐标可能超出图像范围
            cv::Point2f pts_dst[] = {
                cv::Point(r.x, r.y),
                cv::Point(r.x + r.width, r.y),
                cv::Point(r.x + r.width, r.y + r.height),
                cv::Point(r.x, r.y + r.height)
            };
            cv::Mat &&M = cv::getPerspectiveTransform(pts_dst, pts_src); // 我这里交换了输入，因为后面指定了cv::WARP_INVERSE_MAP，你可以试试不交换的效果是什么

             cv::warpPerspective(image, warp, M, image.size(), cv::INTER_LINEAR + cv::WARP_INVERSE_MAP, cv::BORDER_REPLICATE);
            //CV_SHOW(warp);
            namedWindow("warp", 0);
            imshow("warp", warp);
        } //if
    }
    return warp;
}


/*=========================================================
* 函 数 名: slantDetection
* 功能描述: 倾斜图像检测
=========================================================*/

bool slantDetection(string name)
{
    Mat slantImg = imread(name,0);
    Mat slantCorrectionImg = testImageRectification(slantImg);
    Mat centerScreen = interceptScreen(slantCorrectionImg);
    namedWindow("中心屏幕", 0);
    imshow("中心屏幕", centerScreen);
    bool result =true;// detectionBaiYin(centerScreen);
    if (result)
        return true;
    return false;
}


/*=========================================================
* 函 数 名: redCenterScreenDetection
* 功能描述: 红色屏幕区域检测。
=========================================================*/

bool redCenterScreenDetection(Mat centerScreen)
{

    unevenLightCompensate(centerScreen);
   // namedWindow("centerScreen",0);
   // imshow("centerScreen",centerScreen);

    Mat centerScreenGabor = Gaborfilter(centerScreen, 3, 500, 83);
    //imwrite("G:\\MVS\\Data\\new_batch1\\centerScreenGabor.bmp",centerScreenGabor);
   // namedWindow("gabor",0);
   // imshow("gabor", centerScreenGabor);

    Mat Image4;
    medianBlur(centerScreenGabor, Image4, 3);   //中值滤波

    CvScalar cs;
    IplImage temp = IplImage(Image4);
    cs = cvAvg(&temp);
    Mat centerScreenGaborUpperLimit, centerScreenGaborLowerLimit;

    int upperLimitTh = cs.val[0] + REDUPPERLIMITERROR;
    threshold(centerScreenGabor, centerScreenGaborUpperLimit, upperLimitTh, 255, CV_THRESH_BINARY);
    int lowerLimitTh = cs.val[0] - REDLOWERLIMITERROR;
    threshold(centerScreenGabor, centerScreenGaborLowerLimit, lowerLimitTh, 255, CV_THRESH_BINARY_INV);

    //imshow("up", centerScreenGaborUpperLimit);
    //imshow("down", centerScreenGaborLowerLimit);
    Mat centerScreenGaborLimit = centerScreenGaborLowerLimit + centerScreenGaborUpperLimit;

    //namedWindow("灰度平均灰度值",0);
    //imshow("灰度平均灰度值",centerScreenGaborLimit);

    Mat centerScreenGaborLimit1 = centerScreenGaborLimit(Range(0, centerScreenGaborLimit.rows), Range(10, centerScreenGaborLimit.cols));
   // namedWindow("centerScreenGaborLimit1",0);
    //imshow("centerScreenGaborLimit1",centerScreenGaborLimit1);
    Mat Img222 = RemoveLittleHoles(centerScreenGaborLimit1);
   // namedWindow("去除小连通域", 0);
   // imshow("去除小连通域", Img222);

    //Img_Result = Img222;

    bool centerScreenGaborLimitResult = liantongyuLable(Img222, REDUPPERLOWERLIMITAREA);
    if (centerScreenGaborLimitResult)
        return true;
    else
    {
        return false;
    }

}

/*=========================================================
* 函 数 名: redDetection
* 功能描述: 红色底色检测。
=========================================================*/

bool redDetection(string name)
{
    ImgName=name;
    Mat minimumCircumscribedRectangle=interceptScreen(name,0);
    //namedWindow("minimumCircumscribedRectangle",0);
    //imshow("minimumCircumscribedRectangle",minimumCircumscribedRectangle);

    int height = minimumCircumscribedRectangle.rows;
    int width = minimumCircumscribedRectangle.cols;
    cout << "面积" << height*width<<endl;
    if (height*width<HEGEAREA)
    {   causeColor="屏幕面积";
        return true;}

    Mat centerScreen=interceptScreenWithoutOutside(minimumCircumscribedRectangle);
    //namedWindow("centerScreen",0);
    //imshow("centerScreen",centerScreen);

    //Mat centerScreen2=RemoveLittleHoles(centerScreen);

    bool centerResult=redCenterScreenDetection(centerScreen);
    if(centerResult)
    {   causeColor="红色屏幕中心区域";
        return true;}

    return  false;
}


/*=========================================================
* 函 数 名: blueCenterScreenDetection
* 功能描述: 蓝色屏幕区域检测。
=========================================================*/

bool blueCenterScreenDetection(Mat centerScreen)
{

    unevenLightCompensate(centerScreen);
   // namedWindow("centerScreen",0);
   // imshow("centerScreen",centerScreen);

    Mat centerScreenGabor = Gaborfilter(centerScreen, 3, 500, 83);
    //imwrite("G:\\MVS\\Data\\new_batch1\\centerScreenGabor.bmp",centerScreenGabor);
   // namedWindow("gabor",0);
   // imshow("gabor", centerScreenGabor);

    Mat Image4;
    medianBlur(centerScreenGabor, Image4, 3);   //中值滤波

    CvScalar cs;
    IplImage temp = IplImage(Image4);
    cs = cvAvg(&temp);
    Mat centerScreenGaborUpperLimit, centerScreenGaborLowerLimit;

    int upperLimitTh = cs.val[0] + BLUEUPPERLIMITERROR;
    threshold(centerScreenGabor, centerScreenGaborUpperLimit, upperLimitTh, 255, CV_THRESH_BINARY);
    int lowerLimitTh = cs.val[0] - BLUELOWERLIMITERROR;
    threshold(centerScreenGabor, centerScreenGaborLowerLimit, lowerLimitTh, 255, CV_THRESH_BINARY_INV);

    //imshow("up", centerScreenGaborUpperLimit);
    //imshow("down", centerScreenGaborLowerLimit);
    Mat centerScreenGaborLimit = centerScreenGaborLowerLimit + centerScreenGaborUpperLimit;

    //namedWindow("灰度平均灰度值",0);
    //imshow("灰度平均灰度值",centerScreenGaborLimit);

    Mat centerScreenGaborLimit1 = centerScreenGaborLimit(Range(0, centerScreenGaborLimit.rows), Range(10, centerScreenGaborLimit.cols));
   // namedWindow("centerScreenGaborLimit1",0);
    //imshow("centerScreenGaborLimit1",centerScreenGaborLimit1);
    Mat Img222 = RemoveLittleHoles(centerScreenGaborLimit1);
   // namedWindow("去除小连通域", 0);
   // imshow("去除小连通域", Img222);

    //Img_Result = Img222;

    bool centerScreenGaborLimitResult = liantongyuLable(Img222, BLUEUPPERLOWERLIMITAREA);
    if (centerScreenGaborLimitResult)
        return true;
    else
    {
        return false;
    }

}


/*=========================================================
* 函 数 名: blueDetection
* 功能描述: 红色底色检测。
=========================================================*/

bool blueDetection(string name)
{
    ImgName=name;
    Mat minimumCircumscribedRectangle=interceptScreen(name,0);
    //namedWindow("minimumCircumscribedRectangle",0);
    //imshow("minimumCircumscribedRectangle",minimumCircumscribedRectangle);

    int height = minimumCircumscribedRectangle.rows;
    int width = minimumCircumscribedRectangle.cols;
    cout << "面积" << height*width<<endl;
    if (height*width<HEGEAREA)
    {   causeColor="屏幕面积";
        return true;}

    Mat centerScreen=interceptScreenWithoutOutside(minimumCircumscribedRectangle);
    //namedWindow("centerScreen",0);
    //imshow("centerScreen",centerScreen);

    //Mat centerScreen2=RemoveLittleHoles(centerScreen);

    bool centerResult=redCenterScreenDetection(centerScreen);
    if(centerResult)
    {   causeColor="蓝色屏幕中心区域";
        return true;}

    return  false;
}

/*=========================================================
* 函 数 名: greenCenterScreenDetection
* 功能描述: 绿色屏幕区域检测。
=========================================================*/

bool greenCenterScreenDetection(Mat centerScreen)
{

    unevenLightCompensate(centerScreen);
   // namedWindow("centerScreen",0);
   // imshow("centerScreen",centerScreen);

    Mat centerScreenGabor = Gaborfilter(centerScreen, 3, 500, 83);
    //imwrite("G:\\MVS\\Data\\new_batch1\\centerScreenGabor.bmp",centerScreenGabor);
   // namedWindow("gabor",0);
   // imshow("gabor", centerScreenGabor);

    Mat Image4;
    medianBlur(centerScreenGabor, Image4, 3);   //中值滤波

    CvScalar cs;
    IplImage temp = IplImage(Image4);
    cs = cvAvg(&temp);
    Mat centerScreenGaborUpperLimit, centerScreenGaborLowerLimit;

    int upperLimitTh = cs.val[0] + GREENUPPERLIMITERROR;
    threshold(centerScreenGabor, centerScreenGaborUpperLimit, upperLimitTh, 255, CV_THRESH_BINARY);
    int lowerLimitTh = cs.val[0] - GREENLOWERLIMITERROR;
    threshold(centerScreenGabor, centerScreenGaborLowerLimit, lowerLimitTh, 255, CV_THRESH_BINARY_INV);

    //imshow("up", centerScreenGaborUpperLimit);
    //imshow("down", centerScreenGaborLowerLimit);
    Mat centerScreenGaborLimit = centerScreenGaborLowerLimit + centerScreenGaborUpperLimit;

    //namedWindow("灰度平均灰度值",0);
    //imshow("灰度平均灰度值",centerScreenGaborLimit);

    Mat centerScreenGaborLimit1 = centerScreenGaborLimit(Range(0, centerScreenGaborLimit.rows), Range(10, centerScreenGaborLimit.cols));
   // namedWindow("centerScreenGaborLimit1",0);
    //imshow("centerScreenGaborLimit1",centerScreenGaborLimit1);
    Mat Img222 = RemoveLittleHoles(centerScreenGaborLimit1);
   // namedWindow("去除小连通域", 0);
   // imshow("去除小连通域", Img222);

    //Img_Result = Img222;

    bool centerScreenGaborLimitResult = liantongyuLable(Img222, GREENUPPERLOWERLIMITAREA);
    if (centerScreenGaborLimitResult)
        return true;
    else
    {
        return false;
    }

}

/*=========================================================
* 函 数 名: greenDetection
* 功能描述: 红色底色检测。
=========================================================*/

bool greenDetection(string name)
{
    ImgName=name;
    Mat minimumCircumscribedRectangle=interceptScreen(name,0);
    //namedWindow("minimumCircumscribedRectangle",0);
    //imshow("minimumCircumscribedRectangle",minimumCircumscribedRectangle);

    int height = minimumCircumscribedRectangle.rows;
    int width = minimumCircumscribedRectangle.cols;
    cout << "面积" << height*width<<endl;
    if (height*width<HEGEAREA)
    {   causeColor="屏幕面积";
        return true;}

    Mat centerScreen=interceptScreenWithoutOutside(minimumCircumscribedRectangle);
    //namedWindow("centerScreen",0);
    //imshow("centerScreen",centerScreen);

    //Mat centerScreen2=RemoveLittleHoles(centerScreen);

    bool centerResult=redCenterScreenDetection(centerScreen);
    if(centerResult)
    {   causeColor="绿色屏幕中心区域";
        return true;}

    return  false;
}



/*=========================================================
* 函 数 名: 去印章
* 功能描述: 核函数的计算
=========================================================*/
Mat Remove_WaterMaker(Mat image_white){
    Mat img_gray;
    cvtColor(image_white,img_gray,CV_BGR2GRAY);
    Mat img2;
    img2 = img_gray.clone();
    Mat local;
    cout<<"hahaha"<<endl;
    Mat Gabor_img=New_Gabor(img_gray);
    cout<<"ajldajlfa"<<endl;
    adaptiveThreshold(Gabor_img, local, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 5);//3->5
    Mat element = getStructuringElement(MORPH_RECT, Size(17, 17)); //第一个参数MORPH_RECT表示矩形的卷积核，当然还可以选择椭圆形的、交叉型的
    Mat out;
    dilate(local, out, element);
    vector<Vec3f> circles;
    HoughCircles(local,circles, HOUGH_GRADIENT,1,10000,80,20,60,140 );
            Point center(cvRound(circles[0][0]), cvRound(circles[0][1]));
            int radius = cvRound(circles[0][2]);
//    for (size_t i = 0; i < circles.size(); i++)

//    {

//        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));

//        int radius = cvRound(circles[i][2]);

//        //检测圆中心

//        circle(img2, center, 3, Scalar(0, 255, 0), -1, 8, 0);

//        //检测圆轮廓

//        circle(img2, center, radius, Scalar(120, 120, 120), 3, 8, 0);

//    }


//cout<<cvRound(circles[0][0])<<endl;
//cout<<cvRound(circles[0][1])<<endl;
//cout<<cvRound(circles[0][2])<<endl;
  //  Mat image_screen(img2, Rect(cvRound(circles[0][0])-cvRound(circles[0][2]), cvRound(circles[0][1])-cvRound(circles[0][2]), cvRound(circles[0][0])+cvRound(circles[0][2]), cvRound(circles[0][1])+cvRound(circles[0][2])));
//Mat image_screen(img2, Rect(cvRound(circles[0][0])-cvRound(circles[0][2])-10,cvRound(circles[0][1])-cvRound(circles[0][2])-10+250,cvRound(circles[0][2])*2+20,cvRound(circles[0][2])*2+20));
//cout<<img2.size[0]<<endl;
//cout<<img2.size[1]<<endl;


//    image_screen.copyTo(img2(Rect(cvRound(circles[0][0])-cvRound(circles[0][2])-10,cvRound(circles[0][1])-cvRound(circles[0][2])-10,cvRound(circles[0][2])*2+20,cvRound(circles[0][2])*2+20)));
    Mat M( img2.size[0], img2.size[1], CV_8UC1, Scalar(0) );
    Mat out2;
    circle(M, center, radius+30, Scalar(255), -1, 8, 0);
     bitwise_and(out, M,out2, Mat());
     Mat inpainted;
     inpaint(img2, out2, inpainted, 15, INPAINT_NS);
    // Mat inpainted_gabor=New_Gabor(inpainted);
   // imwrite(save_name,inpainted);
     Mat imgRGB;
      cvtColor(inpainted, imgRGB, COLOR_GRAY2RGB);
    return  imgRGB;
}

/*=========================================================
* 函 数 名: 去印章
* 功能描述: 核函数的计算
=========================================================*/
//Rect roi_white(string name)//白色底色下提取黑色ROI
//{
//    Mat src_white = imread(name);
//    Mat img_gray;

//    cvtColor(src_white,img_gray,CV_BGR2GRAY);
////    Mat th1;
////    threshold(img_gray, th1,Edge_leakage_threshold, 255, CV_THRESH_BINARY);// BLACKOUTOFSCREENTHRESHOLD
//    Mat th2;
//    //adaptiveThreshold(img_gray, th2, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 21, 5);//21越大检测越灵敏，5越小越灵敏
//    threshold(img_gray, th2,60, 255, CV_THRESH_BINARY);//100->60
//   // namedWindow("haha",0);
//   // imshow("haha",th2);
//    vector<vector<Point>> contours;
//    findContours(th2, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//    int Num_of_point=0;
//    int Num_of_times=0;
//     int X_1=100,Y_1=100,X_2=100,Y_2=100;
//    vector<Rect> boundRect(contours.size());
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//        {
//        Num_of_times=Num_of_times+1;
//          double area0 = contourArea(contours[i]);
//          cout<<area0<<endl;
//          if (area0>2500000 and area0<4800000)
//          {   Num_of_point=Num_of_point+1;
//              boundRect[i] = boundingRect(Mat(contours[i]));
//              X_1=boundRect[i].tl().x;
//              Y_1=boundRect[i].tl().y;
//              X_2=boundRect[i].br().x;
//              Y_2=boundRect[i].br().y;

////              CvPoint top_lef = cvPoint(X_1, Y_1);
////              CvPoint bottom_right = cvPoint(X_2, Y_2);
////              rectangle(src_white, top_lef, bottom_right, Scalar(0,0,255), 5, 8, 0);

//             }
//        }
//if(X_2-X_1-20<=0||Y_2-Y_1-20<=0)
//{return  Rect(X_1+10,Y_1+10,X_2-X_1,Y_2-Y_1);}
//else {
//    return  Rect(X_1+10,Y_1+10,X_2-X_1-20,Y_2-Y_1-20);
//}
////

//}
Rect roi_white(Mat src_white)//白色底色下提取黑色ROI
{
    Mat img_gray;
    cvtColor(src_white,img_gray,CV_BGR2GRAY);
    Mat th1;
    threshold(img_gray, th1,50, 255, CV_THRESH_BINARY);//100->60
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(15,15));
    Mat  erode1;
    erode(th1, erode1, structure_element1);
    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(19,19));
    Mat dilate1;
    dilate(erode1, dilate1, structure_element2);
    Mat img_temp1,img_temp2,img_temp3,img_temp4;
    int X_1,Y_1,X_2,Y_2,w,h;
    vector<vector<Point>> contours;
    findContours(th1, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
        {
          double area = contourArea(contours[i]);

          if (area>0  and area<6100000 )
          {
              qDebug()<<area<<endl;
              boundRect[i] = boundingRect(Mat(contours[i]));
              X_1=boundRect[i].tl().x;
              Y_1=boundRect[i].tl().y;
              X_2=boundRect[i].br().x;
              Y_2=boundRect[i].br().y;
              w=boundRect[i].width;
              h=boundRect[i].height;
          }
    }

    return  Rect(X_1+5,Y_1+5,w-10,h-10);


}
Mat gamma_new(Mat src)      //伽马校正
{
    Mat temp;
    src.convertTo(temp, CV_32FC3, 1 / 255.0);
    cv::Mat temp1;
    cv::pow(temp,0.7,temp1);//0.7->0.2
    Mat dst;
   temp1.convertTo(dst, CV_8UC1, 255);
   return dst;

}
Mat Remove_WaterMaker1(Mat image_white)
{
    Mat img_gray;
    cvtColor(image_white,img_gray,CV_BGR2GRAY);
    Mat img2;
    img2 = img_gray.clone();
    Mat local;
    Mat Gabor_img=New_Gabor(img_gray);
    Mat result_opencv;
    medianBlur(Gabor_img,result_opencv,3);
    adaptiveThreshold(result_opencv, local, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 5);

    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5)); //第一个参数MORPH_RECT表示矩形的卷积核，当然还可以选择椭圆形的、交叉型的
    Mat out;
    dilate(local, out, element);
    vector<vector<Point>> contours_blackOutOfScr;
    findContours(out, contours_blackOutOfScr,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  //画矩形轮廓
    int Num_of_point_blackOutOfScr=0;
    vector<Rect> boundRect_blackOutOfScr(contours_blackOutOfScr.size());
    for(vector<int>::size_type i = 0; i < contours_blackOutOfScr.size(); i++)
        {
          double area0 = contourArea(contours_blackOutOfScr[i]);
          if (area0<1000)
          {
              Num_of_point_blackOutOfScr=Num_of_point_blackOutOfScr+1;
              boundRect_blackOutOfScr[i] = boundingRect(Mat(contours_blackOutOfScr[i]));
              int X_1=boundRect_blackOutOfScr[i].tl().x;
              int Y_1=boundRect_blackOutOfScr[i].tl().y;
              int X_2=boundRect_blackOutOfScr[i].br().x;
              int Y_2=boundRect_blackOutOfScr[i].br().y;
              out(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1))=uchar(0);

             }
        }
    Mat inpainted;
    inpaint(Gabor_img,  out, inpainted, 30, INPAINT_NS);
    Mat imgRGB;
    cvtColor(inpainted, imgRGB, COLOR_GRAY2RGB);
    return imgRGB;

}

bool video_capture(Rect m_select, double threshold)     //闪屏缺陷
{
    char file[64];
    Mat img;
    Scalar tempVal;
    vector<double> resultSet;
    for( int a = 1; a < 5; a = a + 1 )
    {
        sprintf(file,"D:\\shidi\\MSV_image\\Image_%d.bmp",a);
        img=imread(file);
        img=img(m_select);
        tempVal = cv::mean(img);
        resultSet.push_back(tempVal.val[0]);

    }
    double sum = std::accumulate(std::begin(resultSet), std::end(resultSet), 0.0);
    double mean =  sum / resultSet.size(); //均值
    double accum  = 0.0;
    std::for_each (std::begin(resultSet), std::end(resultSet), [&](const double d) {
        accum  += (d-mean)*(d-mean);

    });
    double stdev = (accum/(resultSet.size()-1)); //方差
    if(stdev>threshold)
    {   causeColor="闪屏缺陷";
        return true;
    }
    return  false;
}
bool whitecheck_loseline(Mat minimumCircumscribedRectangle,string save_name,int signalcheck[8])
{
//#######################缺线#############################
if(signalcheck[3]==1)
{
    Mat lose_line_new=minimumCircumscribedRectangle.clone();
    Mat lose_line_make_new=lose_line_new.clone();
    lose_line_new=New_Gabor(lose_line_new);
    lose_line_make_new=New_Gabor(lose_line_make_new);
    //namedWindow("lose",0);
    //imshow("lose",lose_line_new);
    vector<Mat> channel1s;
    Mat row_line_30=lose_line_new.rowRange(28,29);
    split(row_line_30,channel1s);
    Mat Bb=channel1s.at(0);
    Mat Gg=channel1s.at(1);
    Mat Rr=channel1s.at(2);
    double Bbmean=mean(Bb)[0];
    double Ggmean=mean(Gg)[0];
    double Rrmean=mean(Rr)[0];
    int Num_of_loseline_new=0;
    for(int row_line_new=30;row_line_new<lose_line_new.rows-30;row_line_new=row_line_new+2)
    {
        vector<Mat> channel2s;
        Mat row=lose_line_new.rowRange(row_line_new,row_line_new+1);
        split(row,channel2s);
        Mat Bb1=channel2s.at(0);
        Mat Gg1=channel2s.at(1);
        Mat Rr1=channel2s.at(2);
        double Bb1mean=mean(Bb1)[0];
        double Gg1mean=mean(Gg1)[0];
        double Rr1mean=mean(Rr1)[0];
        if(Bb1mean<Bbmean-5||Rr1mean<Rrmean-5||Gg1mean<Ggmean-5)
        {
            Num_of_loseline_new=Num_of_loseline_new+1;
            CvPoint top_left=cvPoint(0,row_line_new);
            CvPoint bottom_right=cvPoint(3000,row_line_new+10);
            rectangle(lose_line_make_new,top_left,bottom_right,Scalar(0,0,255),1,8,0);
        }
        Bbmean=Bb1mean;
        Ggmean=Gg1mean;
        Rrmean=Rr1mean;
    }
    if(Num_of_loseline_new>0)
          {
              causeColor="缺线";
              imwrite(save_name,lose_line_make_new);

              return true;
          }
}
 return false;
}

bool whitecheck_TaiYang(Mat minimumCircumscribedRectangle,string save_name,int signalcheck[8])
{
    if(signalcheck[6]==1)
    {
    Mat image_taiyang=minimumCircumscribedRectangle.clone();
    Mat hsv8;
    Mat outimage;
    cvtColor(image_taiyang,hsv8,COLOR_BGR2HSV);
    inRange(hsv8,Scalar(0,50,160),Scalar(102,80,220),outimage);
    vector<vector<Point>>contour;
    findContours(outimage,contour,CV_RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    int Num_of_point = 0;
    vector<Rect> boundRect(contour.size());
    for(vector<int>::size_type i =0;i<contour.size();i++)
    {
        double area = contourArea(contour[i]);
        if(area>300 and area <10000)
        {
            Num_of_point=Num_of_point+1;
            boundRect[i]=boundingRect(Mat(contour[i]));
            int x1=boundRect[i].tl().x;
            int y1=boundRect[i].tl().y;
            int x2=boundRect[i].br().x;
            int y2=boundRect[i].br().y;
            CvPoint top_left=CvPoint(x1,y1);
            CvPoint bottom_right=CvPoint(x2,y2);
            rectangle(image_taiyang,top_left,bottom_right,Scalar(0,0,255),5,8,0);
        }
    }
    if(Num_of_point>0)
    {
        causeColor="太阳斑";
      imwrite(save_name,image_taiyang);

        return true;
    }
    }
     return false;
}

//############异物检测##########
bool whitecheck_yiwu(Mat minimumCircumscribedRectangle,string save_name,int signalcheck[8],vector<Rect> shuiyinRect,vector<Rect> shuiyinRect1,vector<Rect> ceguangweizhi)
{
    Mat img_gray=minimumCircumscribedRectangle.clone();
    cvtColor(img_gray,img_gray,CV_BGR2GRAY);
    Mat Gabor_img=New_Gabor(img_gray);
    Mat YIWU_Gabor=Gabor_img.clone();


//########################异物#####################
if(signalcheck[4]==1)
{

         Mat yiwu;
           yiwu=New_Gabor(img_gray);
        //  cvtColor(minimumCircumscribedRectangle,yiwu,CV_BGR2GRAY);
          blur(yiwu,yiwu,Size(5,5));
         Mat centerScreen_yiwu(yiwu, Rect(0, 0, Gabor_img.cols - 0 * 2, Gabor_img.rows - 0*2));
         Mat centerScreen_copy_yiwu= centerScreen_yiwu.clone();
         double minv_yiwu = 0.0, maxv_yiwu = 0.0;
          double* minp_yiwu = &minv_yiwu;
          double* maxp_yiwu = &maxv_yiwu;
          double m_yiwu = 0;
          double diff_yiwu=0;
           vector<Mat> channels2_yiwu;
           Mat M_yiwu(centerScreen_yiwu.size[0], centerScreen_yiwu.size[1], CV_8UC1, Scalar(0) );




         for(int row=0;row<centerScreen_yiwu.rows-50;row=row+30)
         {
             for(int col=0;col<centerScreen_yiwu.cols-50;col=col+30)

             {
                 Rect rect1(col,row,50,50);
                 Mat temp_yiwu=centerScreen_yiwu(rect1);

                 m_yiwu = mean(temp_yiwu)[0];



              minMaxIdx(temp_yiwu,minp_yiwu,maxp_yiwu);
              diff_yiwu=maxv_yiwu-m_yiwu;


            Mat th2_yiwu;
            adaptiveThreshold(temp_yiwu, th2_yiwu, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, Black_Point_adthreshold_kernel_size, Black_Point_adthreshold_constant);//21越大检测越灵敏，5越小越灵敏


                          // threshold(temp_yiwu, th2_yiwu,m_yiwu+3, 255, CV_THRESH_BINARY);

             th2_yiwu.copyTo(M_yiwu(rect1));


              }


             }
         //namedWindow("mm",0);
         //imshow("mm",M_yiwu);
         vector<vector<Point>> contours_yiwu;
         findContours(M_yiwu, contours_yiwu,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);//CV_RETR_EXTERNAL
       //画矩形轮廓

         int Num_of_point_yiwu=0;
         vector<Rect> boundRect_yiwu(contours_yiwu.size());
         for(vector<int>::size_type i = 0; i < contours_yiwu.size(); i++)
             {

               double area_yiwu = contourArea(contours_yiwu[i]);
               //cout<<area0<<endl;
               if (area_yiwu>=4 and area_yiwu<500)//Black_Point_Lower_limit_area......Black_Point_Upper_limit_area\100~500

               {  //  cout<<area0<<endl;
                    bool result =true;
                   boundRect_yiwu[i] = boundingRect(Mat(contours_yiwu[i]));
                   for(int j =0;j<ceguangweizhi.size();j++)
                   {
                       if((boundRect_yiwu[i].tl().x>=ceguangweizhi[j].tl().x)&&(boundRect_yiwu[i].tl().y>=ceguangweizhi[j].tl().y)&&(boundRect_yiwu[i].br().x<=ceguangweizhi[j].br().x)&&(boundRect_yiwu[i].br().y<=ceguangweizhi[j].br().y))
                       {
                               result=false;
                   }
                   }
                   int X_1_yiwu=boundRect_yiwu[i].tl().x-20;
                   int Y_1_yiwu=boundRect_yiwu[i].tl().y-20;
                   int X_2_yiwu=boundRect_yiwu[i].br().x+20;
                   int Y_2_yiwu=boundRect_yiwu[i].br().y+20;
                   if(X_1_yiwu<0)
                   {
                       X_1_yiwu=0;
                   }
                   if(Y_1_yiwu<0)
                   {
                       Y_1_yiwu=0;
                   }
                   if(X_2_yiwu>minimumCircumscribedRectangle.cols)
                   {
                       X_2_yiwu=minimumCircumscribedRectangle.cols;
                   }
                   if(Y_2_yiwu>minimumCircumscribedRectangle.rows)
                   {
                       Y_2_yiwu=minimumCircumscribedRectangle.rows;
                   }
                   Rect quexian(X_1_yiwu,Y_1_yiwu,X_2_yiwu-X_1_yiwu,Y_2_yiwu-Y_1_yiwu);

                   for(int i=0;i<shuiyinRect.size();i++)
                   {
                       if((quexian.tl().x>=shuiyinRect[i].tl().x)&&(quexian.tl().y>=shuiyinRect[i].tl().y)&&(quexian.br().x<=shuiyinRect[i].br().x)&&(quexian.br().y<=shuiyinRect[i].br().y))
                       {
                           result=false;
                       }
                   }

                   for(int i=0;i<shuiyinRect1.size();i++)
                   {
                       if((quexian.tl().x>=shuiyinRect1[i].tl().x)&&(quexian.tl().y>=shuiyinRect1[i].tl().y)&&(quexian.br().x<=shuiyinRect1[i].br().x)&&(quexian.br().y<=shuiyinRect1[i].br().y))
                       {
                           result=false;
                       }
                   }
                   //Rect zuoshang(0,0,200,100);
                   //Rect zuoxia(0,minimumCircumscribedRectangle.rows-100,200,100);
                  // Rect youshang(minimumCircumscribedRectangle.cols-200,0,200,100);
                  // Rect youxia(minimumCircumscribedRectangle.cols-200,minimumCircumscribedRectangle.rows-100,200,100);
                   Rect zuoshang(0,0,minimumCircumscribedRectangle.cols,50);
                   Rect zuoxia(0,minimumCircumscribedRectangle.rows-50,minimumCircumscribedRectangle.cols,50);
                   Rect youshang(0,0,100,minimumCircumscribedRectangle.rows);
                   Rect youxia(minimumCircumscribedRectangle.cols-50,0,50,minimumCircumscribedRectangle.rows);
                   if(((quexian.tl().x>=zuoshang.tl().x)&&(quexian.tl().y>=zuoshang.tl().y)&&(quexian.br().x<=zuoshang.br().x)&&(quexian.br().y<=zuoshang.br().y))
                           ||((quexian.tl().x>=zuoxia.tl().x)&&(quexian.tl().y>=zuoxia.tl().y)&&(quexian.br().x<=zuoxia.br().x)&&(quexian.br().y<=zuoxia.br().y))
                           ||((quexian.tl().x>=youshang.tl().x)&&(quexian.tl().y>=youshang.tl().y)&&(quexian.br().x<=youshang.br().x)&&(quexian.br().y<=youshang.br().y))
                           ||((quexian.tl().x>=youxia.tl().x)&&(quexian.tl().y>=youxia.tl().y)&&(quexian.br().x<=youxia.br().x)&&(quexian.br().y<=youxia.br().y)))
                   {
                       result=false;
                   }
                   if(result==true)
                   {
                       Num_of_point_yiwu=Num_of_point_yiwu+1;
                       CvPoint top_lef_yiwu = cvPoint(X_1_yiwu, Y_1_yiwu);
                       CvPoint bottom_right_yiwu = cvPoint(X_2_yiwu, Y_2_yiwu);
                       rectangle(centerScreen_copy_yiwu, top_lef_yiwu, bottom_right_yiwu, Scalar(0,0,255), 5, 8, 0);
                   }



                  }
             }
        // namedWindow("yiwu",0);
        // imshow("yiwu",centerScreen_copy_yiwu);
         if(Num_of_point_yiwu>0)
         {
             causeColor="背光异物";
           imwrite(save_name,centerScreen_copy_yiwu);

             return true;
         }

}
}
Rect watermark_locationRed(Mat src)//提取红字
{

  Mat src_1 = src(Rect(40,40,src.cols-80,src.rows-80));
  Mat Hsv_watermark;
  Mat mask_1;
  cvtColor(src_1,Hsv_watermark, COLOR_BGR2HSV);
  inRange(Hsv_watermark, Scalar(106, 43, 100), Scalar(180,105, 255),mask_1);//红印章：Scalar(105, 80, 100), Scalar(180,100, 255)、大红字：Scalar(0, 80, 50), Scalar(10,255, 220)
  Mat img_gray;
  cvtColor(src_1,img_gray,CV_BGR2GRAY);
  Mat mask_2;
  adaptiveThreshold( img_gray,mask_2, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 3);
  Mat Candidate;
  bitwise_and(mask_1,mask_2,Candidate);
  //namedWindow("cna",0);
  //imshow("cna",Candidate);
  medianBlur(Candidate,Candidate,11);
  Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(5,5));
  Mat  erode1;
  erode(Candidate, erode1, structure_element1);
  //namedWindow("dilare1",0);
  //imshow("dilare1",erode1);
  Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(30,30));
  Mat dilate1;
  dilate(erode1, dilate1, structure_element2);

  Mat Candidate_2 = dilate1(Rect(40,40,dilate1.cols-80,dilate1.rows-80));
  int X_1=0;//矩形左上角X坐标值
  int Y_1=0;//矩形左上角Y坐标值
  int X_2=0;//矩形右下角X坐标值
  int Y_2=0;//矩形右下角Y坐标值
  int W_1=0;
  int H_1=0;
  double area_max=0;
   vector<vector<Point>> contours;
   findContours( Candidate_2, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
   vector<Rect> boundRect(contours.size());
   for(vector<int>::size_type i = 0; i < contours.size(); i++)
       {
         double area = contourArea(contours[i]);

         if (area>3000 and area<100000)//10000
         {
             if(area>area_max)

           {  area_max=area;
              boundRect[i] = boundingRect(Mat(contours[i]));
              X_1=boundRect[i].tl().x+70;//矩形左上角X坐标值
              Y_1=boundRect[i].tl().y+70;//矩形左上角Y坐标值
              X_2=boundRect[i].br().x+100;//矩形右下角X坐标值
              Y_2=boundRect[i].br().y+100;//矩形右下角Y坐标值
              W_1=boundRect[i].width;
              H_1=boundRect[i].height;

             boundRect[i].x=X_1;
             boundRect[i].y=Y_1;
             boundRect[i].width=X_2-X_1;
             boundRect[i].height=Y_2-Y_1;
             CvPoint top_lef4 = cvPoint(X_1, Y_1);
             CvPoint bottom_right4 = cvPoint(X_2, Y_2);
           }//  rectangle(src , top_lef4, bottom_right4, Scalar(0,0,255), 5, 8, 0);

          }
       }

   return Rect (X_1,Y_1,W_1,H_1) ;

}

bool yiwu(string save_name,Mat white_yiwu ,Mat ceguang)

{
    Rect rect1= watermark_locationRed(white_yiwu);
    int rect_x=rect1.x;
    int rect_y=rect1.y;
    int rect_w=rect1.width;
    int rect_h=rect1.height;

    bool result=false;
    Mat gray_ceguang;
    Mat ceguang1;
    blur(ceguang,ceguang1,Size(2,2));

    cvtColor(ceguang1,gray_ceguang,CV_BGR2GRAY);

    Mat HSV;
    cvtColor(white_yiwu,HSV,CV_BGR2HSV);
    Mat mask1;
    inRange(HSV, Scalar(100, 43,43), Scalar(180,255, 180),mask1);
    Mat structure_element3 = getStructuringElement(MORPH_RECT, Size(15,15));
    Mat mask11;
    dilate(mask1, mask11, structure_element3);

    Mat mask;
    inRange(HSV, Scalar(100, 106, 160), Scalar(120,255, 255),mask);
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(5,5));
    Mat  erode1;
    erode(mask, erode1, structure_element1);
    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(11,11));
    Mat dilate1;
    dilate(erode1, dilate1, structure_element2);
    Mat img_gray;
    cvtColor(white_yiwu,img_gray,CV_BGR2GRAY);
    Mat th1;
    adaptiveThreshold(img_gray,th1, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 31, 7);

    vector<vector<Point>> contours;
    findContours( th1, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
        {
          double area = contourArea(contours[i]);
          if (area>0 and area<200)
          {
              boundRect[i] = boundingRect(Mat(contours[i]));
              float w=boundRect[i].width;
              float h=boundRect[i].height;
              int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
              int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
              int X_2=boundRect[i].br().x;//矩形右下角X坐标值
              int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
              int x_point=X_1+round(w/2);
              int y_point=Y_1+round(h/2);

       if(true)
//X_1-15>0 and Y_1-15>0 and X_1+w+15<gray_ceguang.size[1] and Y_1+15+h<gray_ceguang.size[0]
       {
           int x_lt=x_point-10;
           if(x_lt<0)
           {
               x_lt=0;
           }
           int y_lt=y_point-10;
           if(y_lt<0)
           {
               y_lt=0;
           }
           int x_rt=x_point+20+w;
           if(x_rt>gray_ceguang.size[1]-1)
           {
               x_rt=gray_ceguang.size[1]-1;
           }
          int y_rt=y_point+20+h;
           if(y_rt>gray_ceguang.size[0]-1)
           {
               y_rt=gray_ceguang.size[0]-1;
           }
         //circle(white_yiwu, Point(x_point,y_point), 2, Scalar(0, 0, 255));

           Mat temp_ceguang=gray_ceguang(Rect(x_lt,y_lt,x_rt-x_lt,y_rt-y_lt));
           double mean_ceguang = mean(temp_ceguang)[0];
//              Mat temp_dilate1=dilate1(Rect(x_point,y_point,int(w),int(h)));
//              double mean_dilate1 = mean(temp_dilate1)[0];

//              Mat temp_mask1=mask11(Rect(x_point,y_point,int(w),int(h)));
//              double mean_mask1 = mean(temp_mask1)[0];
             bool red_mark=true;
             if(x_point>rect_x and x_point<rect_x+rect_w  and y_point>rect_y and y_point<rect_y+rect_h)
             {
                 red_mark=false;
             }
              if((red_mark) and abs(mean_ceguang-gray_ceguang.at<uchar>(y_point,x_point))<2 and((X_2-X_1)*(Y_2-Y_1)>25 )and (max(w/h,h/w)<3))//and((X_2-X_1)*(Y_2-Y_1)>30 ) and (max(w/h,h/w)<1.4) and (mean_dilate1<200)and (mean_mask1<100)
              {
                 int  y_qidian=y_point-25;
                 int  y_zhongdian=y_point+25;
                 int  x_qidian=x_point-25;
                 int  x_zhongdian=x_point+25;
                 if(y_qidian<0)
                       { y_qidian=0;}
                 if(x_qidian<0)
                       { x_qidian=0;}
                 if(y_zhongdian>img_gray.size[0])
                       {y_zhongdian=img_gray.size[0];}
                 if(x_zhongdian>img_gray.size[1])
                       {x_zhongdian=img_gray.size[1];}
                  Mat IMG_TEMP_SHIAIXUAN=img_gray(Rect(x_qidian,y_qidian,x_zhongdian-x_qidian,y_zhongdian-y_qidian));
                  Mat th7;
                  adaptiveThreshold(IMG_TEMP_SHIAIXUAN,th7, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 47, 13);
                  vector<vector<Point>> contours7;
                  findContours( th7, contours7,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
                  if(contours7.size()==1)
                  {
                      vector<Rect> boundRect(contours7.size());
                      for(vector<int>::size_type i = 0; i < contours7.size(); i++)
                          {
                            double area7 = contourArea(contours7[i]);
                            if (area7>2 and area7<200)
                            {
                                CvPoint top_lef4 = cvPoint(X_1-10, Y_1-10);
                                CvPoint bottom_right4 = cvPoint(X_2+20, Y_2+20);
                                rectangle(white_yiwu , top_lef4, bottom_right4, Scalar(255,0,5), 5, 8, 0);
                                result=true;
                            }

                           }
                    causeColor="背光异物";
                    //imwrite(save_name,white_yiwu);
                  }


              }

      }


           }
        }
if(true){
    Mat image_l;
    image_l=img_gray(Rect(0,0,20,img_gray.size[0]));
    adaptiveThreshold(image_l,th1, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 31, 21);
    findContours( th1, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
        {
          double area = contourArea(contours[i]);
          if (area>10 and area<100)
          {
              boundRect[i] = boundingRect(Mat(contours[i]));
              float w=boundRect[i].width;
              float h=boundRect[i].height;
              int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
              int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
              int X_2=boundRect[i].br().x;//矩形右下角X坐标值
              int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
              int x_point=X_1+round(w/2);
              int y_point=Y_1+round(h/2);

              int x_lt=X_1-2;
              if(x_lt<0)
              {
                  x_lt=0;
              }
              int x_rt=X_2+2;
              if(x_rt>img_gray.size[1])
              {
                  x_rt=img_gray.size[1];
              }
              int y_u=Y_1-2;
              if(y_u<0)
              {
                  y_u=0;
              }

             int y_b=Y_2+2;
              if(y_b>img_gray.size[0])
              {
                  y_b=img_gray.size[0];
              }

              if((image_l.at<uchar>(int(Y_1+h),X_1)>30)and(image_l.at<uchar>(Y_1,X_1)>30)and(abs(image_l.at<uchar>(y_point,x_point)-image_l.at<uchar>(y_b,x_point))>10 or abs(image_l.at<uchar>(y_point,x_point)-image_l.at<uchar>(y_u,x_point))>10)and (max(w/h,h/w)<3))
              {
                  CvPoint top_lef4 = cvPoint(X_1, Y_1);
                  CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                  rectangle(white_yiwu , top_lef4, bottom_right4, Scalar(255,0,5), 5, 8, 0);
                  result=true;
                  causeColor="背光异物";
                  //imwrite(save_name,white_yiwu);
              }
          }


         }

    image_l=img_gray(Rect(0,0,img_gray.size[1],20));
    adaptiveThreshold(image_l,th1, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 31, 21);
    findContours( th1, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
        {
          double area = contourArea(contours[i]);
          if (area>10 and area<100)
          {
              boundRect[i] = boundingRect(Mat(contours[i]));
              float w=boundRect[i].width;
              float h=boundRect[i].height;
              int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
              int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
              int X_2=boundRect[i].br().x;//矩形右下角X坐标值
              int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
              int x_point=X_1+round(w/2);
              int y_point=Y_1+round(h/2);

              int x_lt=X_1-2;
              if(x_lt<0)
              {
                  x_lt=0;
              }
              int x_rt=X_2+2;
              if(x_rt>img_gray.size[1])
              {
                  x_rt=img_gray.size[1];
              }
              int y_u=Y_1-2;
              if(y_u<0)
              {
                  y_u=0;
              }

             int y_b=Y_2+2;
              if(y_b>img_gray.size[0])
              {
                  y_b=img_gray.size[0];
              }

              if((image_l.at<uchar>(Y_1,int(X_1+w))>30)and(image_l.at<uchar>(Y_1,X_1)>30)and(abs(image_l.at<uchar>(y_point,x_lt)-image_l.at<uchar>(y_point,x_point))>10 or abs(image_l.at<uchar>(y_point,x_rt)-image_l.at<uchar>(y_point,x_point))>10)and (max(w/h,h/w)<3))
              {
                  CvPoint top_lef4 = cvPoint(X_1, Y_1);
                  CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                  rectangle(white_yiwu , top_lef4, bottom_right4, Scalar(255,0,5), 5, 8, 0);
                  result=true;
                  causeColor="背光异物";
                 // imwrite(save_name,white_yiwu);
              }
          }


         }

    image_l=img_gray(Rect(0,img_gray.size[0]-20,img_gray.size[1],20));
    adaptiveThreshold(image_l,th1, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 31, 21);
    findContours( th1, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
        {
          double area = contourArea(contours[i]);
          if (area>10 and area<100)
          {
              boundRect[i] = boundingRect(Mat(contours[i]));
              float w=boundRect[i].width;
              float h=boundRect[i].height;
              int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
              int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
              int X_2=boundRect[i].br().x;//矩形右下角X坐标值
              int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
              int x_point=X_1+round(w/2);
              int y_point=Y_1+round(h/2);

              if(X_1<0)
              {
                  X_1=0;
              }
              if(Y_1<0)
              {
                  Y_1=0;
              }
              if(X_2>image_l.size[1]-1)
              {
                  X_2=image_l.size[1]-1;
              }
              if(Y_2>image_l.size[0]-1)
              {
                  Y_2=image_l.size[0]-1;
              }



              int x_lt=X_1-2;
              if(x_lt<0)
              {
                  x_lt=0;
              }
              int x_rt=X_2+2;
              if(x_rt>img_gray.size[1])
              {
                  x_rt=img_gray.size[1];
              }
              int y_u=Y_1-2;
              if(y_u<0)
              {
                  y_u=0;
              }

             int y_b=Y_2+2;
              if(y_b>img_gray.size[0])
              {
                  y_b=img_gray.size[0];
              }


              int xlt=x_point-10;
              if(xlt<0)
              {
                  xlt=0;
              }
              int ylt=y_point-10+gray_ceguang.size[0]-20;
              if(ylt<0)
              {
                  ylt=0;
              }
              int xrt=x_point+20+w;
              if(xrt>image_l.size[1]-1)
              {
                  xrt=image_l.size[1]-1;
              }
             int yrt=y_point+20+h+gray_ceguang.size[0]-20;
              if(yrt>gray_ceguang.size[0]-1)
              {
                  yrt=gray_ceguang.size[0]-1;
              }

              int X_b_c_point=x_point;
              int Y_b_c_point=y_point+gray_ceguang.size[0]-20;

              Mat temp_ceguang2=gray_ceguang(Rect(xlt,ylt,xrt-xlt,yrt-ylt));
              double mean_ceguang2 = mean(temp_ceguang2)[0];


              if(abs(mean_ceguang2-gray_ceguang.at<uchar>(Y_b_c_point,X_b_c_point))<2 and (image_l.at<uchar>(Y_2,X_1)>30)and(image_l.at<uchar>(Y_2,X_2)>30)and(abs(image_l.at<uchar>(y_point,x_lt)-image_l.at<uchar>(y_point,x_point))>10 or abs(image_l.at<uchar>(y_point,x_rt)-image_l.at<uchar>(y_point,x_point))>10)and (max(w/h,h/w)<3))
              {//(image_l.at<uchar>(Y_1,X_1)>30)and(image_l.at<uchar>(Y_2,X_2)>30) and
                  int qidian=X_1-10;
                  if(qidian<0)
                  {
                      qidian=0;
                  }
                  int zhongdian=X_2+15;
                  if(zhongdian>white_yiwu.size[1])
                  {
                      zhongdian=white_yiwu.size[1];
                  }

                  int yqidian=Y_1+img_gray.size[0]-20-15;
                  if(yqidian<0)
                  {
                      yqidian=0;
                  }
                  int yzhongdian=Y_2+img_gray.size[0]-20+15;
                  if(yzhongdian>white_yiwu.size[0])
                  {
                      yzhongdian=white_yiwu.size[0];
                  }


                  CvPoint top_lef4 = cvPoint(qidian, yqidian);
                  CvPoint bottom_right4 = cvPoint(zhongdian, yzhongdian);
                  rectangle(white_yiwu , top_lef4, bottom_right4, Scalar(255,0,5), 5, 8, 0);
                  result=true;
                  causeColor="背光异物";
                 // imwrite(save_name,white_yiwu);
              }
          }


         }


    image_l=img_gray(Rect(img_gray.size[1]-20,0,20,img_gray.size[0]));
    adaptiveThreshold(image_l,th1, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 31, 21);
    findContours( th1, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
        {
          double area = contourArea(contours[i]);
          if (area>10 and area<100)
          {
              boundRect[i] = boundingRect(Mat(contours[i]));
              float w=boundRect[i].width;
              float h=boundRect[i].height;
              int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
              int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
              int X_2=boundRect[i].br().x;//矩形右下角X坐标值
              int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
              int x_point=X_1+round(w/2);
              int y_point=Y_1+round(h/2);

              if(X_1<0)
              {
                  X_1=0;
              }
              if(Y_1<0)
              {
                  Y_1=0;
              }
              if(X_2>image_l.size[1]-1)
              {
                  X_2=image_l.size[1]-1;
              }
              if(Y_2>image_l.size[0]-1)
              {
                  Y_2=image_l.size[0]-1;
              }



              int x_lt=X_1-2;
              if(x_lt<0)
              {
                  x_lt=0;
              }
              int x_rt=X_2+2;
              if(x_rt>img_gray.size[1])
              {
                  x_rt=img_gray.size[1];
              }
              int y_u=Y_1-2;
              if(y_u<0)
              {
                  y_u=0;
              }

             int y_b=Y_2+2;
              if(y_b>img_gray.size[0])
              {
                  y_b=img_gray.size[0];
              }

              if((image_l.at<uchar>(Y_1,X_2)>30)and(image_l.at<uchar>(Y_2,X_2)>30)and(abs(image_l.at<uchar>(y_point,x_lt)-image_l.at<uchar>(y_point,x_point))>10 or abs(image_l.at<uchar>(y_point,x_rt)-image_l.at<uchar>(y_point,x_point))>10)and (max(w/h,h/w)<3))
              {
                  CvPoint top_lef4 = cvPoint(X_1+img_gray.size[1]-20, Y_1);
                  CvPoint bottom_right4 = cvPoint(X_2+img_gray.size[1]-20, Y_2);
                  rectangle(white_yiwu , top_lef4, bottom_right4, Scalar(255,0,5), 5, 8, 0);
                  result=true;
                  causeColor="背光异物";
                  //imwrite(save_name,white_yiwu);
              }
          }


         }
}
return result;



}


bool light_point(Mat image_black)
{   bool result=false;
    Mat black_gray;
    cvtColor(image_black,black_gray,CV_BGR2GRAY);
     Mat gray_clone=black_gray.clone();
    Mat mask_black;
    threshold(black_gray, mask_black,30, 255, CV_THRESH_BINARY);//100->60
   Scalar tempVal1;
    int co,ro;
    for( int row=0;row<black_gray.rows;row=row+15)//滑动窗口分块检测
    {
        for( int col=0;col<black_gray.cols;col=col+15)
        {  co=col;
           ro=row;
            if(ro+20>black_gray.rows-1)
            {
                ro=black_gray.rows-21;
                row=black_gray.rows;
            }
            if(co+20>black_gray.cols-1)
               {
                co=black_gray.cols-21;
                col=black_gray.cols;
                }

            Rect rect_wp(co,ro,20,20);
            Mat temp_wp=black_gray(rect_wp);
            tempVal1 =mean( temp_wp );
           double matMean_wp = tempVal1.val[0];
            Mat th_wp;
            threshold(temp_wp, th_wp,matMean_wp+5, 255, CV_THRESH_BINARY);//10->20
            th_wp.copyTo(gray_clone(rect_wp));
         }

      }
//======================================================
    vector<vector<Point>> contours;
    findContours( gray_clone(Rect(1,1,gray_clone.cols-2,gray_clone.rows-2)), contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
        {
          double area = contourArea(contours[i]);
          qDebug()<<"minaji"<<area<<endl;
          if (area>10 and area<1000)
          {
              boundRect[i] = boundingRect(Mat(contours[i]));
              float w=boundRect[i].width;
              float h=boundRect[i].height;
              int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
              int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
              int X_2=boundRect[i].br().x;//矩形右下角X坐标值
              int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
              int x_point=X_1+round(w/2);
              int y_point=Y_1+round(h/2);
              if(max(w/h,h/w)<4)
            {
                  X_1=X_1-10;
                  Y_1=Y_1-10;
                  X_2=X_2+10+int(w);
                  Y_2=Y_2+10+int(h);
                   if(X_1<0)
                   {
                       X_1=0;
                   }
                   if(Y_1<0)
                   {
                       Y_1=0;
                   }
                   if(X_2>image_black.cols-1)
                   {
                      X_2=image_black.cols-1;
                   }
                   if(Y_2>image_black.rows-1)
                   {
                       Y_2=image_black.rows-1;
                   }
                   Mat temp_ceguang=black_gray(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
                  // double mean_ceguang = mean(temp_ceguang)[0];
                   //gray_ceguang.at<uchar>(y_point,x_point)
                   if(black_gray.at<uchar>(y_point,x_point)>10)
                   {
                      CvPoint top_lef4 = cvPoint(X_1, Y_1);
                      CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                      rectangle(image_black , top_lef4, bottom_right4, Scalar(0,0,255), 2, 8, 0);
                      result=true;
                   }
                  }

          }
    }
    if(result==true)
    {
       // imwrite(result_lujing,image_black);
        causeColor="白点";
        return result;
    }
   //  imwrite(result_lujing,gray_clone);
    return result;
}

vector<Rect> watermark_locationBlue(Mat src)//提取蓝色印章范围
{

  Mat src_1 = src(Rect(40,40,src.cols-80,src.rows-80));
  Mat Hsv_watermark;
  Mat mask_1;
  cvtColor(src_1,Hsv_watermark, COLOR_BGR2HSV);
  inRange(Hsv_watermark, Scalar(100, 100, 50), Scalar(130,255, 255),mask_1);//红印章：Scalar(105, 80, 100), Scalar(180,100, 255)、大红字：Scalar(0, 80, 50), Scalar(10,255, 220),蓝色：Scalar(100, 100, 50), Scalar(130,255, 255)
  //namedWindow("hscc",0);
  //imshow("hscc",mask_1);
  Mat img_gray;
  cvtColor(src_1,img_gray,CV_BGR2GRAY);
  Mat mask_2;
  adaptiveThreshold( img_gray,mask_2, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 3);
  Mat Candidate;
  bitwise_and(mask_1,mask_2,Candidate);

  medianBlur(Candidate,Candidate,11);
  Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(5,5));
  Mat  erode1;
  erode(Candidate, erode1, structure_element1);
  //namedWindow("dilare1",0);
  //imshow("dilare1",erode1);
  Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(55,55));
  Mat dilate1;
  dilate(erode1, dilate1, structure_element2);

  Mat Candidate_2 = dilate1(Rect(40,40,dilate1.cols-80,dilate1.rows-80));
   vector<vector<Point>> contours;
   findContours( Candidate_2, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
   vector<Rect> boundRect(contours.size());
   for(vector<int>::size_type i = 0; i < contours.size(); i++)
       {
         double area = contourArea(contours[i]);
         if (area>10000 and area<100000)//10000
         {
             boundRect[i] = boundingRect(Mat(contours[i]));
             int X_1=boundRect[i].tl().x+40;//矩形左上角X坐标值
             int Y_1=boundRect[i].tl().y+40;//矩形左上角Y坐标值
             int X_2=boundRect[i].br().x+120;//矩形右下角X坐标值
             int Y_2=boundRect[i].br().y+120;//矩形右下角Y坐标值
             boundRect[i].x=X_1;
             boundRect[i].y=Y_1;
             boundRect[i].width=X_2-X_1;
             boundRect[i].height=Y_2-Y_1;
            // CvPoint top_lef4 = cvPoint(X_1, Y_1);
            // CvPoint bottom_right4 = cvPoint(X_2, Y_2);
            // rectangle(src , top_lef4, bottom_right4, Scalar(0,0,255), 5, 8, 0);

          }
       }

   return boundRect;

}

Mat watermark_locationBluepicture(Mat src)//提取蓝色印章图片
{

  Mat src_1 = src(Rect(40,40,src.cols-80,src.rows-80));
  Mat Hsv_watermark;
  Mat mask_1;
  cvtColor(src_1,Hsv_watermark, COLOR_BGR2HSV);
  inRange(Hsv_watermark, Scalar(100, 100, 50), Scalar(130,255, 255),mask_1);//红印章：Scalar(105, 80, 100), Scalar(180,100, 255)、大红字：Scalar(0, 80, 50), Scalar(10,255, 220),蓝色：Scalar(100, 100, 50), Scalar(130,255, 255)
  //namedWindow("hscc",0);
  //imshow("hscc",mask_1);
  Mat img_gray;
  cvtColor(src_1,img_gray,CV_BGR2GRAY);
  Mat mask_2;
  adaptiveThreshold( img_gray,mask_2, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 3);
  Mat Candidate;
  bitwise_and(mask_1,mask_2,Candidate);

  medianBlur(Candidate,Candidate,11);
  Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(5,5));
  Mat  erode1;
  erode(Candidate, erode1, structure_element1);
  //namedWindow("dilare1",0);
  //imshow("dilare1",erode1);
  Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(55,55));
  Mat dilate1;
  dilate(erode1, dilate1, structure_element2);

  Mat Candidate_2 = dilate1(Rect(40,40,dilate1.cols-80,dilate1.rows-80));
   vector<vector<Point>> contours;
   findContours( Candidate_2, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
   vector<Rect> boundRect(contours.size());
   for(vector<int>::size_type i = 0; i < contours.size(); i++)
       {
         double area = contourArea(contours[i]);
         if (area>10000 and area<100000)//10000
         {
             boundRect[i] = boundingRect(Mat(contours[i]));
             int X_1=boundRect[i].tl().x+60;//矩形左上角X坐标值
             int Y_1=boundRect[i].tl().y+60;//矩形左上角Y坐标值
             int X_2=boundRect[i].br().x+100;//矩形右下角X坐标值
             int Y_2=boundRect[i].br().y+100;//矩形右下角Y坐标值
             boundRect[i].x=X_1;
             boundRect[i].y=Y_1;
             boundRect[i].width=X_2-X_1;
             boundRect[i].height=Y_2-Y_1;
             CvPoint top_lef4 = cvPoint(X_1, Y_1);
             CvPoint bottom_right4 = cvPoint(X_2, Y_2);
             rectangle(src , top_lef4, bottom_right4, Scalar(0,0,255), 5, 8, 0);

          }
       }

   return src;

}

//返回水印位置
vector<Rect> watermark_location(Mat src)//红喷印
{

  Mat src_1 = src(Rect(40,40,src.cols-80,src.rows-80));
  Mat Hsv_watermark;
  Mat mask_1;
  cvtColor(src_1,Hsv_watermark, COLOR_BGR2HSV);
  inRange(Hsv_watermark, Scalar(120, 43, 43), Scalar(180,255, 255),mask_1);
  Mat img_gray;
  cvtColor(src_1,img_gray,CV_BGR2GRAY);
  Mat mask_2;
  adaptiveThreshold( img_gray,mask_2, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 3);
  Mat Candidate;
  bitwise_and(mask_1,mask_2,Candidate);
 // medianBlur(Candidate,Candidate,11);
 // Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(5,5));
 // Mat  erode1;
 // erode(Candidate, erode1, structure_element1);
  Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(55,55));
  Mat dilate1;
  dilate(Candidate, dilate1, structure_element2);
  Mat Candidate_2 = dilate1(Rect(40,40,dilate1.cols-80,dilate1.rows-80));
   vector<vector<Point>> contours;
   findContours( Candidate_2, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
   vector<Rect> boundRect(contours.size());
   for(vector<int>::size_type i = 0; i < contours.size(); i++)
       {
         double area = contourArea(contours[i]);
         if (area>100 and area<100000)
         {
             boundRect[i] = boundingRect(Mat(contours[i]));
             int X_1=boundRect[i].tl().x-60;//矩形左上角X坐标值
             int Y_1=boundRect[i].tl().y-60;//矩形左上角Y坐标值
             int X_2=boundRect[i].br().x+100;//矩形右下角X坐标值
             int Y_2=boundRect[i].br().y+100;//矩形右下角Y坐标值
             boundRect[i].x=X_1;
             boundRect[i].y=Y_1;
             boundRect[i].width=X_2-X_1;
             boundRect[i].height=Y_2-Y_1;
             CvPoint top_lef4 = cvPoint(X_1, Y_1);
             CvPoint bottom_right4 = cvPoint(X_2, Y_2);
             rectangle(src , top_lef4, bottom_right4, Scalar(0,0,255), 5, 8, 0);

          }
       }
    //namedWindow("红色喷印",0);
    //imshow("红色喷印",src);
   return boundRect ;

}


Mat watermark_locationpicture(Mat src)//红喷印
{

  Mat src_1 = src(Rect(40,40,src.cols-80,src.rows-80));
  Mat Hsv_watermark;
  Mat mask_1;
  cvtColor(src_1,Hsv_watermark, COLOR_BGR2HSV);
  inRange(Hsv_watermark, Scalar(120, 43, 43), Scalar(180,255, 255),mask_1);
  //namedWindow("mask",0);
  //imshow("mask",mask_1);
  Mat img_gray;
  cvtColor(src_1,img_gray,CV_BGR2GRAY);
  Mat mask_2;
  adaptiveThreshold( img_gray,mask_2, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 3);
  Mat Candidate;
  bitwise_and(mask_1,mask_2,Candidate);
 // medianBlur(Candidate,Candidate,11);
 // Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(5,5));
 // Mat  erode1;
 //erode(Candidate, erode1, structure_element1);
  Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(55,55));
  Mat dilate1;
  dilate(Candidate, dilate1, structure_element2);
  Mat Candidate_2 = dilate1(Rect(40,40,dilate1.cols-80,dilate1.rows-80));
   vector<vector<Point>> contours;
   findContours( Candidate_2, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
   vector<Rect> boundRect(contours.size());
   for(vector<int>::size_type i = 0; i < contours.size(); i++)
       {
         double area = contourArea(contours[i]);
         if (area>100 and area<100000)
         {
             boundRect[i] = boundingRect(Mat(contours[i]));
             int X_1=boundRect[i].tl().x+80;//矩形左上角X坐标值
             int Y_1=boundRect[i].tl().y+80;//矩形左上角Y坐标值
             int X_2=boundRect[i].br().x+80;//矩形右下角X坐标值
             int Y_2=boundRect[i].br().y+80;//矩形右下角Y坐标值
             boundRect[i].x=X_1;
             boundRect[i].y=Y_1;
             boundRect[i].width=X_2-X_1;
             boundRect[i].height=Y_2-Y_1;
             CvPoint top_lef4 = cvPoint(X_1, Y_1);
             CvPoint bottom_right4 = cvPoint(X_2, Y_2);
             rectangle(src , top_lef4, bottom_right4, Scalar(0,0,255), 5, 8, 0);

          }
       }

   return src ;

}
//####################灰尘位置
Mat ceguang(Mat  src)
{
    //Mat srcwhite = imread(img_name_white);
   // Mat srcceguang=imread(img_name_ceguang);
   // Mat srcblack = imread(img_name_black);
    //CvRect B= roi_white(img_name_white);//标定位置
   // Mat image_ceguang = srcceguang(B);//根据位置截
    Mat image_ceguang=src.clone();
    cvtColor(image_ceguang,image_ceguang,CV_BGR2GRAY);
    //namedWindow("ce",0);
    //imshow("ce",image_ceguang);
    Mat image_ceguang_clone=image_ceguang.clone();

    double huichen=0;
    double minp_huichen=0;
    double maxp_huichen=0;
    double diff_huichen;


  //for(int row=0;row<image_ceguang_clone.rows-50;row=row+30)
//  {
  //    for(int col=0;col<image_ceguang_clone.cols-50;col=col+30)

   //   {
   //       Rect rect1(col,row,50,50);
  //        Mat temp_huichen=image_ceguang_clone(rect1);

   //       huichen = mean(temp_huichen)[0];



     //  minMaxIdx(temp_huichen,&minp_huichen,&maxp_huichen);
    //   diff_huichen=maxp_huichen-minp_huichen;


   //  Mat th2_yiwu;
     //threshold(temp_huichen,th2_yiwu,minp_huichen,CV_THRESH_BINARY);
     //adaptiveThreshold(temp_huichen, th2_yiwu, 0, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 21, 5);//21越大检测越灵敏，5越小越灵敏

Mat resultpicture;
                   threshold(image_ceguang_clone, resultpicture,80, 255, CV_THRESH_BINARY);
                  // namedWindow("erzhi",0);
                  // imshow("erzhi",resultpicture);
                  medianBlur(resultpicture,resultpicture,3);
//namedWindow("zhongzhi",0);
//imshow("zhongzhi",resultpicture);
Mat structure = getStructuringElement(MORPH_RECT,Size(2,2));
Mat erode1;
//erode(resultpicture,erode1,structure);
//namedWindow("fushi",0);
imshow("fushi",erode1);
Mat structure1 = getStructuringElement(MORPH_RECT,Size(4,4));
Mat dilate1;
dilate(erode1,dilate1,structure1);
//namedWindow("pengzhang",0);
//imshow("pengzhang",dilate1);
Mat M_yiwu(dilate1.size[0], dilate1.size[1], CV_8UC1, Scalar(0) );
//namedWindow("m_yiwu",0);
//imshow("m_yiwu",M_yiwu);
vector<vector<Point>> contours_huichen;
findContours(dilate1,contours_huichen,CV_RETR_LIST,CHAIN_APPROX_SIMPLE);
vector<Rect> boundRect_huichen(contours_huichen.size());
for(vector<int>::size_type i = 0 ;i<contours_huichen.size();i++)
{
    double area_huicehn=contourArea(contours_huichen[i]);
    if(area_huicehn>10 and area_huicehn<500)
    {
        boundRect_huichen[i]=boundingRect(Mat(contours_huichen[i]));
        int x1=boundRect_huichen[i].tl().x-4;
        int y1=boundRect_huichen[i].tl().y-4;
        int x2=boundRect_huichen[i].br().x+4;
        int y2=boundRect_huichen[i].br().y+4;
        if(x1<0)
        {
            x1=0;
        }
        if(x2<0)
        {
            x2=0;
        }
        if(x2>resultpicture.cols)
        {
            x2=resultpicture.cols;
        }
        if(y2>resultpicture.rows)
        {
            y2=resultpicture.rows;
        }
        CvPoint top_left=cvPoint(x1,y1);
        CvPoint bottom_right=cvPoint(x2,y2);
        rectangle(image_ceguang_clone,top_left,bottom_right,Scalar(255,0,0),5,8,0);
   }
}
namedWindow("weizhi",0);
  imshow("weizhi",image_ceguang_clone);
      //th2_yiwu.copyTo(M_yiwu(rect1));


     //  }


     // }
  //namedWindow("二值化",0);
  //imshow("二值化",resultpicture);
    return  image_ceguang_clone;
}

vector<Rect> ceguanglocation(Mat  src)
{
    //Mat srcwhite = imread(img_name_white);
   // Mat srcceguang=imread(img_name_ceguang);
   // Mat srcblack = imread(img_name_black);
    //CvRect B= roi_white(img_name_white);//标定位置
   // Mat image_ceguang = srcceguang(B);//根据位置截
    Mat image_ceguang=src.clone();
    cvtColor(image_ceguang,image_ceguang,CV_BGR2GRAY);
    //namedWindow("ce",0);
    //imshow("ce",image_ceguang);
    Mat image_ceguang_clone=image_ceguang.clone();

    double huichen=0;
    double minp_huichen=0;
    double maxp_huichen=0;
    double diff_huichen;

Mat resultpicture=image_ceguang_clone.clone();
 threshold(image_ceguang_clone, resultpicture,80, 255, CV_THRESH_BINARY);
  for(int row=0;row<image_ceguang_clone.rows-50;row=row+30)
  {
      for(int col=0;col<image_ceguang_clone.cols-50;col=col+30)

      {
          Rect rect1(col,row,50,50);
          Mat temp_huichen=image_ceguang_clone(rect1);

          huichen = mean(temp_huichen)[0];



       minMaxIdx(temp_huichen,&minp_huichen,&maxp_huichen);
       diff_huichen=maxp_huichen-minp_huichen;


     Mat th2_yiwu;
     //threshold(temp_huichen,th2_yiwu,minp_huichen+6,255,CV_THRESH_BINARY);
     adaptiveThreshold(temp_huichen, th2_yiwu, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 25, 4);//21越大检测越灵敏，5越小越灵敏
     th2_yiwu.copyTo(resultpicture(rect1));
        }
  }


                   //namedWindow("er",0);
                   //imshow("er",resultpicture);
                  medianBlur(resultpicture,resultpicture,3);
//namedWindow("zhongzhi",0);
//imshow("zhongzhi",resultpicture);
Mat structure = getStructuringElement(MORPH_RECT,Size(2,2));
Mat erode1;
erode(resultpicture,erode1,structure);
//namedWindow("fushi",0);
//imshow("fushi",erode1);
Mat structure1 = getStructuringElement(MORPH_RECT,Size(4,4));
Mat dilate1;
dilate(erode1,dilate1,structure1);
//namedWindow("pengzhang",0);
//imshow("pengzhang",dilate1);
Mat M_yiwu(dilate1.size[0], dilate1.size[1], CV_8UC1, Scalar(0) );
//namedWindow("m_yiwu",0);
//imshow("m_yiwu",M_yiwu);
vector<vector<Point>> contours_huichen;
findContours(dilate1,contours_huichen,CV_RETR_LIST,CHAIN_APPROX_SIMPLE);
vector<Rect> boundRect_huichen(contours_huichen.size());
for(vector<int>::size_type i = 0 ;i<contours_huichen.size();i++)
{
    double area_huicehn=contourArea(contours_huichen[i]);
    if(area_huicehn>1 and area_huicehn<50000)
    {
        boundRect_huichen[i]=boundingRect(Mat(contours_huichen[i]));
        int x1=boundRect_huichen[i].tl().x-4;
        int y1=boundRect_huichen[i].tl().y-4;
        int x2=boundRect_huichen[i].br().x+4;
        int y2=boundRect_huichen[i].br().y+4;
        if(x1<0)
        {
            x1=0;
        }
        if(x2<0)
        {
            x2=0;
        }
        if(x2>resultpicture.cols)
        {
            x2=resultpicture.cols;
        }
        if(y2>resultpicture.rows)
        {
            y2=resultpicture.rows;
        }
        boundRect_huichen[i].x=x1;
        boundRect_huichen[i].y=y1;
        boundRect_huichen[i].height=y2-y1;
        boundRect_huichen[i].width=x2-x1;
        CvPoint top_left=cvPoint(x1,y1);
        CvPoint bottom_right=cvPoint(x2,y2);
        rectangle(image_ceguang_clone,top_left,bottom_right,Scalar(255,0,0),5,8,0);
   }
}
namedWindow("位置",0);
imshow("位置",image_ceguang_clone);
      //th2_yiwu.copyTo(M_yiwu(rect1));


     //  }


     // }
  //namedWindow("二值化",0);
  //imshow("二值化",resultpicture);
   // return  image_ceguang;
return boundRect_huichen;
}


/*=========================================================
* 函 数 名: 去印章
* 功能描述: 核函数的计算
=========================================================*/
Mat roi_white_yiwu(Mat src_white)//白色底色下提取黑色ROI
{
    Mat img_gray;
    cvtColor(src_white,img_gray,CV_BGR2GRAY);
    Mat th1;
    threshold(img_gray, th1,40, 255, CV_THRESH_BINARY);//100->60
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3,3));
    Mat  erode1;
    erode(th1, erode1, structure_element1);
    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(9,9));
    Mat dilate1;
    dilate(erode1, dilate1, structure_element2);
    Mat img_temp1,img_temp2,img_temp3,img_temp4;
    int X_1,Y_1,X_2,Y_2,w,h;
    int X_lt,Y_lt,X_rt,Y_rt,X_lb,Y_lb,X_rb,Y_rb,w1,h1;
    vector<vector<Point>> contours1;
    vector<vector<Point>> contours;
    findContours(dilate1, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
        {
          double area = contourArea(contours[i]);

          if (area>100000 and area<45000000)
          {
              boundRect[i] = boundingRect(Mat(contours[i]));
              X_1=boundRect[i].tl().x;
              Y_1=boundRect[i].tl().y;
              X_2=boundRect[i].br().x;
              Y_2=boundRect[i].br().y;
              w=boundRect[i].width;
              h=boundRect[i].height;
              img_temp1=dilate1(Rect(X_1,Y_1,100,100));
              findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
              X_lt=X_1+boundingRect(Mat(contours1[0])).tl().x;
              Y_lt=Y_1+boundingRect(Mat(contours1[0])).tl().y;

              img_temp1=dilate1(Rect(X_1+w-100,Y_1,100,100));
              findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
              X_rt=X_1+w-(100-boundingRect(Mat(contours1[0])).width);
              Y_rt=Y_1+(100-boundingRect(Mat(contours1[0])).height);

              img_temp1=dilate1(Rect(X_1,Y_1+h-100,100,100));
              findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
              X_lb=X_1+(boundingRect(Mat(contours1[0])).tl().x);
              Y_lb=Y_1+h-(100-boundingRect(Mat(contours1[0])).height);

              img_temp1=dilate1(Rect(X_2-100,Y_2-100,100,100));
              findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
              X_rb=X_1+w-(100-boundingRect(Mat(contours1[0])).width);
              Y_rb=Y_1+h-(100-boundingRect(Mat(contours1[0])).height);

          }
        }
    cv::Point2f src_points[] = {

        cv::Point2f(X_lt+0, Y_lt+0),

        cv::Point2f(X_rt-0, Y_rt+0),

        cv::Point2f(X_lb+0, Y_lb-0),

        cv::Point2f(X_rb-0, Y_rb-0)
                                };
    cv::Point2f dst_points[] = {

        cv::Point2f(0, 0),

        cv::Point2f(2650, 0),

        cv::Point2f(0, 1240),

        cv::Point2f(2650, 1240) };

Mat M = cv::getPerspectiveTransform(src_points, dst_points);

return  M;

}
//Mat roi_louguang(Mat src_white)//白色底色下提取黑色ROI
//{
//    Mat img_gray;
//    cvtColor(src_white,img_gray,CV_BGR2GRAY);
//    Mat th1;
//    threshold(img_gray, th1,50, 255, CV_THRESH_BINARY);//100->60
//    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(15,15));
//    Mat  erode1;
//    erode(th1, erode1, structure_element1);
//    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(19,19));
//    Mat dilate1;
//    dilate(erode1, dilate1, structure_element2);
//    Mat img_temp1,img_temp2,img_temp3,img_temp4;
//    int X_1,Y_1,X_2,Y_2,w,h;
//    int X_lt,Y_lt,X_rt,Y_rt,X_lb,Y_lb,X_rb,Y_rb,w1,h1;
//    vector<vector<Point>> contours1;
//    vector<vector<Point>> contours;
//    findContours(dilate1, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//        {
//          double area = contourArea(contours[i]);

//          if (area>3500000 and area<4500000)
//          {
//              boundRect[i] = boundingRect(Mat(contours[i]));
//              X_1=boundRect[i].tl().x;
//              Y_1=boundRect[i].tl().y;
//              X_2=boundRect[i].br().x;
//              Y_2=boundRect[i].br().y;
//              w=boundRect[i].width;
//              h=boundRect[i].height;
//              img_temp1=dilate1(Rect(X_1,Y_1,100,100));
//              findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//              X_lt=X_1+boundingRect(Mat(contours1[0])).tl().x;
//              Y_lt=Y_1+boundingRect(Mat(contours1[0])).tl().y;

//              img_temp1=dilate1(Rect(X_1+w-100,Y_1,100,100));
//              findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//              X_rt=X_1+w-(100-boundingRect(Mat(contours1[0])).width);
//              Y_rt=Y_1+(100-boundingRect(Mat(contours1[0])).height);

//              img_temp1=dilate1(Rect(X_1,Y_1+h-100,100,100));
//              findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//              X_lb=X_1+(boundingRect(Mat(contours1[0])).tl().x);
//              Y_lb=Y_1+h-(100-boundingRect(Mat(contours1[0])).height);

//              img_temp1=dilate1(Rect(X_2-100,Y_2-100,100,100));
//              findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//              X_rb=X_1+w-(100-boundingRect(Mat(contours1[0])).width);
//              Y_rb=Y_1+h-(100-boundingRect(Mat(contours1[0])).height);

//          }
//        }
//    cv::Point2f src_points[] = {

//        cv::Point2f(X_lt-10, Y_lt-10),

//        cv::Point2f(X_rt+10, Y_rt-10),

//        cv::Point2f(X_lb-10, Y_lb+10),

//        cv::Point2f(X_rb+10, Y_rb+10)
//                                };
//    cv::Point2f dst_points[] = {

//        cv::Point2f(0, 0),

//        cv::Point2f(2900, 0),

//        cv::Point2f(0, 1420),

//        cv::Point2f(2900, 1420) };

//Mat M = cv::getPerspectiveTransform(src_points, dst_points);

//return  M;

//}
Mat toushi(Mat image,Mat M)
{   Mat perspective;
    cv::warpPerspective(image, perspective, M, cv::Size(2650, 1240), cv::INTER_LINEAR);
    return perspective;
}

/*=========================================================
* 函 数 名: 白印检测
* 功能描述: 核函数的计算
=========================================================*/
Mat ROI(Mat src_white)
{
        Mat M=roi_white_yiwu(src_white);
        return M;
}
Mat pre_process_white(Mat src_white,Mat M)
{
       // Mat M=roi_white_yiwu(src_white);
        Mat white=toushi(src_white,M);
        Mat gabor=New_Gabor(white);
        return gabor;
}



//之前不用的历史程序
/*=========================================================
* 函 数 名:
* 功能描述: 核函数的计算        //两侧相机仿射变换映射关系的计算
=========================================================*/
//bool roi_ceshi(Mat src_white,int border_white,Mat *Mwhite)//白屏图片侧相机找透视变换的映射关系
//{
//    Mat img_gray;
//    cvtColor(src_white,img_gray,CV_BGR2GRAY);
//    Mat th1;
//    threshold(img_gray, th1,20, 255, CV_THRESH_BINARY);//100->60
//    //imwrite("E:\\MSV_image\\day1115\\Image_6_7.bmp",th1);
//    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3,3));
//    Mat  erode1;
//    erode(th1, erode1, structure_element1);
//    //       Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(9,9));
//    //       Mat dilate1;
//    //       dilate(erode1, dilate1, structure_element2);
//    Mat img_temp1,img_temp2,img_temp3,img_temp4;
//    vector<vector<Point>> contours1;
//    vector<vector<Point>> contours;
//    std::vector<cv::Point> approxCurve;
//    int num_contours=0;

//    findContours(erode1, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    Rect rect1;
//    Point2f P[4];
//    Point2f shunxu[4];
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//    {
//        double area = contourArea(contours[i]);
//        qDebug()<<area<<endl;
//        //qDebug()<<area<<endl;
//        if (area>500000 and area<4000000)   //这是侧视相机找到的轮廓面积所在的范围
//        {   num_contours++;
//            //              double epsilon = 0.1*arcLength(contours[i], true);
//            //              cv::approxPolyDP(contours[i], approxCurve, epsilon, true); // 多边形逼近，精度(即最小边长)设为30是为了得到4个角点
//            rect1 = cv::boundingRect(contours[i]);
//            //              rectangle(src_white, rect1, Scalar(0, 255, 0), 1);  //先不画出这个边框
//        }
//    }
//    if(num_contours==0)
//    {
//        isCeshi = true;
//        cv::Point2f src_points[] = {
//            cv::Point2f(0, 0),
//            cv::Point2f(0, 10),
//            cv::Point2f(10, 10),
//            cv::Point2f(10, 0)
//        };

//        cv::Point2f dst_points[] = {
//            cv::Point2f(0, 0),
//            cv::Point2f(0, screenWidth-1),
//            cv::Point2f(screenLength-1, screenWidth-1),
//            cv::Point2f(screenLength-1,0)
//        };
//        *Mwhite= cv::getPerspectiveTransform(src_points, dst_points);

//    }
//    else
//    {
//        int x1 = rect1.tl().x;
//        int y1 = rect1.tl().y;
//        int x2 = rect1.tl().x;
//        int y2 = rect1.br().y;
//        int x3 = rect1.br().x;
//        int y3 = rect1.br().y;
//        int x4 = rect1.br().x;
//        int y4 = rect1.tl().y;
//        int  width =rect1.width;
//        int height =rect1.height;
//        vector<Point2f> src_corner(4);
//        Point2f tl = CrossPointReturn(erode1, x1 - 100, y1 - 100, width / 4, height / 3 );   // 改过的
//        Point2f bl = CrossPointReturn(erode1, x2 - 100, y2 + 100 - height / 3, width / 4, height / 3);
//        Point2f br = CrossPointReturn(erode1, x3 + 100 - width / 4, y3 + 100 - height / 3, width / 4, height / 3);
//        Point2f tr = CrossPointReturn(erode1, x4 + 100 - width / 4, y4 - 100, width / 4, height / 3);
//        if(tl!=Point2f(-1,-1) && bl!=Point2f(-1,-1) && br!=Point2f(-1,-1) && tr!=Point2f(-1,-1))  //如果找不到两条直线,会返回-1点,来表示没有提取到
//        {
//            src_corner[0] = Point2f(x1 - 100 + tl.x, y1 - 100 + tl.y);
//            src_corner[1] = Point2f(x2 - 100 + bl.x, y2 + 100 - height / 3 + bl.y);
//            src_corner[2] = Point2f(x3 + 100 - width / 4 + br.x, y3 + 100 - height / 3 + br.y);
//            src_corner[3] = Point2f(x4 + 100 - width / 4 + tr.x, y4 - 100 + tr.y);
//        }
//        else
//        {
//            src_corner[0] = Point2f(0,0);
//            src_corner[1] = Point2f(0,10);
//            src_corner[2] = Point2f(10,10);
//            src_corner[3] = Point2f(10,0);
//            isCeshi=true;
//        }
//        vector<Point2f> dst_corner(4);
//        dst_corner[0] = Point(0, 0);
//        dst_corner[1] = Point(0, screenWidth-1);
//        dst_corner[2] = Point(screenLength-1, screenWidth-1);
//        dst_corner[3] = Point(screenLength-1, 0);
//        *Mwhite= cv::getPerspectiveTransform(src_corner, dst_corner);
//    }
//    return true;
//}


/*=========================================================
* 函 数 名:
* 功能描述: 核函数的计算        //主相机仿射变换映射关系的计算
=========================================================*/
//bool roi_white(Mat src_white,int border_white,int border_black,int border_lougunag,Mat *Mwhite,Mat *Mblack,Mat *Mlouguang,int ID)//白色底色下提取黑色ROI
//{
//    Mat img_gray;
//    cvtColor(src_white,img_gray,CV_BGR2GRAY);
//    Mat th1;
//    threshold(img_gray, th1,20, 255, CV_THRESH_BINARY);//100->60
//    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3,3));
//    Mat  erode1;
//    erode(th1, erode1, structure_element1);
////    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(9,9));
////    Mat dilate1;
////    dilate(erode1, dilate1, structure_element2);
//    Mat img_temp1,img_temp2,img_temp3,img_temp4;
//    int X_1,Y_1,X_2,Y_2,w,h;
//    int X_lt,Y_lt,X_rt,Y_rt,X_lb,Y_lb,X_rb,Y_rb,w1,h1;
//    int unm_contours=0;
//    vector<vector<Point>> contours1;
//    vector<vector<Point>> contours;
//    std::vector<cv::Point> poly;
//    std::vector<cv::Point> approxCurve;
//    Rect rect1;
//    Point2f P[4];
//    Point2f shunxu[4];
//    findContours(erode1, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//        {
//          double area = contourArea(contours[i]);
//         // qDebug()<<area<<endl;
//          qDebug()<<area<<endl;
//          if (area>3000000 and area<4000000)
//          {   unm_contours++;
////              double epsilon = 0.1*arcLength(contours[i], true);
////              cv::approxPolyDP(contours[i], approxCurve, epsilon, true);
//              rect1 = cv::boundingRect(contours[i]);
////              RotatedRect rect = minAreaRect(contours[i]);
////              rect.points(P);
////               rectangle(erode1, rect1, Scalar(0, 0, 255), 1);
////                  imwrite("E:\\picture\\0107\\1\\lunkuo.png",erode1);
//          }
//        }
//    if(unm_contours>0&&ID==1)
//    {
//        isArea_1=false;
//    }
//     if(unm_contours==0&&ID==1)
//    {
//        isArea_1=true;
//    }

//    if(unm_contours>0&&ID==2)
//    {
//        isArea_2=false;
//    }
//   if(unm_contours==0&&ID==2)
//    {
//        isArea_2=true;
//    }

//if(unm_contours==0)
//{
////  vector<cv::Point> approxCurve1;
////  approxCurve1[0].x=0; approxCurve1[0].y=0;
////  approxCurve1[1].x=0; approxCurve1[1].y=0;
////  approxCurve1[2].x=0; approxCurve1[2].y=0;
////  approxCurve1[3].x=0; approxCurve1[3].y=0;

//   cv::Point2f src_points[] = {

//       cv::Point2f(0, 0),
//       cv::Point2f(0, 10),
//      cv::Point2f(10, 10),
//      cv::Point2f(10, 0)
//                               };
//   cv::Point2f dst_points[] = {
//   cv::Point2f(0, 0),
//   cv::Point2f(0, 1755),
//   cv::Point2f(3000, 1775) ,
//   cv::Point2f(3000,0)};
// *Mwhite= cv::getPerspectiveTransform(src_points, dst_points);
// *Mlouguang=cv::getPerspectiveTransform(src_points, dst_points);
// *Mblack=cv::getPerspectiveTransform(src_points, dst_points);
//}
//else
//{
////    for (vector<int>::size_type i = 0; i<4; i++)
////    {
////        if (P[i].x<erode1.cols / 2 && P[i].y<erode1.rows / 2)
////        {
////            shunxu[0] = P[i];
////        }
////        if (P[i].x<erode1.cols / 2 && P[i].y>erode1.rows / 2)
////        {
////            shunxu[1] = P[i];
////        }
////        if (P[i].x>erode1.cols / 2 && P[i].y>erode1.rows / 2)
////        {
////            shunxu[2] = P[i];
////        }
////        if (P[i].x>erode1.cols / 2 && P[i].y<erode1.rows / 2)
////        {
////            shunxu[3] = P[i];
////        }
////    }

////   cv::Point2f src_points[] = {
////        Point2f(shunxu[0].x+5,shunxu[0].y+5),
////        Point2f(shunxu[1].x+5,shunxu[1].y-5),
////        Point2f(shunxu[2].x-5,shunxu[2].y-5),
////        Point2f(shunxu[3].x-5,shunxu[3].y+5)
////                               };
////   cv::Point2f dst_points[] = {
////   cv::Point2f(0, 0),
////   cv::Point2f(0, screenWidth-1),
////   cv::Point2f(screenLength-1, screenWidth-1) ,
////   cv::Point2f(screenLength-1,0)};
////    *Mwhite= cv::getPerspectiveTransform(src_points, dst_points);
////    *Mlouguang=cv::getPerspectiveTransform(src_points, dst_points);
////    *Mblack=cv::getPerspectiveTransform(src_points, dst_points);
//    int x1 = rect1.tl().x;
//        int y1 = rect1.tl().y;
//        int x2 = rect1.tl().x;
//        int y2 = rect1.br().y;
//        int x3 = rect1.br().x;
//        int y3 = rect1.br().y;
//        int x4 = rect1.br().x;
//        int y4 = rect1.tl().y;
//        int  width =rect1.width;
//        int height =rect1.height;
//        vector<Point2f> src_corner(4);
//            Point2f tl = CrossPointReturn(erode1, x1 - 100, y1 - 100, width / 4, height / 3 );   // 改过的
//            Point2f bl = CrossPointReturn(erode1, x2 - 100, y2 + 100 - height / 3, width / 4, height / 3);
//            Point2f br = CrossPointReturn(erode1, x3 + 100 - width / 4, y3 + 100 - height / 3, width / 4, height / 3);
//            Point2f tr = CrossPointReturn(erode1, x4 + 100 - width / 4, y4 - 100, width / 4, height / 3);
//            if(tl!=Point2f(-1,-1) && bl!=Point2f(-1,-1) && br!=Point2f(-1,-1) && tr!=Point2f(-1,-1))  //如果找不到两条直线,会返回-1点,来表示没有提取到
//            {
//                src_corner[0] = Point2f(x1 - 100 + tl.x, y1 - 100 + tl.y);
//                src_corner[1] = Point2f(x2 - 100 + bl.x, y2 + 100 - height / 3 + bl.y);
//                src_corner[2] = Point2f(x3 + 100 - width / 4 + br.x, y3 + 100 - height / 3 + br.y);
//                src_corner[3] = Point2f(x4 + 100 - width / 4 + tr.x, y4 - 100 + tr.y);
//            }
//            else
//            {
//                src_corner[0] = Point2f(0,0);
//                src_corner[1] = Point2f(0,10);
//                src_corner[2] = Point2f(10,10);
//                src_corner[3] = Point2f(10,0);
//                isCeshi=true;
//            }

//        vector<Point2f> dst_corner(4);
//        dst_corner[0] = Point(0, 0);
//        dst_corner[1] = Point(0, screenWidth-1);
//        dst_corner[2] = Point(screenLength-1, screenWidth-1);
//        dst_corner[3] = Point(screenLength-1, 0);
// *Mwhite= cv::getPerspectiveTransform(src_corner, dst_corner);
// *Mlouguang=cv::getPerspectiveTransform(src_corner, dst_corner);
// *Mblack=cv::getPerspectiveTransform(src_corner, dst_corner);
//}
//return true;
//}






//======================================================================================
//MV_CC_SetEnumValue(camera->m_hDevHandle_1, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//MV_CC_SetEnumValue(camera->m_hDevHandle_1,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
//MV_CC_SetFloatValue(camera->m_hDevHandle_1,"ExposureTime", 150000);
//delay(1000);
//cv::Mat getImage1=camera->saveImage(camera->m_hDevHandle_1);
//cv::imwrite("E:\\MSV_image\\dalongshan\\Image_1_2.bmp",getImage1);
//MV_CC_SetEnumValue(camera->m_hDevHandle_1, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//MV_CC_SetEnumValue(camera->m_hDevHandle_1,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
//MV_CC_SetFloatValue(camera->m_hDevHandle_1,"ExposureTime", 10000);
//delay(1000);
//F=true;
//F_start=false;
//if(F_start==false)
//      return 0;
//mstimer.start();
//while(true && F_start==true)
//{   read_Modbus(4150);
//   //侧光开启信号
//   if (Data_Form_Plc==1)
//   {   Data_Form_Plc=0;
//       break;
//   }
//   else
//   {
//      delay(100);
//   }
//}
//if(F_start==false)
//  return 0;
//ceguag_1=camera->saveImage(camera->m_hDevHandle_2);  //侧光1
//ceguag_2=camera->saveImage(camera->m_hDevHandle_5);  //侧光2
//if(false)
//{
//}
////=================================白色==================================================
//while(!write_Modbus(10673,1) && F_start==true)//关闭光源打开测试盒
//{
//  delay(100);
//}
//if(F_start==false)
//  return 0;

//read_Modbus(10679);//读测试架打开完成信号
//while(true && F_start==true)
//{
//    //读测试架打开完成信号
//   if (Data_Form_Plc==1)
//   {   Data_Form_Plc=0;
//       break;
//   }
//   else
//   {
//       delay(100);
//   }
//}
//if(F_start==false)
//  return 0;

//Image_1_white=camera->saveImage(camera->m_hDevHandle_2);//拍摄1#工位白色图片
//Image_2_white=camera->saveImage(camera->m_hDevHandle_5);//拍摄2#工位白色图片
//Image_1_Right=camera->saveImage(camera->m_hDevHandle_2);//拍摄1#工位右侧相机图片
//Image_2_Right=camera->saveImage(camera->m_hDevHandle_6);//拍摄2#工位右侧相机图片
//Image_1_Left=camera->saveImage(camera->m_hDevHandle_1);//拍摄1#工位左侧相机图片
//Image_2_Left=camera->saveImage(camera->m_hDevHandle_4);//拍摄2#工位左侧相机图片
////================================黑色========================================================
//while(!write_Modbus(10673,1) && F_start==true)//通知PLC切换测试盒
//{
//  delay(100);
//}

//if(F_start==false)
//  return 0;
//read_Modbus(10679);//从PLC读测试架切换完成信号
//while(true && F_start==true)
//{

//   if (Data_Form_Plc==1)
//   {   Data_Form_Plc=0;
//       break;
//   }
//   else
//   {
//       delay(100);
//   }
//}

//Image_1_black=camera->saveImage(camera->m_hDevHandle_2);//拍摄1#工位黑色图片
//Image_2_black=camera->saveImage(camera->m_hDevHandle_5);//拍摄2#工位黑色图片

//if(F_start==false)
//  return 0;
////===============================蓝色====================================================
//while(!write_Modbus(10673,1) && F_start==true)//通知PLC切换测试盒
//{
//  delay(100);
//}
//if(F_start==false)
//  return 0;
//read_Modbus(10679);//从PLC读测试架切换完成信号
//while(true && F_start==true)
//{

//   if (Data_Form_Plc==1)
//   {   Data_Form_Plc=0;
//       break;
//   }
//   else
//   {
//       delay(100);
//   }
//}

//Image_1_blue=camera->saveImage(camera->m_hDevHandle_2);//拍摄1#工位蓝色图片
//Image_2_blue=camera->saveImage(camera->m_hDevHandle_5);//拍摄2#工位蓝色图片
//if(F_start==false)
//  return 0;
////============================红色=======================================================
//while(!write_Modbus(10673,1) && F_start==true)//通知PLC切换测试盒
//{
//  delay(100);
//}
//if(F_start==false)
//  return 0;
//read_Modbus(10679);//从PLC读测试架切换完成信号
//while(true && F_start==true)
//{

//   if (Data_Form_Plc==1)
//   {   Data_Form_Plc=0;
//       break;
//   }
//   else
//   {
//       delay(100);
//   }

//}

//Image_1_red=camera->saveImage(camera->m_hDevHandle_2);//拍摄1#工位红色图片
//Image_2_red=camera->saveImage(camera->m_hDevHandle_5);//拍摄2#工位红色图片

//if(F_start==false)
//  return 0;
////==============================绿色=====================================================
//while(!write_Modbus(10673,1) && F_start==true)//通知PLC切换测试盒
//{
//  delay(100);
//}
//if(F_start==false)
//  return 0;
//read_Modbus(10679);//从PLC读测试架切换完成信号
//while(true && F_start==true)
//{
//   if (Data_Form_Plc==1)
//   {   Data_Form_Plc=0;
//       break;
//   }
//   else
//   {
//       delay(100);
//   }
//}
//Image_1_green=camera->saveImage(camera->m_hDevHandle_2);//拍摄1#工位绿色图片
//Image_2_green=camera->saveImage(camera->m_hDevHandle_5);//拍摄2#工位绿色图片

//if(F_start==false)
//  return 0;
////============================灰色=======================================================
//while(!write_Modbus(10673,1) && F_start==true)//通知PLC切换测试盒
//{
//  delay(100);
//}

//if(F_start==false)
//  return 0;
//read_Modbus(10679);//从PLC读测试架切换完成信号
//while(true && F_start==true)
//{

//   if (Data_Form_Plc==1)
//   {   Data_Form_Plc=0;
//       break;
//   }
//   else
//   {
//       delay(100);
//   }
//}
//Image_1_gray=camera->saveImage(camera->m_hDevHandle_2);//拍摄1#工位灰色图片
//Image_2_gray=camera->saveImage(camera->m_hDevHandle_5);//拍摄2#工位灰色图片
//if(F_start==false)
//  return 0;
////===================================================================================

//if(true)
//{
// imwrite("D:\\MSV_image\\temp\\ceguang_1.bmp", ceguag_1);
// imwrite("D:\\MSV_image\\temp\\ceguang_2.bmp", ceguag_2);
// imwrite("D:\\MSV_image\\temp\\black_1.bmp", Image_1_black);
// imwrite("D:\\MSV_image\\temp\\black_2.bmp", Image_2_black);
// imwrite("D:\\MSV_image\\temp\\white_1.bmp", Image_1_white);
// imwrite("D:\\MSV_image\\temp\\white_2.bmp", Image_2_white);
// imwrite("D:\\MSV_image\\temp\\right_1.bmp", Image_1_Right);
// imwrite("D:\\MSV_image\\temp\\right_2.bmp", Image_2_Right);
// imwrite("D:\\MSV_image\\temp\\left_1.bmp", Image_1_Left);
// imwrite("D:\\MSV_image\\temp\\left_2.bmp", Image_2_Left);
// imwrite("D:\\MSV_image\\temp\\blue_1.bmp", Image_1_blue);
// imwrite("D:\\MSV_image\\temp\\blue_2.bmp", Image_2_blue);
// imwrite("D:\\MSV_image\\temp\\red_1.bmp", Image_1_red);
// imwrite("D:\\MSV_image\\temp\\red_2.bmp", Image_1_red);
// imwrite("D:\\MSV_image\\temp\\green_1.bmp", Image_1_green);
// imwrite("D:\\MSV_image\\temp\\green_2.bmp", Image_1_green);
// imwrite("D:\\MSV_image\\temp\\gray_1.bmp", Image_1_gray);
// imwrite("D:\\MSV_image\\temp\\gray_2.bmp", Image_2_gray);
//}
//time_1 = (double)mstimer.nsecsElapsed()/(double)1000000;
//ui->label_10->setText( QString("%1").arg(time_1));

////屏幕合格
//while(!write_Modbus(10736,1) && F_start==true)////拍照完成
//{
//delay(100);
//}
//F=true;
//return 1;




//cv::Mat HikvisionSDK::saveImage7(int id)
// {

//       //cv::Mat getImage;
//       void* handel=NULL;
//       switch(id)
//       {
//       case 1:handel=m_hDevHandle_1;break;
//       case 2:handel=m_hDevHandle_2;break;
//       case 3:handel=m_hDevHandle_3;break;
//       case 4:handel=m_hDevHandle_4;break;
//       case 5:handel=m_hDevHandle_5;break;
//       case 6:handel=m_hDevHandle_6;break;
//       }
//       unsigned char*  pFrameBuf = NULL;
////     if (NULL ==  pFrameBuf)
////     {

//        MVCC_INTVALUE stIntvalue = {0};
//       //  MVCC_INTVALUE stParam;
//        // memset(&stParam, 0, sizeof(MVCC_INTVALUE));
//         int nRet = MV_CC_GetIntValue(handel, "PayloadSize", &stIntvalue);
//         unsigned int nRecvBufSize = 0;
//         nRecvBufSize = stIntvalue.nCurValue; //一帧数据大小
////         if (nRet == MV_OK)
////         {
////            // qDebug()<<stIntvalue.nCurValue<<endl;

////         }

//         pFrameBuf = (unsigned char*)malloc(nRecvBufSize);
//   //  }
//     MV_FRAME_OUT_INFO_EX stImageInfo = {0};
//     memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
//     unsigned int    nTestFrameSize = 0;
//     while(1)
//         {
//             if (nTestFrameSize > 20)
//             {
//                 break;
//             }
//             nRet = MV_CC_GetOneFrameTimeout(handel,pFrameBuf,nRecvBufSize, &stImageInfo, 100);

//             if (MV_OK != nRet)
//             {  // qDebug()<<"第几次"<<nTestFrameSize<<endl;
//                 Sleep(100);
//             }
//             else
//             {
//                 //图片数据输入输出参数
////                 if(pImage==NULL)
////                {
//                     unsigned int    m_nBufSizeForSaveImage;
//                     m_nBufSizeForSaveImage = stImageInfo.nWidth * stImageInfo.nHeight * 3 + 2048;
//                     unsigned char* pImage=NULL;
//                     pImage = (unsigned char*)malloc(m_nBufSizeForSaveImage);
//               // }
//                  MV_SAVE_IMAGE_PARAM stParam ={0};
//                 //源数据
//                 stParam.pData         = pFrameBuf;                //原始图像数据
//                 stParam.nDataLen      = stImageInfo.nFrameLen;    //原始图像数据长度
//                 stParam.enPixelType   = stImageInfo.enPixelType;  //原始图像数据的像素格式
//                 stParam.nWidth        = stImageInfo.nWidth;       //图像宽
//                 stParam.nHeight       = stImageInfo.nHeight;      //图像高
//                 //目标数据
//                 stParam.enImageType   = MV_Image_Bmp;    // MV_Image_Jpeg  // MV_Image_Bmp     //需要保存的图像类型，转换成JPEG格式
//                 stParam.nBufferSize   = m_nBufSizeForSaveImage;                 //存储节点的大小
//                 stParam.pImageBuffer  = pImage;                   //输出数据缓冲区，存放转换之后的图片数据
//                 nRet = MV_CC_SaveImage(&stParam);
//                 if(MV_OK != nRet)
//                 {
//                     break;
//                 }

//                 switch(id)
//                 {
//                 case 1:cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage1);
//                        free(pImage);
//                        free(pFrameBuf);
//                        return getImage1;
//                 case 2: cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage2);
//                        free(pImage);
//                        free(pFrameBuf);
//                        return getImage2;

//                 case 3:cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage3);
//                        free(pImage);
//                        free(pFrameBuf);
//                        return getImage3;
//                 case 4: cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage4);
//                        free(pImage);
//                        free(pFrameBuf);
//                        return getImage4;
//                 case 5:cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage5);
//                        free(pImage);
//                        free(pFrameBuf);
//                        return getImage5;
//                 case 6: cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage6);
//                        free(pImage);
//                        free(pFrameBuf);
//                        return getImage6;


//                 }
////                 if(id==1)
////                 {
////                     cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage1);
////                    // getImage.release();
////                     free(pImage);
////                     free(pFrameBuf);
////                     return true;
////                 }
////                 if(id==2)
////                 {
////                      cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage2);
////                      free(pImage);
////                      free(pFrameBuf);
////                      return getImage2;
////                 }

////                 if(id==3)
////                 {
////                      cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage3);
////                      free(pImage);
////                      free(pFrameBuf);
////                      return getImage3;
////                 }

////                 if(id==4)
////                 {
////                      cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage4);
////                      free(pImage);
////                      free(pFrameBuf);
////                      return getImage4;
////                 }

////                 if(id==5)
////                 {
////                      cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage5);
////                      free(pImage);
////                      free(pFrameBuf);
////                      return getImage5;
////                 }

////                 if(id==6)
////                 {
////                      cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage6);
////                      free(pImage);
////                      free(pFrameBuf);
////                      return getImage6;
////                 }




//             }
//              nTestFrameSize++;
//         }
// }








//void Form_Camera::on_pushButton_8_clicked()//连接相机并显示画面
//{

//    camera->Find_Device();
//    for( int index=0;index<camera->m_stDevList.nDeviceNum;index++)
//    {
//           camera->connectCamera(index);

//    }
//                delay7(1000);

////                camera->startCamera( camera->m_hDevHandle_1);
////                HWND appwnd_1;
////                appwnd_1 = (HWND)ui->label_PreView_1->winId();
////                MV_CC_Display(camera->m_hDevHandle_1,appwnd_1);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_1, "PixelFormat",  PixelType_Gvsp_Mono8);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_1, "AcquisitionFrameRate", 20);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_1, "TriggerMode", MV_TRIGGER_MODE_OFF);
//                camera->startCamera(camera->m_hDevHandle_1);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_1, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_1,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_1,"ExposureTime", Num_1_ceLL_Exposure_white);
//                HWND appwnd_1;
//                appwnd_1 = (HWND)ui->label_PreView_1->winId();
//                MV_CC_Display(camera->m_hDevHandle_1,appwnd_1);

//                MV_CC_SetEnumValue(camera->m_hDevHandle_2, "PixelFormat",  PixelType_Gvsp_Mono8);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_2, "AcquisitionFrameRate", 20);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_2, "TriggerMode", MV_TRIGGER_MODE_OFF);
//                camera->startCamera( camera->m_hDevHandle_2);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_2, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_2,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_2,"ExposureTime", Num_1_L_Exposure_white);
//                MV_CC_SetBoolValue(camera->m_hDevHandle_2, "GammaEnable", 1);
//               // MV_CC_SetEnumValue(camera->m_hDevHandle_2, "GammaSelector", MV_GAMMA_SELECTOR_SRGB);
//                MV_CC_SetGamma(camera->m_hDevHandle_2, 0.7);
//                MV_CC_SetBalanceWhiteAuto(camera->m_hDevHandle_2, MV_BALANCEWHITE_AUTO_OFF);
//                MV_CC_SetBalanceRatioGreen(camera->m_hDevHandle_2, 1000);
//                MV_CC_SetBalanceRatioRed(camera->m_hDevHandle_2, 1782);
//                MV_CC_SetBalanceRatioBlue(camera->m_hDevHandle_2, 1400);
//                HWND appwnd_2;
//                appwnd_2 = (HWND)ui->label_PreView_2->winId();
//                MV_CC_Display(camera->m_hDevHandle_2,appwnd_2);


//                MV_CC_SetEnumValue(camera->m_hDevHandle_3, "PixelFormat", PixelType_Gvsp_BayerGR8);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_3, "AcquisitionFrameRate", 20);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_3, "TriggerMode", MV_TRIGGER_MODE_OFF);
//                camera->startCamera( camera->m_hDevHandle_3);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_3, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_3,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_3,"ExposureTime", Num_1_ceLR_Exposure_white);
//                HWND appwnd_3;
//                appwnd_3 = (HWND)ui->label_PreView_3->winId();
//                MV_CC_Display(camera->m_hDevHandle_3,appwnd_3);

//                MV_CC_SetEnumValue(camera->m_hDevHandle_4, "PixelFormat", PixelType_Gvsp_Mono8);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_4, "AcquisitionFrameRate", 10);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_4, "TriggerMode", MV_TRIGGER_MODE_OFF);
//                camera->startCamera( camera->m_hDevHandle_4);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_4, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_4,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_4,"ExposureTime", 1000);

//                HWND appwnd_4;
//                appwnd_4 = (HWND)ui->label_PreView_4->winId();
//                MV_CC_Display(camera->m_hDevHandle_4,appwnd_4);




///*                MV_CC_SetEnumValue(camera->m_hDevHandle_4, "PixelFormat", PixelType_Gvsp_BayerGR8);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_4, "AcquisitionFrameRate", 10);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_4, "TriggerMode", MV_TRIGGER_MODE_OFF);
//                camera->startCamera( camera->m_hDevHandle_4);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_4, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_4,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_4,"ExposureTime", 1000);

//                HWND appwnd_4;
//                appwnd_4 = (HWND)ui->label_PreView_4->winId();
//                MV_CC_Display(camera->m_hDevHandle_4,appwnd_4);
////                camera->startCamera( camera->m_hDevHandle_3);
////                HWND appwnd_3;
////                appwnd_3 = (HWND)ui->label_PreView_3->winId();
////                MV_CC_Display(camera->m_hDevHandle_3,appwnd_3);

////                camera->startCamera( camera->m_hDevHandle_4);
////                HWND appwnd_4;
////                appwnd_4 = (HWND)ui->label_PreView_4->winId();
////                MV_CC_Display(camera->m_hDevHandle_4,appwnd_4);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_5, "PixelFormat", PixelType_Gvsp_BayerGR8);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_5, "AcquisitionFrameRate", 10);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_5, "TriggerMode", MV_TRIGGER_MODE_OFF);
//                camera->startCamera( camera->m_hDevHandle_5);
//                HWND appwnd_5;
//                appwnd_5 = (HWND)ui->label_PreView_5->winId();
//                MV_CC_Display(camera->m_hDevHandle_5,appwnd_5);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_5, "ExposureMode", MV_EXPOSURE_MODE_TIMED);//555
//                MV_CC_SetEnumValue(camera->m_hDevHandle_5,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);//555
//                MV_CC_SetFloatValue(camera->m_hDevHandle_5,"ExposureTime", 1000);//555

//                MV_CC_SetEnumValue(camera->m_hDevHandle_6, "PixelFormat", PixelType_Gvsp_BayerGR8);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_6, "AcquisitionFrameRate", 10);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_6, "TriggerMode", MV_TRIGGER_MODE_OFF);
//                camera->startCamera( camera->m_hDevHandle_6);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_6, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//                MV_CC_SetEnumValue(camera->m_hDevHandle_6,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
//                MV_CC_SetFloatValue(camera->m_hDevHandle_6,"ExposureTime", 1000);

//                HWND appwnd_6;
//                appwnd_6 = (HWND)ui->label_PreView_6->winId();
//                MV_CC_Display(camera->m_hDevHandle_6,appwnd_6);
//                camera->startCamera( camera->m_hDevHandle_6);
//*/
////                HWND appwnd_6;
////                appwnd_6 = (HWND)ui->label_PreView_6->winId();
////                MV_CC_Display(camera->m_hDevHandle_6,appwnd_6);

//                //delay7(5000);

////    camera->connectCamera(ui->comboBox_Device_List->currentIndex(),&camera->m_hDevHandle_1);
////    char SerialNumber[32]={0};
////    sprintf_s(SerialNumber, "%s",camera->m_stDevList.pDeviceInfo[ui->comboBox_Device_List->currentIndex()]->SpecialInfo.stGigEInfo.chSerialNumber );

////    camera->startCamera( camera->m_hDevHandle_1);
////    MV_CC_SetEnumValue(camera->m_hDevHandle_1, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
////    MV_CC_SetEnumValue(camera->m_hDevHandle_1,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
////    MV_CC_SetFloatValue(camera->m_hDevHandle_1,"ExposureTime", 150000);
////    appwnd_1 = (HWND)ui->label_PreView_1->winId();
////    MV_CC_Display(camera->m_hDevHandle_1,appwnd_1);
////    ui->pushButton_Add_View_1->setEnabled(0);
////    ui->comboBox_Device_List->setItemData(0, 0, Qt::UserRole - 1);

//}


//// 算法参数栏 mura
//black_mura_step=-15;        //步长
//black_mura_size=9;          //窗口大小
//black_mura_gray_th=9;       //亮度阈值7
//black_mura_area_lower=100;  //面积阈值下限
//black_mura_area_upper=6100; //面积阈值上限5000
//black_mura_boder=5;         //外围宽度范围
//black_mura_corner=5;        //边角宽度范围
//black_mura_area_1=400;      //一级灰度面积
//black_mura_th_1=5;          //一级灰度差值
//black_mura_area_2=100;      //二级灰度面积
//black_mura_th_2=10;         //二级灰度差值

///*=========================================================
//* 函 数 名: mura
//* 功能描述: 白印检测  mura:指显示器亮度不均匀, 造成各种痕迹的现象.
//=========================================================*/
//bool mura(Mat image_ceguang ,Mat image_ceguang2,Mat *mresult1,Mat *mura,QString *causecolor)
//{
//    bool result=false;
//    Mat img_0=image_ceguang;
//    Mat img_gray=img_0.clone();

//    //主要屏幕部分参数设置
//    Mat binaryzation= Ployfit_Col7(img_gray, 3, 0, black_mura_gray_th);
//    imwrite("D:\\binaryzation.bmp",binaryzation);

////    //针对边界位置取原图的边界
////    //Mat img_top = img_gray(Rect(0, 0, img_gray.cols - 1, 10));
////    //Mat img_bottom = img_gray(Rect(0, img_gray.rows - 10, img_gray.cols - 1, 10));
////    Mat img_left = img_gray(Rect(0, 0, 150, img_gray.rows - 1));
////    //Mat img_right = img_gray(Rect(img_gray.cols - 10, 0, 10, img_gray.rows - 1));
////    Mat top_th, bottom_th, left_th, right_th;

////    left_th=Ployfit_Col7(img_left, 3, 0, black_mura_gray_th+6);

////    //针对边界位置拷贝
////    //top_th.copyTo(binaryzation(Rect(0, 0, binaryzation.cols - 1, 10)));                    //上边界
////    //bottom_th.copyTo(binaryzation(Rect(0, binaryzation.rows - 10, binaryzation.cols - 1, 10)));     //下边界
////    left_th.copyTo(binaryzation(Rect(0, 0, 150, binaryzation.rows - 1)));                   //左边界
////    //right_th.copyTo(binaryzation(Rect(binaryzation.cols - 10, 0, 10, binaryzation.rows - 1)));      //右边界
////    imwrite("D:\\binaryzation1.bmp",binaryzation);

//    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3,3));
//    Mat  erode1;
//    erode(binaryzation, erode1, structure_element1);

//    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(6,6));
//    Mat dilate1;
//    dilate(erode1, dilate1, structure_element2);
//    imwrite("D:\\dilate1.bmp",dilate1);

//    dilate1(Rect(dilate1.cols - 280, dilate1.rows-360, 280, 360)) = uchar(0);	     //去除易撕贴部分右下角
//    dilate1(Rect(0, 0, dilate1.cols - 1, 10)) = uchar(0);                            //上
//    dilate1(Rect(0, dilate1.rows - 10, dilate1.cols - 1, 10)) = uchar(0);            //下
//    dilate1(Rect(0, 0, 200, dilate1.rows - 1)) = uchar(0);                           //左
//    dilate1(Rect(dilate1.cols - 10, 0, 10, dilate1.rows - 1)) = uchar(0);            //右

//    imwrite("D:\\dilatefinal.bmp",dilate1);

//    vector<vector<Point>> contours;
//    findContours( dilate1, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//    {
//        double area = contourArea(contours[i]);
//        if (area>black_mura_area_lower and area<black_mura_area_upper)
//        {
//            boundRect[i] = boundingRect(Mat(contours[i]));
//            float w=boundRect[i].width;
//            float h=boundRect[i].height;
//            RotatedRect rect = minAreaRect(contours[i]);  //包覆轮廓的最小斜矩形 划伤缺陷有旋转特点
//            double w1 = rect.size.height;
//            double h1 = rect.size.width;
//            int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
//            int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
//            int X_2=boundRect[i].br().x;//矩形右下角X坐标值
//            int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
//            int x_point=X_1+round(w/2);
//            int y_point=Y_1+round(h/2);
//            if((max(w1 / h1, h1 / w1) <= 1.6))
//            {
//                X_1=X_1-black_mura_boder-int(w);
//                Y_1=Y_1-black_mura_boder-int(h);
//                X_2=X_2+black_mura_boder+int(w);
//                Y_2=Y_2+black_mura_boder+int(h);
//                if(X_1<0)
//                {
//                    X_1=0;
//                }
//                if(Y_1<0)
//                {
//                    Y_1=0;
//                }
//                if(X_2>binaryzation.cols-1)
//                {
//                    X_2=binaryzation.cols-1;
//                }
//                if(Y_2>binaryzation.rows-1)
//                {
//                    Y_2=binaryzation.rows-1;
//                }
//                Mat temp_ceguang=img_gray(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
//                double mean_ceguang = mean(temp_ceguang)[0];
//                //四个边角区域不检测
//                if((x_point<black_mura_corner and y_point<black_mura_corner) or (binaryzation.cols-x_point<black_mura_corner and binaryzation.rows-y_point<black_mura_corner) or(x_point<black_mura_corner and binaryzation.rows-y_point<black_mura_corner) or (binaryzation.cols-x_point<black_mura_corner and y_point<black_mura_corner))//(img_5.cols-x_point<5 and or img_5.rows-x_point<15 or x_point<15 or y_point<15) and(mean_ceguang)>200
//                {
//                    continue;
//                }
//                //屏幕上面和下面区域不检测
//                if((y_point<48) or (binaryzation.rows-y_point<70))
//                {
//                    continue;
//                }

//                double yuzhi=img_gray.at<uchar>(y_point,x_point)-mean_ceguang;
//                string b = "yuzhi " + to_string(yuzhi) ;
//                //putText(img_0, b, Point(X_1 + 10, Y_1 + 10), FONT_HERSHEY_SIMPLEX, 0.9, Scalar(0, 0, 255), 1, 8, 0);
//                //imwrite("D:\\mura0507.bmp",img_0);

//                if(img_gray.at<uchar>(y_point,x_point)-mean_ceguang>8)//black_gray.at<uchar>(y_point,x_point)-mean_ceguang>black_lightPoint_diff
//                {
//                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
//                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
//                    Mat temp_mask=dilate1(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
//                    int w7=(X_2-X_1)*(Y_2-Y_1);
//                    Mat R_m0;
//                    Mat R_sd0;
//                    meanStdDev(temp_mask, R_m0, R_sd0);
//                    double area7=R_m0.at<double>(0,0)*w7/255;
//                    // qDebug()<<"实际面积"<<area7<<endl;
//                    Mat temp_mask2=~temp_mask;
//                    Mat img_6;
//                    bitwise_and(temp_ceguang,temp_ceguang,img_6,temp_mask);
//                    //  imshow("",img_5);
//                    //  waitKey(0);
//                    Mat img_7;
//                    bitwise_and(temp_ceguang,temp_ceguang,img_7,temp_mask2);
//                    // imshow("",img_6);
//                    // waitKey(0)
//                    Mat R_m;
//                    Mat R_sd;
//                    meanStdDev(img_6, R_m, R_sd);
//                    // qDebug()<<R_m.at<double>(0,0)*w7/area7<<endl;
//                    Mat R_m1;
//                    Mat R_sd1;
//                    meanStdDev(img_7, R_m1, R_sd1);

//                    //上下边缘处亮度不均匀分割出较小噪声点
//                    vector<vector<Point>> contours1;
//                    findContours(temp_mask, contours1, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//                    double domainnum=contours1.size();
//                    if (domainnum >5)
//                    {
//                        continue;
//                    }

//                    if ((((R_m.at<double>(0,0)*w7/area7)-(R_m1.at<double>(0,0)*w7/(w7-area7))>black_mura_th_1) and (area7>black_mura_area_1) )or(((R_m.at<double>(0,0)*w7/area7)-(R_m1.at<double>(0,0)*w7/(w7-area7))>black_mura_th_2) and (area7>black_mura_area_2) ) )
//                    {
//                        rectangle(img_0 , cvPoint(X_1, Y_1), cvPoint(X_2, Y_2), Scalar(255,255,255), 5, 8, 0);
//                        Point p3(x_point, y_point);
//                        circle(img_0,p3,3,Scalar(0,0,255),-1);
//                        //imwrite("D:\\muraresult.bmp",img_0);
//                        result=true;

//                        string abc = "area: " + to_string(area) +"num: " + to_string(domainnum);
//                        putText(img_0, abc, Point(X_1 + 10, Y_1 +20), FONT_HERSHEY_SIMPLEX, 1.4, Scalar(0, 0, 255), 1, 8, 0);
//                        imwrite("D:\\muraresult.bmp",img_0);
//                    }
//                }

//            }
//        }
//    }
//    *mura=binaryzation;
//    if(result==true)
//    {
//        *mresult1=img_0;
//        *causecolor="白印";
//        return result;
//    }
//    return result;
//}

///*=========================================================
//* 函 数 名: color_red
//* 功能描述: 红屏 暂时没有用到
//=========================================================*/
//bool color_red(Mat image_red ,Mat *mresult ,QString *causecolor)
//{
//    Mat Hsv_watermark;
//    Mat mask_1;
//    cvtColor(image_red,Hsv_watermark, COLOR_BGR2HSV);
//    inRange(Hsv_watermark, Scalar(0, 43, 46), Scalar(50,255, 255),mask_1);
//    return true;
//}




//白印缺陷5月16日早上版本
//bool mura(Mat image_ceguang ,Mat image_ceguang2,Mat *mresult1,Mat *mura,QString *causecolor)
//{
//    bool result=false;
//    Mat img_0=image_ceguang;
//    Mat img_gray=img_0.clone();

//    //主要屏幕部分参数设置
//    Mat binaryzation= Ployfit_Col7(img_gray, 3, 0, black_mura_gray_th);
//    imwrite("D:\\binaryzation.bmp",binaryzation);

////    //针对边界位置取原图的边界
////    //Mat img_top = img_gray(Rect(0, 0, img_gray.cols - 1, 10));
////    //Mat img_bottom = img_gray(Rect(0, img_gray.rows - 10, img_gray.cols - 1, 10));
////    Mat img_left = img_gray(Rect(0, 0, 150, img_gray.rows - 1));
////    //Mat img_right = img_gray(Rect(img_gray.cols - 10, 0, 10, img_gray.rows - 1));
////    Mat top_th, bottom_th, left_th, right_th;

////    left_th=Ployfit_Col7(img_left, 3, 0, black_mura_gray_th+6);

////    //针对边界位置拷贝
////    //top_th.copyTo(binaryzation(Rect(0, 0, binaryzation.cols - 1, 10)));                    //上边界
////    //bottom_th.copyTo(binaryzation(Rect(0, binaryzation.rows - 10, binaryzation.cols - 1, 10)));     //下边界
////    left_th.copyTo(binaryzation(Rect(0, 0, 150, binaryzation.rows - 1)));                   //左边界
////    //right_th.copyTo(binaryzation(Rect(binaryzation.cols - 10, 0, 10, binaryzation.rows - 1)));      //右边界
////    imwrite("D:\\binaryzation1.bmp",binaryzation);

//    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3,3));
//    Mat  erode1;
//    erode(binaryzation, erode1, structure_element1);

//    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(6,6));
//    Mat dilate1;
//    dilate(erode1, dilate1, structure_element2);
//    imwrite("D:\\dilate1.bmp",dilate1);

//    dilate1(Rect(dilate1.cols - 280, dilate1.rows-360, 280, 360)) = uchar(0);	     //去除易撕贴部分右下角
//    dilate1(Rect(0, 0, dilate1.cols - 1, 10)) = uchar(0);                            //上
//    dilate1(Rect(0, dilate1.rows - 10, dilate1.cols - 1, 10)) = uchar(0);            //下
//    dilate1(Rect(0, 0, 200, dilate1.rows - 1)) = uchar(0);                           //左
//    dilate1(Rect(dilate1.cols - 10, 0, 10, dilate1.rows - 1)) = uchar(0);            //右

//    imwrite("D:\\dilatefinal.bmp",dilate1);

//    vector<vector<Point>> contours;
//    findContours( dilate1, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//    {
//        double area = contourArea(contours[i]);
//        if (area>black_mura_area_lower and area<black_mura_area_upper)
//        {
//            boundRect[i] = boundingRect(Mat(contours[i]));
//            float w=boundRect[i].width;
//            float h=boundRect[i].height;
//            RotatedRect rect = minAreaRect(contours[i]);  //包覆轮廓的最小斜矩形 划伤缺陷有旋转特点
//            double w1 = rect.size.height;
//            double h1 = rect.size.width;
//            int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
//            int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
//            int X_2=boundRect[i].br().x;//矩形右下角X坐标值
//            int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
//            int x_point=X_1+round(w/2);
//            int y_point=Y_1+round(h/2);
//            if((max(w1 / h1, h1 / w1) <= 1.6))
//            {
//                X_1=X_1-black_mura_boder-int(w);
//                Y_1=Y_1-black_mura_boder-int(h);
//                X_2=X_2+black_mura_boder+int(w);
//                Y_2=Y_2+black_mura_boder+int(h);
//                if(X_1<0)
//                {
//                    X_1=0;
//                }
//                if(Y_1<0)
//                {
//                    Y_1=0;
//                }
//                if(X_2>binaryzation.cols-1)
//                {
//                    X_2=binaryzation.cols-1;
//                }
//                if(Y_2>binaryzation.rows-1)
//                {
//                    Y_2=binaryzation.rows-1;
//                }
//                Mat temp_ceguang=img_gray(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
//                double mean_ceguang = mean(temp_ceguang)[0];
//                //四个边角区域不检测
//                if((x_point<black_mura_corner and y_point<black_mura_corner) or (binaryzation.cols-x_point<black_mura_corner and binaryzation.rows-y_point<black_mura_corner) or(x_point<black_mura_corner and binaryzation.rows-y_point<black_mura_corner) or (binaryzation.cols-x_point<black_mura_corner and y_point<black_mura_corner))//(img_5.cols-x_point<5 and or img_5.rows-x_point<15 or x_point<15 or y_point<15) and(mean_ceguang)>200
//                {
//                    continue;
//                }
//                //屏幕上面和下面区域不检测
//                if((y_point<48) or (binaryzation.rows-y_point<70))
//                {
//                    continue;
//                }

//                double yuzhi=img_gray.at<uchar>(y_point,x_point)-mean_ceguang;
//                string b = "yuzhi " + to_string(yuzhi) ;
//                //putText(img_0, b, Point(X_1 + 10, Y_1 + 10), FONT_HERSHEY_SIMPLEX, 0.9, Scalar(0, 0, 255), 1, 8, 0);
//                //imwrite("D:\\mura0507.bmp",img_0);

//                if(img_gray.at<uchar>(y_point,x_point)-mean_ceguang>8)//black_gray.at<uchar>(y_point,x_point)-mean_ceguang>black_lightPoint_diff
//                {
//                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
//                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
//                    Mat temp_mask=dilate1(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
//                    int w7=(X_2-X_1)*(Y_2-Y_1);
//                    Mat R_m0;
//                    Mat R_sd0;
//                    meanStdDev(temp_mask, R_m0, R_sd0);
//                    double area7=R_m0.at<double>(0,0)*w7/255;
//                    // qDebug()<<"实际面积"<<area7<<endl;
//                    Mat temp_mask2=~temp_mask;
//                    Mat img_6;
//                    bitwise_and(temp_ceguang,temp_ceguang,img_6,temp_mask);
//                    //  imshow("",img_5);
//                    //  waitKey(0);
//                    Mat img_7;
//                    bitwise_and(temp_ceguang,temp_ceguang,img_7,temp_mask2);
//                    // imshow("",img_6);
//                    // waitKey(0)
//                    Mat R_m;
//                    Mat R_sd;
//                    meanStdDev(img_6, R_m, R_sd);
//                    // qDebug()<<R_m.at<double>(0,0)*w7/area7<<endl;
//                    Mat R_m1;
//                    Mat R_sd1;
//                    meanStdDev(img_7, R_m1, R_sd1);

//                    //上下边缘处亮度不均匀分割出较小噪声点
//                    vector<vector<Point>> contours1;
//                    findContours(temp_mask, contours1, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//                    double domainnum=contours1.size();
//                    if (domainnum >5)
//                    {
//                        continue;
//                    }

//                    if ((((R_m.at<double>(0,0)*w7/area7)-(R_m1.at<double>(0,0)*w7/(w7-area7))>black_mura_th_1) and (area7>black_mura_area_1) )or(((R_m.at<double>(0,0)*w7/area7)-(R_m1.at<double>(0,0)*w7/(w7-area7))>black_mura_th_2) and (area7>black_mura_area_2) ) )
//                    {
//                        rectangle(img_0 , cvPoint(X_1, Y_1), cvPoint(X_2, Y_2), Scalar(255,255,255), 5, 8, 0);
//                        Point p3(x_point, y_point);
//                        circle(img_0,p3,3,Scalar(0,0,255),-1);
//                        //imwrite("D:\\muraresult.bmp",img_0);
//                        result=true;

//                        string abc = "area: " + to_string(area) +"num: " + to_string(domainnum);
//                        putText(img_0, abc, Point(X_1 + 10, Y_1 +20), FONT_HERSHEY_SIMPLEX, 1.4, Scalar(0, 0, 255), 1, 8, 0);
//                        imwrite("D:\\muraresult.bmp",img_0);
//                    }
//                }

//            }
//        }
//    }
//    *mura=binaryzation;
//    if(result==true)
//    {
//        *mresult1=img_0;
//        *causecolor="白印";
//        return result;
//    }
//    return result;
//}

//左右相机侧面图像提取的方法
//bool roi_ceshi(Mat src_white,int border_white,Mat *Mwhite)//左右侧面相机白色底色下提取黑色ROI
//{
//    Mat img_gray;
//    cvtColor(src_white,img_gray,CV_BGR2GRAY);
//    Mat th1;
//    threshold(img_gray, th1,50, 255, CV_THRESH_BINARY);//100->60
//    //    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3,3));
//    //    Mat  erode1;
//    //    erode(th1, erode1, structure_element1);
//    //    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(9,9));
//    //    Mat dilate1;
//    //    dilate(erode1, dilate1, structure_element2);
//    Mat img_temp1,img_temp2,img_temp3,img_temp4;
//    int X_1,Y_1,X_2,Y_2,w,h;
//    int X_lt,Y_lt,X_rt,Y_rt,X_lb,Y_lb,X_rb,Y_rb,w1,h1;
//    vector<vector<Point>> contours1;
//    vector<vector<Point>> contours;
//    findContours(th1, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//    {
//        double area = contourArea(contours[i]);
//        // qDebug()<<area<<endl;
//        //qDebug()<<area<<endl;
//        if (area>100000 and area<380000)
//        {

//            boundRect[i] = boundingRect(Mat(contours[i]));
//            X_1=boundRect[i].tl().x;
//            Y_1=boundRect[i].tl().y;
//            X_2=boundRect[i].br().x;
//            Y_2=boundRect[i].br().y;
//            w=boundRect[i].width;
//            h=boundRect[i].height;
//            img_temp1=th1(Rect(X_1,Y_1,100,100));
//            findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//            X_lt=X_1+boundingRect(Mat(contours1[0])).tl().x;
//            Y_lt=Y_1+boundingRect(Mat(contours1[0])).tl().y;

//            img_temp1=th1(Rect(X_1+w-100,Y_1,100,100));
//            findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//            X_rt=X_1+w-(100-boundingRect(Mat(contours1[0])).width);
//            Y_rt=Y_1+(100-boundingRect(Mat(contours1[0])).height);

//            img_temp1=th1(Rect(X_1,Y_1+h-100,100,100));
//            findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//            X_lb=X_1+(boundingRect(Mat(contours1[0])).tl().x);
//            Y_lb=Y_1+h-(100-boundingRect(Mat(contours1[0])).height);

//            img_temp1=th1(Rect(X_2-100,Y_2-100,100,100));
//            findContours(img_temp1, contours1,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//            X_rb=X_1+w-(100-boundingRect(Mat(contours1[0])).width);
//            Y_rb=Y_1+h-(100-boundingRect(Mat(contours1[0])).height);

//        }
//    }


//    //    circle(src_white, cv::Point2f(X_lt, Y_lt), 10, Scalar(0, 255, 0), 2);
//    //    circle(src_white, cv::Point2f(X_rt, Y_rt), 10, Scalar(0, 255, 0), 2);
//    //    circle(src_white, cv::Point2f(X_lb, Y_lb), 10, Scalar(0, 255, 0), 2);
//    //    circle(src_white, cv::Point2f(X_rb, Y_rb), 10, Scalar(0, 255, 0), 2);

//    cv::Point2f src_points[] = {

//        cv::Point2f(X_lt-border_white, Y_lt-border_white),

//        cv::Point2f(X_rt+border_white, Y_rt-border_white),

//        cv::Point2f(X_lb-border_white, Y_lb+border_white),

//        cv::Point2f(X_rb+border_white, Y_rb+border_white)
//    };
//    cv::Point2f dst_points[] = {

//        cv::Point2f(0, 0),

//        cv::Point2f(620+2*border_white, 0),

//        cv::Point2f(0, 600+2*border_white),

//        cv::Point2f(620+2*border_white, 600+2*border_white) };

//    *Mwhite= cv::getPerspectiveTransform(src_points, dst_points);

//    return true;

//}


//5.5寸相机曝光参数值
//相机参数栏
//Exposure_Ceguang_Front=30000;              //前黑白相机侧光曝光值
//Exposure_Ceguang_Back=30000;               //后黑白相机侧光曝光值
//Exposure_Ceguang_Left=30000;               //左黑白相机侧光曝光值
//Exposure_Ceguang_Right=30000;              //右黑白相机侧光曝光值
//Exposure_Ceguang_Main=15000;               //主黑白相机侧光曝光值

//Exposure_White_Black_Left=300000;          //左黑白白底相机曝光值
//Exposure_White_Black_Middle=35000;         //主黑白白底相机曝光值      5.5寸屏幕2万
//Exposure_White_Color_Right=300000;         //右侧黑白白底相机曝光值
//Exposure_Ceguang_Back=300000;              //后侧黑白白底相机曝光值
//Exposure_Ceguang_Front=300000;             //前侧黑白白底相机曝光值
//Exposure_White_Color_Middle=1800;          //主踩死白底相机曝光值

//Exposure_Black_Middle=200000;              //主黑白相机黑色底色曝光值
//Exposure_Black_Color=780000;               //主彩色相机黑色底色曝光值

//Num_1_L_Exposure_white=11000;              //其余颜色曝光值

/*=========================================================
* 函 数 名: white_point7
* 功能描述: 白点检测
=========================================================*/
//bool white_point7(Mat image_black,Mat *mresult,QString *causecolor)
//{   bool result=false;
//    Mat black_gray;
//    cvtColor(image_black,black_gray,CV_BGR2GRAY);
//    Mat gray_clone=black_gray.clone();
//    //    Mat mask_black;
//    //    threshold(black_gray, mask_black,30, 255, CV_THRESH_BINARY);//100->60
//    Scalar tempVal1;
//    int co,ro;
//    for( int row=0;row<black_gray.rows;row=row+black_lightPoint_step)//滑动窗口分块检测
//    {
//        for( int col=0;col<black_gray.cols;col=col+black_lightPoint_step)
//        {  co=col;
//            ro=row;
//            if(ro+black_lightPoint_size>black_gray.rows-1)
//            {
//                ro=black_gray.rows-black_lightPoint_size-1;
//                row=black_gray.rows;
//            }
//            if(co+black_lightPoint_size>black_gray.cols-1)
//            {
//                co=black_gray.cols-black_lightPoint_size-1;
//                col=black_gray.cols;
//            }
//            Rect rect_wp(co,ro,black_lightPoint_size,black_lightPoint_size);
//            Mat temp_wp=black_gray(rect_wp);
//            tempVal1 =mean( temp_wp );
//            double matMean_wp = tempVal1.val[0];
//            Mat th_wp;
//            threshold(temp_wp, th_wp,matMean_wp+black_lightPoint_gray_th, 255, CV_THRESH_BINARY);//10->20
//            th_wp.copyTo(gray_clone(rect_wp));
//        }
//    }
//    //======================================================
//    vector<vector<Point>> contours;
//    findContours( gray_clone(Rect(1,1,gray_clone.cols-2,gray_clone.rows-2)), contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//    {
//        double area = contourArea(contours[i]);
//        if (area>black_lightPoint_area_lower and area<black_lightPoint_area_upper)
//        {
//            boundRect[i] = boundingRect(Mat(contours[i]));
//            float w=boundRect[i].width;
//            float h=boundRect[i].height;
//            int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
//            int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
//            int X_2=boundRect[i].br().x;//矩形右下角X坐标值
//            int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
//            int x_point=X_1+round(w/2);
//            int y_point=Y_1+round(h/2);
//            if(max(w/h,h/w)<4)
//            {
//                X_1=X_1-25-int(w);
//                Y_1=Y_1-25-int(h);
//                X_2=X_2+25+int(w);
//                Y_2=Y_2+25+int(h);
//                if(X_1<0)
//                {
//                    X_1=0;
//                }
//                if(Y_1<0)
//                {
//                    Y_1=0;
//                }
//                if(X_2>image_black.cols-1)
//                {
//                    X_2=image_black.cols-1;
//                }
//                if(Y_2>image_black.rows-1)
//                {
//                    Y_2=image_black.rows-1;
//                }
//                Mat temp_ceguang=black_gray(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
//                double mean_ceguang = mean(temp_ceguang)[0];
//                //gray_ceguang.at<uchar>(y_point,x_point)
//                if(black_gray.at<uchar>(y_point,x_point)-mean_ceguang>black_lightPoint_diff)
//                {
//                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
//                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
//                    rectangle(image_black , top_lef4, bottom_right4, Scalar(0,0,255), 2, 8, 0);
//                    result=true;
//                }
//            }
//        }
//    }
//    if(result==true)
//    {
//        // imwrite(result_lujing,image_black);
//        *mresult=image_black;
//        *causecolor="亮点";
//        return result;
//    }
//    //  imwrite(result_lujing,gray_clone);
//    return result;
//}

///*=========================================================
//* 函 数 名: yiwu_new_sample
//* 功能描述: 异物检测 暂时没有用到 用的是上面的yiwu_7
//=========================================================*/
//bool yiwu_new_sample(Mat white_yiwu,Mat *mresult)
//{   bool result=false;
//    //==========爆灯检测===================
//    Mat th_roi2_gray;
//    cvtColor(white_yiwu,th_roi2_gray,CV_BGR2GRAY);
//    Mat th_roi2;
//    double mg = mean(th_roi2_gray)[0];
//    threshold(th_roi2_gray, th_roi2,mg+50, 255, CV_THRESH_BINARY);
//    double m2 = mean(th_roi2)[0];
//    if(m2>2)
//    {
//        //  imwrite(result_lujing,white_yiwu);
//        causeColor="爆灯或死灯";
//        return true;
//    }
//    threshold(th_roi2_gray, th_roi2,mg-50, 255, CV_THRESH_BINARY_INV);
//    m2 = mean(th_roi2)[0];
//    if(m2>2)
//    {
//        //imwrite(result_lujing,white_yiwu);
//        causeColor="爆灯或死灯";
//        return true;
//    }
//    //=====================================
//    //    Rect rect1= yisitei(white_yiwu);
//    //    int rect_x=rect1.x;
//    //    int rect_y=rect1.y;
//    //    int rect_w=rect1.width;
//    //    int rect_h=rect1.height;
//    Mat img_gray;
//    cvtColor(white_yiwu,img_gray,CV_BGR2GRAY);
//    Mat th1;
//    adaptiveThreshold(img_gray,th1, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 11,7);
//    vector<vector<Point>> contours;
//    findContours( th1, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//    {
//        double area = contourArea(contours[i]);

//        if (area>250
//                and area<500)
//        {
//            boundRect[i] = boundingRect(Mat(contours[i]));
//            float w=boundRect[i].width;
//            float h=boundRect[i].height;
//            int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
//            int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
//            int X_2=boundRect[i].br().x;//矩形右下角X坐标值
//            int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
//            int x_point=X_1+round(w/2);
//            int y_point=Y_1+round(h/2);
//            bool red_mark=true;
//            //                      if(x_point>rect_x and x_point<rect_x+rect_w  and y_point>rect_y and y_point<rect_y+rect_h)
//            //                      {
//            //                          red_mark=false;
//            //                      }
//            if((red_mark) and ( max(w/h,h/w)<4))

//            {
//                X_1=X_1-10;
//                Y_1=Y_1-10;
//                X_2=X_2+10+int(w);
//                Y_2=Y_2+10+int(h);
//                if(X_1<0)
//                {
//                    X_1=0;
//                }
//                if(Y_1<0)
//                {
//                    Y_1=0;
//                }
//                if(X_2>white_yiwu.cols-1)
//                {
//                    X_2=white_yiwu.cols-1;
//                }
//                if(Y_2>white_yiwu.rows-1)
//                {
//                    Y_2=white_yiwu.rows-1;
//                }
//                CvPoint top_lef4 = cvPoint(X_1, Y_1);
//                CvPoint bottom_right4 = cvPoint(X_2, Y_2);
//                rectangle(white_yiwu , top_lef4, bottom_right4, Scalar(0,0,255), 1, 8, 0);
//                result=true;}
//        }
//    }
//    if(result==true)
//    {
//        *mresult=white_yiwu;
//        causeColor="背光异物";
//        result= true;
//    }//imwrite(result_lujing,th1);
//    return result;
//}

///*=========================================================
//* 函 数 名: yiwu
//* 功能描述: 异物检测 暂时没有用到
//=========================================================*/
//bool yiwu(Mat image_white,Mat ceguang)
//{   bool result=false;
//    Mat white_clone=image_white.clone();
//    Mat img_gray;
//    cvtColor(image_white,img_gray,CV_BGR2GRAY);
//    Mat ceguang_gray;
//    cvtColor(ceguang,ceguang_gray,CV_BGR2GRAY);
//    Mat Hsv_watermark;
//    //    Mat mask_1;
//    //    cvtColor(image_white,Hsv_watermark, COLOR_BGR2HSV);
//    //    inRange(Hsv_watermark, Scalar(50, 10, 0), Scalar(150,150, 255),mask_1);//蓝色inRange(Hsv_watermark, Scalar(90, 20, 200), Scalar(100,100, 255),mask_1);
//    //     // 粉色inRange(Hsv_watermark, Scalar(110, 20, 0), Scalar(150,70, 255),mask_1);//白色Scalar(110, 0, 0), Scalar(150,20, 255)
//    //    Mat mask_2;
//    //    inRange(Hsv_watermark, Scalar(0, 20, 0), Scalar(255,100, 255),mask_2);
//    //    Mat mask_3;
//    //    inRange(Hsv_watermark, Scalar(0, 0, 0), Scalar(255,20, 255),mask_3);
//    //    bitwise_or(mask_1,mask_2,mask_1);
//    //    bitwise_or(mask_1,mask_3,mask_1);

//    //    Mat mask_black;
//    //   // threshold(black_gray, mask_black,30, 255, CV_THRESH_BINARY);//100->60
//    //   Scalar tempVal1;
//    int co,ro;
//    for( int row=0;row<image_white.rows;row=row+15)//滑动窗口分块检测
//    {
//        for( int col=0;col<image_white.cols;col=col+15)
//        {  co=col;
//            ro=row;
//            if(ro+20>image_white.rows-1)
//            {
//                ro=image_white.rows-21;
//                row=image_white.rows;
//            }
//            if(co+20>image_white.cols-1)
//            {
//                co=image_white.cols-21;
//                col=image_white.cols;
//            }

//            Rect rect_wp(co,ro,20,20);
//            Mat temp_wp=Hsv_watermark(rect_wp);
//            Mat mask;
//            inRange(temp_wp, Scalar(50, 10, 0), Scalar(150,150, 255),mask);
//            mask.copyTo(img_gray(rect_wp));
//        }
//    }
//    //======================================================
//    vector<vector<Point>> contours;
//    findContours( img_gray(Rect(1,1,img_gray.cols-2,img_gray.rows-2)), contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//    {
//        double area = contourArea(contours[i]);
//        if (area>10 and area<1000)
//        {
//            boundRect[i] = boundingRect(Mat(contours[i]));
//            float w=boundRect[i].width;
//            float h=boundRect[i].height;
//            int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
//            int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
//            int X_2=boundRect[i].br().x;//矩形右下角X坐标值
//            int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
//            int x_point=X_1+round(w/2);
//            int y_point=Y_1+round(h/2);
//            if(max(w/h,h/w)<4)
//            {
//                //                  X_1=X_1-10;
//                //                  Y_1=Y_1-10;
//                //                  X_2=X_2+10+int(w);
//                //                  Y_2=Y_2+10+int(h);
//                //                   if(X_1<0)
//                //                   {
//                //                       X_1=0;
//                //                   }
//                //                   if(Y_1<0)
//                //                   {
//                //                       Y_1=0;
//                //                   }
//                //                   if(X_2>img_gray.cols-1)
//                //                   {
//                //                      X_2=img_gray.cols-1;
//                //                   }
//                //                   if(Y_2>img_gray.rows-1)
//                //                   {
//                //                       Y_2=img_gray.rows-1;
//                //                   }
//                // Mat temp_ceguang=ceguang_gray(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
//                // double mean_ceguang = mean(temp_ceguang)[0];
//                //gray_ceguang.at<uchar>(y_point,x_point)
//                if(ceguang_gray.at<uchar>(y_point,x_point)>10)
//                {
//                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
//                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
//                    rectangle(image_white , top_lef4, bottom_right4, Scalar(0,0,255), 2, 8, 0);
//                    result=true;
//                }
//            }
//        }
//    }
//    if(result==true)
//    {
//        // imwrite(result_lujing,image_black);
//        causeColor="异物";
//        return result;
//    }
//    //  imwrite(result_lujing,gray_clone);
//    return result;
//}

///*=========================================================
//* 函 数 名: yiwu7
//* 功能描述: 异物检测 暂时没有用到
//=========================================================*/
//bool yiwu7(Mat image_white,Mat *mresult)//颜色检测
//{   bool result=false;
//    // Mat ceguang_gray;
//    // cvtColor(ceguang,ceguang_gray,CV_BGR2GRAY);
//    Mat Hsv_watermark;
//    cvtColor(image_white,Hsv_watermark, COLOR_BGR2HSV);
//    Mat mask;
//    inRange(Hsv_watermark, Scalar(0, 0, 0), Scalar(180,255, 150),mask);
//    //======================================================
//    vector<vector<Point>> contours;
//    findContours( mask(Rect(1,1,mask.cols-2,mask.rows-2)), contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    for(vector<int>::size_type i = 0; i < contours.size(); i++)
//    {
//        double area = contourArea(contours[i]);
//        if (area>10 and area<1000)
//        {
//            boundRect[i] = boundingRect(Mat(contours[i]));
//            float w=boundRect[i].width;
//            float h=boundRect[i].height;
//            int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
//            int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
//            int X_2=boundRect[i].br().x;//矩形右下角X坐标值
//            int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
//            int x_point=X_1+round(w/2);
//            int y_point=Y_1+round(h/2);
//            if(max(w/h,h/w)<4)
//            {
//                X_1=X_1-10;
//                Y_1=Y_1-10;
//                X_2=X_2+10+int(w);
//                Y_2=Y_2+10+int(h);
//                if(X_1<0)
//                {
//                    X_1=0;
//                }
//                if(Y_1<0)
//                {
//                    Y_1=0;
//                }
//                if(X_2>mask.cols-1)
//                {
//                    X_2=mask.cols-1;
//                }
//                if(Y_2>mask.rows-1)
//                {
//                    Y_2=mask.rows-1;
//                }
//                //     Mat temp_ceguang=ceguang_gray(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
//                //    double mean_ceguang = mean(temp_ceguang)[0];

//                //                if((ceguang_gray.at<uchar>(y_point,x_point)-mean_ceguang)<10)
//                //                   {
//                CvPoint top_lef4 = cvPoint(X_1, Y_1);
//                CvPoint bottom_right4 = cvPoint(X_2, Y_2);
//                rectangle(image_white , top_lef4, bottom_right4, Scalar(0,0,255), 2, 8, 0);
//                result=true;
//                //  }
//            }
//        }
//    }
//    if(result==true)
//    {
//        *mresult=image_white;
//        causeColor="异物";
//        return result;
//    }
//    return result;
//}

/*=========================================================
* 函 数 名: white_point
* 功能描述: 白点 有缺陷 '白点'
* 输入: 左右侧相机白底图像	white_yiwu左相机白底图像  ceguang右相机白底图像
* //3月2日修改白点缺陷检测程序
=========================================================*/
//bool white_point(Mat white_yiwu, Mat ceguang, Mat *mresult, QString *causecolor)//灰度检测

//{
//    bool result = false;
//    white_yiwu = white_yiwu(Rect(5, 5, white_yiwu.cols - 10, white_yiwu.rows - 10));
//    ceguang = ceguang(Rect(5, 5, ceguang.cols - 10, ceguang.rows - 10));
//    Mat gray_ceguang;
//    cvtColor(ceguang, gray_ceguang, CV_BGR2GRAY);
//    Mat img_gray;
//    cvtColor(white_yiwu, img_gray, CV_BGR2GRAY);
//    //左侧相机白点判断
//    Mat th1, th2;
//    adaptiveThreshold(img_gray, th1, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
//    threshold(img_gray, th2, 30, 255, CV_THRESH_BINARY);   //以一个较低的阈值对原图二值化,得出相与用的模板
//    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(9, 9));
//    Mat dilate2;
//    erode(th2, dilate2, structure_element2);    //将模板腐蚀一下,为了去除边界影响,否则相与过后会有白边
//    Mat th_result;
//    bitwise_and(th1, dilate2, th_result);
//    th_result(Rect(0, th_result.rows - 346, 208, 346)) = uchar(0);	 //白点缺陷去除易撕贴部分
//    th_result(Rect(0, 0, th_result.cols - 1, 10)) = uchar(0);          //去除四个边界的影响上边
//    th_result(Rect(0, th_result.rows - 24, th_result.cols - 1, 24)) = uchar(0);//下边
//    th_result(Rect(0, 0, 40, th_result.rows - 1)) = uchar(0);          // 左边有误分割情况15
//    th_result(Rect(th_result.cols - 11, 0, 11, th_result.rows - 1)) = uchar(0);//右

//    vector<vector<Point>> contours;
//    findContours(th_result, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    for (vector<int>::size_type i = 0; i < contours.size(); i++)
//    {
//        double area = contourArea(contours[i]);
//        if (area > whitePoint_lowerArea && area < whitePoint_higherArea)
//        {
//            cout << area << endl;
//            boundRect[i] = boundingRect(Mat(contours[i]));
//            float w = boundRect[i].width;
//            float h = boundRect[i].height;
//            int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
//            int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
//            int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
//            int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
//            Moments m = moments(contours[i]);   //查找轮廓的重心
//            int x_point = int(m.m10 / m.m00);
//            int y_point = int(m.m01 / m.m00);
//            if (true)
//            {

//                int x_lt = X_1 - 5;
//                if (x_lt < 0)
//                {
//                    x_lt = 0;
//                }
//                int y_lt = Y_1 - 5;
//                if (y_lt < 0)
//                {
//                    y_lt = 0;
//                }
//                int x_rt = X_2 + 5;
//                if (x_rt > gray_ceguang.size[1] - 1)
//                {
//                    x_rt = gray_ceguang.size[1] - 1;
//                }
//                int y_rt = Y_2 + 5;
//                if (y_rt > gray_ceguang.size[0] - 1)
//                {
//                    y_rt = gray_ceguang.size[0] - 1;
//                }

//                Mat temp_ceguang = gray_ceguang(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));

//                Mat temp_gray = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
//                double mean_ceguang = mean(temp_ceguang)[0];
//                double mean_gray = mean(temp_gray)[0];
//                double chazhi = img_gray.at<uchar>(y_point, x_point) - mean_gray;
//                if (img_gray.at<uchar>(y_point, x_point) - mean_gray > 20 && ((X_2 - X_1)*(Y_2 - Y_1) > 10) && (max(w / h, h / w) < 5))
//                {
//                    CvPoint top_lef4 = cvPoint(X_1 - 10, Y_1 - 10);
//                    CvPoint bottom_right4 = cvPoint(X_2 + 20, Y_2 + 20);
//                    rectangle(white_yiwu, top_lef4, bottom_right4, Scalar(0, 0, 255), 5, 8, 0);
//                    result = true;
//                    string c = "white_point_left";
//                    putText(white_yiwu, c, Point(x_point, y_point), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255), 1);
//                    break;

//                }

//            }
//        }
//    }

//    if (result == true)
//    {
//        *causecolor = "白点";
//        *mresult = white_yiwu;
//    }
//    //右侧相机白点缺陷判断
//    else {
//        Mat th3, th4;
//        adaptiveThreshold(gray_ceguang, th3, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
//        threshold(gray_ceguang, th4, 30, 255, CV_THRESH_BINARY);   //以一个较低的阈值对原图二值化,得出相与用的模板
//        Mat dilate3;
//        erode(th4, dilate3, structure_element2);    //将模板腐蚀一下,为了去除边界影响,否则相与过后会有白边
//        Mat th_result1;
//        bitwise_and(th3, dilate3, th_result1);
//        th_result1(Rect(0, th_result1.rows - 346, 208, 346)) = uchar(0);	 //白点缺陷去除易撕贴部分
//        th_result1(Rect(0, 0, th_result1.cols - 1, 10)) = uchar(0);          //去除四个边界的影响上边
//        th_result1(Rect(0, th_result1.rows - 24, th_result1.cols - 1, 24)) = uchar(0);//下边
//        th_result1(Rect(0, 0, 40, th_result1.rows - 1)) = uchar(0);          // 左边有误分割情况15
//        th_result1(Rect(th_result1.cols - 11, 0, 11, th_result1.rows - 1)) = uchar(0);//右
//        vector<vector<Point>> contours1;
//        findContours(th_result1, contours1, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//        vector<Rect> boundRect1(contours1.size());
//        for (vector<int>::size_type i = 0; i < contours1.size(); i++)
//        {
//            double area = contourArea(contours1[i]);
//            if (area > whitePoint_lowerArea && area < whitePoint_higherArea)
//            {
//                cout << area << endl;
//                boundRect1[i] = boundingRect(Mat(contours1[i]));
//                float w = boundRect1[i].width;
//                float h = boundRect1[i].height;
//                int X_1 = boundRect1[i].tl().x;//矩形左上角X坐标值
//                int Y_1 = boundRect1[i].tl().y;//矩形左上角Y坐标值
//                int X_2 = boundRect1[i].br().x;//矩形右下角X坐标值
//                int Y_2 = boundRect1[i].br().y;//矩形右下角Y坐标值
//                Moments m = moments(contours1[i]);   //查找轮廓的重心
//                int x_point = int(m.m10 / m.m00);
//                int y_point = int(m.m01 / m.m00);
//                if (true)
//                {
//                    int x_lt = X_1 - 5;
//                    if (x_lt < 0)
//                    {
//                        x_lt = 0;
//                    }
//                    int y_lt = Y_1 - 5;
//                    if (y_lt < 0)
//                    {
//                        y_lt = 0;
//                    }
//                    int x_rt = X_2 + 5;
//                    if (x_rt > gray_ceguang.size[1] - 1)
//                    {
//                        x_rt = gray_ceguang.size[1] - 1;
//                    }
//                    int y_rt = Y_2 + 5;
//                    if (y_rt > gray_ceguang.size[0] - 1)
//                    {
//                        y_rt = gray_ceguang.size[0] - 1;
//                    }
//                    Mat temp_gray = gray_ceguang(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
//                    double mean_gray = mean(temp_gray)[0];
//                    double chazhi = gray_ceguang.at<uchar>(y_point, x_point) - mean_gray;
//                    if (gray_ceguang.at<uchar>(y_point, x_point) - mean_gray > 20 && ((X_2 - X_1)*(Y_2 - Y_1) > 10) && (max(w / h, h / w) < 5))
//                    {
//                        CvPoint top_lef4 = cvPoint(X_1 - 10, Y_1 - 10);
//                        CvPoint bottom_right4 = cvPoint(X_2 + 20, Y_2 + 20);
//                        rectangle(ceguang, top_lef4, bottom_right4, Scalar(255, 0, 255), 5, 8, 0);
//                        result = true;
//                        string c = "white_point_right";
//                        putText(ceguang, c, Point(x_point, y_point), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 0, 255), 1);
//                    }

//                }
//            }
//        }
//    }

//    if (result == true)
//    {
//        *causecolor = "白点";
//        *mresult = ceguang;
//    }

//    return result;

//}





