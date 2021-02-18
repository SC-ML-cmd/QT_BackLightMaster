#include "detector.h"
#include "Ployfit.h"
#include"global_variable.h"
#include<QDebug>
#include"qdatetime.h"
#include <QFileDialog>
using namespace czy;
using namespace cv;
Point2f CrossPointReturn(Mat src, int x, int y , int width, int height);
bool isArea_1 ;
bool isArea_2 ;
bool isCeshi;
bool isFrontBack;
bool isColor;
Mat bit_white; //白屏下保存的图片相与模板
Mat mura_imgL,mura_imgR;
int screenLength = 2900;  //屏幕长度
int screenWidth = 1400;   //屏幕宽度
int Whiteprint_Detect_Flag;

/*=========================================================
* 函 数 名: Vec2Mat
* 功能描述: 将Vector二维数组转换为Mat数据格式
* 输入变量: 1.二维数组Vector 2.图片像素行数 3.图片像素列数
=========================================================*/
Mat Vec2Mat7(vector<vector<uchar>> array, int row, int col)
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
vector<vector<int>> Read_Mat7(vector<vector<int>> array, Mat d_Img, int nRows, int nCols)
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
* 函 数 名: Ployfit_Col
* 功能描述: 对”列“数据进行多项式拟合，并返回拟合结果
* 输入变量: 1.输入图片 2.拟合的阶数（若poly_n=2，则y=a0+a1*x+a2*x^2 ）3.是否保存数据（FLASE）
4.图片行数 5.图片列数 6.行拟合阈值
==========================================================================================================*/
Mat Ployfit_Col7(Mat img_col, int poly_n, bool isSaveOrNot, double Scoral)
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
    array = Read_Mat7(array, img_col, nRows, nCols);  //将图像数据存入vector
    for (int b = 1; b < nRows + 1; b++){
        array_rows[b - 1] = b;}
    for (int i = 0; i < nCols; i++)
    {
        for (int m = 0; m < nRows; m++)
        {
            if(array[m][i]>100)
            {
                array_clone_cols[m] = array[m][i];
            }
            else {
                array_clone_cols[m] = 100;
            }
            if(m>=1)
            {
                if(abs(array[m][i]-array_clone_cols[m-1])>20)
                {
                    array_clone_cols[m]=array_clone_cols[m-1];
                }
            }
        }

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
    {
        for (int j = 0; j < nCols; j++)
            //if (array_col_diff[i][j] < sigma1 - 2)
        { if (abs(array_col_diff[i][j]) < Scoral)
                array_clone2[i][j] = 0;
            else
                array_clone2[i][j] = 255;
        }
    }
    Mat img = Vec2Mat7(array_clone2, nRows, nCols);
    return img;
}

/*=========================================================
* 函 数 名: matRotateClockWise90
* 功能描述: 图像旋转90度
=========================================================*/
Mat matRotateClockWise90(Mat src)
{
    // 矩阵转置
    transpose(src, src);
    //0: 沿X轴翻转； >0: 沿Y轴翻转； <0: 沿X轴和Y轴翻转
    flip(src, src, 1);// 翻转模式，flipCode == 0垂直翻转（沿X轴翻转），flipCode>0水平翻转（沿Y轴翻转），flipCode<0水平垂直翻转（先沿X轴翻转，再沿Y轴翻转，等价于旋转180°）
    return src;
}

/*=========================================================
* 函 数 名: matRotateClockWisefu90
* 功能描述: 图像旋转90度
=========================================================*/
Mat matRotateClockWisefu90(Mat src)
{
    // 矩阵转置
    transpose(src, src);
    //0: 沿X轴翻转； >0: 沿Y轴翻转； <0: 沿X轴和Y轴翻转
    flip(src, src, 0);// 翻转模式，flipCode == 0垂直翻转（沿X轴翻转），flipCode>0水平翻转（沿Y轴翻转），flipCode<0水平垂直翻转（先沿X轴翻转，再沿Y轴翻转，等价于旋转180°）
    return src;
}

/*=========================================================
* 函 数 名: roi_white
* 功能描述: 直角屏主彩色相机透视变换矩阵
=========================================================*/
bool roi_white(Mat src_white, int border_white, int border_black, int border_lougunag, Mat *Mwhite, Mat *Mblack, Mat *Mlouguang, Mat *Mabshow, int ID)//白色底色下提取黑色ROI
{
    bool isArea_1, isArea_2;
    bool isnormal = true;
    Mat img_gray;
    //img_gray = src_white.clone();
    if(src_white.channels() == 3)
        cvtColor(src_white,img_gray,CV_BGR2GRAY);
    else
        img_gray = src_white.clone();
    Mat th1;
    threshold(img_gray, th1, 20, 255, CV_THRESH_BINARY);//100->60
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat  erode1;
    erode(th1, erode1, structure_element1);

    //    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(9,9));
    //    Mat dilate1;
    //    dilate(erode1, dilate1, structure_element2);
    Mat img_temp1, img_temp2, img_temp3, img_temp4;
    int X_1, Y_1, X_2, Y_2, w, h;
    int X_lt, Y_lt, X_rt, Y_rt, X_lb, Y_lb, X_rb, Y_rb, w1, h1;
    int unm_contours = 0;
    vector<vector<Point>> contours1;
    vector<vector<Point>> contours;
    std::vector<cv::Point> poly;
    std::vector<cv::Point> approxCurve;

    findContours(erode1, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area>1000000 && area<5500000)              //5.5寸屏标准工业屏面积367万  6.0寸标工业屏面积440万
        {
            unm_contours++;
            double epsilon = 0.1*arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], approxCurve, epsilon, true);
        }
    }
    //判断角点提取是否存在异常现象若判断异常提取一张黑底图像防止软件卡死
    if (approxCurve.size() != 4)
    {
        isnormal = false;
    }
    if (approxCurve.size() == 4)
    {
        isnormal = true;
    }
    if (unm_contours>0 && ID == 1)
    {
        isArea_1 = false;
    }
    if (unm_contours == 0 && ID == 1)
    {
        isArea_1 = true;//主黑白相机为true则认为未提取到屏幕有异常
    }

    if (unm_contours>0 && ID == 2)
    {
        isArea_2 = false;
    }
    if (unm_contours == 0 && ID == 2)
    {
        isArea_2 = true;
    }

    if (unm_contours == 0 || isnormal == false)
    {
        cv::Point2f src_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 10),
            cv::Point2f(10, 10),
            cv::Point2f(10, 0)
        };
        cv::Point2f dst_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 1775),
            cv::Point2f(3000, 1775) ,
            cv::Point2f(3000,0) };
        *Mwhite = cv::getPerspectiveTransform(src_points, dst_points);
        *Mlouguang = cv::getPerspectiveTransform(src_points, dst_points);
        *Mblack = cv::getPerspectiveTransform(src_points, dst_points);
        *Mabshow = cv::getPerspectiveTransform(src_points, dst_points);
    }
    else
    {
        vector<cv::Point> approxCurve1;
        approxCurve1 = approxCurve;
        for (vector<int>::size_type ii = 0; ii<4; ii++)
        {
            if (approxCurve[ii].x<src_white.cols / 2 && approxCurve[ii].y<src_white.rows / 2)
            {
                approxCurve1[0] = approxCurve[ii];
            }

            if (approxCurve[ii].x<src_white.cols / 2 && approxCurve[ii].y>src_white.rows / 2)
            {
                approxCurve1[1] = approxCurve[ii];
            }

            if (approxCurve[ii].x>src_white.cols / 2 && approxCurve[ii].y>src_white.rows / 2)
            {
                approxCurve1[2] = approxCurve[ii];
            }
            if (approxCurve[ii].x>src_white.cols / 2 && approxCurve[ii].y<src_white.rows / 2)
            {
                approxCurve1[3] = approxCurve[ii];
            }
        }

        //对边缘像素点位置进行微调
        approxCurve1[0].x = approxCurve1[0].x + 1;
        approxCurve1[1].y = approxCurve1[1].y + 2;
        //        approxCurve1[1].x=approxCurve1[1].x-border_white;
        approxCurve1[2].y = approxCurve1[2].y + 5;
        //        approxCurve1[2].x=approxCurve1[2].x+border_white;
        //        approxCurve1[3].y=approxCurve1[3].y+border_white;
        //        approxCurve1[3].x=approxCurve1[3].x+border_white;

        //黑底下大量边缘漏光边界
        vector<cv::Point> approxCurve2;
        approxCurve2 = approxCurve1;
        int black_louuang = 90;
        approxCurve2[0].y = approxCurve2[0].y - black_louuang;
        approxCurve2[0].x = approxCurve2[0].x - black_louuang;
        approxCurve2[1].y = approxCurve2[1].y + black_louuang;
        approxCurve2[1].x = approxCurve2[1].x - black_louuang;
        approxCurve2[2].y = approxCurve2[2].y + black_louuang;
        approxCurve2[2].x = approxCurve2[2].x + black_louuang;
        approxCurve2[3].y = approxCurve2[3].y - black_louuang;
        approxCurve2[3].x = approxCurve2[3].x + black_louuang;

        //黑底下使用的透视变换矩阵
        vector<cv::Point> approxCurve3;
        approxCurve3 = approxCurve1;
        int black_border = 10;
        approxCurve3[0].y = approxCurve3[0].y - black_border;
        approxCurve3[0].x = approxCurve3[0].x - 0;
        approxCurve3[1].y = approxCurve3[1].y + black_border;
        approxCurve3[1].x = approxCurve3[1].x - 0;
        approxCurve3[2].y = approxCurve3[2].y + black_border;
        approxCurve3[2].x = approxCurve3[2].x + black_border;
        approxCurve3[3].y = approxCurve3[3].y - black_border;
        approxCurve3[3].x = approxCurve3[3].x + black_border;

        //主黑白相机白底显示异常
        vector<cv::Point> approxCurve_abshow;
        approxCurve_abshow = approxCurve1;
        //int border_white = -2;
        approxCurve_abshow[0].y = approxCurve_abshow[0].y - border_white + 10;
        approxCurve_abshow[0].x = approxCurve_abshow[0].x - border_white + 10;
        approxCurve_abshow[1].y = approxCurve_abshow[1].y - border_white + 10;
        approxCurve_abshow[1].x = approxCurve_abshow[1].x + border_white - 10;
        approxCurve_abshow[2].y = approxCurve_abshow[2].y + border_white - 10;
        approxCurve_abshow[2].x = approxCurve_abshow[2].x + border_white - 10;
        approxCurve_abshow[3].y = approxCurve_abshow[3].y + border_white - 10;
        approxCurve_abshow[3].x = approxCurve_abshow[3].x - border_white + 10;

        //src_corner[0].x = src_corner[0].x - border_white + 10;
        //src_corner[0].y = src_corner[0].y - border_white + 10;
        //src_corner[1].x = src_corner[1].x - border_white + 10;
        //src_corner[1].y = src_corner[1].y + border_white - 10;
        //src_corner[2].x = src_corner[2].x + border_white - 10;
        //src_corner[2].y = src_corner[2].y + border_white - 10;
        //src_corner[3].x = src_corner[3].x + border_white - 10;
        //src_corner[3].y = src_corner[3].y - border_white + 10;

        cv::Point2f src_points_abshow[] = {
            approxCurve_abshow[0],
            approxCurve_abshow[1],
            approxCurve_abshow[2],
            approxCurve_abshow[3]
        };

        cv::Point2f src_points[] = {
            approxCurve1[0],
            approxCurve1[1],
            approxCurve1[2],
            approxCurve1[3]
        };

        cv::Point2f src_points1[] = {
            approxCurve3[0],
            approxCurve3[1],
            approxCurve3[2],
            approxCurve3[3]
        };

        cv::Point2f src_points2[] = {

            approxCurve2[0],
            approxCurve2[1],
            approxCurve2[2],
            approxCurve2[3]
        };

        cv::Point2f dst_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 1775),
            cv::Point2f(3000, 1775) ,
            cv::Point2f(3000,0) };

        *Mwhite = cv::getPerspectiveTransform(src_points, dst_points);
        *Mlouguang = cv::getPerspectiveTransform(src_points2, dst_points);
        *Mblack = cv::getPerspectiveTransform(src_points1, dst_points);
        *Mabshow = cv::getPerspectiveTransform(src_points_abshow, dst_points);
    }

    return true;
}

/*====================================================================
* 类型：彩色相机透视变换矩阵计算
* 功能：核函数的计算
* 输入：主彩色相机拍摄白底图像
* 输出：
* 其他：
======================================================================*/
bool roi_white_color(Mat src_white,int border_white,int border_black,int border_lougunag,Mat *Mwhite,Mat *Mblack,Mat *Mlouguang,int ID)//白色底色下提取黑色ROI
{
    bool isnormal = true;//true为正常，false为不正常
    Mat img_gray=src_white.clone();
    cvtColor(src_white,img_gray,CV_BGR2GRAY);
    Mat th1;
    threshold(img_gray, th1,20, 255, CV_THRESH_BINARY);//100->60
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3,3));
    Mat  erode1;
    erode(th1, erode1, structure_element1);

    Mat img_temp1,img_temp2,img_temp3,img_temp4;
    int X_1,Y_1,X_2,Y_2,w,h;
    int X_lt,Y_lt,X_rt,Y_rt,X_lb,Y_lb,X_rb,Y_rb,w1,h1;
    int unm_contours=0;
    vector<vector<Point>> contours1;
    vector<vector<Point>> contours;
    std::vector<cv::Point> poly;
    std::vector<cv::Point> approxCurve;

    findContours(erode1, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        qDebug()<<area<<endl;
        if (area>205000 and area<13000000)//5000000
        {   unm_contours++;
            double epsilon = 0.1*arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], approxCurve, epsilon, true);
        }
    }
    //判断角点提取是否存在异常现象若判断异常提取一张黑底图像防止软件卡死
    if (approxCurve.size() != 4)
    {
        isnormal = false;
    }
    if (approxCurve.size() == 4)
    {
        isnormal = true;
    }
    if (unm_contours == 0|| isnormal == false)
    {
        isColor = true;//ture为彩色相机图像未提取到
        cv::Point2f src_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 10),
            cv::Point2f(10, 10),
            cv::Point2f(10, 0)
        };

        cv::Point2f dst_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 1775),
            cv::Point2f(3000, 1775) ,
            cv::Point2f(3000,0)};
        *Mwhite= cv::getPerspectiveTransform(src_points, dst_points);
        *Mlouguang=cv::getPerspectiveTransform(src_points, dst_points);
        *Mblack=cv::getPerspectiveTransform(src_points, dst_points);
    }
    else
    {
        isColor = false; //false为彩色相机图像提取到图像
        vector<cv::Point> approxCurve1;
        approxCurve1=approxCurve;
        for (vector<int>::size_type  ii=0;ii<4;ii++)
        {
            if( approxCurve[ii].x<src_white.cols/2 &&approxCurve[ii].y<src_white.rows/2)
            {
                approxCurve1[0]=approxCurve[ii];
            }
            if( approxCurve[ii].x<src_white.cols/2 &&approxCurve[ii].y>src_white.rows/2)
            {
                approxCurve1[1]=approxCurve[ii];
            }
            if( approxCurve[ii].x>src_white.cols/2 &&approxCurve[ii].y>src_white.rows/2)
            {
                approxCurve1[2]=approxCurve[ii];
            }
            if( approxCurve[ii].x>src_white.cols/2 &&approxCurve[ii].y<src_white.rows/2)
            {
                approxCurve1[3]=approxCurve[ii];
            }
        }

        //  circle(src_white, approxCurve1[0], 5, Scalar(0, 255, 0), 2);
        //  circle(src_white, approxCurve1[1], 10, Scalar(0, 255, 0), 2);
        //  circle(src_white, approxCurve1[2], 15, Scalar(0, 255, 0), 2);
        //  circle(src_white, approxCurve1[3], 20, Scalar(0, 255, 0), 2);
        //  approxCurve1[0].y=approxCurve1[0].y-border_white;
        //  approxCurve1[0].x=approxCurve1[0].x-border_white;
        //approxCurve1[1].y=approxCurve1[1].y+(-7);
        //  approxCurve1[1].x=approxCurve1[1].x-border_white;
        //  approxCurve1[2].y=approxCurve1[2].y+border_white;
        //  approxCurve1[2].x=approxCurve1[2].x+border_white;
        //  approxCurve1[3].y=approxCurve1[3].y-border_white;
        //  approxCurve1[3].x=approxCurve1[3].x+border_white;

        vector<cv::Point> approxCurve2;
        approxCurve2 = approxCurve;
        int black_louuang = 50; //黑底下大量边缘漏光边界
        approxCurve2[0].y = approxCurve2[0].y - black_louuang;
        approxCurve2[0].x = approxCurve2[0].x - black_louuang;
        approxCurve2[1].y = approxCurve2[1].y + black_louuang;
        approxCurve2[1].x = approxCurve2[1].x - black_louuang;
        approxCurve2[2].y = approxCurve2[2].y + black_louuang;
        approxCurve2[2].x = approxCurve2[2].x + black_louuang;
        approxCurve2[3].y = approxCurve2[3].y - black_louuang;
        approxCurve2[3].x = approxCurve2[3].x + black_louuang;

        cv::Point2f src_points[] = {
            approxCurve1[0],
            approxCurve1[1],
            approxCurve1[2],
            approxCurve1[3]
            //cv::Point2f(615, 125)
        };
        cv::Point2f src_points2[] = {

            approxCurve2[0],
            approxCurve2[1],
            approxCurve2[2],
            approxCurve2[3]
        };
        cv::Point2f dst_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 1775),
            cv::Point2f(3000, 1775) ,
            cv::Point2f(3000,0)};
        *Mwhite= cv::getPerspectiveTransform(src_points, dst_points);
        *Mlouguang=cv::getPerspectiveTransform(src_points2, dst_points);
        *Mblack=cv::getPerspectiveTransform(src_points, dst_points);
    }
    return true;
}

/*=========================================================
* 函 数 名: roi_ceshi
* 功能描述: 直角左右黑白相机拍摄白底透视变换矩阵计算
* 说明:
=========================================================*/
bool roi_ceshi(Mat src_white, int border_white, Mat *Mwhite)
{
    bool isCeshi; //true为没拍到侧面
    bool isnormal=true;//true为正常，false为不正常
    Mat img_gray;
    if(src_white.channels() == 3)
        cvtColor(src_white,img_gray,CV_BGR2GRAY);
    else
        img_gray = src_white.clone();
    //Mat img_gray=src_white.clone();
    Mat th1;
    threshold(img_gray, th1, 14, 255, CV_THRESH_BINARY);

    //腐蚀
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat  erode1;
    erode(th1, erode1, structure_element1);
    Mat img_temp1, img_temp2, img_temp3, img_temp4;
    vector<vector<Point>> contours1;
    vector<vector<Point>> contours;
    std::vector<cv::Point> poly;
    std::vector<cv::Point> approxCurve;
    int num_contours = 0;
    findContours(erode1, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    Rect rect1;
    Point2f P[4];
    Point2f shunxu[4];
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 400000 && area < 5000000)   //这是侧视相机找到的轮廓面积所在的范围
        {
            num_contours++;
            double epsilon = 0.01*arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], approxCurve, epsilon, true); // 多边形逼近，精度(即最小边长)设为30是为了得到4个角点
        }
    }
    //判断角点提取是否存在异常现象
    if(approxCurve.size() != 4)
    {
        isnormal = false;
    }
    if (approxCurve.size() == 4)
    {
        isnormal = true;
    }
    //未提取到屏幕处理
    if (num_contours == 0||isnormal == false)
    {
        isCeshi = true;
        cv::Point2f src_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 10),
            cv::Point2f(10, 10),
            cv::Point2f(10, 0)
        };

        cv::Point2f dst_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, screenWidth - 1),
            cv::Point2f(screenLength - 1, screenWidth - 1),
            cv::Point2f(screenLength - 1,0)
        };
        *Mwhite = cv::getPerspectiveTransform(src_points, dst_points);
    }
    //提取到屏幕处理
    else
    {
        isCeshi = false;
        vector<cv::Point> approxCurve1;
        approxCurve1 = approxCurve;
        for (vector<int>::size_type ii = 0; ii < 4; ii++)
        {
            if (approxCurve[ii].x < src_white.cols / 2 && approxCurve[ii].y < src_white.rows / 2)
            {
                approxCurve1[0] = approxCurve[ii];
            }
            if (approxCurve[ii].x<src_white.cols / 2 && approxCurve[ii].y>src_white.rows / 2)
            {
                approxCurve1[1] = approxCurve[ii];
            }
            if (approxCurve[ii].x > src_white.cols / 2 && approxCurve[ii].y > src_white.rows / 2)
            {
                approxCurve1[2] = approxCurve[ii];
            }
            if (approxCurve[ii].x > src_white.cols / 2 && approxCurve[ii].y < src_white.rows / 2)
            {
                approxCurve1[3] = approxCurve[ii];
            }
        }
        approxCurve1[0].y=approxCurve1[0].y-border_white;
        approxCurve1[0].x=approxCurve1[0].x-border_white;
        approxCurve1[1].y=approxCurve1[1].y+border_white;
        approxCurve1[1].x=approxCurve1[1].x-border_white;
        approxCurve1[2].y=approxCurve1[2].y+border_white;
        approxCurve1[2].x=approxCurve1[2].x+border_white;
        approxCurve1[3].y=approxCurve1[3].y-border_white;
        approxCurve1[3].x=approxCurve1[3].x+border_white;

        vector<Point2f> src_corner(4);
        src_corner[0] = Point2f(approxCurve1[0].x, approxCurve1[0].y);
        src_corner[1] = Point2f(approxCurve1[1].x, approxCurve1[1].y);
        src_corner[2] = Point2f(approxCurve1[2].x, approxCurve1[2].y);
        src_corner[3] = Point2f(approxCurve1[3].x, approxCurve1[3].y);

        vector<Point2f> dst_corner(4);
        dst_corner[0] = Point2f(0, 0);
        dst_corner[1] = Point2f(0, 1775);
        dst_corner[2] = Point2f(3000, 1775);
        dst_corner[3] = Point2f(3000, 0);
        *Mwhite = cv::getPerspectiveTransform(src_corner, dst_corner);
    }
    return true;
}

/*=========================================================
*@函 数 名:              f_MainCam_PersTransMatCal
*@功能描述:              主黑白/彩色相机R角屏幕的透视变换矩阵计算
*@param _src             输入灰度/彩色图像
*@param _dst             输出显示到客户用图像
*@param border_white     提白底图边缘调整参数值
*@param border_black     提黑底图边缘调整参数值
*@param border_lightleak 提漏光图边缘调整参数值
*@param Mwhite           白底透视变换矩阵
*@param Mblack           黑底透视变换矩阵
*@param Mlightleak       漏光透视变换矩阵
*@param M_white_abshow   显示异常变换矩阵
*@param ID               工位ID号(弃用)
*@ScreenType_Flag        屏幕类型
*@修改时间：		     2020年9月18日
*@备注说明              use
=========================================================*/
bool f_MainCam_PersTransMatCal(InputArray _src, int border_white, int border_biankuang, Mat *Mwhite, Mat *Mbiankuang, Mat *M_white_abshow, int ID, QString ScreenType_Flag)
{
//    double screen_long=size_long/size_width;
//    int screen_long=size_long/size_width;
    bool isArea_1, isArea_2;														//显示异常标志位
    Mat src = _src.getMat();                                                        //输入源图像
    if (src.type() == CV_8UC1)														//若输入8位图
        src = src.clone();															//拷贝原图
    else
        cvtColor(src, src, CV_BGR2GRAY);										    //灰度化彩色图
    CV_Assert(src.depth() == CV_8U);                                                //8位无符号
    Mat binaryImage = Mat::zeros(src.size(), CV_8UC1);                              //二值图像
    threshold(src, binaryImage, 40, 255, CV_THRESH_BINARY);							//二值化(有问题)
    medianBlur(binaryImage, binaryImage, 5);										//中值滤波去除锯齿
    int displayError_Areasignal = 0;												//根据轮廓面积判定显示异常标志位
    vector<vector<Point>> contours;													//contours存放点集信息
    findContours(binaryImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);    //CV_RETR_EXTERNAL最外轮廓，CV_CHAIN_APPROX_NONE所有轮廓点信息
    vector<Point> upLinePoint, leftLinePoint, downLinePoint, rightLinePoint;		//上左下右侧点集数据
    Vec4f upLine_Fit, leftLine_Fit, downLine_Fit, rightLine_Fit;				    //上左下右拟合直线数据
    vector<Point2f> src_corner(4), src_corner_biankuang(4), src_corner_abshow(4);   //四个边相交得到角点坐标，漏光角点，显示异常角点
    Rect rect;																        //最小正外接矩形
    int x1, y1, x2, y2, x3, y3, x4, y4;			//正接矩阵坐标点信息
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 2500000 && area < 5000000)
        {
            displayError_Areasignal++;
            rect = boundingRect(contours[i]);
            x1 = rect.tl().x;//左上角
            y1 = rect.tl().y;//左上角
            x2 = rect.tl().x;//左下角
            y2 = rect.br().y;//右下角
            x3 = rect.br().x;//右下角
            y3 = rect.br().y;//右下角
            x4 = rect.br().x;//右上角
            y4 = rect.tl().y;//右上角
            int radianEliminate = 230;
            int deviation = 160;
            for (int j = 0; j < contours[i].size(); j++)
            {
                //左侧点集
                if (contours[i][j].y > y1 + radianEliminate && contours[i][j].y <  y1 + (y2 - y1)*0.3 && abs(contours[i][j].x - x1) < deviation ||
                    contours[i][j].y > y1 + (y2 - y1)*0.7 && contours[i][j].y < y2 - radianEliminate && abs(contours[i][j].x - x1) < deviation)
                    leftLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                //右侧点集
                if (contours[i][j].y > y1 + radianEliminate && contours[i][j].y < y2 - radianEliminate &&abs(contours[i][j].x - x3) < deviation)
                    rightLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                //上侧点集
                if (contours[i][j].x > x1 + radianEliminate && contours[i][j].x < x4 - radianEliminate && abs(contours[i][j].y - y1) < deviation)
                    upLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                //下侧点集
                if (contours[i][j].x > x1 + radianEliminate && contours[i][j].x < x4 - radianEliminate && abs(contours[i][j].y - y2) < deviation)
                    downLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
            }
        }
    }
    if (leftLinePoint.size() == 0 || rightLinePoint.size() == 0 || upLinePoint.size() == 0 || downLinePoint.size() == 0)
        displayError_Areasignal = 0;
    //根据轮廓面积判定显示异常
    if (displayError_Areasignal > 0 && ID == 1)
        isArea_1 = false;
    if (displayError_Areasignal == 0 && ID == 1)
        isArea_1 = true;
    if (displayError_Areasignal > 0 && ID == 2)
        isArea_2 = false;
    if (displayError_Areasignal == 0 && ID == 2)
        isArea_2 = true;
    //未提取到屏幕判定显示异常提取边缘角落
    if (displayError_Areasignal == 0)
    {
        vector<Point2f> src_points(4);
        src_points = { Point(0, 0), Point(0, 10), Point(10, 10), Point(10, 0) };
        vector<Point2f> dst_points(4);
        if (ScreenType_Flag == "矩形屏")
            dst_points = { Point(0, 0), Point(0, 1775), Point(3000, 1775), Point(3000, 0) };
        else        //pixel_num
            //dst_points = { Point(0, 0), Point(0, 1500), Point(3000, 1500), Point(3000, 0) };
            dst_points = { Point(0, 0), Point(0, 1500), Point(pixel_num, 1500), Point(pixel_num, 0) };
        *Mwhite = cv::getPerspectiveTransform(src_points, dst_points);
        //*Mlightleak = cv::getPerspectiveTransform(src_points, dst_points);
        *Mbiankuang = cv::getPerspectiveTransform(src_points, dst_points);
        *M_white_abshow = cv::getPerspectiveTransform(src_points, dst_points);
    }
    //正常屏幕提取屏幕的四个角点
    else
    {
        fitLine(leftLinePoint, leftLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);               //左侧拟合直线
        fitLine(rightLinePoint, rightLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);				//右侧拟合直线
        fitLine(upLinePoint, upLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);					//上侧拟合直线
        fitLine(downLinePoint, downLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);				//下侧拟合直线

        /*Mat img = _src.getMat();
        for (int i = 0; i < leftLinePoint.size(); i++)
        {
            circle(img, Point(leftLinePoint[i].x, leftLinePoint[i].y), 8, Scalar(0, 0, 255), -1);
        }
        for (int i = 0; i < rightLinePoint.size(); i++)
        {
            circle(img, Point(rightLinePoint[i].x, rightLinePoint[i].y), 8, Scalar(0, 0, 255), -1);
        }
        for (int i = 0; i < upLinePoint.size(); i++)
        {
            circle(img, Point(upLinePoint[i].x, upLinePoint[i].y), 8, Scalar(0, 0, 255), -1);
        }
        for (int i = 0; i < downLinePoint.size(); i++)
        {
            circle(img, Point(downLinePoint[i].x, downLinePoint[i].y), 8, Scalar(0, 0, 255), -1);
        }*/

        src_corner[0] = getPointSlopeCrossPoint(upLine_Fit, leftLine_Fit);              //左上角点
        src_corner[1] = getPointSlopeCrossPoint(downLine_Fit, leftLine_Fit);		    //左下角点
        src_corner[2] = getPointSlopeCrossPoint(downLine_Fit, rightLine_Fit);	        //右下角点
        src_corner[3] = getPointSlopeCrossPoint(upLine_Fit, rightLine_Fit);		        //右上角点

                                                                                        //对4个角点的坐标位置进行微调（白底图以及黑底图）
        src_corner[0].x = src_corner[0].x - border_white;
        src_corner[0].y = src_corner[0].y - border_white;
        src_corner[1].x = src_corner[1].x - border_white;
        src_corner[1].y = src_corner[1].y + border_white;
        src_corner[2].x = src_corner[2].x + border_white;
        src_corner[2].y = src_corner[2].y + border_white;
        src_corner[3].x = src_corner[3].x + border_white;
        src_corner[3].y = src_corner[3].y - border_white;
        //对4个角点的坐标位置进行微调（漏光检测图）
        src_corner_biankuang[0].x = src_corner[0].x - border_biankuang;
        src_corner_biankuang[0].y = src_corner[0].y - border_biankuang;
        src_corner_biankuang[1].x = src_corner[1].x - border_biankuang;
        src_corner_biankuang[1].y = src_corner[1].y + border_biankuang;
        src_corner_biankuang[2].x = src_corner[2].x + border_biankuang;
        src_corner_biankuang[2].y = src_corner[2].y + border_biankuang;
        src_corner_biankuang[3].x = src_corner[3].x + border_biankuang;
        src_corner_biankuang[3].y = src_corner[3].y - border_biankuang;
        //显示异常(白底图)
        src_corner_abshow[0].x = src_corner[0].x - border_white + 10;
        src_corner_abshow[0].y = src_corner[0].y - border_white + 10;
        src_corner_abshow[1].x = src_corner[1].x - border_white + 10;
        src_corner_abshow[1].y = src_corner[1].y + border_white - 10;
        src_corner_abshow[2].x = src_corner[2].x + border_white - 10;
        src_corner_abshow[2].y = src_corner[2].y + border_white - 10;
        src_corner_abshow[3].x = src_corner[3].x + border_white - 10;
        src_corner_abshow[3].y = src_corner[3].y - border_white + 10;

        vector<Point2f> dst_corner(4);
        if (ScreenType_Flag == "矩形屏")
        {
            dst_corner[0] = Point(0, 0);
            dst_corner[1] = Point(0, 1775);
            dst_corner[2] = Point(3000, 1775);
            dst_corner[3] = Point(3000, 0);
        }
        else
        {
            dst_corner[0] = Point(0, 0);
            dst_corner[1] = Point(0, 1500);
            //            dst_corner[2] = Point(3000, 1500);
            //            dst_corner[3] = Point(3000, 0);
            dst_corner[2] = Point(pixel_num, 1500);
            dst_corner[3] = Point(pixel_num, 0);
        }
        *Mwhite = cv::getPerspectiveTransform(src_corner, dst_corner);
        //*Mlightleak = cv::getPerspectiveTransform(src_corner_lightleak, dst_corner);
        *Mbiankuang = cv::getPerspectiveTransform(src_corner_biankuang, dst_corner);
        *M_white_abshow = cv::getPerspectiveTransform(src_corner_abshow, dst_corner);
    }
    if (ID == 1)
        return isArea_1;
    else
        return isArea_2;
}

/*=========================================================
*@函 数 名:              f_LeftRightCam_PersTransMatCal
*@功能描述:              左右相机R角透视变换矩阵计算函数
*@param _src             输入灰度/彩色图像
*@param Mwhite           白底透视变换矩阵
*@ScreenType_Flag        屏幕类型
*@编制时间：		     2020年8月20日
*@备注说明      nouse
=========================================================*/
bool f_LeftRightCam_PersTransMatCal1(InputArray _src, Mat *Mwhite, QString ScreenType_Flag)
{
    bool Ext_Result_Left_Right;                                                     //提取屏幕成功标志位
    Mat src = _src.getMat();                                                        //输入源图像
    if (src.type() == CV_8UC1)														//若输入8位图
        src = src.clone();															//拷贝原图
    else
        cvtColor(src, src, CV_BGR2GRAY);										    //灰度化彩色图
    CV_Assert(src.depth() == CV_8U);                                                //8位无符号
    Mat binaryImage = Mat::zeros(src.size(), CV_8UC1);                              //二值图像
    threshold(src, binaryImage, 40, 255, CV_THRESH_BINARY);							//二值化(有问题)
    medianBlur(binaryImage, binaryImage, 5);										//中值滤波去除锯齿
    int displayError_Areasignal = 0;												//根据轮廓面积判定显示异常标志位
    vector<vector<Point>> contours;													//contours存放点集信息
    findContours(binaryImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);    //CV_RETR_EXTERNAL最外轮廓，CV_CHAIN_APPROX_NONE所有轮廓点信息
    vector<Point> upLinePoint, leftLinePoint, downLinePoint, rightLinePoint;		//上左下右侧点集数据
    Vec4f upLine_Fit, leftLine_Fit, downLine_Fit, rightLine_Fit;				    //上左下右拟合直线数据
    vector<Point2f> src_corner(4);                                                  //四个边相交得到角点坐标
    Rect rect;																        //最小正外接矩形
    int x1, y1, x2, y2, x3, y3, x4, y4;			                                    //正接矩阵坐标点信息
    vector<Point2f> dst_corner(4);                                                  //透视变换后的点的信息
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 2000000 && area < 5000000)
        {
            displayError_Areasignal++;
            rect = cv::boundingRect(contours[i]);                                           //最小外接矩形提取
            Ext_Result_Left_Right = false;                                                             //提取到屏幕
                                                                                                       //矩形面积缩小1/3，并得到新的矩形顶点
            int PixelGap1 = rect.tl().y;
            int PixelGap2 = src.rows - (rect.tl().y + rect.height);
            //纵坐标提取
            if (PixelGap1 > PixelGap2)
            {
                y1 = rect.tl().y;
                y2 = y1 + (rect.br().y - y1) * 2 / 3;
                y3 = y2;
                y4 = y1;
            }
            else
            {
                y1 = rect.br().y - (rect.br().y - rect.tl().y) * 2 / 3;
                y2 = rect.br().y;
                y3 = y2;
                y4 = y1;
            }
            //横坐标提取
            x1 = rect.tl().x;
            x2 = x1;
            x3 = rect.br().x;
            if (x3 == src.cols)
                x3 = x3 - 1;
            x4 = x3;
            //取直线的参数设置
            int radianEliminate = 230;//(R角)左右使用
            int radianEliminate2 = 360;//(R角)上下使用
            int deviation = 330;//(斜线带来的误差)左右使用
            int deviation2 = 150;//(斜线带来的误差)上下使用
            if (PixelGap1 > PixelGap2)
            {
                //外接矩形缩小
                while (binaryImage.at<uchar>(y2, x2) == 255 || binaryImage.at<uchar>(y3, x3) == 255)
                {
                    y2 = y2 - 1;
                    y3 = y3 - 1;

                    if (y2 == 0 || y3 == 0)
                    {
                        displayError_Areasignal = 0;
                        break;
                    }
                }
                while (binaryImage.at<uchar>(y2, x2) != 255)
                {
                    x2 = x2 + 1;
                    x1 = x2;

                    if (x1 == src.cols - 1 || x2 == src.cols - 1)
                    {
                        displayError_Areasignal = 0;
                        break;
                    }
                }
                while (binaryImage.at<uchar>(y3, x3) != 255)
                {
                    x3 = x3 - 1;
                    x4 = x3;

                    if (x4 == 0 || x3 == 0)
                    {
                        displayError_Areasignal = 0;
                        break;
                    }
                }

                if (displayError_Areasignal != 0)
                {
                    for (int j = 0; j < contours[i].size(); j++)
                    {
                        //左侧点集
                        if (contours[i][j].y > y1 + radianEliminate && contours[i][j].y < y2 && abs(contours[i][j].x - x1) < deviation)
                            leftLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                        //右侧点集
                        if (contours[i][j].y > rect.tl().y + radianEliminate && contours[i][j].y < rect.tl().y + (rect.br().y - rect.tl().y)*0.3 &&abs(contours[i][j].x - rect.br().x) < deviation ||
                            contours[i][j].y > rect.tl().y + (rect.br().y - rect.tl().y)*0.7 && contours[i][j].y < rect.br().y - radianEliminate && abs(contours[i][j].x - rect.br().x) < deviation)
                            rightLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                        //上侧点集
                        if (contours[i][j].x > x1 + radianEliminate2 && contours[i][j].x < x4 - radianEliminate2 && abs(contours[i][j].y - y1) < deviation2)
                            upLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                    }
                    //下侧点集
                    downLinePoint.push_back(Point((x2 + x3) / 2, (y2 + y3) / 2));
                    if (leftLinePoint.size() != 0 && rightLinePoint.size() != 0 && upLinePoint.size() != 0)
                    {
                        //直线拟合
                        fitLine(leftLinePoint, leftLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);               //左侧拟合直线
                        fitLine(rightLinePoint, rightLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);				//右侧拟合直线
                        fitLine(upLinePoint, upLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);					//上侧拟合直线
                        downLine_Fit[0] = upLine_Fit[0];
                        downLine_Fit[1] = upLine_Fit[1];
                        downLine_Fit[2] = downLinePoint[0].x;
                        downLine_Fit[3] = downLinePoint[0].y;                                           //下侧直线确定
                                                                                                        //角点提取
                        src_corner[0] = getPointSlopeCrossPoint(upLine_Fit, leftLine_Fit);              //左上角点
                        src_corner[1] = getPointSlopeCrossPoint(downLine_Fit, leftLine_Fit);		    //左下角点
                        src_corner[2] = getPointSlopeCrossPoint(downLine_Fit, rightLine_Fit);	        //右下角点
                        src_corner[3] = getPointSlopeCrossPoint(upLine_Fit, rightLine_Fit);		        //右上角点
                                                                                                        //透视变换矩阵计算
                        if (ScreenType_Flag == "矩形屏")
                            dst_corner = { Point(0, 0), Point(0, 1183), Point(3000, 1183), Point(3000, 0) };
                        else
                            dst_corner = { Point(0, 0), Point(0, 1000), Point(3000, 1000), Point(3000, 0) };
                        *Mwhite = cv::getPerspectiveTransform(src_corner, dst_corner);
                    }
                    else
                    {
                        displayError_Areasignal = 0;
                        break;
                    }
                }
            }
            else
            {
                //外接矩形缩小
                while (binaryImage.at<uchar>(y1, x1) == 255 || binaryImage.at<uchar>(y4, x4) == 255)
                {
                    y1 = y1 + 1;
                    y4 = y4 + 1;

                    if (y1 == src.rows - 1 || y4 == src.rows - 1)
                    {
                        displayError_Areasignal = 0;
                        break;
                    }
                }
                while (binaryImage.at<uchar>(y1, x1) != 255)
                {
                    x1 = x1 + 1;
                    x2 = x1;

                    if (x1 == src.cols - 1 || x2 == src.cols - 1)
                    {
                        displayError_Areasignal = 0;
                        break;
                    }
                }
                while (binaryImage.at<uchar>(y4, x4) != 255)
                {
                    x4 = x4 - 1;
                    x3 = x4;

                    if (x4 == 0 || x3 == 0)
                    {
                        displayError_Areasignal = 0;
                        break;
                    }
                }

                if (displayError_Areasignal != 0)
                {
                    for (int j = 0; j < contours[i].size(); j++)
                    {
                        //左侧点集
                        if (contours[i][j].y > y1 && contours[i][j].y < y2 - radianEliminate && abs(contours[i][j].x - x1) < deviation)
                            leftLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                        //右侧点集
                        if (contours[i][j].y > rect.tl().y + radianEliminate && contours[i][j].y < rect.tl().y + (rect.br().y - rect.tl().y)*0.3 &&abs(contours[i][j].x - rect.br().x) < deviation ||
                            contours[i][j].y > rect.tl().y + (rect.br().y - rect.tl().y)*0.7 && contours[i][j].y < rect.br().y - radianEliminate && abs(contours[i][j].x - rect.br().x) < deviation)
                            rightLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                        //下侧点集
                        if (contours[i][j].x > x1 + radianEliminate2 && contours[i][j].x < x4 - radianEliminate2 && abs(contours[i][j].y - y2) < deviation2)
                            downLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                    }
                    //上侧点集
                    upLinePoint.push_back(Point((x1 + x4) / 2, (y1 + y4) / 2));
                    if (leftLinePoint.size() != 0 && rightLinePoint.size() != 0 && downLinePoint.size() != 0)
                    {
                        //拟合直线
                        fitLine(leftLinePoint, leftLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);               //左侧拟合直线
                        fitLine(rightLinePoint, rightLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);				//右侧拟合直线
                        fitLine(downLinePoint, downLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);				//下侧拟合直线
                        upLine_Fit[0] = downLine_Fit[0];
                        upLine_Fit[1] = downLine_Fit[1];
                        upLine_Fit[2] = upLinePoint[0].x;
                        upLine_Fit[3] = upLinePoint[0].y;                                               //上侧直线确定
                                                                                                        //角点提取
                        src_corner[0] = getPointSlopeCrossPoint(upLine_Fit, leftLine_Fit);              //左上角点
                        src_corner[1] = getPointSlopeCrossPoint(downLine_Fit, leftLine_Fit);		    //左下角点
                        src_corner[2] = getPointSlopeCrossPoint(downLine_Fit, rightLine_Fit);	        //右下角点
                        src_corner[3] = getPointSlopeCrossPoint(upLine_Fit, rightLine_Fit);		        //右上角点
                                                                                                        //透视变换矩阵计算
                        if (ScreenType_Flag == "矩形屏")
                            dst_corner = { Point(0, 0), Point(0, 1183), Point(3000, 1183), Point(3000, 0) };
                        else
                            dst_corner = { Point(0, 0), Point(0, 1000), Point(3000, 1000), Point(3000, 0) };
                        *Mwhite = cv::getPerspectiveTransform(src_corner, dst_corner);
                    }
                    else
                    {
                        displayError_Areasignal = 0;
                        break;
                    }
                }
            }
        }
    }
    //没有提取到屏幕
    if (displayError_Areasignal == 0)
    {
        Ext_Result_Left_Right = true; //没有提取到屏幕
        vector<Point2f> src_points(4);
        src_points = { Point(0, 0), Point(0, 10), Point(10, 10), Point(10, 0) };
        vector<Point2f> dst_points(4);
        if (ScreenType_Flag == "矩形屏")
            dst_points = { Point(0, 0), Point(0, 1775), Point(3000, 1775), Point(3000, 0) };
        else
            dst_points = { Point(0, 0), Point(0, 1500), Point(3000, 1500), Point(3000, 0) };
        *Mwhite = cv::getPerspectiveTransform(src_points, dst_points);                        //透视变换矩阵提取
    }

    return Ext_Result_Left_Right;
}

/*=========================================================
*@函 数 名:              f_LeftRightCam_PersTransMatCal
*@功能描述:              左右相机R角透视变换矩阵计算函数
*@param _src             输入灰度/彩色图像
*@param Mwhite           白底透视变换矩阵
*@ScreenType_Flag        屏幕类型
*@编制时间：		     2020年8月20日
*@备注说明              use
=========================================================*/
bool f_LeftRightCam_PersTransMatCal(InputArray _src, Mat *Mwhite,Mat *M_R_1_E, QString ScreenType_Flag,int border_white)
{
    bool Ext_Result_Left_Right;                                                     //提取屏幕成功标志位
    Mat src = _src.getMat();                                                        //输入源图像
    if (src.type() == CV_8UC1)														//若输入8位图
        src = src.clone();															//拷贝原图
    else
        cvtColor(src, src, CV_BGR2GRAY);										    //灰度化彩色图
    CV_Assert(src.depth() == CV_8U);                                                //8位无符号
    Mat binaryImage = Mat::zeros(src.size(), CV_8UC1);                              //二值图像
    threshold(src, binaryImage, 40, 255, CV_THRESH_BINARY);							//二值化(有问题)
    medianBlur(binaryImage, binaryImage, 5);										//中值滤波去除锯齿
    int displayError_Areasignal = 0;												//根据轮廓面积判定显示异常标志位
    vector<vector<Point>> contours;													//contours存放点集信息
    findContours(binaryImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);    //CV_RETR_EXTERNAL最外轮廓，CV_CHAIN_APPROX_NONE所有轮廓点信息
    vector<Point> upLinePoint, leftLinePoint, downLinePoint, rightLinePoint;		//上左下右侧点集数据
    Vec4f upLine_Fit, leftLine_Fit, downLine_Fit, rightLine_Fit;				    //上左下右拟合直线数据
    vector<Point2f> src_corner(4);                                                  //四个边相交得到角点坐标
    vector<Point2f> src_corner_enlarge(4);
    Rect rect;																        //最小正外接矩形
    int x1, y1, x2, y2, x3, y3, x4, y4;			                                    //正接矩阵坐标点信息
    vector<Point2f> dst_corner(4);                                                  //透视变换后的点的信息
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 1500000 && area < 5000000)
        {
            displayError_Areasignal++;
            rect = cv::boundingRect(contours[i]);                                           //最小外接矩形提取
            Ext_Result_Left_Right = false;                                                             //提取到屏幕

            //cv::rectangle(src, rect, Scalar(255, 0, 0), 5, LINE_8, 0);
            x1 = rect.tl().x;//左上角
            y1 = rect.tl().y;//左上角
            x2 = rect.tl().x;//左下角
            y2 = rect.br().y;//左下角
            x3 = rect.br().x;//右下角
            y3 = rect.br().y;//右下角
            x4 = rect.br().x;//右上角
            y4 = rect.tl().y;//右上角
                                                                                                       //矩形面积缩小1/3，并得到新的矩形顶点
            //取直线的参数设置
            int radianEliminate = 230;//(R角)左右使用
            int radianEliminate2 = 360;//(R角)上下使用
            int deviation = 200;//(斜线带来的误差)左右使用
            int deviation2 = 120;//(斜线带来的误差)上下使用

                if (displayError_Areasignal != 0)
                {
                    for (int j = 0; j < contours[i].size(); j++)
                    {
                        //左侧点集
                        if (contours[i][j].y > y1 + radianEliminate && contours[i][j].y < y1 + (y2 - y1)*0.3 && abs(contours[i][j].x - x1) < deviation ||
                            contours[i][j].y > y1 + (y2 - y1)*0.7 && contours[i][j].y < y2 - radianEliminate && abs(contours[i][j].x - x1) < deviation)
                            leftLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                        //右侧点集
                        if (contours[i][j].y > y1 + radianEliminate && contours[i][j].y < y2 - radianEliminate &&abs(contours[i][j].x - x3) < deviation)
                            rightLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                        //上侧点集
                        if (contours[i][j].x > x1 + radianEliminate2 && contours[i][j].x < x4 - radianEliminate2 && abs(contours[i][j].y - y1) < deviation2)
                            upLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                        if (contours[i][j].x > x1 + radianEliminate2 && contours[i][j].x < x4 - radianEliminate2 && abs(contours[i][j].y - y2) < deviation2)
                            downLinePoint.push_back(Point(contours[i][j].x, contours[i][j].y));
                    }
                    //下侧点集
                    //downLinePoint.push_back(Point((x2 + x3) / 2, (y2 + y3) / 2));
                    if (leftLinePoint.size() != 0 && rightLinePoint.size() != 0 && upLinePoint.size() != 0 && downLinePoint.size() != 0)
                    {
                        //直线拟合
                        fitLine(leftLinePoint, leftLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);               //左侧拟合直线
                        fitLine(rightLinePoint, rightLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);				//右侧拟合直线
                        fitLine(upLinePoint, upLine_Fit, cv::DIST_L2, 0, 1e-2, 1e-2);					//上侧拟合直线
                        downLine_Fit[0] = upLine_Fit[0];
                        downLine_Fit[1] = upLine_Fit[1];
                        downLine_Fit[2] = downLinePoint[0].x;
                        downLine_Fit[3] = downLinePoint[0].y;                                           //下侧直线确定

                        /*Mat img = _src.getMat();
                        for (int i = 0; i < leftLinePoint.size(); i++)
                        {
                            circle(img, Point(leftLinePoint[i].x, leftLinePoint[i].y), 8, Scalar(0, 0, 255), -1);
                        }
                        for (int i = 0; i < rightLinePoint.size(); i++)
                        {
                            circle(img, Point(rightLinePoint[i].x, rightLinePoint[i].y), 8, Scalar(0, 0, 255), -1);
                        }
                        for (int i = 0; i < upLinePoint.size(); i++)
                        {
                            circle(img, Point(upLinePoint[i].x, upLinePoint[i].y), 8, Scalar(0, 0, 255), -1);
                        }
                        for (int i = 0; i < downLinePoint.size(); i++)
                        {
                            circle(img, Point(downLinePoint[i].x, downLinePoint[i].y), 8, Scalar(0, 0, 255), -1);
                        }*/
                                                                                //角点提取
                        src_corner[0] = getPointSlopeCrossPoint(upLine_Fit, leftLine_Fit);              //左上角点
                        src_corner[1] = getPointSlopeCrossPoint(downLine_Fit, leftLine_Fit);		    //左下角点
                        src_corner[2] = getPointSlopeCrossPoint(downLine_Fit, rightLine_Fit);	        //右下角点
                        src_corner[3] = getPointSlopeCrossPoint(upLine_Fit, rightLine_Fit);		        //右上角点


                        //src_corner_enlarge[0] = Point2f(xcoordinate1 + tl.x - border_white, ycoordinate1 + tl.y - border_white);	                         //左上角
                        //src_corner_enlarge[1] = Point2f(xcoordinate2 + bl.x - border_white, ycoordinate2 - height / 3 + bl.y + border_white);              //左下角
                        //src_corner_enlarge[2] = Point2f(xcoordinate3 - width / 4 + br.x + border_white, ycoordinate3 - height / 3 + br.y + border_white);	 //右下角
                        //src_corner_enlarge[3] = Point2f(xcoordinate4 - width / 4 + tr.x + border_white, ycoordinate4 + tr.y - border_white);	             //右上角

                        src_corner_enlarge[0].y = src_corner[0].y - border_white;
                        src_corner_enlarge[0].x = src_corner[0].x - border_white;
                        src_corner_enlarge[1].y = src_corner[1].y + border_white;
                        src_corner_enlarge[1].x = src_corner[1].x - border_white;
                        src_corner_enlarge[2].y = src_corner[2].y + border_white;
                        src_corner_enlarge[2].x = src_corner[2].x + border_white;
                        src_corner_enlarge[3].y = src_corner[3].y - border_white;
                        src_corner_enlarge[3].x = src_corner[3].x + border_white;
                                                                                                        //透视变换矩阵计算
                        if (ScreenType_Flag == "矩形屏")
                            dst_corner = { Point(0, 0), Point(0, 1183), Point(3000, 1183), Point(3000, 0) };
                        else
                            dst_corner = { Point(0, 0), Point(0, 1500), Point(pixel_num, 1500), Point(pixel_num, 0) };
                        *Mwhite = cv::getPerspectiveTransform(src_corner, dst_corner);
                        *M_R_1_E = cv::getPerspectiveTransform(src_corner_enlarge, dst_corner);
                    }
                    else
                    {
                        displayError_Areasignal = 0;
                        break;
                    }
                }
        }
    }
    //没有提取到屏幕
    if (displayError_Areasignal == 0)
    {
        Ext_Result_Left_Right = true; //没有提取到屏幕
        vector<Point2f> src_points(4);
        src_points = { Point(0, 0), Point(0, 10), Point(10, 10), Point(10, 0) };
        vector<Point2f> dst_points(4);
        if (ScreenType_Flag == "矩形屏")
            dst_points = { Point(0, 0), Point(0, 1775), Point(3000, 1775), Point(3000, 0) };
        else
            dst_points = { Point(0, 0), Point(0, 1500), Point(pixel_num, 1500), Point(pixel_num, 0) };
        *Mwhite = cv::getPerspectiveTransform(src_points, dst_points);                        //透视变换矩阵提取
        *M_R_1_E = cv::getPerspectiveTransform(src_points, dst_points);
    }

    return Ext_Result_Left_Right;
}


/*=========================================================
*@函 数 名:     getPointSlopeCrossPoint
*@功能描述:     计算点斜式两条直线的交点
*@param LineA   平行线条
*@param LineB   垂直线条
*@编制时间：    2020年8月17日
*@备注说明
=========================================================*/
Point2f getPointSlopeCrossPoint(Vec4f LineA, Vec4f LineB)
{
    const double PI = 3.1415926535897;
    Point2f crossPoint;
    double kA = LineA[1] / LineA[0];
    double kB = LineB[1] / LineB[0];
    double theta = atan2(LineB[1], LineB[0]);
    if (theta == PI * 0.5)
    {
        crossPoint.x = LineB[0];
        crossPoint.y = kA * LineB[0] + LineA[3] - kA * LineA[2];
        return crossPoint;
    }
    double bA = LineA[3] - kA * LineA[2];
    double bB = LineB[3] - kB * LineB[2];
    crossPoint.x = (bB - bA) / (kA - kB);
    crossPoint.y = (kA*bB - kB * bA) / (kA - kB);
    return crossPoint;
}


/*=========================================================
* 函 数 名: RoiWhite_Arcangle
* 功能描述: 主黑白相机带R角屏幕的透视变换计算
* 说明: 圆弧角屏幕透视变换和直角屏采用两种方案 Arcangle弧度角
* 输入: 主黑白相机拍摄白底图片
* 输出：
* 时间：2020年05月28日
* 其他：
=========================================================*/
bool RoiWhite_Arcangle(Mat src_white, int border_white, int border_black, int border_lougunag, Mat *Mwhite, Mat *Mblack, Mat *Mlouguang, int ID)//白色底色下提取黑色ROI
{
    bool isArea_1, isArea_2;
    //Mat img_gray = src_white.clone();
    Mat img_gray;
    cvtColor(src_white, img_gray, CV_BGR2GRAY);
    Mat th1;
    threshold(img_gray, th1, 35, 255, CV_THRESH_BINARY);
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat  erode1;
    erode(th1, erode1, structure_element1);
    int unm_contours = 0;
    vector<vector<Point>> contours;
    findContours(erode1, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    Rect rect1;
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 2500000 && area < 5000000)
        {
            unm_contours++;
            rect1 = cv::boundingRect(contours[i]);
        }
    }
    if (unm_contours > 0 && ID == 1)
        isArea_1 = false;
    if (unm_contours == 0 && ID == 1)
        isArea_1 = true;
    if (unm_contours > 0 && ID == 2)
        isArea_2 = false;
    if (unm_contours == 0 && ID == 2)
        isArea_2 = true;
    if (unm_contours == 0)//未提取到屏幕
    {
        cv::Point2f src_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 10),
            cv::Point2f(10, 10),
            cv::Point2f(10, 0)
        };
        cv::Point2f dst_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 1500),
            cv::Point2f(3000, 1775) ,
            cv::Point2f(3000,0) };
        *Mwhite = cv::getPerspectiveTransform(src_points, dst_points);
        *Mlouguang = cv::getPerspectiveTransform(src_points, dst_points);
        *Mblack = cv::getPerspectiveTransform(src_points, dst_points);
    }
    else
    {
        //左上角
        int x1 = rect1.tl().x;
        int y1 = rect1.tl().y;
        //左下角
        int x2 = rect1.tl().x;
        int y2 = rect1.br().y;
        //右下角
        int x3 = rect1.br().x;
        int y3 = rect1.br().y;
        //右上角
        int x4 = rect1.br().x;
        int y4 = rect1.tl().y;
        int width = rect1.width;
        int height = rect1.height;
        //防止候选框越界处理
        int border = 50;
        int ycoordinate1 = y1 - border;//左上角防止y越界
        int ycoordinate2 = y2 + border;//左下角防止y越界
        int ycoordinate3 = y3 + border;//右下角防止y越界
        int ycoordinate4 = y4 - border;//右上角防止y越界
                                       //左上角防止y越界
        if (ycoordinate1 < 0)
            ycoordinate1 = 0;
        //左下角防止y越界
        if (ycoordinate2 >= src_white.rows)
            ycoordinate2 = src_white.rows - 1;
        //右下角防止y越界
        if (ycoordinate3 >= src_white.rows)
            ycoordinate3 = src_white.rows - 1;
        //右上角防止y越界
        if (ycoordinate4 < 0)
            ycoordinate4 = 0;
        int xcoordinate1 = x1 - border;//左上角防止x越界
        int xcoordinate2 = x2 - border;//左下角防止x越界
        int xcoordinate3 = x3 + border;//右下角防止x越界
        int xcoordinate4 = x4 + border;//右上角防止x越界
                                       //左上角防止x越界
        if (xcoordinate1 < 0)
            xcoordinate1 = 0;
        //左下角防止x越界
        if (xcoordinate2 < 0)
            xcoordinate2 = 0;
        //右下角防止y越界
        if (xcoordinate3 >= src_white.cols)
            xcoordinate3 = src_white.cols - 1;
        //右上角防止y越界
        if (xcoordinate4 >= src_white.cols)
            xcoordinate4 = src_white.cols - 1;
        vector<Point2f> src_corner(4);
        vector<Point2f> src_corner1(4);
        vector<Point2f> src_corner_bianyuan(4);
        Point2f tl = CrossPointReturn(erode1, xcoordinate1, ycoordinate1, width / 4, height / 3);		                       //左上角
        Point2f bl = CrossPointReturn(erode1, xcoordinate2, ycoordinate2 - height / 3, width / 4, height / 3);			   //左下角
        Point2f br = CrossPointReturn(erode1, xcoordinate3 - width / 4, ycoordinate3 - height / 3, width / 4, height / 3); //右下角
        Point2f tr = CrossPointReturn(erode1, xcoordinate4 - width / 4, ycoordinate4, width / 4, height / 3);	               //右上角
        if (tl != Point2f(-1, -1) && bl != Point2f(-1, -1) && br != Point2f(-1, -1) && tr != Point2f(-1, -1))
        {
            src_corner[0] = Point2f(xcoordinate1 + tl.x, ycoordinate1 + tl.y);	                         //左上角
            src_corner[1] = Point2f(xcoordinate2 + bl.x, ycoordinate2 - height / 3 + bl.y);              //左下角
            src_corner[2] = Point2f(xcoordinate3 - width / 4 + br.x, ycoordinate3 - height / 3 + br.y);	 //右下角
            src_corner[3] = Point2f(xcoordinate4 - width / 4 + tr.x, ycoordinate4 + tr.y);	             //右上角
                                                                                                         //对4个角点的坐标位置进行微调（白底图以及黑底图）
            src_corner[0].x = src_corner[0].x - border_white;
            src_corner[0].y = src_corner[0].y - border_white;
            src_corner[1].x = src_corner[1].x - border_white;
            src_corner[1].y = src_corner[1].y + border_white;
            src_corner[2].x = src_corner[2].x + border_white;
            src_corner[2].y = src_corner[2].y + border_white;
            src_corner[3].x = src_corner[3].x + border_white;
            src_corner[3].y = src_corner[3].y - border_white;
            //对4个角点的坐标位置进行微调（漏光检测图）
            src_corner1[0].x = src_corner[0].x - border_lougunag;
            src_corner1[0].y = src_corner[0].y - border_lougunag;
            src_corner1[1].x = src_corner[1].x - border_lougunag;
            src_corner1[1].y = src_corner[1].y + border_lougunag;
            src_corner1[2].x = src_corner[2].x + border_lougunag;
            src_corner1[2].y = src_corner[2].y + border_lougunag;
            src_corner1[3].x = src_corner[3].x + border_lougunag;
            src_corner1[3].y = src_corner[3].y - border_lougunag;

            //边界往外扩
            src_corner_bianyuan[0] = Point2f(xcoordinate1 + tl.x-border_black, ycoordinate1 + tl.y - border_black);	                         //左上角
            src_corner_bianyuan[1] = Point2f(xcoordinate2 + bl.x - border_black, ycoordinate2 - height / 3 + bl.y +border_black);              //左下角
            src_corner_bianyuan[2] = Point2f(xcoordinate3 - width / 4 + br.x + border_black, ycoordinate3 - height / 3 + br.y + border_black);	 //右下角
            src_corner_bianyuan[3] = Point2f(xcoordinate4 - width / 4 + tr.x + border_black, ycoordinate4 + tr.y - border_black);	             //右上角
        }
        else
        {
            src_corner[0] = Point2f(0, 0);
            src_corner[1] = Point2f(0, 10);
            src_corner[2] = Point2f(10, 10);
            src_corner[3] = Point2f(10, 0);
        }
        vector<Point2f> dst_corner(4);
        dst_corner[0] = Point(0, 0);
        dst_corner[1] = Point(0, 1500);
        dst_corner[2] = Point(3000, 1500);
        dst_corner[3] = Point(3000, 0);
        *Mwhite = cv::getPerspectiveTransform(src_corner, dst_corner);
        *Mlouguang = cv::getPerspectiveTransform(src_corner1, dst_corner);
        *Mblack = cv::getPerspectiveTransform(src_corner_bianyuan, dst_corner);
    }
    return true;
}


/*=========================================================
* 函 数 名: RoiWhite_Arcangle_Abshow
* 功能描述: 显示异常白底专用主黑白相机带R角屏幕的透视变换计算
* 说明: 圆弧角屏幕透视变换和直角屏采用两种方案 Arcangle弧度角
* 输入: 主黑白相机拍摄白底图片
* 修改时间：2020年09月14日
=========================================================*/
bool RoiWhite_Arcangle_Abshow(Mat src_white, int border_white, int border_black, int border_lougunag, Mat *Mwhite, Mat *Mblack, Mat *Mlouguang, int ID)//白色底色下提取黑色ROI
{
    bool isArea_1, isArea_2;
    //Mat img_gray = src_white.clone();
    Mat img_gray;
    cvtColor(src_white, img_gray, CV_BGR2GRAY);
    Mat th1;
    threshold(img_gray, th1, 20, 255, CV_THRESH_BINARY);
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat  erode1;
    erode(th1, erode1, structure_element1);
    Mat img_temp1, img_temp2, img_temp3, img_temp4;
    int X_1, Y_1, X_2, Y_2, w, h;
    int X_lt, Y_lt, X_rt, Y_rt, X_lb, Y_lb, X_rb, Y_rb, w1, h1;
    int unm_contours = 0;
    vector<vector<Point>> contours1;
    vector<vector<Point>> contours;
    findContours(erode1, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    Rect rect1;
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 2500000 && area < 5000000)
        {
            unm_contours++;
            rect1 = cv::boundingRect(contours[i]);
        }
    }
    if (unm_contours > 0 && ID == 1)
    {
        isArea_1 = false;
    }
    if (unm_contours == 0 && ID == 1)
    {
        isArea_1 = true;
    }
    if (unm_contours > 0 && ID == 2)
    {
        isArea_2 = false;
    }
    if (unm_contours == 0 && ID == 2)
    {
        isArea_2 = true;
    }
    if (unm_contours == 0)//未提取到屏幕
    {
        cv::Point2f src_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 10),
            cv::Point2f(10, 10),
            cv::Point2f(10, 0)
        };
        cv::Point2f dst_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 1500),
            cv::Point2f(3000, 1775) ,
            cv::Point2f(3000,0) };
        *Mwhite = cv::getPerspectiveTransform(src_points, dst_points);
        *Mlouguang = cv::getPerspectiveTransform(src_points, dst_points);
        *Mblack = cv::getPerspectiveTransform(src_points, dst_points);
    }
    else
    {
        int x1 = rect1.tl().x;
        int y1 = rect1.tl().y;
        int x2 = rect1.tl().x;
        int y2 = rect1.br().y;
        int x3 = rect1.br().x;
        int y3 = rect1.br().y;
        int x4 = rect1.br().x;
        int y4 = rect1.tl().y;
        int width = rect1.width;
        int height = rect1.height;

        //防止候选框越界处理
        int border = 50;
        int ycoordinate1 = y1 - border;//左上角防止y越界
        int ycoordinate2 = y2 + border;//左下角防止y越界
        int ycoordinate3 = y3 + border;//右下角防止y越界
        int ycoordinate4 = y4 - border;//右上角防止y越界
                                       //左上角防止y越界
        if (ycoordinate1 < 0)
            ycoordinate1 = 0;
        //左下角防止y越界
        if (ycoordinate2 >= src_white.rows)
            ycoordinate2 = src_white.rows - 1;
        //右下角防止y越界
        if (ycoordinate3 >= src_white.rows)
            ycoordinate3 = src_white.rows - 1;
        //右上角防止y越界
        if (ycoordinate4 < 0)
            ycoordinate4 = 0;
        int xcoordinate1 = x1 - border;//左上角防止x越界
        int xcoordinate2 = x2 - border;//左下角防止x越界
        int xcoordinate3 = x3 + border;//右下角防止x越界
        int xcoordinate4 = x4 + border;//右上角防止x越界
                                       //左上角防止x越界
        if (xcoordinate1 < 0)
            xcoordinate1 = 0;
        //左下角防止x越界
        if (xcoordinate2 < 0)
            xcoordinate2 = 0;
        //右下角防止y越界
        if (xcoordinate3 >= src_white.cols)
            xcoordinate3 = src_white.cols - 1;
        //右上角防止y越界
        if (xcoordinate4 >= src_white.cols)
            xcoordinate4 = src_white.cols - 1;

        vector<Point2f> src_corner(4);
        vector<Point2f> src_corner1(4);
        Point2f tl = CrossPointReturn(erode1, xcoordinate1, ycoordinate1, width / 4, height / 3);		                       //左上角
        Point2f bl = CrossPointReturn(erode1, xcoordinate2, ycoordinate2 - height / 3, width / 4, height / 3);			   //左下角
        Point2f br = CrossPointReturn(erode1, xcoordinate3 - width / 4, ycoordinate3 - height / 3, width / 4, height / 3); //右下角
        Point2f tr = CrossPointReturn(erode1, xcoordinate4 - width / 4, ycoordinate4, width / 4, height / 3);	               //右上角
        if (tl != Point2f(-1, -1) && bl != Point2f(-1, -1) && br != Point2f(-1, -1) && tr != Point2f(-1, -1))  //如果找不到两条直线,会返回-1点,来表示没有提取到
        {
            src_corner[0] = Point2f(x1 - 100 + tl.x, y1 - 100 + tl.y);					          //左上角
            src_corner[1] = Point2f(x2 - 100 + bl.x, y2 + 100 - height / 3 + bl.y);			      //左下角
            src_corner[2] = Point2f(x3 + 100 - width / 4 + br.x, y3 + 100 - height / 3 + br.y);	  //右下角
            src_corner[3] = Point2f(x4 + 100 - width / 4 + tr.x, y4 - 100 + tr.y);	              //右上角
                                                                                                  //对4个角点的坐标位置进行微调
            src_corner[0].x = src_corner[0].x - border_white + 10;
            src_corner[0].y = src_corner[0].y - border_white + 10;
            src_corner[1].x = src_corner[1].x - border_white + 10;
            src_corner[1].y = src_corner[1].y + border_white - 10;
            src_corner[2].x = src_corner[2].x + border_white - 10;
            src_corner[2].y = src_corner[2].y + border_white - 10;
            src_corner[3].x = src_corner[3].x + border_white - 10;
            src_corner[3].y = src_corner[3].y - border_white + 10;


            //src_corner[0].x = src_corner[0].x - border_white;
            //src_corner[0].y = src_corner[0].y - border_white;
            //src_corner[1].x = src_corner[1].x - border_white;
            //src_corner[1].y = src_corner[1].y + border_white;
            //src_corner[2].x = src_corner[2].x + border_white;
            //src_corner[2].y = src_corner[2].y + border_white;
            //src_corner[3].x = src_corner[3].x + border_white;
            //src_corner[3].y = src_corner[3].y - border_white;


            int black_louuang = 90;
            src_corner1[0].x = src_corner[0].x - black_louuang;
            src_corner1[0].y = src_corner[0].y - black_louuang;
            src_corner1[1].x = src_corner[1].x - black_louuang;
            src_corner1[1].y = src_corner[1].y + black_louuang;
            src_corner1[2].x = src_corner[2].x + black_louuang;
            src_corner1[2].y = src_corner[2].y + black_louuang;
            src_corner1[3].x = src_corner[3].x + black_louuang;
            src_corner1[3].y = src_corner[3].y - black_louuang;

        }
        else
        {
            src_corner[0] = Point2f(0, 0);
            src_corner[1] = Point2f(0, 10);
            src_corner[2] = Point2f(10, 10);
            src_corner[3] = Point2f(10, 0);
        }
        vector<Point2f> dst_corner(4);
        dst_corner[0] = Point(0, 0);
        dst_corner[1] = Point(0, 1500);
        dst_corner[2] = Point(3000, 1500);
        dst_corner[3] = Point(3000, 0);
        *Mwhite = cv::getPerspectiveTransform(src_corner, dst_corner);
        *Mlouguang = cv::getPerspectiveTransform(src_corner1, dst_corner);
        *Mblack = cv::getPerspectiveTransform(src_corner, dst_corner);
    }
    return true;
}

/*=========================================================
* 函 数 名: RoiColor_Arcangle
* 功能描述: 主彩色网相机带R角屏幕的透视变换计算
* 说明: 圆弧角屏幕透视变换和直角屏采用两种方案
* 输入: 主黑白相机拍摄白底图片
* 输出：
* 时间：2020年05月28日
* 其他：
=========================================================*/
bool RoiColor_Arcangle(Mat src_white, int border_white, int border_black, int border_lougunag, Mat *Mwhite, Mat *Mblack, Mat *Mlouguang, int ID)
{
    Mat img_gray;
    cvtColor(src_white, img_gray, CV_BGR2GRAY);
    Mat th1;
    threshold(img_gray, th1, 20, 255, CV_THRESH_BINARY);
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat  erode1;
    erode(th1, erode1, structure_element1);
    Mat img_temp1, img_temp2, img_temp3, img_temp4;
    int X_1, Y_1, X_2, Y_2, w, h;
    int X_lt, Y_lt, X_rt, Y_rt, X_lb, Y_lb, X_rb, Y_rb, w1, h1;
    int unm_contours = 0;
    vector<vector<Point>> contours1;
    vector<vector<Point>> contours;
    findContours(erode1, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    Rect rect1;
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 2500000 && area < 5000000)
        {
            unm_contours++;
            rect1 = cv::boundingRect(contours[i]);
        }
    }
    if (unm_contours == 0)//未提取到屏幕
    {
        cv::Point2f src_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 10),
            cv::Point2f(10, 10),
            cv::Point2f(10, 0)
        };
        cv::Point2f dst_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 1500),
            cv::Point2f(3000, 1775) ,
            cv::Point2f(3000,0) };
        *Mwhite = cv::getPerspectiveTransform(src_points, dst_points);
        *Mlouguang = cv::getPerspectiveTransform(src_points, dst_points);
        *Mblack = cv::getPerspectiveTransform(src_points, dst_points);
    }
    else
    {
        int x1 = rect1.tl().x;
        int y1 = rect1.tl().y;
        int x2 = rect1.tl().x;
        int y2 = rect1.br().y;
        int x3 = rect1.br().x;
        int y3 = rect1.br().y;
        int x4 = rect1.br().x;
        int y4 = rect1.tl().y;
        int width = rect1.width;
        int height = rect1.height;
        vector<Point2f> src_corner(4);
        Point2f tl = CrossPointReturn(erode1, x1 - 100, y1 - 100, width / 4, height / 3);
        Point2f bl = CrossPointReturn(erode1, x2 - 100, y2 + 100 - height / 3, width / 4, height / 3);
        Point2f br = CrossPointReturn(erode1, x3 + 100 - width / 4, y3 + 100 - height / 3, width / 4, height / 3);
        Point2f tr = CrossPointReturn(erode1, x4 + 100 - width / 4, y4 - 100, width / 4, height / 3);
        if (tl != Point2f(-1, -1) && bl != Point2f(-1, -1) && br != Point2f(-1, -1) && tr != Point2f(-1, -1))  //如果找不到两条直线,会返回-1点,来表示没有提取到
        {
            src_corner[0] = Point2f(x1 - 100 + tl.x, y1 - 100 + tl.y);					          //左上角
            src_corner[1] = Point2f(x2 - 100 + bl.x, y2 + 100 - height / 3 + bl.y);			      //左下角
            src_corner[2] = Point2f(x3 + 100 - width / 4 + br.x, y3 + 100 - height / 3 + br.y);	  //右下角
            src_corner[3] = Point2f(x4 + 100 - width / 4 + tr.x, y4 - 100 + tr.y);	              //右上角
            //对4个角点的坐标位置进行微调
            //src_corner[0].x = src_corner[0].x + border_white;
            //src_corner[1].y = src_corner[1].y + border_white;
            //src_corner[1].x = src_corner[1].x - border_white;
            //src_corner[2].y = src_corner[2].y + border_white;
            //src_corner[2].x = src_corner[2].x + border_white;
            //src_corner[3].y = src_corner[3].y + border_white;
            //src_corner[3].x = src_corner[3].x + border_white;
        }
        else
        {
            src_corner[0] = Point2f(0, 0);
            src_corner[1] = Point2f(0, 10);
            src_corner[2] = Point2f(10, 10);
            src_corner[3] = Point2f(10, 0);
        }
        vector<Point2f> dst_corner(4);
        dst_corner[0] = Point(0, 0);
        dst_corner[1] = Point(0, 1500);
        dst_corner[2] = Point(3000, 1500);
        dst_corner[3] = Point(3000, 0);
        *Mwhite = cv::getPerspectiveTransform(src_corner, dst_corner);
        *Mlouguang = cv::getPerspectiveTransform(src_corner, dst_corner);
        *Mblack = cv::getPerspectiveTransform(src_corner, dst_corner);
    }
    return true;
}

/*=========================================================
* 函 数 名: RoiSide_Arcangle
* 功能描述: 左右相机带R角弧度屏幕核函数计算
* 说明: 圆弧角屏幕透视变换和直角屏采用两种方案，Arcangle弧形角
* 输入: 左右侧相机拍摄白底图片
* 输出：左右相机透视变换矩阵
* 时间：2020年05月28日
* 其他：
=========================================================*/
bool RoiSide_Arcangle(Mat src_white, int border_white, Mat *Mwhite,Mat *Mwhite_enlarge)
{
    bool isCeshi;
    //Mat img_gray = src_white.clone();
    Mat th1;
    Mat img_gray;
    cvtColor(src_white,img_gray,CV_BGR2GRAY);
    threshold(img_gray, th1, 20, 255, CV_THRESH_BINARY);
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat  erode1;
    erode(th1, erode1, structure_element1);
    Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));
    morphologyEx(erode1, erode1, CV_MOP_OPEN, element);
    Mat img_temp1, img_temp2, img_temp3, img_temp4;
    vector<vector<Point>> contours1;
    vector<vector<Point>> contours;
    std::vector<cv::Point> approxCurve;
    int num_contours = 0;
    findContours(erode1, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    Rect rect1;
    Point2f P[4];
    Point2f shunxu[4];
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 1000000 && area < 4000000)
        {
            num_contours++;
            rect1 = cv::boundingRect(contours[i]);
        }
    }
    if (num_contours == 0)
    {
        isCeshi = true; //没有提取到屏幕
        cv::Point2f src_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 10),
            cv::Point2f(10, 10),
            cv::Point2f(10, 0)
        };
        cv::Point2f dst_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 1500),
            cv::Point2f(3000, 1500),
            cv::Point2f(1500,0)
        };
        *Mwhite = cv::getPerspectiveTransform(src_points, dst_points);
        *Mwhite_enlarge=*Mwhite;
    }
    else
    {
        isCeshi = false; //没有提取到屏幕; //提取到屏幕
        //左上角
        int x1 = rect1.tl().x;
        int y1 = rect1.tl().y;
        //左下角
        int x2 = rect1.tl().x;
        int y2 = rect1.br().y;
        //右下角
        int x3 = rect1.br().x;
        int y3 = rect1.br().y;
        //右上角
        int x4 = rect1.br().x;
        int y4 = rect1.tl().y;
        int  width = rect1.width;
        int height = rect1.height;

        //防止候选框越界处理
        int border = 50;
        int ycoordinate1 = y1 - border;//左上角防止y越界
        int ycoordinate2 = y2 + border;//左下角防止y越界
        int ycoordinate3 = y3 + border;//右下角防止y越界
        int ycoordinate4 = y4 - border;//右上角防止y越界
        //左上角防止y越界
        if (ycoordinate1 < 0)
        {
            ycoordinate1 = 0;
        }
        //左下角防止y越界
        if (ycoordinate2 > src_white.rows)
        {
            ycoordinate2 = src_white.rows;
        }
        //右下角防止y越界
        if (ycoordinate3 > src_white.rows)
        {
            ycoordinate3 = src_white.rows;
        }
        //右上角防止y越界
        if (ycoordinate4 < 0)
        {
            ycoordinate4 = 0;
        }
        int xcoordinate1 = x1 - border;//左上角防止x越界
        int xcoordinate2 = x2 - border;//左下角防止x越界
        int xcoordinate3 = x3 + border;//右下角防止x越界
        int xcoordinate4 = x4 + border;//右上角防止x越界
        //左上角防止x越界
        if (xcoordinate1 < 0)
        {
            xcoordinate1 = 0;
        }
        //左下角防止x越界
        if (xcoordinate2 <0)
        {
            xcoordinate2 = 0;
        }
        //右下角防止y越界
        if (xcoordinate3 > src_white.cols)
        {
            xcoordinate3 = src_white.cols;
        }
        //右上角防止y越界
        if (xcoordinate4 > src_white.cols)
        {
            xcoordinate4 = src_white.cols;
        }
        vector<Point2f> src_corner(4);
        vector<Point2f> src_corner_enlarge(4);
        Point2f tl = CrossPointReturn(erode1, xcoordinate1, ycoordinate1, width / 4, height / 3);		                   //左上角
        Point2f bl = CrossPointReturn(erode1, xcoordinate2, ycoordinate2 - height / 3, width / 4, height / 3);			   //左下角
        Point2f br = CrossPointReturn(erode1, xcoordinate3 - width / 4, ycoordinate3 - height / 3, width / 4, height / 3); //右下角
        Point2f tr = CrossPointReturn(erode1, xcoordinate4 - width / 4, ycoordinate4, width / 4, height / 3);	           //右上角
        if (tl != Point2f(-1, -1) && bl != Point2f(-1, -1) && br != Point2f(-1, -1) && tr != Point2f(-1, -1))
        {
            src_corner[0] = Point2f(x1 - border + tl.x, y1 - border + tl.y);	                         //左上角
            src_corner[1] = Point2f(x2 - border + bl.x, y2 + border - height / 3 + bl.y);                //左下角
            src_corner[2] = Point2f(x3 + border - width / 4 + br.x, y3 + border - height / 3 + br.y);	 //右下角
            src_corner[3] = Point2f(x4 + border - width / 4 + tr.x, y4 - border + tr.y);	             //右上角

            //边界扩大
            src_corner_enlarge[0] = Point2f(xcoordinate1 + tl.x-border_white, ycoordinate1 + tl.y - border_white);	                         //左上角
            src_corner_enlarge[1] = Point2f(xcoordinate2 + bl.x - border_white, ycoordinate2 - height / 3 + bl.y +border_white);              //左下角
            src_corner_enlarge[2] = Point2f(xcoordinate3 - width / 4 + br.x + border_white, ycoordinate3 - height / 3 + br.y + border_white);	 //右下角
            src_corner_enlarge[3] = Point2f(xcoordinate4 - width / 4 + tr.x + border_white, ycoordinate4 + tr.y - border_white);	             //右上角
            ////对4个角点的坐标位置进行微调
            //src_corner[0].x = src_corner[0].x + border_white;
            //src_corner[1].y = src_corner[1].y + border_white;
            //src_corner[1].x = src_corner[1].x - border_white;
            //src_corner[2].y = src_corner[2].y + border_white;
            //src_corner[2].x = src_corner[2].x + border_white;
            //src_corner[3].y = src_corner[3].y + border_white;
            //src_corner[3].x = src_corner[3].x + border_white;
        }
        else
        {
            isCeshi = true;//面积达到限制但进行ROI提取的时候出现问题
            src_corner[0] = Point2f(0, 0);
            src_corner[1] = Point2f(0, 10);
            src_corner[2] = Point2f(10, 10);
            src_corner[3] = Point2f(10, 0);
            src_corner_enlarge=src_corner;
        }
        vector<Point2f> dst_corner(4);
        dst_corner[0] = Point(0, 0);
        dst_corner[1] = Point(0, 1500);
        dst_corner[2] = Point(3000, 1500);
        dst_corner[3] = Point(3000, 0);
        *Mwhite = cv::getPerspectiveTransform(src_corner, dst_corner);
        *Mwhite_enlarge = cv::getPerspectiveTransform(src_corner_enlarge, dst_corner);
    }
    return true;
}

/*=========================================================
* 函 数 名: CrossPointReturn
* 功能描述: 屏幕的大致4个角位置直线提取计算交点
* 说明:
* 输入: 屏幕四个角候选框图
* 输出：交点坐标
* 时间：2020年05月28日
* 其他：
=========================================================*/
Point2f CrossPointReturn(Mat src, int x, int y, int width, int height)
{
    Mat topleft = src(Rect(x, y, width, height));
    Mat mid_Gsrc;
    Canny(topleft, mid_Gsrc, 80, 160, 3);
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat  erode1;
    dilate(mid_Gsrc, erode1, structure_element1);
    vector<Vec4i>lines;
    Vec4i points = (0, 0, 0, 0);
    //rho半径的分辨率;theta:角度分辨率;threshold:判断直线点数的阈值;minLineLength：线段长度阈值;minLineGap:线段上最近两点之间的阈值
    HoughLinesP(erode1, lines, 1, CV_PI / 180, 40, 80, 20);	//直线格式 (x_1,y_1, x_2, y_2）
    //提取失败返回(-1,-1)
    if (lines.size() < 2)
    {
        return Point2f(-1, -1);
    }
    Vec4i line_parallel = (0, 0, 0, 0);//类平行线
    Vec4i line_vertical = (0, 0, 0, 0);//类垂直线
    for (size_t i = 0; i < lines.size(); i++)
    {
        float xlength = abs(lines[i][2] - lines[i][0]), ylength = abs(lines[i][3] - lines[i][1]);
        if (xlength != 0)//不完全垂直的线条
        {
            double theta = atan(ylength / xlength);
            if (theta >= 0.8 && theta < 1.57)//类似垂直线但不完全垂60度到90度之间
            {
                line_vertical = lines[i];
                continue;
            }
            if (theta >= 0 && theta < 0.52) //类似平行直线角度在0度与30度之间
            {
                line_parallel = lines[i];
                continue;
            }
        }
        if (xlength == 0)//完全垂直的线条
        {
            line_vertical = lines[i];
            continue;
        }

    }
    Point2f cross_Point;
    cross_Point = getCrossPoint(line_parallel, line_vertical);
    return cross_Point;
}

/*=========================================================
* 函 数 名: getCrossPoint
* 功能描述: 计算两条直线的交点
* 输入:平行线条和垂直线条

=========================================================*/
Point2f getCrossPoint(Vec4i LineA, Vec4i LineB)
{
    double ka, kb;
    Point2f crossPoint;
    double chushu = (double)(LineA[2] - LineA[0]);  //类平行线的宽
    double chushu1 = (double)(LineB[2] - LineB[0]); //类垂直线是完全垂直的话
    if (chushu1 == 0)//类垂直线是完全垂直的情况
    {
        crossPoint.x = LineB[0];
        crossPoint.y = LineA[1];
        return crossPoint;
    }
    ka = (double)(LineA[3] - LineA[1]) / chushu;  //求出类平行线斜率
    kb = (double)(LineB[3] - LineB[1]) / chushu1; //求出类垂直线斜率
    crossPoint.x = int((ka*LineA[0] - LineA[1] - kb * LineB[0] + LineB[1]) / (ka - kb));
    crossPoint.y = int((ka*kb*(LineA[0] - LineB[0]) + ka * LineB[1] - kb * LineA[1]) / (ka - kb));
    return crossPoint;
}

/*=========================================================
* 函 数 名: 前后相机仿射变换映射关系的计算
* 功能描述: 核函数的计算
* 说明: 目前都是直角屏幕下的不需要圆弧角确定角点 需要修改
=========================================================*/
bool roi_front_back(Mat src_white, int border_white, Mat *Mwhite)//白屏图片侧相机找透视变换的映射关系
{
    bool isFrontBack;  //true为没拍到侧面
    bool isnormal=true;//true为正常，false为不正常
    Mat img_gray = src_white.clone();
    //cvtColor(src_white, img_gray, CV_BGR2GRAY);

    //二值化分割数值计算Segmental_Num
    Mat image_white = img_gray(Rect(2200, 550, 300, 400));//亮色区域
    Mat image_black = img_gray(Rect(3500, 550, 300, 400));//亮色区域
    double image_white_mean = mean(image_white)[0];	//val[0]表示第一个通道的均值白色边缘灰度均值
    double image_black_mean = mean(image_black)[0];	//val[0]表示第一个通道的均值黑色边缘灰度均值
    double Segmental_Ratio = 0.3;                                                 //分割系数
    double Segmental_Num = (image_white_mean - image_black_mean)*Segmental_Ratio; //二值化分割数值
    Mat th1;
    threshold(img_gray, th1, 15, 255, CV_THRESH_BINARY);

    //腐蚀缩小边界
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat  erode1;
    erode(th1, erode1, structure_element1);
    //       Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(9,9));
    //       Mat dilate1;
    //       dilate(erode1, dilate1, structure_element2);
    Mat img_temp1, img_temp2, img_temp3, img_temp4;
    vector<vector<Point>> contours1;
    vector<vector<Point>> contours;
    std::vector<cv::Point> poly;
    std::vector<cv::Point> approxCurve;
    int num_contours = 0;

    findContours(erode1, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    Rect rect1;
    Point2f P[4];
    Point2f shunxu[4];
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 500000 && area < 5000000)   //这是侧视相机找到的轮廓面积所在的范围
        {
            num_contours++;
            double epsilon = 0.08*arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], approxCurve, epsilon, true); // 多边形逼近，精度(即最小边长)设为30是为了得到4个角
        }
    }
    //判断角点提取是否存在异常现象
    if(approxCurve.size() != 4)
    {
        isnormal = false;
    }
    if (approxCurve.size() == 4)
    {
        isnormal = true;
    }
    //未提取到屏幕处理
    if (num_contours == 0||isnormal ==false)
    {
        isFrontBack = true;
        cv::Point2f src_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 10),
            cv::Point2f(10, 10),
            cv::Point2f(10, 0)
        };

        cv::Point2f dst_points[] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, screenWidth - 1),
            cv::Point2f(screenLength - 1, screenWidth - 1),
            cv::Point2f(screenLength - 1,0)
        };
        *Mwhite = cv::getPerspectiveTransform(src_points, dst_points);
    }
    //提取到 屏幕处理
    else
    {
        isFrontBack = false;
        vector<cv::Point> approxCurve1;
        approxCurve1 = approxCurve;
        for (vector<int>::size_type ii = 0; ii < 4; ii++)
        {
            if (approxCurve[ii].x < src_white.cols / 2 && approxCurve[ii].y < src_white.rows / 2)
            {
                approxCurve1[0] = approxCurve[ii];
            }
            if (approxCurve[ii].x<src_white.cols / 2 && approxCurve[ii].y>src_white.rows / 2)
            {
                approxCurve1[1] = approxCurve[ii];
            }
            if (approxCurve[ii].x > src_white.cols / 2 && approxCurve[ii].y > src_white.rows / 2)
            {
                approxCurve1[2] = approxCurve[ii];
            }
            if (approxCurve[ii].x > src_white.cols / 2 && approxCurve[ii].y < src_white.rows / 2)
            {
                approxCurve1[3] = approxCurve[ii];
            }
        }
        approxCurve1[0].y = approxCurve1[0].y + (border_white + 4);
        approxCurve1[0].x = approxCurve1[0].x + (border_white - 2);
        approxCurve1[1].y = approxCurve1[1].y - (border_white + 16);
        approxCurve1[1].x = approxCurve1[1].x + (border_white + 7);
        approxCurve1[2].y = approxCurve1[2].y + (border_white);
        approxCurve1[2].x = approxCurve1[2].x - (border_white);
        approxCurve1[3].y = approxCurve1[3].y - (border_white + 9);
        approxCurve1[3].x = approxCurve1[3].x + (border_white + 5);

        vector<Point2f> src_corner(4);
        src_corner[0] = Point2f(approxCurve1[0].x, approxCurve1[0].y);
        src_corner[1] = Point2f(approxCurve1[1].x, approxCurve1[1].y);
        src_corner[2] = Point2f(approxCurve1[2].x, approxCurve1[2].y);
        src_corner[3] = Point2f(approxCurve1[3].x, approxCurve1[3].y);

        vector<Point2f> dst_corner(4);
        dst_corner[0] = Point2f(0, 0);
        dst_corner[1] = Point2f(0, 1775);
        dst_corner[2] = Point2f(3000, 1775);
        dst_corner[3] = Point2f(3000, 0);
        *Mwhite = cv::getPerspectiveTransform(src_corner, dst_corner);
    }
    return true;
}

/*=========================================================
* 函 数 名: Gabor9
* 功能描述: 暂时不需要
=========================================================*/
Mat Gabor9(Mat img_1)
{
    Mat kernel1 = getGaborKernel(Size(5, 5), 0.5, CV_PI / 2, 3, 0.5, 0, CV_32F);//Size(5, 5),0.5, CV_PI / 2, 3.0, 0.5, 0, CV_32F

        float sum = 0.0;
        for (int i = 0; i < kernel1.rows; i++)
        {
            for (int j = 0; j < kernel1.cols; j++)
            {
                sum = sum + kernel1.ptr<float>(i)[j];
            }
        }
        Mat mmm = kernel1 / sum;//归一化
        Mat kernel2 = getGaborKernel(Size(5, 5), 0.5, 0,  3,  0.5, 0, CV_32F);//垂直卷积核
        float sum2 = 0.0;
        for (int i = 0; i < kernel2.rows; i++)
        {
            for (int j = 0; j < kernel2.cols; j++)
            {
                sum2 = sum2 + kernel2.ptr<float>(i)[j];
            }
        }
        Mat mmm2 = kernel2 / sum2;//归一化
        Mat img_4, img_5;
        filter2D(img_1, img_4, CV_8UC3, mmm);//卷积运算
        filter2D(img_4, img_5, CV_8UC3, mmm2);
        return img_5;
}
/*====================================================================
* 函 数 名: strong
* 功能描述: 自适应图像增强  mura:指显示器亮度不均匀, 造成各种痕迹的现象.
* 输入： 左右相机白底图像、系数、窗口尺寸
* 输出：增强结果
* 其他：
======================================================================*/
Mat strong(Mat input, double k,Size window1,Size window2)
{
    Mat result;
    Mat result1;
    Mat result2;
    vector<Mat> assemble1;
    vector<Mat> assemble2;
    vector<vector<double>> all;
    vector<double> number1;
    vector<double> number2;
    Size a = window1;
    Size b = window2;
    double I_min1;
    double I_max1;
    Mat temp_image1;
    Mat agR1;
    Mat gvR1;
    double cur1;
    uchar I_1;
    Mat temp1;

    double I_min2;
    double I_max2;
    Mat temp_image2;
    Mat agR2;
    Mat gvR2;
    double cur2;
    uchar  I_2;
    Mat temp2;
    int count = 0;//记录增强的像素点个数

    result1 = input.clone();
    result2 = input.clone();

    int count1 = slidingWnd(result1,assemble1, a ,1,1);
    int count2 = slidingWnd(result2, assemble2, b, 1, 1);

    for (vector<Mat>::size_type i = 0; i < assemble1.size(); i++)
    {
        Mat temp = assemble1[i].clone();
        temp_image1 = assemble1[i];
        temp1 = temp_image1.clone();
        meanStdDev(temp1, agR1, gvR1);
        double average = agR1.at<double>(0, 0);//灰度均值
        double variance = gvR1.at<double>(0, 0);//灰度方差
        I_min1 = average - 2 * variance;
        I_max1 = average + 2 * variance;
        for (int x = 0; x < temp_image1.rows; x++)
        {
            uchar *data1 = temp_image1.ptr<uchar>(x);//获取第i行首地址
            for (int y = 0; y < temp_image1.cols; y++)
            {
                I_1 = data1[y];
                double b1 = k * (((I_1 - I_min1)*(I_max1 - I_1)) / (I_max1 - I_min1)*(I_max1 - I_min1));
                cur1 = exp(b1);//公式

                if (cur1 < 0.85)
                {
                    count++;
                    if (I_1 < average)
                    {
                        if ((I_1+ 8 * exp(1 - cur1)) < 230)
                        {
                            temp_image1.at<uchar>(x, y) -= 6*exp(1-cur1);
                        }
                    }
                    else if (I_1 > average)
                    {

                        if ((I_1+ 8 * exp(1 - cur1)) > 40)
                        {
                            temp_image1.at<uchar>(x, y) += 6*exp(1-cur1);
                        }
                    }
                }
            }
            if (count >= 150)
            {
                for (int x = 0; x < temp.rows; x++)
                {
                    uchar *data = temp.ptr<uchar>(x);//获取第i行首地址
                    for (int y = 0; y < temp.cols; y++)
                    {
                        uchar cur = data[y];
                        temp_image1.at<uchar>(x, y) = data[y];
                    }
                }
                count = 0;
                break;
            }
        }

    }

    return result1;
}
int slidingWnd(Mat& src, vector<Mat>& wnd, Size& wndSize, double x_percent, double y_percent)
{
    int count = 0;  //记录滑动窗口的数目
    int x_step = cvCeil(x_percent*wndSize.width);
    int y_step = cvCeil(y_percent*wndSize.height);
    int row;
    int col;
    Mat ROI;
    //利用窗口对图像进行遍历
    for (int i = 0; i < src.rows ; i += y_step)
    {
        for (int j = 0; j < src.cols ; j += x_step)
        {
            row = i;
            col = j;
            if (i + y_step > src.rows - 1)
            {
                row = src.rows - y_step ;
                i = src.rows;
            }
            if (j + x_step > src.cols )
            {
                col = src.cols - x_step - 1;
                j = src.cols;
            }
            //Rect roi(Point(j, i), wndSize);
            ROI = src(Rect(col,row, wndSize.width, wndSize.height));
            wnd.push_back(ROI);
            count++;
        }
    }
    return count;
}
Mat polyfit1(vector<Point>& in_point, int n)
{
    int size = in_point.size();
    //所求未知数个数
    int x_num = n + 1;
    //构造矩阵U和Y
    Mat mat_u(size, x_num, CV_64F);
    Mat mat_y(size, 1, CV_64F);

    for (int i = 0; i < mat_u.rows; ++i)
        for (int j = 0; j < mat_u.cols; ++j)
        {
            mat_u.at<double>(i, j) = pow(in_point[i].x, j);
        }

    for (int i = 0; i < mat_y.rows; ++i)
    {
        mat_y.at<double>(i, 0) = in_point[i].y;
    }

    //矩阵运算，获得系数矩阵K
    Mat mat_k(x_num, 1, CV_64F);
    mat_k = (mat_u.t()*mat_u).inv()*mat_u.t()*mat_y;
    cout << mat_k << endl;
    return mat_k;
}
/*====================================================================
* 函 数 名: CrossLine
* 功能描述: 十字检测
* 输入：主相机白底图像
* 输出：主相机白底下检测结果图和result
* 其他：
======================================================================*/
bool CrossLine(Mat white, Mat *mresult, QString *causecolor)
{
    bool result = false;
    Mat img_gray = white.clone();
    Mat strong_result;
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(3, 3));
    clahe->apply(img_gray, strong_result);
    img_gray = strong_result.clone();

    double temp_av = 0.0;
    vector<Point> in;
    vector<Point2d> out1;
    Point P;
    in.resize(strong_result.rows);
    out1.resize(strong_result.rows);
    for (int i = 0; i < strong_result.rows; i++)
    {
        uchar *data = strong_result.ptr<uchar>(i);//获取第i行首地址
        Mat rowRange = strong_result.rowRange(i, i + 1);
        temp_av = mean(rowRange)[0];
        P.x = i;
        P.y = temp_av;
        in[i] = P;
    }
    int n = 8;
    Mat mat_k = polyfit1(in, n);
    //计算结果可视化
    Mat out(500, 1500, CV_8UC3, Scalar::all(255));

    //画出拟合曲线
    for (int i = in[0].x; i < in[in.size() - 1].x; ++i)
    {
        Point2d ipt;
        ipt.x = i;
        ipt.y = 0;
        for (int j = 0; j < n + 1; ++j)
        {
            ipt.y += mat_k.at<double>(j, 0)*pow(i, j);
        }
        out1[i] = ipt;
        circle(out, ipt, 1, Scalar(0, 255, 0), 1, 8, 0);
        //circle(out, ipt, 1, Scalar(255, 255, 255), CV_FILLED, CV_AA);
    }
    Mat th_result = Mat::zeros(strong_result.rows, strong_result.cols, CV_8UC1);

    //画出原始散点
    for (int i = 0; i < in.size(); ++i)
    {
        Point2d ipt = in[i];
        circle(out, ipt, 1, Scalar(0, 0, 255), 1, 8, 0);
        if (in[i].y - out1[i].y > 6 && in[i].y - out1[i].y < 20)
        {
            th_result(Rect(0, i, th_result.cols, 1)) = uchar(255);
        }
        //circle(out, ipt, 1, Scalar(0, 0, 255), CV_FILLED, CV_AA);
    }
    vector<vector<Point>> contours;
    findContours(th_result, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        Mat mask_for_temp_mask;
        threshold(img_gray, mask_for_temp_mask, 30, 255, CV_THRESH_BINARY);
        Mat temp_mask = Mat::zeros(th_result.rows, th_result.cols, CV_8UC1);
        drawContours(temp_mask, contours, i, 255, FILLED, 8);
        bitwise_and(temp_mask, mask_for_temp_mask, temp_mask);
        double area = contourArea(contours[i]);
        if (area > 20 && area < 100000)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            float w = boundRect[i].width;
            float h = boundRect[i].height;
            int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
            Moments m = moments(contours[i]);
            int x_point = int(m.m10 / m.m00);
            int y_point = int(m.m01 / m.m00);
            int long_lackLine = boundRect[i].width;//长宽暂时不用
            int short_lackLine = boundRect[i].height;//长宽暂时不用
            int border = short_lackLine * 2;
            int x_lt = X_1 - border;
            if (x_lt < 0)
            {
                x_lt = 0;
            }
            int y_lt = Y_1 - border;
            if (y_lt < 0)
            {
                y_lt = 0;
            }
            int x_rt = X_2 + border;
            if (x_rt > img_gray.size[1] - 1)
            {
                x_rt = img_gray.size[1] - 1;
            }
            int y_rt = Y_2 + border;
            if (y_rt > img_gray.size[0] - 1)
            {
                y_rt = img_gray.size[0] - 1;
            }

            //边界位置处不进行检测，防止检测到外边界处
            int corner = 10;
            if ((x_point < corner and y_point < corner) or (th_result.cols - x_point < corner and th_result.rows - y_point < corner) or (x_point < corner and th_result.rows - y_point < corner) or (th_result.cols - x_point < corner and y_point < corner))
            {
                continue;
            }
            Mat imagedoubt = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
            Mat mask = temp_mask(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
            Mat temp_mask1;
            Mat temp_mask2;
            double mean_in_gray = mean(imagedoubt, mask)[0];
            threshold(imagedoubt, temp_mask1, 30, 255, CV_THRESH_BINARY);
            bitwise_and(temp_mask1, ~mask, temp_mask2);
            double mean_out_gray = mean(imagedoubt, temp_mask2)[0];
            double intensity = mean_in_gray - mean_out_gray;
            double ROI_ag = mean(img_gray)[0];
            if (mean_in_gray < ROI_ag)
            {
                continue;
            }
            string a = "long:" + to_string(long_lackLine) + "short:" + to_string(short_lackLine) + "th:" + to_string(intensity);
            //putText(white, a, Point(X_1 - 400, Y_1), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 5, 8, 0);

            if (intensity > 7)
            {
                result = true;
                CvPoint top_lef4 = cvPoint(x_lt, y_lt);
                CvPoint bottom_right4 = cvPoint(x_rt, y_rt);
                rectangle(white, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
            }
    }
    }
    if (result == true)
    {
        *mresult = white;
        *causecolor = "十字";
        result = true;
    }
    return result;
}

//比较函数对象
bool compareContourAreas(std::vector< cv::Point> contour1,std::vector< cv::Point> contour2)
{
    return (cv::contourArea(contour1) > cv::contourArea(contour2));
}

/*====================================================================
* 函 数 名: BoomDead_light
* 功能描述: 爆灯死灯检测  屏幕左侧发光LED管损坏，呈现局部发亮
* 输入：主相机白底图像
* 输出：主相机白底下检测结果图和result
* 其他：
======================================================================*/
bool BoomDead_light(Mat white, Mat ceguang, Mat *mresult, QString *causecolor)
{
    bool result = false;
    bool result_flag = false;
    int heith = 5;
    Mat img_gray = white.clone();
    Mat strong_result,binaryImage, BinaryResult;
    Ptr<CLAHE> clahe = createCLAHE(5.0, Size(3, 3));
    clahe->apply(img_gray, strong_result);
    Mat edge_left = strong_result(Rect(0, 0, 100, strong_result.rows)).clone();
    medianBlur(edge_left, edge_left, 3);									//中值滤波去除锯齿

    Mat edge_img_X;
    Mat edge_img_Y;
    Mat edge_img_result;
    Mat sobel_result;
    Mat structure_element = getStructuringElement(MORPH_RECT, Size(5, 5));
    Sobel(edge_left, edge_img_X, CV_16S, 0, 1, 3, 1, 2, BORDER_DEFAULT);
    convertScaleAbs(edge_img_X, edge_img_X);
    Sobel(edge_left, edge_img_Y, CV_16S, 1, 0, 3, 1, 2, BORDER_DEFAULT);
    convertScaleAbs(edge_img_Y, edge_img_Y);
    addWeighted(edge_img_X, 0.5, edge_img_Y, 0.5, 0, edge_img_result);
    clahe->apply(edge_img_result, edge_img_result);
    threshold(edge_img_result, sobel_result, 30, 255, CV_THRESH_BINARY);
    Mat F_result = ~sobel_result;
    Mat edge_thresold;
    erode(F_result, edge_thresold, structure_element);

    Mat Image_left = img_gray(Rect(0, 0, 100, strong_result.rows)).clone();

    threshold(img_gray, binaryImage, 40, 255, CV_THRESH_BINARY);				//二值化
    Mat edge_right = img_gray(Rect(strong_result.cols - 100, 0, 100, strong_result.rows)).clone();
    Mat Binary_right = binaryImage(Rect(strong_result.cols - 100, 0, 100, strong_result.rows)).clone();

    double mean_ceguang = mean(ceguang)[0];
    if (mean_ceguang > 200) {
        result = true;
    }

    if (result == true) {
        if (boom_light_Checked && LampEye_Checked)
        {
            vector<vector<Point>> contours;
            findContours(edge_thresold, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
            vector<Rect> boundRect(contours.size());
            std::sort(contours.begin(), contours.end(), compareContourAreas);
            for (vector<int>::size_type i = 0; i < contours.size(); i++)
            {
                boundRect[i] = boundingRect(Mat(contours[i]));
                int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
                int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
                int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
                int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
                if ((X_1 == 0 && Y_1 == 0) || (X_1 == 0 && Y_2 == strong_result.rows))
                    continue;
                Mat Image_BoomDead = Image_left(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                Mat Binary_BoomDead = edge_thresold(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                double mean_In = mean(Image_BoomDead, Binary_BoomDead)[0];
                double mean_Out = mean(edge_right, Binary_right)[0];

                X_1 = X_1 - heith;
                if (X_1 <= 0)
                    X_1 = 0;
                Y_1 = Y_1 - heith;
                if (Y_1 <= 0)
                    Y_1 = 0;
                X_2 = X_2 + heith;
                if (X_2 > img_gray.cols - 1)
                {
                    X_2 = img_gray.cols - 1;
                }
                Y_2 = Y_2 + heith;
                if (Y_2 > img_gray.rows - 1)
                {
                    Y_2 = img_gray.rows - 1;
                }
                if (mean_Out - mean_In > 20)
                {
                    *causecolor = "死灯";
                    result_flag = true;
                    result = true;

                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                    rectangle(white, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
                    break;
                }
                else if (mean_In - mean_Out > 20)
                {
                    *causecolor = "爆灯";
                    result_flag = true;
                    result = true;
                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                    rectangle(white, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
                    break;
                }
            }
            if (!result_flag)
            {
                int random = rand() % 10;
                if (random >= 5)
                {
                    *causecolor = "灯眼";
                    result = true;
                    CvPoint top_left = cvPoint(10, 20);
                    CvPoint bottom_right = cvPoint(100, img_gray.rows - 20);
                    rectangle(white, top_left, bottom_right, Scalar(255, 255, 255), 5, 8, 0);
                }
                else
                {
                    *causecolor = "爆灯";
                    result = true;
                    CvPoint top_left = cvPoint(10, 20);
                    CvPoint bottom_right = cvPoint(100, img_gray.rows - 20);
                    rectangle(white, top_left, bottom_right, Scalar(255, 255, 255), 5, 8, 0);
                }
            }
        }
        else if (boom_light_Checked && !LampEye_Checked)
        {
            vector<vector<Point>> contours;
            findContours(edge_thresold, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
            vector<Rect> boundRect(contours.size());
            std::sort(contours.begin(), contours.end(), compareContourAreas);
            for (vector<int>::size_type i = 0; i < contours.size(); i++)
            {
                boundRect[i] = boundingRect(Mat(contours[i]));
                int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
                int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
                int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
                int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
                if ((X_1 == 0 && Y_1 == 0) || (X_1 == 0 && Y_2 == strong_result.rows))
                    continue;
                Mat Image_BoomDead = Image_left(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                Mat Binary_BoomDead = edge_thresold(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                double mean_In = mean(Image_BoomDead, Binary_BoomDead)[0];
                double mean_Out = mean(edge_right, Binary_right)[0];

                X_1 = X_1 - heith;
                if (X_1 <= 0)
                    X_1 = 0;
                Y_1 = Y_1 - heith;
                if (Y_1 <= 0)
                    Y_1 = 0;
                X_2 = X_2 + heith;
                if (X_2 > img_gray.cols - 1)
                {
                    X_2 = img_gray.cols - 1;
                }
                Y_2 = Y_2 + heith;
                if (Y_2 > img_gray.rows - 1)
                {
                    Y_2 = img_gray.rows - 1;
                }
                if (mean_In - mean_Out > 20)
                {
                    *causecolor = "爆灯";
                    result_flag = true;
                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                    rectangle(white, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
                    break;
                }
                else if (mean_Out - mean_In > 20)
                {
                    result = false;
                    result_flag = true;
                    break;
                }
            }
            if (!result_flag)
            {
                *causecolor = "爆灯";
                CvPoint top_left = cvPoint(10, 20);
                CvPoint bottom_right = cvPoint(100, img_gray.rows - 20);
                rectangle(white, top_left, bottom_right, Scalar(255, 255, 255), 5, 8, 0);
            }
        }
        else if (!boom_light_Checked && LampEye_Checked)
        {
            vector<vector<Point>> contours;
            findContours(edge_thresold, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
            vector<Rect> boundRect(contours.size());
            std::sort(contours.begin(), contours.end(), compareContourAreas);
            for (vector<int>::size_type i = 0; i < contours.size(); i++)
            {
                boundRect[i] = boundingRect(Mat(contours[i]));
                int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
                int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
                int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
                int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
                if ((X_1 == 0 && Y_1 == 0) || (X_1 == 0 && Y_2 == strong_result.rows))
                    continue;
                Mat Image_BoomDead = Image_left(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                Mat Binary_BoomDead = edge_thresold(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                double mean_In = mean(Image_BoomDead, Binary_BoomDead)[0];
                double mean_Out = mean(edge_right, Binary_right)[0];
                X_1 = X_1 - heith;
                if (X_1 <= 0)
                    X_1 = 0;
                Y_1 = Y_1 - heith;
                if (Y_1 <= 0)
                    Y_1 = 0;
                X_2 = X_2 + heith;
                if (X_2 > img_gray.cols - 1)
                {
                    X_2 = img_gray.cols - 1;
                }
                Y_2 = Y_2 + heith;
                if (Y_2 > img_gray.rows - 1)
                {
                    Y_2 = img_gray.rows - 1;
                }

                if (mean_Out - mean_In > 20)
                {
                    *causecolor = "灯眼";
                    result_flag = true;
                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                    rectangle(white, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
                    break;
                }
                else if (mean_Out - mean_In > 20)
                {
                    result = false;
                    result_flag = true;
                    break;
                }
            }
            if (!result_flag)
            {
                *causecolor = "灯眼";
                CvPoint top_left = cvPoint(10, 20);
                CvPoint bottom_right = cvPoint(100, img_gray.rows - 20);
                rectangle(white, top_left, bottom_right, Scalar(255, 255, 255), 5, 8, 0);
            }
        }
        else
        {
            result = false;
        }
    }
    if (result == true)
    {
        *mresult = white;
    }
    return result;
}

/*====================================================================
* 函 数 名: boom_light
* 功能描述: 死灯检测  屏幕左侧发光LED管损坏，呈现局部发亮
* 输入：主相机白底图像
* 输出：主相机白底下检测结果图和result
* 其他：
======================================================================*/
bool Dead_light(Mat white, Mat ceguang, Mat *mresult, QString *causecolor)
{
    int boder = 5;

    bool result = false;
    Mat img_gray = white.clone();

    Mat strong_result;
    Ptr<CLAHE> clahe = createCLAHE(5.0, Size(3, 3));
    clahe->apply(img_gray, strong_result);

    //Mat edge_img = strong_result(Rect(strong_result.cols - 100, 0, 100, strong_result.rows)).clone();
    Mat edge_img = strong_result(Rect(0, 0, 100, strong_result.rows)).clone();
    medianBlur(edge_img, edge_img, 3);
    //Mat ad_result;
    //adaptiveThreshold(edge_img, ad_result, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 15, 3);

    Mat edge_img_X;
    Mat edge_img_Y;
    Mat edge_img_result;
    Mat sobel_result;
    Mat structure_element = getStructuringElement(MORPH_RECT, Size(5, 5));
    Sobel(edge_img, edge_img_X, CV_16S, 0, 1, 3, 1, 2, BORDER_DEFAULT);
    convertScaleAbs(edge_img_X, edge_img_X);
    Sobel(edge_img, edge_img_Y, CV_16S, 1, 0, 3, 1, 2, BORDER_DEFAULT);
    convertScaleAbs(edge_img_Y, edge_img_Y);
    addWeighted(edge_img_X, 0.5, edge_img_Y, 0.5, 0, edge_img_result);
    clahe->apply(edge_img_result, edge_img_result);
    threshold(edge_img_result, sobel_result, 30, 255, CV_THRESH_BINARY);
    Mat F_result = ~sobel_result;
    Mat edge_thresold;
    erode(F_result, edge_thresold, structure_element);
    Mat th_result = Mat::zeros(img_gray.size(), img_gray.type());
    edge_thresold.copyTo(th_result(Rect(0, 0, 100, th_result.rows)));

    Mat maskR1, maskR1_Binary, maskR2, maskR2_Binary, binaryationR1, binaryationR2;
    maskR1 = img_gray(Rect(0, 0, 100, 100));
    threshold(maskR1, maskR1_Binary, 30, 255, CV_THRESH_BINARY);
    double meanR1 = mean(maskR1, maskR1_Binary)[0];
    if (meanR1 >= 100)
    {
        threshold(maskR1, maskR1_Binary, meanR1-30, 255, CV_THRESH_BINARY);
    }
    else if (meanR1 <= 20)
    {
        maskR1_Binary = Mat::zeros(maskR1.size(), maskR1.type());
        maskR1_Binary = ~maskR1_Binary;
    }
    maskR2 = img_gray(Rect(0, img_gray.rows-100, 100, 100));
    threshold(maskR2, maskR2_Binary, 30, 255, CV_THRESH_BINARY);
    double meanR2 = mean(maskR2, maskR2_Binary)[0];
    if (meanR2 >= 100)
    {
        threshold(maskR2, maskR2_Binary, meanR2-30, 255, CV_THRESH_BINARY);
    }
    else if (meanR2 <= 20)
    {
        maskR2_Binary = Mat::zeros(maskR1.size(), maskR1.type());
        maskR2_Binary = ~maskR2_Binary;
    }
    binaryationR1 = th_result(Rect(0, 0, 100, 100));
    binaryationR2 = th_result(Rect(0, img_gray.rows-100, 100, 100));
    bitwise_and(maskR1_Binary, binaryationR1, binaryationR1);
    bitwise_and(maskR2_Binary, binaryationR2, binaryationR2);
    binaryationR1.copyTo(th_result(Rect(0, 0, 100, 100)));
    binaryationR2.copyTo(th_result(Rect(0, img_gray.rows - 100, 100, 100)));

    vector<vector<Point>> contours;
    findContours(th_result, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    std::sort(contours.begin(), contours.end(), compareContourAreas);

    vector<Rect> boundRect(contours.size());
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        Mat temp_mask = Mat::zeros(th_result.rows, th_result.cols, CV_8UC1);
        drawContours(temp_mask, contours, i, 255, FILLED, 8);
        double area = contourArea(contours[i]);
        if (area > 200 && area < 150000)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            float w = boundRect[i].width;
            float h = boundRect[i].height;
            RotatedRect rect = minAreaRect(contours[i]);  //包覆轮廓的最小斜矩形 划伤缺陷有旋转特点
            Point p = rect.center;
            double w1 = rect.size.height;
            double h1 = rect.size.width;
            int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
            int x_1 = X_1;//矩形左上角X坐标值
            int y_1 = Y_1;//矩形左上角Y坐标值
            int x_2 = X_2;//矩形右下角X坐标值
            int y_2 = Y_2;//矩形右下角Y坐标值
            int x_point = X_1 + round(w / 2);
            int y_point = Y_1 + round(h / 2);
            if (max(w1 / h1, h1 / w1) < 14)//4
            {
                //                //排除R角干扰
                //                if (area >= 1800 && area <= 2800 && ((X_1 == 0 && Y_1 == 0) || (X_1 == 0 && Y_2 >= temp_mask.rows - 1)))
                //                    continue;
                X_1 = X_1 - boder - int(w);
                Y_1 = Y_1 - boder - int(h);
                X_2 = X_2 + boder + int(w);
                Y_2 = Y_2 + boder + int(h);
                if (X_1 < 0)
                {
                    X_1 = 0;
                }
                if (Y_1 < 0)
                {
                    Y_1 = 0;
                }
                if (X_2 > th_result.cols - 1)
                {
                    X_2 = th_result.cols - 1;
                }
                if (Y_2 > th_result.rows - 1)
                {
                    Y_2 = th_result.rows - 1;
                }
            }

            Mat imagedoubt = img_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
            Mat mask = th_result(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
            Mat temp_mask1;
            Mat temp_mask2;
            double mean_in_gray = mean(imagedoubt, mask)[0];
            if (mean_in_gray > 180)
            {
                continue;
            }
            threshold(imagedoubt, temp_mask1, 40, 255, CV_THRESH_BINARY);
            bitwise_and(temp_mask1, ~mask, temp_mask2);
            Mat TempImage_Binary;
            double mean_out_gray = mean(imagedoubt, temp_mask2)[0];
            double intensity = mean_out_gray - mean_in_gray;
            if (intensity <= 25 && mean_in_gray <= 90 && mean_out_gray <= 90)
            {
                Mat tempImage = img_gray(Rect(img_gray.cols - 500, 0, 300, img_gray.rows)).clone();
                threshold(tempImage, TempImage_Binary, 30, 255, CV_THRESH_BINARY);
                mean_out_gray = mean(tempImage, TempImage_Binary)[0];
                intensity = mean_out_gray - mean_in_gray;
            }
            if ((mean_out_gray <= 120 && mean_in_gray <= 120 && intensity >= 15) || (intensity > 25))
            {
                result = true;
                CvPoint top_lef4 = cvPoint(x_1, y_1);
                CvPoint bottom_right4 = cvPoint(x_2, y_2);
                rectangle(white, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
                break;
            }
        }
    }
    if (result == true)
    {
        *mresult = white;
        *causecolor = "死灯";
        result = true;
    }
    return result;
}

/*====================================================================
* 函 数 名: boom_light
* 功能描述: 爆灯检测  屏幕左侧发光LED管损坏，呈现局部发亮
* 输入：主相机白底图像
* 输出：主相机白底下检测结果图和result
* 其他：
======================================================================*/
bool boom_light(Mat white, Mat ceguang, Mat *mresult, QString *causecolor)
{
    int boder = 5;
    int decter_length = 200;
    bool result = false;
    Mat img_gray = white.clone();

    Mat strong_result;
    Ptr<CLAHE> clahe = createCLAHE(5.0, Size(3, 3));
    clahe->apply(img_gray, strong_result);

    //Mat edge_img = strong_result(Rect(strong_result.cols - 100, 0, 100, strong_result.rows)).clone();
    Mat edge_img = strong_result(Rect(0, 0, decter_length, strong_result.rows)).clone();
    medianBlur(edge_img, edge_img, 3);
    //Mat ad_result;
    //adaptiveThreshold(edge_img, ad_result, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 15, 3);

    Mat edge_img_X;
    Mat edge_img_Y;
    Mat edge_img_result;
    Mat sobel_result;
    Mat structure_element = getStructuringElement(MORPH_RECT, Size(5, 5));
    Sobel(edge_img, edge_img_X, CV_16S, 0, 1, 3, 1, 2, BORDER_DEFAULT);
    convertScaleAbs(edge_img_X, edge_img_X);
    Sobel(edge_img, edge_img_Y, CV_16S, 1, 0, 3, 1, 2, BORDER_DEFAULT);
    convertScaleAbs(edge_img_Y, edge_img_Y);
    addWeighted(edge_img_X, 0.5, edge_img_Y, 0.5, 0, edge_img_result);
    clahe->apply(edge_img_result, edge_img_result);
    threshold(edge_img_result, sobel_result, 30, 255, CV_THRESH_BINARY);
    Mat F_result = ~sobel_result;
    Mat edge_thresold;
    erode(F_result, edge_thresold, structure_element);
    Mat th_result = Mat::zeros(img_gray.size(), img_gray.type());
    edge_thresold.copyTo(th_result(Rect(0, 0, decter_length, th_result.rows)));

    vector<vector<Point>> contours;
    findContours(th_result, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    std::sort(contours.begin(), contours.end(), compareContourAreas);

    vector<Rect> boundRect(contours.size());
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        Mat temp_mask = Mat::zeros(th_result.rows, th_result.cols, CV_8UC1);
        drawContours(temp_mask, contours, i, 255, FILLED, 8);
        double area = contourArea(contours[i]);
        if (area > 100 && area < 150000)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            float w = boundRect[i].width;
            float h = boundRect[i].height;
            RotatedRect rect = minAreaRect(contours[i]);  //包覆轮廓的最小斜矩形 划伤缺陷有旋转特点
            Point p = rect.center;
            double w1 = rect.size.height;
            double h1 = rect.size.width;
            int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
            int x_1 = X_1;//矩形左上角X坐标值
            int y_1 = Y_1;//矩形左上角Y坐标值
            int x_2 = X_2;//矩形右下角X坐标值
            int y_2 = Y_2;//矩形右下角Y坐标值
            int x_point = X_1 + round(w / 2);
            int y_point = Y_1 + round(h / 2);
            if (max(w1 / h1, h1 / w1) < 14)//4
            {
                X_1 = X_1 - boder - int(w);
                Y_1 = Y_1 - boder - int(h);
                X_2 = X_2 + boder + int(w);
                Y_2 = Y_2 + boder + int(h);
                if (X_1 < 0)
                {
                    X_1 = 0;
                }
                if (Y_1 < 0)
                {
                    Y_1 = 0;
                }
                if (X_2 > decter_length - 1)
                {
                    X_2 = decter_length - 1;
                }
                if (Y_2 > th_result.rows - 1)
                {
                    Y_2 = th_result.rows - 1;
                }
            }

            Mat imagedoubt = img_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
            Mat mask = th_result(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
            Mat temp_mask1;
            Mat temp_mask2;
            threshold(imagedoubt, temp_mask1, 30, 255, CV_THRESH_BINARY);
            double mean_all = mean(imagedoubt, temp_mask1)[0];
            threshold(imagedoubt, temp_mask1, mean_all-20, 255, CV_THRESH_BINARY);
            bitwise_and(temp_mask1, mask, temp_mask1);
            double mean_in_gray = mean(imagedoubt, temp_mask1)[0];
            if (mean_in_gray < 50)
            {
                continue;
            }
            threshold(imagedoubt, temp_mask1, 30, 255, CV_THRESH_BINARY);
            bitwise_and(temp_mask1, ~mask, temp_mask2);
            double mean_out_gray = mean(imagedoubt, temp_mask2)[0];
            double intensity = mean_in_gray - mean_out_gray;
            Mat TempImage_Binary;
            if (mean_in_gray >= 180 && mean_out_gray >= 180 && intensity <= 23)
            {
                Mat tempImage = img_gray(Rect(img_gray.cols - 500, 0, 300, img_gray.rows)).clone();
                threshold(tempImage, TempImage_Binary, 30, 255, CV_THRESH_BINARY);
                mean_out_gray = mean(tempImage, TempImage_Binary)[0];
                intensity = mean_out_gray - mean_in_gray;
            }
            if (intensity > 23)
            {
                result = true;
                CvPoint top_lef4 = cvPoint(x_1, y_1);
                CvPoint bottom_right4 = cvPoint(x_2, y_2);
                rectangle(white, top_lef4, bottom_right4, Scalar(0), 5, 8, 0);
                break;
            }
        }
    }
    if (result == true)
    {
        *mresult = white;
        *causecolor = "爆灯";
        result = true;
    }
    return result;
}

void dividedLinearStrength(cv::Mat& matInput, cv::Mat& matOutput, float fStart, float fEnd,
    float fSout, float fEout)
{
    //计算直线参数
    //L1
    float fK1 = fSout / fStart;
    //L2
    float fK2 = (fEout - fSout) / (fEnd - fStart);
    float fC2 = fSout - fK2 * fStart;
    //L3
    float fK3 = (255.0f - fEout) / (255.0f - fEnd);
    float fC3 = 255.0f - fK3 * 255.0f;

    //建立查询表
    std::vector<unsigned char> loolUpTable(256);
    for (size_t m = 0; m < 256; m++)
    {
        if (m < fStart)
        {
            loolUpTable[m] = static_cast<unsigned char>(m * fK1);
        }
        else if (m > fEnd)
        {
            loolUpTable[m] = static_cast<unsigned char>(m * fK3 + fC3);
        }
        else
        {
            loolUpTable[m] = static_cast<unsigned char>(m * fK2 + fC2);
        }
    }
    //构造输出图像
    matOutput = cv::Mat::zeros(matInput.rows, matInput.cols, matInput.type());
    //灰度映射
    for (size_t r = 0; r < matInput.rows; r++)
    {
        unsigned char* pInput = matInput.data + r * matInput.step[0];
        unsigned char* pOutput = matOutput.data + r * matOutput.step[0];
        for (size_t c = 0; c < matInput.cols; c++)
        {
            //查表gamma变换
            pOutput[c] = loolUpTable[pInput[c]];
        }
    }
}

void adaptiveThresholdCustom(const cv::Mat &src, cv::Mat &dst, double maxValue, int method, int type, int blockSize, double delta, double ratio, double fillValCoeffi)
{
    CV_Assert(src.type() == CV_8UC1);               // 原图必须是单通道无符号8位,CV_Assert（）若括号中的表达式值为false，则返回一个错误信息
    CV_Assert(blockSize % 2 == 1 && blockSize > 1);	// 块大小必须大于1，并且是奇数
    CV_Assert(maxValue > 0);                        //二值图像最大值
    CV_Assert(ratio > DBL_EPSILON);	                //输入均值比例系数
    Size size = src.size();							//源图像的尺寸
    Mat _dst(size, src.type());						//目标图像的尺寸
    Mat mean;	                                    //存放均值图像
    if (src.data != _dst.data)
        mean = _dst;


    int top = (blockSize - 1)*0.5;     //填充的上边界行数
    int bottom = (blockSize - 1)*0.5;  //填充的下边界行数
    int left = (blockSize - 1)*0.5;	   //填充的左边界行数
    int right = (blockSize - 1)*0.5;   //填充的右边界行数
    int border_type = BORDER_CONSTANT; //边界填充方式
    Mat src_Expand;	                   //对原图像进行边界扩充

    Mat topImage = src(Rect(0, 0, src.cols, 3));//上边界一行图像

    cv::Scalar color = cv::mean(topImage)* fillValCoeffi;//35-80之间均可以  该值需要确定

                                                         //Scalar color = Scalar(50);//35-80之间均可以
    copyMakeBorder(src, src_Expand, top, bottom, left, right, border_type, color);

    if (method == ADAPTIVE_THRESH_MEAN_C)
    {
        /*
        @param src 单通道灰度图
        @param dst 单通道处理后的图
        @param int类型的ddepth，输出图像的深度
        @param Size类型的ksize，内核的大小
        @param Point类型的anchor，表示锚点
        @param bool类型的normaliz,即是否归一化
        @param borderType 图像外部像素的某种边界模式
        */
        boxFilter(src_Expand, mean, src.type(), Size(blockSize, blockSize), Point(-1, -1), true, BORDER_CONSTANT);
    }
    else if (method == ADAPTIVE_THRESH_GAUSSIAN_C)
    {
        GaussianBlur(src, mean, Size(blockSize, blockSize), 0, 0, BORDER_DEFAULT);
    }
    else
        CV_Error(CV_StsBadFlag, "Unknown/unsupported adaptive threshold method");

    mean = mean(cv::Rect(top, top, src_Expand.cols - top * 2, src_Expand.rows - top * 2)); //删除扩充的图像边界

    int i, j;
    uchar imaxval = saturate_cast<uchar>(maxValue);	                       //将maxValue由double类型转换为uchar型
    int idelta = type == THRESH_BINARY ? cvCeil(delta) : cvFloor(delta);   //将idelta由double类型转换为int型
    if (src.isContinuous() && mean.isContinuous() && _dst.isContinuous())
    {
        size.width *= size.height;
        size.height = 1;
    }
    for (i = 0; i < size.height; i++)
    {
        const uchar* sdata = src.data + src.step * i;		   //指向源图像
        const uchar* mdata = mean.data + mean.step * i;		   //指向均值图
        uchar* ddata = _dst.data + _dst.step * i;	           //指向输出图
        for (j = 0; j < size.width; j++)
        {
            double Thresh = mdata[j] * ratio - idelta;	        //阈值
            if (CV_THRESH_BINARY == type)	                    //S>T时为imaxval
            {
                ddata[j] = sdata[j] > Thresh ? imaxval : 0;
            }
            else if (CV_THRESH_BINARY_INV == type)	            //S<T时为imaxval
            {
                ddata[j] = sdata[j] > Thresh ? 0 : imaxval;
            }
            else
                CV_Error(CV_StsBadFlag, "Unknown/unsupported threshold type");
        }
    }
    dst = _dst.clone();
}

bool mura_1(Mat grayImage, Mat imageCeguang, Mat fit_result, Mat* mresult1, Mat* mura, QString* causecolor, QString imageType)
{
    bool result = false;
    double minValue;
    double maxValue;
    int sum = 0;
    int count = 0;
    int mean_fore_1;
    int mean_fore_2;
    int mean_fore_3;
    Mat result_image = grayImage.clone();//框出缺陷结果图，防止对后续检测产生干扰
    Mat img_gray = grayImage.clone();
    Mat ceguang_gray = imageCeguang.clone();
    Mat binaryzation = fit_result.clone();
    Mat structureElement = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
    dilate(binaryzation, binaryzation, structureElement, Point(-1, -1), 1);
    erode(binaryzation, binaryzation, structureElement);
    vector<vector<Point>> contours;
    vector<vector<Point>> contours_1;
    bool areaflag = true;
    findContours(binaryzation, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for (int i = 0; i < contours.size(); i++)
    {
        areaflag = true;
        boundRect[i] = boundingRect(Mat(contours[i]));//外接矩形
        int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
        int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
        int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
        int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
        Mat Crop_Image_last = binaryzation(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
        Mat CropGrayImage = img_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
        Mat Ceguang_Crop_Image = ceguang_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
        double area = countNonZero(Crop_Image_last);//白色区域的像素个数作为面积
        Mat mean_, std_;
        if (area > 100 && area < 30000)
        {
            Mat mean_In, stddev_In;
            meanStdDev(CropGrayImage, mean_In, stddev_In);
            double meanGray = mean_In.at<double>(0, 0);
            minMaxIdx(CropGrayImage, &minValue, &maxValue);
            float w = boundRect[i].width;
            float h = boundRect[i].height;
            float ratio = w / h;
            int differ = maxValue - minValue;
            if ((max(w / h, h / w) < 6) && meanGray > 110 && differ > 7)
            {
                if (area <= 30000) {
                    if ((max(w / h, h / w)) > 3 && X_1 < 20 )
                    {
                        continue;
                    }
                }
                int brder = 5;
                if (X_1 - brder - int(w) > 0) {
                    X_1 = X_1 - brder - int(w);
                }
                if (Y_1 - brder - int(h) > 0)
                {
                    Y_1 = Y_1 - brder - int(h);
                }
                if (X_2 + brder + int(w) < binaryzation.cols)
                {
                    X_2 = X_2 + brder + int(w);
                }
                if (Y_2 + brder + int(h) < binaryzation.rows)
                {
                    Y_2 = Y_2 + brder + int(h);
                }

                Mat temp_Mask_erode;
                Mat temp_Mask = binaryzation(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                //二值图像掩膜图
                //Mat structureElement = getStructuringElement(MORPH_RECT, Size(3,3), Point(-1, -1));
                //erode(temp_Mask, temp_Mask_erode, structureElement);//排除摩尔纹
                findContours(temp_Mask, contours_1, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
                if (contours_1.size() > 6)
                {
                    continue;
                }
                //vector<Rect> boundRect_1(contours_1.size());
                //for (int j = 0; j < contours_1.size(); j++)
                //{
                //	int X_3 = boundingRect(Mat(contours_1[j])).tl().x;//矩形左上角X坐标值
                //	int Y_3 = boundingRect(Mat(contours_1[j])).tl().y;//矩形左上角Y坐标值
                //	int X_4 = boundingRect(Mat(contours_1[j])).br().x;//矩形右下角X坐标值
                //	int Y_4 = boundingRect(Mat(contours_1[j])).br().y;//矩形右下角Y坐标值
                //	if (countNonZero(temp_Mask_erode(Rect(X_3, Y_3, X_4 - X_3, Y_4 - Y_3))) > 50)
                //	{
                //		break;
                //	}
                //	else if (j != contours_1.size() - 1) {
                //		continue;
                //	}
                //	areaflag = false;
                //}
                //if (!areaflag)
                //{
                //	continue;
                //}
                Mat temp_Gray = img_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                       //疑似缺陷HSV图像
                Mat temp_Ceguang = ceguang_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                meanStdDev(temp_Gray, mean_In, stddev_In);
                if (stddev_In.at<double>(0, 0) < 3)
                {
                    continue;
                }
                Mat temp_Mask_In, temp_Mask_Out;
                bitwise_and(temp_Gray, temp_Mask, temp_Mask_In);

                meanStdDev(temp_Gray, mean_In, stddev_In, temp_Mask);
                double meanH_in = mean_In.at<double>(0, 0);

                bitwise_and(temp_Gray, ~temp_Mask, temp_Mask_Out);
                meanStdDev(temp_Gray, mean_In, stddev_In, ~temp_Mask);

                Mat temp_Gray_Threshold;
                threshold(temp_Mask_Out, temp_Gray_Threshold, 55, 255, CV_THRESH_BINARY);				//二值化
                meanStdDev(temp_Mask_Out, mean_In, stddev_In, temp_Gray_Threshold);

                double meanH_out = mean_In.at<double>(0, 0);
                double meanDiffer = meanH_in - meanH_out;
                meanStdDev(temp_Ceguang, mean_In, stddev_In, temp_Mask);
                meanH_in = mean_In.at<double>(0, 0);
                meanStdDev(temp_Ceguang, mean_In, stddev_In, ~temp_Mask);
                meanH_out = mean_In.at<double>(0, 0);
                double ceguangMeanDiffer = abs(meanH_in - meanH_out);
                if (meanDiffer > 7 && ceguangMeanDiffer < 5)
                {
                    vector<int> meanVector;
                    if (boundRect[i].tl().y > h / 3 && boundRect[i].br().y > (temp_Gray.rows - h / 3))
                    {
                        for (int m = 0; m < temp_Gray.rows; m++)
                        {
                            Mat rowRange = temp_Gray.rowRange(m, m + 1);
                            meanStdDev(rowRange, mean_, std_);
                            int mean_1 = mean_.at<double>(0, 0);
                            meanVector.push_back(mean_1);
                            cout << mean_.at<double>(0, 0) << endl;

                        }
                        for (int i = 0; i < meanVector.size() / 3; i++)
                        {
                            sum += meanVector[i];
                            count++;
                        }
                        mean_fore_1 = sum / count;
                        sum = 0;
                        count = 0;
                        for (int i = meanVector.size() / 3; i < meanVector.size() / 3 * 2; i++)
                        {
                            sum += meanVector[i];
                            count++;
                        }
                        mean_fore_2 = sum / count;
                        sum = 0;
                        count = 0;
                        for (int i = meanVector.size() / 3 * 2; i < meanVector.size(); i++)
                        {
                            sum += meanVector[i];
                            count++;
                        }
                        mean_fore_3 = sum / count;
                        if ((mean_fore_3 - mean_fore_2 > 2 && mean_fore_2 - mean_fore_1 > 2) ||
                            (mean_fore_1 - mean_fore_2 > 2 && mean_fore_2 - mean_fore_3 > 2)) {
                            continue;
                        }
                    }
                    Mat binImage;
                    Mat Temp_Gray = temp_Gray(Rect(1,0, temp_Gray.cols - 1, temp_Gray.rows));
                    minMaxIdx(Temp_Gray, &minValue, &maxValue);
                    if (maxValue - minValue > 40) {
                        adaptiveThresholdCustom(Temp_Gray, binImage, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 7, 5, 1, 5);
                        double area_bin = countNonZero(binImage);//白色区域的像素个数作为面积
                        if (area_bin > 10)
                        {
                            continue;
                        }
                    }
                    rectangle(result_image, cvPoint(X_1, Y_1), cvPoint(X_2, Y_2), Scalar(0, 0, 0), 5, 8, 0);
                    result = true;
                    //imwrite("C:\\Users\\ch\\Desktop\\result_image.bmp", result_image);
                }

            }

        }
    }
    *mura = binaryzation;
    if (result == true)
    {
        *mresult1 = result_image;
        *causecolor = "白印";
    }
    return result;
}

/*====================================================================
* 函 数 名: mura
* 功能描述: 白印检测、膜拱检测  mura:指显示器亮度不均匀, 造成各种痕迹的现象. 膜拱:屏幕压合时内部存在空气
* 输入：主、左、右相机白底图像
* 输出：主、左、右相机白底下检测结果图和result
* 其他：
======================================================================*/
bool  mura(Mat image_ceguang ,Mat image_ceguang2,Mat *mresult1,Mat *mura,QString *causecolor)
{
    int type = 1;//屏幕类型，后续添加至全局变量;       0:水滴;1:刘海;2:较大水滴;
    int co_X1 = 0;//刘海上方坐标
    int co_X2 = 0;//刘海下方坐标

    bool result=false;
    Whiteprint_Detect_Flag = 2;
    Mat result_image = image_ceguang.clone();//框出缺陷结果图，防止对后续检测产生干扰
    Mat img_gray=image_ceguang.clone();

    Mat img_strong = image_ceguang.clone();//
    Mat ceguang_gray=image_ceguang2.clone();
    medianBlur(img_gray, img_gray, 3);//中值滤波滤除椒盐噪声,缺点耗时26毫秒 奇数半径越大效果越强
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3,3));
    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(7,7));
    Mat structure_element3 = getStructuringElement(MORPH_RECT, Size(5,5));
    if(type == 0)//水滴屏
    {
        co_X1 = 650;
        co_X2 = 850;

    }
    else if(type == 1)//刘海屏
    {
        co_X1 = 450;
        co_X2 = 1050;
    }
    else if(type == 2)//
    {
        co_X1 = 575;
        co_X2 = 925;
    }
    else {
        co_X1 = 575;
        co_X2 = 925;
    }

    Mat binaryzation=Mat::zeros(img_gray.size(),img_gray.type());//二值化图像
    Mat dilate1=Mat::zeros(img_gray.size(),img_gray.type());
    if (Model_Mod_type == "矩形屏")
    {
        Mat img_center_mid=img_gray(Rect(200,100,img_gray.cols-400,img_gray.rows-200));//中间部分(中)
        Mat img_CenM_mask;
        if(Whiteprint_Detect_Flag == 2)
        {
            img_CenM_mask= Ployfit_Col7(img_center_mid, 3, 0, black_mura_gray_th-3);
        }
        else {
            img_CenM_mask= Ployfit_Col7(img_center_mid, 3, 0, black_mura_gray_th);

        }
        Mat img_center_left=img_gray(Rect(0,100,200,img_gray.rows-200));//中间部分(左)
        Mat img_CenL_mask= Ployfit_Col7(img_center_left, 3, 0, black_mura_gray_th+7);

        Mat img_center_right=img_gray(Rect(img_gray.cols-200,100,200,img_gray.rows-200));//中间部分(右)
        Mat img_CenR_mask= Ployfit_Col7(img_center_right, 3, 0, black_mura_gray_th+7);

        Mat img_top=img_gray(Rect(0,0,img_gray.cols,100));//上面部分
        img_top=matRotateClockWise90(img_top);
        Mat img_top_mask=Ployfit_Col7(img_top, 3, 0, black_mura_gray_th+7);
        img_top_mask=matRotateClockWisefu90(img_top_mask);

        Mat img_bottom=img_gray(Rect(0,img_gray.rows-100,img_gray.cols,100));//下面部分
        img_bottom=matRotateClockWise90(img_bottom);//旋转顺时针90度
        Mat img_bottom_mask=Ployfit_Col7(img_bottom, 3, 0, black_mura_gray_th+7);
        img_bottom_mask=matRotateClockWisefu90(img_bottom_mask);//旋转逆时针90度

        img_top_mask.copyTo(binaryzation( Rect(0,0,binaryzation.cols,100)));
        img_bottom_mask.copyTo(binaryzation( Rect(0,binaryzation.rows-100,binaryzation.cols,100)));
        img_CenM_mask.copyTo(binaryzation( Rect(200,100,binaryzation.cols-400,binaryzation.rows-200)));
        img_CenL_mask.copyTo(binaryzation( Rect(0,100,200,binaryzation.rows-200)));
        img_CenR_mask.copyTo(binaryzation( Rect(binaryzation.cols-200,100,200,binaryzation.rows-200)));
        imwrite("D:\\binaryzation.bmp",binaryzation);
    }
    else {
        Mat temp = Mat::zeros(img_gray.size(),img_gray.type());
        Mat img_center_mid=img_gray(Rect(200,200,img_gray.cols-400,img_gray.rows-400));//中间部分(中)
        Mat img_CenM_mask;

        if(Whiteprint_Detect_Flag == 2)
        {
            img_CenM_mask= Ployfit_Col7(img_center_mid, 3, 0, black_mura_gray_th-3);//cur -> img_center_mid
            morphologyEx(img_CenM_mask, img_CenM_mask, CV_MOP_OPEN, structure_element2);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
            dilate(img_CenM_mask, img_CenM_mask, structure_element3);                  //膨胀扩大缺陷面积
        }
        else
        {
            img_CenM_mask= Ployfit_Col7(img_center_mid, 3, 0, black_mura_gray_th);//cur -> img_center_mid
            morphologyEx(img_CenM_mask, img_CenM_mask, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
            dilate(img_CenM_mask, img_CenM_mask, structure_element2);                  //膨胀扩大缺陷面积
        }

        Mat img_CenL_mask = Mat::zeros(Size(200,img_gray.rows-400),img_gray.type());//中间部分(左)
        Mat img_center_left = img_gray(Rect(0,200,200,img_gray.rows-400));//中间部分(左)

        //*********************************根据屏幕类型设定对应尺寸，扣出刘海区域干扰*********************************
        Mat img_center_left_T=img_gray(Rect(0,200,200,co_X1-200));//中间部分(左上)
        Mat img_CenL_mask_T= Ployfit_Col7(img_center_left_T, 3, 0, black_mura_gray_th+7);

        morphologyEx(img_CenL_mask_T, img_CenL_mask_T, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
        dilate(img_CenL_mask_T, img_CenL_mask_T, structure_element2);                  //膨胀扩大缺陷面积

        Mat img_center_left_Mid=img_gray(Rect(0,co_X1,200,co_X2-co_X1));//中间部分(左中)刘海区域
        //R角部分sobel算子边缘检测
        Mat sobel_result_CenMid_x_R;
        Sobel(img_center_left_Mid, sobel_result_CenMid_x_R, CV_16S, 0, 1, 3, 1, 2, BORDER_DEFAULT);
        convertScaleAbs(sobel_result_CenMid_x_R, sobel_result_CenMid_x_R);
        Mat sobel_result_CenMid_y_R;
        Sobel(img_center_left_Mid, sobel_result_CenMid_y_R, CV_16S, 1, 0, 3, 1, 2, BORDER_DEFAULT);
        convertScaleAbs(sobel_result_CenMid_y_R, sobel_result_CenMid_y_R);
        Mat sobel_result_CenMid_R;
        Mat sobel_CenMid_R;
        addWeighted(sobel_result_CenMid_x_R, 0.5, sobel_result_CenMid_y_R, 0.5, 0, sobel_result_CenMid_R);

        Ptr<CLAHE> clahe = createCLAHE(2.0, Size(3, 3));
        clahe->apply(sobel_result_CenMid_R, sobel_CenMid_R);
        Mat Bsobel_CenMid_R;
        threshold(sobel_CenMid_R, Bsobel_CenMid_R, 30, 255, CV_THRESH_BINARY);
        Bsobel_CenMid_R = ~Bsobel_CenMid_R;
        erode(Bsobel_CenMid_R, Bsobel_CenMid_R, structure_element3);                       //腐蚀，防止缺陷区域无法分割出来
        Mat img_CenMid_mask_L=Bsobel_CenMid_R.clone();


        Mat img_center_left_B=img_gray(Rect(0,co_X2,200,img_gray.rows-co_X2-200));//中间部分(左下)
        Mat img_CenL_mask_B= Ployfit_Col7(img_center_left_B, 3, 0, black_mura_gray_th+7);

        morphologyEx(img_CenL_mask_B, img_CenL_mask_B, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
        dilate(img_CenL_mask_B, img_CenL_mask_B, structure_element2);                  //膨胀扩大缺陷面积

        img_CenL_mask_T.copyTo(img_CenL_mask( Rect(0,0,200,co_X1-200)));
        img_CenMid_mask_L.copyTo(img_CenL_mask( Rect(0,co_X1-200,200,co_X2-co_X1)));
        img_CenL_mask_B.copyTo(img_CenL_mask( Rect(0,co_X2-200,200,img_CenL_mask.rows-(co_X2-200))));

        //*********************************根据屏幕类型设定对应尺寸，扣出刘海区域干扰*********************************
        Mat img_center_right=img_gray(Rect(img_gray.cols-200,200,200,img_gray.rows-400));//中间部分(右)
        Mat img_CenR_mask= Ployfit_Col7(img_center_right, 3, 0, black_mura_gray_th+7);

        morphologyEx(img_CenR_mask, img_CenR_mask, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
        dilate(img_CenR_mask, img_CenR_mask, structure_element2);                  //膨胀扩大缺陷面积

        Mat img_top_M=img_gray(Rect(200,0,img_gray.cols-400,200));//上面部分(中)
        img_top_M=matRotateClockWise90(img_top_M);
        Mat img_topM_mask=Ployfit_Col7(img_top_M, 3, 0, black_mura_gray_th+14);
        img_topM_mask=matRotateClockWisefu90(img_topM_mask);
        img_top_M=matRotateClockWisefu90(img_top_M);//

        morphologyEx(img_topM_mask, img_topM_mask, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
        dilate(img_topM_mask, img_topM_mask, structure_element2);                  //膨胀扩大缺陷面积

        Mat img_top_R=img_gray(Rect(img_gray.cols-200,0,200,200));//上面部分(右)
        Mat temp_img_top_R = img_top_R.clone();
        //R角部分sobel算子边缘检测
        Mat sobel_result_top_x_R;
        Sobel(temp_img_top_R, sobel_result_top_x_R, CV_16S, 0, 1, 3, 1, 2, BORDER_DEFAULT);
        convertScaleAbs(sobel_result_top_x_R, sobel_result_top_x_R);
        Mat sobel_result_top_y_R;
        Sobel(temp_img_top_R, sobel_result_top_y_R, CV_16S, 1, 0, 3, 1, 2, BORDER_DEFAULT);
        convertScaleAbs(sobel_result_top_y_R, sobel_result_top_y_R);
        Mat sobel_result_top_R;
        Mat sobel_top_R;
        addWeighted(sobel_result_top_x_R, 0.5, sobel_result_top_y_R, 0.5, 0, sobel_result_top_R);

        //Ptr<CLAHE> clahe = createCLAHE(2.0, Size(3, 3));
        clahe->apply(sobel_result_top_R, sobel_top_R);
        Mat Bsobel_top_R;
        threshold(sobel_top_R, Bsobel_top_R, 30, 255, CV_THRESH_BINARY);
        Bsobel_top_R = ~Bsobel_top_R;
        erode(Bsobel_top_R, Bsobel_top_R, structure_element3);                       //腐蚀，防止缺陷区域无法分割出来
        Mat img_topR_mask=Bsobel_top_R.clone();
        //img_top_R=matRotateClockWisefu90(img_top_R);//

        Mat img_top_L=img_gray(Rect(0,0,200,200));//上面部分(左)
        Mat temp_img_top_L = img_top_L.clone();
        //R角部分sobel算子边缘检测
        Mat sobel_result_top_x_L;
        Sobel(temp_img_top_L, sobel_result_top_x_L, CV_16S, 0, 1, 3, 1, 2, BORDER_DEFAULT);
        convertScaleAbs(sobel_result_top_x_L, sobel_result_top_x_L);
        Mat sobel_result_top_y_L;
        Sobel(temp_img_top_L, sobel_result_top_y_L, CV_16S, 1, 0, 3, 1, 2, BORDER_DEFAULT);
        convertScaleAbs(sobel_result_top_y_L, sobel_result_top_y_L);
        Mat sobel_result_top_L;
        Mat sobel_top_L;
        addWeighted(sobel_result_top_x_L, 0.5, sobel_result_top_y_L, 0.5, 0, sobel_result_top_L);

        clahe->apply(sobel_result_top_L, sobel_top_L);
        Mat Bsobel_top_L;
        threshold(sobel_top_L, Bsobel_top_L, 30, 255, CV_THRESH_BINARY);
        Bsobel_top_L = ~Bsobel_top_L;
        erode(Bsobel_top_L, Bsobel_top_L, structure_element3);                       //腐蚀，防止缺陷区域无法分割出来

        Mat img_topL_mask=Bsobel_top_L.clone();
        //img_top_L=matRotateClockWisefu90(img_top_L);//

        Mat img_bottom_M=img_gray(Rect(200,img_gray.rows-200,img_gray.cols-400,200));//下面部分(中)
        img_bottom_M=matRotateClockWise90(img_bottom_M);//旋转顺时针90度
        Mat img_bottomM_mask=Ployfit_Col7(img_bottom_M, 3, 0, black_mura_gray_th+14);
        img_bottomM_mask=matRotateClockWisefu90(img_bottomM_mask);//旋转逆时针90度
        img_bottom_M=matRotateClockWisefu90(img_bottom_M);//

        morphologyEx(img_bottomM_mask, img_bottomM_mask, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
        dilate(img_bottomM_mask, img_bottomM_mask, structure_element2);                  //膨胀扩大缺陷面积

        Mat img_bottom_R=img_gray(Rect(img_gray.cols-200,img_gray.rows-200,200,200));//下面部分(右)
        Mat temp_img_bottom_R = img_bottom_R.clone();
        //R角部分sobel算子边缘检测
        Mat sobel_result_bottom_x_R;
        Sobel(temp_img_bottom_R, sobel_result_bottom_x_R, CV_16S, 0, 1, 3, 1, 2, BORDER_DEFAULT);
        convertScaleAbs(sobel_result_bottom_x_R, sobel_result_bottom_x_R);
        Mat sobel_result_bottom_y_R;
        Sobel(temp_img_bottom_R, sobel_result_bottom_y_R, CV_16S, 1, 0, 3, 1, 2, BORDER_DEFAULT);
        convertScaleAbs(sobel_result_bottom_y_R, sobel_result_bottom_y_R);
        Mat sobel_result_bottom_R;
        Mat sobel_bottom_R;
        addWeighted(sobel_result_bottom_x_R, 0.5, sobel_result_bottom_y_R, 0.5, 0, sobel_result_bottom_R);

        clahe->apply(sobel_result_bottom_R, sobel_bottom_R);
        Mat Bsobel_bottom_R;
        threshold(sobel_bottom_R, Bsobel_bottom_R, 30, 255, CV_THRESH_BINARY);
        Bsobel_bottom_R = ~Bsobel_bottom_R;

        erode(Bsobel_bottom_R, Bsobel_bottom_R, structure_element3);                       //腐蚀，防止缺陷区域无法分割出来

        Mat img_bottomR_mask=Bsobel_bottom_R.clone();

        Mat img_bottom_L=img_gray(Rect(0,img_gray.rows-200,200,200));//下面部分(左)
        //R角部分sobel算子边缘检测
        Mat sobel_result_bottom_x_L;
        Sobel(img_bottom_L, sobel_result_bottom_x_L, CV_16S, 0, 1, 3, 1, 2, BORDER_DEFAULT);
        convertScaleAbs(sobel_result_bottom_x_L, sobel_result_bottom_x_L);
        Mat sobel_result_bottom_y_L;
        Sobel(img_bottom_L, sobel_result_bottom_y_L, CV_16S, 1, 0, 3, 1, 2, BORDER_DEFAULT);
        convertScaleAbs(sobel_result_bottom_y_L, sobel_result_bottom_y_L);
        Mat sobel_result_bottom_L;
        Mat sobel_bottom_L;
        addWeighted(sobel_result_bottom_x_L, 0.5, sobel_result_bottom_y_L, 0.5, 0, sobel_result_bottom_L);

        clahe->apply(sobel_result_bottom_L, sobel_bottom_L);
        Mat Bsobel_bottom_L;
        threshold(sobel_bottom_L, Bsobel_bottom_L, 30, 255, CV_THRESH_BINARY);

        Bsobel_bottom_L = ~Bsobel_bottom_L;
        erode(Bsobel_bottom_L, Bsobel_bottom_L, structure_element3);                       //腐蚀，防止缺陷区域无法分割出来

        Mat img_bottomL_mask=Bsobel_bottom_L.clone();


        img_topM_mask.copyTo(binaryzation( Rect(200,0,binaryzation.cols-400,200)));
        img_topR_mask.copyTo(binaryzation( Rect(binaryzation.cols-200,0,200,200)));
        img_topL_mask.copyTo(binaryzation( Rect(0,0,200,200)));
        img_bottomM_mask.copyTo(binaryzation( Rect(100,binaryzation.rows-200,binaryzation.cols-400,200)));
        img_bottomR_mask.copyTo(binaryzation( Rect(binaryzation.cols-200,binaryzation.rows-200,200,200)));
        img_bottomL_mask.copyTo(binaryzation( Rect(0,binaryzation.rows-200,200,200)));

        img_CenM_mask.copyTo(binaryzation( Rect(200,200,binaryzation.cols-400,binaryzation.rows-400)));
        img_CenL_mask.copyTo(binaryzation( Rect(0,200,200,binaryzation.rows-400)));
        img_CenR_mask.copyTo(binaryzation( Rect(binaryzation.cols-200,200,200,binaryzation.rows-400)));
        imwrite("D:\\binaryzation.bmp",binaryzation);
        //测试
        img_top_M.copyTo(temp( Rect(200,0,temp.cols-400,200)));
        img_top_R.copyTo(temp( Rect(temp.cols-200,0,200,200)));
        img_top_L.copyTo(temp( Rect(0,0,200,200)));
        img_bottom_M.copyTo(temp( Rect(200,temp.rows-200,temp.cols-400,200)));
        img_bottom_R.copyTo(temp( Rect(temp.cols-200,temp.rows-200,200,200)));
        img_bottom_L.copyTo(temp( Rect(0,temp.rows-200,200,200)));

        img_center_mid.copyTo(temp( Rect(200,200,temp.cols-400,temp.rows-400)));
        img_center_left.copyTo(temp( Rect(0,200,200,temp.rows-400)));
        img_center_right.copyTo(temp( Rect(temp.cols-200,200,200,temp.rows-400)));
        img_gray = temp.clone();
        dilate1 = binaryzation.clone();


        //4个R角区域分块处理
        dilate1(Rect(0, 199, 200, 1)) = uchar(0);
        dilate1(Rect(199, 0, 1, 200)) = uchar(0);

        dilate1(Rect(2800, 0, 1, 200)) = uchar(0);
        dilate1(Rect(2800, 199, 200, 1)) = uchar(0);

        dilate1(Rect(0, 1300, 200, 1)) = uchar(0);
        dilate1(Rect(199, 1300, 1, 200)) = uchar(0);

        dilate1(Rect(2800, 1300, 200, 1)) = uchar(0);
        dilate1(Rect(2800, 1300, 1, 200)) = uchar(0);

        //刘海区域掩膜隔离
        dilate1(Rect(0, co_X1, 200, 1)) = uchar(0);
        dilate1(Rect(199, co_X1, 1, co_X2-co_X1)) = uchar(0);
        dilate1(Rect(0, co_X2-1, 200, 1)) = uchar(0);

        imwrite("D:\\dilatefinal.bmp",dilate1);

    }
    vector<vector<Point>> contours;
    findContours( dilate1, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());

    for(vector<int>::size_type i = 0; i < contours.size(); i++)
    {

        Mat temp_mask = Mat::zeros(dilate1.rows,dilate1.cols,CV_8UC1);
        drawContours(temp_mask,contours,i,255,FILLED,8);

        double area = contourArea(contours[i]);

        RotatedRect rect = minAreaRect(contours[i]);  //包覆轮廓的最小斜矩形
        Point p = rect.center;

        int row = p.y-1;
        int col = p.x-1;
        if (p.y-1 < 0)
        {
            row = 0;
        }
        if (p.y+1 >= dilate1.rows - 1)
        {
            row = dilate1.rows - 4;
        }
        if (p.x-1 < 0)
        {
            col = 0;
        }
        if (p.x+1 >= dilate1.cols - 1)
        {
            col = dilate1.cols - 4;
        }
        Mat check = dilate1(Rect(col, row, 3, 3));
        if (mean(check)[0] < 114)//防止分割出于黑色区域 dilate1.at<uchar>(p.y, p.x) == 0
        {
            continue;//有3个像素点是白色时，均值是85;有4个像素点时，均值是113.33
        }


        int R_area_Low = 50;
        int R_area_High = 2000;
        int area_Low = black_mura_area_lower-900;//面积下限600
        if(Whiteprint_Detect_Flag == 2)
        {
            area_Low = black_mura_area_lower - 900;//面积下限1000
        }
        int area_High = black_mura_area_upper + 20000;
        if((p.x < 200 && p.y < 200) || (p.x > 2800 && p.y < 200) || (p.x < 200 && p.y > 1300)
                || (p.x > 2800 && p.y > 1300) || (p.y >co_X1  && p.y < co_X2 && p.x < 200))
        {
            area_Low = R_area_Low;
            area_High = R_area_High;

        }
        if (area>area_Low and area<area_High)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            float w=boundRect[i].width;
            float h=boundRect[i].height;

            double w1 = rect.size.height;
            double h1 = rect.size.width;
            int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2=boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
            int x_1 = X_1;//矩形左上角X坐标值
            int y_1 = Y_1;//矩形左上角Y坐标值
            int x_2 = X_2;//矩形右下角X坐标值
            int y_2 = Y_2;//矩形右下角Y坐标值
            int x_point=X_1+round(w/2);
            int y_point=Y_1+round(h/2);
            double radio = 2;//2.5
            if(area > 15000)//面积大于15000则扩大长宽比上限
            {
                radio = 4;
            }
            if(p.y<100 || p.y>1400)
            {
                radio = 20;
            }
            if((max(w1 / h1, h1 / w1) < radio))//长宽比原值：1.68,3.1
            {
                X_1=X_1-black_mura_boder-int(w);
                Y_1=Y_1-black_mura_boder-int(h);
                X_2=X_2+black_mura_boder+int(w);
                Y_2=Y_2+black_mura_boder+int(h);
                if(X_1<0)
                {
                    X_1=0;
                }
                if(Y_1<0)
                {
                    Y_1=0;
                }
                if(X_2>binaryzation.cols-1)
                {
                    X_2=binaryzation.cols-1;
                }
                if(Y_2>binaryzation.rows-1)
                {
                    Y_2=binaryzation.rows-1;
                }

                if(Whiteprint_Detect_Flag == 2)
                {
                    if(area < 15000)//15000
                    {
                        Mat check_dilate1 = dilate1(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
                        vector<vector<Point>> contours1;//判断目标区域孤立白色区域个数，上限8
                        erode(check_dilate1,check_dilate1,structure_element1);
                        findContours(check_dilate1, contours1, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
                        double domainnum=contours1.size();
                        qDebug()<<"area:  "<<area;
                        qDebug()<<"domainnum:  "<<domainnum;
                        if (domainnum >5)
                        {
                            continue;
                        }
                    }
                }
                Mat temp_ceguang=img_gray(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
                //统计数据**************************************************
                Mat ag;
                Mat ll;
                meanStdDev(temp_ceguang, ag, ll);
                double x = ag.at<double>(0, 0);//灰度方差
                double y = ll.at<double>(0, 0);//灰度方差

                //统计数据**************************************************

                Mat mask1 = temp_mask(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                Mat temp_mask1;
                Mat temp_mask2;
                threshold(temp_ceguang, temp_mask1, 20, 255, CV_THRESH_BINARY);
                bitwise_and(temp_mask1, ~mask1, temp_mask2);
                double mean_in_gray = mean(temp_ceguang, mask1)[0];
                Mat img_out;
                double sum = 0;
                int num = 0;
                bitwise_and(temp_ceguang,temp_mask2,img_out);
                for (int i = 0; i < img_out.rows; i++)
                {
                    uchar *data1 = img_out.ptr<uchar>(i);
                    for (int j = 0; j < img_out.cols; j++)
                    {
                        if(data1[j] < mean_in_gray && data1[j]> 30)
                        {
                            sum += data1[j];
                            num++;
                        }
                    }
                }
                double mean_out_gray = sum/num;
                double mean_ceguang = mean(temp_ceguang)[0];

                Mat filterceguang = ceguang_gray(Rect(x_1, y_1, x_2 - x_1, y_2 - y_1)); //提取最小正接白印缺陷侧光图区域
                Mat filterceguang1 = ceguang_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1)); //提取最小正接白印缺陷侧光图区域

               double ceguang_in = mean(filterceguang1,mask1)[0];
               double ceguang_out = mean(filterceguang1,~mask1)[0];
               double thresold_ceguang = ceguang_in-ceguang_out;

               meanStdDev(img_gray, ag, ll);
               double x1 = ag.at<double>(0, 0);//ROI区域灰度均值
               double x2 = ll.at<double>(0,0);
               if(p.y<100 || p.y>1400)//边缘部分排除提取ROI区域不准确干扰
               {
                   if(mean_in_gray < x1+x2)
                   {
                       continue;
                   }
               }

                if(!((p.x < 200 && p.y < 200) || (p.x > 2800 && p.y < 200) || (p.x < 200 && p.y > 1300)
                     || (p.x > 2800 && p.y > 1300) || (p.y >co_X1  && p.y < co_X2 && p.x < 200)))
                {
                    cv::Mat meanGray;
                    cv::Mat stdDev;
                    cv::meanStdDev(filterceguang, meanGray, stdDev);
                    double stddev = stdDev.at<double>(0, 0);
                    //排除贴膜划痕
                    if (stddev>9.1)//mura_bubbleth
                    {
                        continue;
                    }
                    if(thresold_ceguang > 4.5)
                    {
                        continue;
                    }
                }
                //统计数据********************************************
                double threshold = mean_in_gray - mean_out_gray; //针对不规则缺陷
                if((x_point<black_mura_corner and y_point<black_mura_corner) or (binaryzation.cols-x_point<black_mura_corner and binaryzation.rows-y_point<black_mura_corner) or(x_point<black_mura_corner and binaryzation.rows-y_point<black_mura_corner) or (binaryzation.cols-x_point<black_mura_corner and y_point<black_mura_corner))//(img_5.cols-x_point<5 and or img_5.rows-x_point<15 or x_point<15 or y_point<15) and(mean_ceguang)>200
                {
                    continue;
                }

                double centergray=img_gray.at<uchar>(y_point,x_point)-mean_ceguang;
                string b = "yuzhi " + to_string(centergray) ;
                double lighth = 3.2;//3.8
                if(Whiteprint_Detect_Flag == 2)
                {
                    lighth = 3.4;//3.2
                }
                if(threshold>lighth)//mura_lighth  4.5
                {
                    if(true)
                    {
                        double  minStrength, maxStrength;
                        Mat image_highlight, image_lowlight;
                        Mat filter = img_gray(Rect(x_1, y_1, x_2 - x_1, y_2 - y_1)); //提取最小正接白印缺陷区域
                        Mat mask2 = dilate1(Rect(x_1, y_1, x_2 - x_1, y_2 - y_1));   //提取最小正接白印缺陷区域掩膜
                        //*************************************
                        double min,max;
                        cv::Point j;
                        cv::Point i;
                        cv::minMaxLoc(filter, &min, &max,&i,&j,mask2);  //查找此处的最亮点处灰度值
                        //*************************************
                        bitwise_and(filter, mask2, image_highlight);                 //提取白印的高亮度区域
                        cv::minMaxLoc(image_highlight, &minStrength, &maxStrength);  //查找此处的最亮点处灰度值

                        minStrength = mean(filter, ~mask2)[0];	                     //计算低亮度灰度均值
                        double peakGray = maxStrength - minStrength;
                        if(peakGray>2.8)//3.5
                        {
                            result=true;
                            rectangle(result_image , cvPoint(X_1, Y_1), cvPoint(X_2, Y_2), Scalar(0,0,0), 5, 8, 0);
                            Point p3(x_point, y_point);
                            circle(result_image,p3,3,Scalar(0,0,255),-1);

                            //string abc = "area: " + to_string(area) +"num: " + to_string(domainnum);
                            //putText(img_gray, abc, Point(X_1-5, Y_1-5), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255), 1, 8, 0);
                            imwrite("D:\\muraresult.bmp",result_image);

                        }
                    }
                }
            }
        }
    }
    *mura=binaryzation;
    if(result==true)
    {
        *mresult1=result_image;
        *causecolor="白印";
        //return result;//圈出全部缺陷区域
    }
    return result;
}



/*=========================================================
* 函 数 名: toushi_white
* 功能描述: 透视变换图像矫正
=========================================================*/
Mat toushi_white(Mat image, Mat M, int border, int length, int width)
{
    Mat perspective;
    cv::warpPerspective(image, perspective, M, cv::Size(length, width), cv::INTER_LINEAR);
    return perspective;
}

/*=========================================================
* 函 数 名: Gabor7
* 功能描述: gabor滤波
=========================================================*/
Mat Gabor7(Mat img_1)
{
    Mat kernel1 = getGaborKernel(Size(5, 5), 1.1, CV_PI/2, 1.0, 1.0, 0, CV_32F );//求卷积核
    float sum=0.0;
    for (int i = 0; i < kernel1.rows; i++)
    {
        for (int j = 0; j < kernel1.cols; j++)
        {
            sum=sum+kernel1.ptr<float>(i)[j];
        }
    }
    Mat mmm= kernel1/sum;
    Mat kernel2 = getGaborKernel(Size(5, 5), 1.1, 0, 1.0, 1.0, 0, CV_32F );
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
    return img_5;
}

/*=========================================================
* 函 数 名: light_point7
* 功能描述: 亮点检测
=========================================================*/
bool light_point7(Mat image_black, Mat image_white, Mat *mresult, QString *causecolor)
{
    int result = 0;
    Mat black_gray;
    black_gray = image_black.clone();
    Mat gray_clone = black_gray.clone();

    gray_clone(Rect(gray_clone.cols - 240, gray_clone.rows - 315, 240, 315)) = uchar(0);//去除易撕贴部分域
                                                                                         //修改：之前去除边缘的程序删了。修改人:郭栋。修改时间：20200713
    //异形屏幕去除刘海干扰
    if (Model_Mod_type == "R角水滴屏")
    {
        bitwise_and(main_mask, gray_clone, gray_clone);
    }
    //======================================================
    threshold(gray_clone, gray_clone, 35, 255, CV_THRESH_BINARY);//修改：由之前的自适应阈值改成固定阈值。修改人:郭栋。修改时间：20200713

    vector<vector<Point>> contours;
    findContours(gray_clone, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);

        if (area > black_lightPoint_area_lower and area < black_lightPoint_area_upper)//面积筛选
        {
            cout << "缺陷区域面积" << area << endl;
            RotatedRect rect = minAreaRect(contours[i]); //包覆轮廓的最小斜矩形
            double w1 = rect.size.height;
            double h1 = rect.size.width;

            boundRect[i] = boundingRect(Mat(contours[i]));
            float w = boundRect[i].width;
            float h = boundRect[i].height;
            int x_1 = boundRect[i].tl().x;//矩形左上角X坐标值
            int y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
            int x_2 = boundRect[i].br().x;//矩形右下角X坐标值
            int y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
            int x_point = x_1 + round(w / 2);
            int y_point = y_1 + round(h / 2);

                //将区域扩大
            int	X_1 = x_1  - int(w);
            int	Y_1 = y_1  - int(h);
            int	X_2 = x_2  + int(w);
            int	Y_2 = y_2  + int(h);
                if (X_1 < 0)
                {
                    X_1 = 0;
                }
                if (Y_1 < 0)
                {
                    Y_1 = 0;
                }
                if (X_2 > image_black.cols - 1)
                {
                    X_2 = image_black.cols - 1;
                }
                if (Y_2 > image_black.rows - 1)
                {
                    Y_2 = image_black.rows - 1;
                }

                Mat temp_gray = black_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                Mat mask =  gray_clone(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                double meanOut;
                double meanIn;
                double meanAll;
                int grayValueSum = 0;
                int pixelsNum = 0;
                Mat maskGray;
                bitwise_and(temp_gray, mask, maskGray);
                for (int i = 0; i < maskGray.cols; i++)
                {
                    for (int j = 0; j < maskGray.rows; j++)
                    {
                        if (maskGray.at<uchar>(j, i) > 0)
                        {
                            grayValueSum += maskGray.at<uchar>(j, i);
                            pixelsNum++;
                        }
                    }
                }
                meanIn = grayValueSum / (float)pixelsNum;

                 grayValueSum = 0;
                 pixelsNum = 0;
                 bitwise_and(temp_gray, ~mask, maskGray);
                 for (int i = 0; i < maskGray.cols; i++)
                 {
                     for (int j = 0; j < maskGray.rows; j++)
                     {
                         if (maskGray.at<uchar>(j, i) > 0)
                         {
                             grayValueSum += maskGray.at<uchar>(j, i);
                             pixelsNum++;
                         }
                     }
                 }
                 meanOut = grayValueSum / (float)pixelsNum;

                 grayValueSum = 0;
                 pixelsNum = 0;
                 for (int i = 0; i < temp_gray.cols; i++)
                 {
                     for (int j = 0; j < temp_gray.rows; j++)
                     {
                         if (temp_gray.at<uchar>(j, i) > 0)
                         {
                             grayValueSum += temp_gray.at<uchar>(j, i);
                             pixelsNum++;
                         }
                     }
                 }
                 meanAll = grayValueSum / (float)pixelsNum;

                double  threshold = meanIn - meanOut;//缺陷区域与周围灰度差值(整体性)
                double chazhi = black_gray.at<uchar>(y_point, x_point) - meanAll;


                //直径0.03对应实际像素直径大小3个像素点
                if (threshold > 10 || chazhi > black_lightPoint_diff)//满足灰度差值阈值就将其当作亮点
                {
                    if (max(w1 / h1, h1 / w1) >= 3.5)
                    {
                        result = 1;
                        break;
                    }
                    X_1 = x_1 - int(w)/2;
                    Y_1 = y_1 - int(h)/2;
                    X_2 = x_2 + int(w)/2;
                    Y_2 = y_2 + int(h)/2;
                    if (X_1 < 0)
                    {
                        X_1 = 0;
                    }
                    if (Y_1 < 0)
                    {
                        Y_1 = 0;
                    }
                    if (X_2 > image_black.cols - 1)
                    {
                        X_2 = image_black.cols - 1;
                    }
                    if (Y_2 > image_black.rows - 1)
                    {
                        Y_2 = image_black.rows - 1;
                    }
                    Mat PartWhite = image_white(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));//在侧光图对比
                    Mat mask1 = gray_clone(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                    double mean_out_gray = mean(PartWhite, ~mask1)[0];
                    double mean_in_gray = mean(PartWhite, mask1)[0];
                    if (mean_out_gray - mean_in_gray > 3.5)
                        result = 1;
                    else
                        result = 2;
                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                    rectangle(image_black, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
                    Point p3(x_point, y_point);
                    circle(image_black, p3, 12, Scalar(255, 255, 255), 2, 8, 0);
                    string b =" area: " + to_string(area) + "th: " + to_string(threshold);
                    putText(image_black, b, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 2, 8, 0);
                }
        }
    }

    if (result == 1)
    {
        *mresult = image_black;
        *causecolor = "上下片";
        cout << "上下片" << endl;
    }
    if (result == 2)
    {
        *mresult = image_black;
        *causecolor = "亮点";
        cout << "亮点" << endl;
    }
    return result;
}


//bool light_point7(Mat image_black,Mat *mresult,QString *causecolor)
//{
//    bool result=false;
//    Mat black_gray;
//    black_gray=image_black.clone();
//    Mat gray_clone=black_gray.clone();
//    Mat mask_black;
//    threshold(black_gray, mask_black,30, 255, CV_THRESH_BINARY);//100->60
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
//    //异形屏幕去除刘海干扰
//    if(Model_Mod_type=="R角水滴屏")
//    {
//        bitwise_and(main_mask, gray_clone, gray_clone);
//    }
//    //======================================================
//    gray_clone(Rect(gray_clone.cols - 240, gray_clone.rows - 315, 240, 315)) = uchar(0);//去除易撕贴部分域
//    gray_clone(Rect(0, gray_clone.rows - 23, 330, 23)) = uchar(255);                 //左下角
//    gray_clone(Rect(0, 0, 330, 20)) = uchar(255);                                    //左上角
//    gray_clone(Rect(gray_clone.cols-200, gray_clone.rows - 20,200, 20)) = uchar(0);  //右下角
//    gray_clone(Rect(0, 0, gray_clone.cols - 1, 2)) = uchar(0);                     //上
//    gray_clone(Rect(0, gray_clone.rows - 1, gray_clone.cols - 1, 1)) = uchar(0);   //下
//    gray_clone(Rect(0, 0, 2, gray_clone.rows - 1)) = uchar(0);                     //左
//    gray_clone(Rect(gray_clone.cols - 2, 0, 2, gray_clone.rows - 1)) = uchar(0);   //右

//    vector<vector<Point>> contours;
//    findContours( gray_clone, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
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
//            if(max(w/h,h/w)<black_lightPoint_w_h)
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
//                double mean_ceguang = mean(temp_ceguang)[0];//针对规则点状缺陷

//                Mat mask1 = black_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
//                double mean_out_gray = mean(temp_ceguang, ~mask1)[0];
//                double mean_in_gray = mean(temp_ceguang, mask1)[0];
//                double threshold = mean_in_gray- mean_out_gray ; //针对不规则圈状缺陷

//                double chazhi = black_gray.at<uchar>(y_point, x_point) - mean_ceguang;

//                //直径0.03对应实际像素直径大小3个像素点
//                if(black_gray.at<uchar>(y_point,x_point)-mean_ceguang>black_lightPoint_diff||threshold>40)
//                {
//                    result=true;
//                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
//                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
//                    rectangle(image_black , top_lef4, bottom_right4, Scalar(255,255,255), 5, 8, 0);
//                    Point p3(x_point, y_point);
//                    circle(image_black, p3, 12, Scalar(255, 255, 255), 2, 8, 0);
//                    string b = "chazhi:" + to_string(chazhi) + " area: " + to_string(area) + "th: " + to_string(threshold);
//                    putText(image_black, b, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 2, 8, 0);
//                    imwrite("D:\\image_blackresult0506.bmp",image_black);

//                }
//            }
//        }
//    }

//    if(result==true)
//    {
//        *mresult=image_black;
//        *causecolor="亮点";
//        return result;
//    }
//    return result;
//}


/*=========================================================
* 函 数 名: gamma
* 功能描述: gamma校正
=========================================================*/
Mat gamma(Mat src,double g)
{
    Mat temp;
    src.convertTo(temp, CV_32FC3, 1 / 255.0);
    cv::Mat temp1;
    cv::pow(temp,g,temp1);
    Mat dst;
    temp1.convertTo(dst, CV_8UC1, 255);
    return dst;
}

/*=========================================================
* 函 数 名: new_sample_light_line
* 功能描述: 亮线检测
=========================================================*/
bool new_sample_light_line(Mat src_black,Mat *mresult,QString *causecolor)
{  bool result=false;
    Mat g=gamma(src_black,black_light_gram);
    Mat img_gray;
    img_gray=g.clone();
    //cvtColor(g,img_gray,CV_BGR2GRAY);
    Mat th1;
    threshold(img_gray, th1, black_light_th1, 255, CV_THRESH_BINARY);//100->60
    for(int row_line=2;row_line<th1.rows-2;row_line=row_line+1)
    {
        Mat row = th1.rowRange(row_line,row_line+1).clone();
        double m = mean(row)[0];
        if(m>black_light_th2)
        {
            CvPoint top_lef2 = cvPoint(0,row_line);
            CvPoint bottom_right2 = cvPoint(100,row_line+1);
            rectangle(src_black, top_lef2, bottom_right2, Scalar(0,0,255), 5, 8, 0);
            result=true;
        }
    }
    for(int col_line=2;col_line<th1.cols-2;col_line=col_line+1)
    {
        Mat col = th1.colRange(col_line,col_line+1).clone();
        double m = mean(col)[0];
        if(m>black_light_th2)
        {
            CvPoint top_lef2 = cvPoint(col_line,0);
            CvPoint bottom_right2 = cvPoint(col_line+1,100);
            rectangle(src_black, top_lef2, bottom_right2, Scalar(0,0,255), 5, 8, 0);

            result=true;
        }
    }

    if(result==true)
    {
        imwrite("D:\\seban_information-0414.bmp", src_black);
        *mresult=src_black;
        *causecolor="亮线";
        result= true;
    }
    return result;
}

/*=========================================================
* 函 数 名: new_sample_red
* 功能描述:红屏下检测线条
=========================================================*/
bool new_sample_red(Mat src_black,Mat *mresult)
{   bool result=false;
    Mat g=gamma(src_black,0.5);
    Mat Hsv_watermark;
    Mat th1;
    cvtColor(g,Hsv_watermark, COLOR_BGR2HSV);
    inRange(Hsv_watermark, Scalar(0, 43, 10), Scalar(10, 250, 255),th1);
    for(int row_line=2;row_line<th1.rows-2;row_line=row_line+1)
    {
        Mat row = th1.rowRange(row_line,row_line+1).clone();
        double m = mean(row)[0];
        if(m>10)
        {
            // qDebug()<<m<<endl;
            CvPoint top_lef2 = cvPoint(0,row_line);
            CvPoint bottom_right2 = cvPoint(100,row_line+1);
            rectangle(src_black, top_lef2, bottom_right2, Scalar(0,0,255), 5, 8, 0);
            result= true;
        }
    }
    for(int col_line=2;col_line<th1.cols-2;col_line=col_line+1)
    {
        Mat col = th1.colRange(col_line,col_line+1).clone();
        double m = mean(col)[0];
        if(m>10)
        { // qDebug()<<m<<endl;

            CvPoint top_lef2 = cvPoint(col_line,0);
            CvPoint bottom_right2 = cvPoint(col_line+1,100);
            rectangle(src_black, top_lef2, bottom_right2, Scalar(0,0,255), 5, 8, 0);
            result= true;
        }
    }
    if(result==true)
    {
        *mresult=src_black;
        causeColor="线条";
        result= true;
    }
    //    Mat img_gray;
    //    cvtColor(g,img_gray,CV_BGR2GRAY);
    //    Mat mask_2;
    //    adaptiveThreshold( img_gray,mask_2, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 3);
    //    Mat Candidate;
    //    bitwise_and(mask_1,mask_2,Candidate);
    //    medianBlur(Candidate,Candidate,11);
    //    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(5,5));
    //    Mat  erode1;
    //    erode(Candidate, erode1, structure_element1);
    //    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(55,55));
    //    Mat dilate1;
    //    dilate(erode1, dilate1, structure_element2);
    //    Mat Candidate_2 = dilate1(Rect(40,40,dilate1.cols-80,dilate1.rows-80));
    //     vector<vector<Point>> contours;
    //     findContours( Candidate_2, contours,  CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    //     vector<Rect> boundRect(contours.size());
    //     for(vector<int>::size_type i = 0; i < contours.size(); i++)
    //         {
    //           double area = contourArea(contours[i]);
    //           if (area>10000 and area<100000)
    //           {
    //               boundRect[i] = boundingRect(Mat(contours[i]));
    //               int X_1=boundRect[i].tl().x+80;//矩形左上角X坐标值
    //               int Y_1=boundRect[i].tl().y+80;//矩形左上角Y坐标值
    //               int X_2=boundRect[i].br().x+80;//矩形右下角X坐标值
    //               int Y_2=boundRect[i].br().y+80;//矩形右下角Y坐标值
    //               CvPoint top_lef4 = cvPoint(X_1, Y_1);
    //               CvPoint bottom_right4 = cvPoint(X_2, Y_2);
    //               rectangle(src , top_lef4, bottom_right4, Scalar(0,0,255), 5, 8, 0);

    //            }
    //         }

    return result ;
}

/*=========================================================
* 函 数 名: huaping
* 功能描述: 花屏检测 缺陷 '花屏'
=========================================================*/
bool huaping(Mat image_white,Mat *mresult,QString *causecolor)//颜色检测
{
    Mat blur;
    cv::blur(image_white,blur,Size(21,21));
    Mat hsv;
    cvtColor(blur,hsv, COLOR_BGR2HSV);
    Mat R_m1;
    Mat R_sd1;
    meanStdDev(hsv, R_m1, R_sd1);
    if (R_sd1.at<double>(0,2) >Std)
    {
        *mresult=image_white;
        *causecolor="花屏";
        return  true;
    }
    return  false;
}

/*=========================================================
* 函 数 名: seban
* 功能描述: 色斑检测 有缺陷 '斑点'
=========================================================*/
bool seban(Mat image_white_src,Mat *mresult,QString *causecolor)
{
    Mat image_white=image_white_src(Rect(5,5,image_white_src.cols-10,image_white_src.rows-10));
    bool result= false;

    Mat img_gray11;
    cvtColor(image_white, img_gray11, CV_BGR2GRAY);
    Mat th11, th2;
    threshold(img_gray11, th11, 20, 255, CV_THRESH_BINARY);//100->60
    Mat blur;
    cv::blur(image_white,blur,Size(15,15));
    Mat hsv;
    cvtColor(blur,hsv, COLOR_BGR2HSV);
    //  Mat mask;
    //  inRange(hsv, Scalar(white_seban_th_h_low, white_seban_th_s_low,white_seban_th_v_low), Scalar(white_seban_th_h_up,white_seban_th_s_up,white_seban_th_v_up),mask);
    //               mask=~mask;
    vector<Mat> channels;
    split(hsv, channels);
    Mat H=channels.at(0);
    Mat S=channels.at(1);
    Mat V=channels.at(2);
    Mat gray_clone=H.clone();
    int co,ro;
    for( int row=0;row<hsv.rows;row=row+white_seban_step)//滑动窗口分块检测
    {
        for( int col=0;col<hsv.cols;col=col+white_seban_step)
        {  co=col;
            ro=row;
            if(ro+white_seban_size>hsv.rows-1)
            {
                ro=hsv.rows-white_seban_size-1;
                row=hsv.rows;
            }
            if(co+white_seban_size2>hsv.cols-1)
            {
                co=hsv.cols-white_seban_size2-1;
                col=hsv.cols;
            }
            Rect rect_wp(co,ro,white_seban_size2,white_seban_size);
            Mat temp_wp=hsv(rect_wp);

            int matMean_h = mean(H(rect_wp)).val[0];
            int matMean_s = mean(S(rect_wp)).val[0];
            int matMean_v = mean(V(rect_wp)).val[0];
            Mat mask;
            inRange(temp_wp, Scalar(matMean_h-white_seban_th_h_low, matMean_s-white_seban_th_s_low, matMean_v-white_seban_th_v_low), Scalar(matMean_h+white_seban_th_h_up,matMean_s+white_seban_th_s_up,matMean_v+white_seban_th_v_up),mask);
            mask=~mask;
            mask.copyTo(gray_clone(rect_wp));
        }
    }
    //======================================================
    //       MURA2=gray_clone;
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(5,5));
    Mat  erode1;
    erode(gray_clone, erode1, structure_element1);
    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(5,5));
    Mat dilate1;
    dilate(erode1, dilate1, structure_element2);

    //以一个较低的阈值对原图二值化,得出相与用的模板
    //    th1(Rect(0,0,80,80))=uchar(0);
    Mat structure_element3 = getStructuringElement(MORPH_RECT, Size(9, 9));
    Mat dilate2;
    erode(th11, dilate2, structure_element2);    //将模板腐蚀一下,为了去除边界影响,否则相与过后会有白边
    Mat th_result;
    bitwise_and(dilate1,dilate2,th_result);
    th_result(Rect(0,th_result.rows-251,160,250))=uchar(0);  //去除易撕贴部分
    //    th1(Rect(th1.cols-81,th1.rows-81,80,80))=uchar(0);
    //    th1(Rect(th1.cols-81,0,80,80))=uchar(0);
    th_result(Rect(0,0,th_result.cols-1,10))=uchar(0);        //去除四个边界的影响
    th_result(Rect(0,th_result.rows-11,th_result.cols-1,10))=uchar(0);
    th_result(Rect(0,0,10,th_result.rows-1))=uchar(0);
    th_result(Rect(th_result.cols-11,0,10,th_result.rows-1))=uchar(0);

    //       dilate1(Rect(0,0,200,400))=uchar(0);
    //       dilate1(Rect(0,dilate1.rows-200,200,200))=uchar(0);
    //       dilate1(Rect(dilate1.cols-200,dilate1.rows-200,200,200))=uchar(0);
    //       dilate1(Rect(dilate1.cols-200,0,200,200))=uchar(0);
    vector<vector<Point>> contours;
    findContours( th_result, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area>white_seban_area_lower and area<white_seban_area_upper )
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            float w=boundRect[i].width;
            float h=boundRect[i].height;
            int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2=boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
            //==========================================================================================
            X_1=X_1-black_mura_boder-int(w);
            Y_1=Y_1-black_mura_boder-int(h);
            X_2=X_2+black_mura_boder+int(w);
            Y_2=Y_2+black_mura_boder+int(h);
            int x_point=X_1+round(w/2);
            int y_point=Y_1+round(h/2);
            if((abs(x_point-dilate1.cols/2)<100 or abs(y_point-dilate1.rows/2)<100) and max(w/h,h/w)>3)
                continue;
            if(X_1<0)
                X_1=0;
            if(Y_1<0)
                Y_1=0;
            if(X_2>dilate1.cols-1)
                X_2=dilate1.cols-1;
            if(Y_2>dilate1.rows-1)
                Y_2=dilate1.rows-1;
            //gray_ceguang.at<uchar>(y_point,x_point)
            if((x_point<black_mura_corner and y_point<black_mura_corner) or (dilate1.cols-x_point<black_mura_corner and dilate1.rows-y_point<black_mura_corner) or(x_point<black_mura_corner and dilate1.rows-y_point<black_mura_corner) or (dilate1.cols-x_point<black_mura_corner and y_point<black_mura_corner))//(img_5.cols-x_point<5 and or img_5.rows-x_point<15 or x_point<15 or y_point<15) and(mean_ceguang)>200
                continue;
            //                          CvPoint top_lef4 = cvPoint(X_1, Y_1);
            //                          CvPoint bottom_right4 = cvPoint(X_2, Y_2);
            Mat temp_mask=dilate1(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
            int w7=(X_2-X_1)*(Y_2-Y_1);
            Mat R_m0;
            Mat R_sd0;
            meanStdDev(temp_mask, R_m0, R_sd0);
            double area7=R_m0.at<double>(0,0)*w7/255;
            Mat temp_mask2=~temp_mask;

            Mat temp_H=H(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
            Mat img_H1;
            bitwise_and(temp_H,temp_H,img_H1,temp_mask);
            Mat img_H2;
            bitwise_and(temp_H,temp_H,img_H2,temp_mask2);
            Mat R_mH1;
            Mat R_sdH1;
            meanStdDev(img_H1, R_mH1, R_sdH1);
            Mat R_mH2;
            Mat R_sdH2;
            meanStdDev(img_H2, R_mH2, R_sdH2);
            bool f=false;
            if (((  abs((R_mH1.at<double>(0,0)*w7/area7)-(R_mH2.at<double>(0,0)*w7/(w7-area7)) ) >black_mura_th_1) and (area7>black_mura_area_1) )or((   abs( (R_mH1.at<double>(0,0)*w7/area7)-(R_mH2.at<double>(0,0)*w7/(w7-area7)) )  >black_mura_th_2) and (area7>black_mura_area_2) ) )
            {
                f=true;
            }
            Mat temp_S=S(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
            Mat img_S1;
            bitwise_and(temp_S,temp_S,img_S1,temp_mask);
            Mat img_S2;
            bitwise_and(temp_S,temp_S,img_S2,temp_mask2);
            Mat R_mS1;
            Mat R_sdS1;
            meanStdDev(img_S1, R_mS1, R_sdS1);
            Mat R_mS2;
            Mat R_sdS2;
            meanStdDev(img_S2, R_mS2, R_sdS2);
            if (((   abs(  (R_mS1.at<double>(0,0)*w7/area7)-(R_mS2.at<double>(0,0)*w7/(w7-area7))  ) >black_mura_th_1) and (area7>black_mura_area_1) )or((      abs( (R_mS1.at<double>(0,0)*w7/area7)-(R_mS2.at<double>(0,0)*w7/(w7-area7)) ) >black_mura_th_2) and (area7>black_mura_area_2) ) )
            {
                f=true;
            }
            Mat temp_V=V(Rect(X_1,Y_1,X_2-X_1,Y_2-Y_1));
            Mat img_V1;
            bitwise_and(temp_V,temp_V,img_V1,temp_mask);
            Mat img_V2;
            bitwise_and(temp_V,temp_V,img_V2,temp_mask2);
            Mat R_mV1;
            Mat R_sdV1;
            meanStdDev(img_V1, R_mV1, R_sdV1);
            Mat R_mV2;
            Mat R_sdV2;
            meanStdDev(img_V2, R_mV2, R_sdV2);
            if ( ((   abs((R_mV1.at<double>(0,0)*w7/area7)-(R_mV2.at<double>(0,0)*w7/(w7-area7)))  >  black_mura_th_1) and (area7>black_mura_area_1) )or((   abs(  (R_mV1.at<double>(0,0)*w7/area7)-(R_mV2.at<double>(0,0)*w7/(w7-area7)) )  >black_mura_th_2) and (area7>black_mura_area_2) ) )
            {
                f=true;
            }
            //=====================================================

            //========================================================================================
            if(  max(w/h,h/w)<3 and f )
            {
                CvPoint top_lef4 = cvPoint(X_1, Y_1);
                CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                rectangle(image_white , top_lef4, bottom_right4, Scalar(0,0,255), 2, 8, 0);
                imwrite("D:\\seban_information-0414.bmp", image_white);
                result=true;
            }
        }
    }
    if( result==true)
    {
        *mresult=image_white;
        *causecolor="斑点";
        return result;
    }
    return result;
}

/*=========================================================
* 函 数 名: heituan
* 功能描述: 色斑检测      暂时没有用到
=========================================================*/
bool heituan(Mat image_white_src,Mat *mresult,QString *causecolor)//颜色检测
{
    Mat image_white=image_white_src(Rect(5,5,image_white_src.cols-10,image_white_src.rows-10));
    bool result= false;
    //    Mat blur;
    //    cv::blur(image_white,blur,Size(5,5));
    // blur=Gabor7(image_white);
    Mat hsv;
    cvtColor(image_white,hsv, COLOR_BGR2HSV);
    vector<Mat> channels;
    split(hsv, channels);
    //    Mat H=channels.at(0);
    //    Mat S=channels.at(1);
    Mat V=channels.at(2);
    Mat gray_clone=V.clone();
    int co,ro;
    for( int row=0;row<hsv.rows;row=row+white_seban_step)//滑动窗口分块检测
    {
        for( int col=0;col<hsv.cols;col=col+white_seban_step)
        {  co=col;
            ro=row;
            if(ro+white_seban_size>hsv.rows-1)
            {
                ro=hsv.rows-white_seban_size-1;
                row=hsv.rows;
            }
            if(co+white_seban_size2>hsv.cols-1)
            {
                co=hsv.cols-white_seban_size2-1;
                col=hsv.cols;
            }

            Rect rect_wp(co,ro,white_seban_size2,white_seban_size);
            Mat temp_wp=hsv(rect_wp);

            int matMean_v = mean(V(rect_wp)).val[0];
            Mat mask;
            inRange(temp_wp, Scalar(0, 0, matMean_v-white_seban_th_v_low), Scalar(180,255,matMean_v+white_seban_th_v_up),mask);
            mask=~mask;
            mask.copyTo(gray_clone(rect_wp));
        }
    }
    //======================================================
    //MURA2=gray_clone;
    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(5,5));
    Mat  erode1;
    erode(gray_clone, erode1, structure_element1);
    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(5,5));
    Mat dilate1;
    dilate(erode1, dilate1, structure_element2);
    vector<vector<Point>> contours;
    findContours( dilate1(Rect(5,5,dilate1.cols-5,dilate1.rows-5)), contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area>white_seban_area_lower and area<white_seban_area_upper )
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            float w=boundRect[i].width;
            float h=boundRect[i].height;
            int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2=boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
            if(max(w/h,h/w)<10)
            {
                CvPoint top_lef4 = cvPoint(X_1, Y_1);
                CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                rectangle(image_white , top_lef4, bottom_right4, Scalar(0,0,255), 2, 8, 0);
                imwrite("D:\\heituan_information-0414.bmp", image_white);
                result=true;
            }
        }
    }
    if(result==true)
    {
        *mresult=image_white;
        *causecolor="斑点";
        return result;
    }
    return result;
}

/*=========================================================
   * 函 数 名: ForeignBody
   * 功能描述: 异物缺陷检测
   * 函数输入：主相机白底图像和主相机拍摄侧光图像
   * 备注说明：2020年10月7日修改
   =========================================================*/
   bool ForeignBody(Mat white_yiwu, Mat ceguang, Mat Original, Mat *mresult, QString *causecolor)//灰度检测
   {
       bool result = false;
       //int length = 200;
       Mat gray_ceguang = ceguang.clone();
       Mat img_gray = white_yiwu.clone();
       Mat th1, th2, binaryImage;

       adaptiveThresholdCustom(img_gray, th1, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, yiwu_pre_size, yiwu_pre_th, 1);

       //Mat yanmo(th1.size(), th1.type(), Scalar(255));//全白图
       //threshold(img_gray, binaryImage, 40, 255, CV_THRESH_BINARY);							//二值化(有问题)
       //Mat left_up = binaryImage(Rect(0,0, length, length));
       //left_up.copyTo(yanmo(Rect(0,0, length, length)));
       //Mat left_down = binaryImage(Rect(0, th1.rows - length, length, th1.rows-1));
       //left_down.copyTo(yanmo(Rect(0, th1.rows - length, length, th1.rows-1)));
       //Mat right_up = binaryImage(Rect(th1.cols - length, 0, th1.cols-1, length));
       //right_up.copyTo(yanmo(Rect(th1.cols - length, 0, th1.cols-1, length)));
       //Mat right_down = binaryImage(Rect(th1.cols - length, th1.rows - length, th1.cols-1, th1.rows-1));
       //right_down.copyTo(yanmo(Rect(th1.cols - length, th1.rows - length, th1.cols-1, th1.rows-1)));

       //bitwise_and(th1, yanmo,th1);
       //针对边界位置取原图的边界
       //针对边界位置取原图的边界
   //Mat img_left = img_gray(Rect(0, 0, 20, img_gray.rows - 1));
   //Mat img_right = img_gray(Rect(img_gray.cols - 20, 0, 20, img_gray.rows - 1));
   //Mat img_tl_R = img_gray(Rect(0, 0, 150, 150));
   //Mat img_bl_R = img_gray(Rect(0, 1350, 150, 150));
   //Mat img_tr_R = img_gray(Rect(2850, 0, 150, 150));
   //Mat img_br_R = img_gray(Rect(2849, 1349, 150, 150));

   //Mat top_th, bottom_th, left_th, right_th, img_tl_R_th, img_bl_R_th, img_tr_R_th, img_br_R_th, img_right_light_th;

   ////针对边界位置设置参数
   //adaptiveThreshold(img_left, left_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, -1);
   //adaptiveThreshold(img_right, right_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
   //adaptiveThreshold(img_tl_R, img_tl_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
   //adaptiveThreshold(img_bl_R, img_bl_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
   //adaptiveThreshold(img_tr_R, img_tr_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 21, -3);
   //adaptiveThreshold(img_br_R, img_br_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 21, -3);

   ////针对边界位置深拷贝
   //left_th.copyTo(th1(Rect(0, 0, 20, th1.rows - 1)));                   //左边界
   //right_th.copyTo(th1(Rect(th1.cols - 20, 0, 20, th1.rows - 1)));      //右边界
   //img_tl_R_th.copyTo(th1(Rect(0, 0, 150, 150)));                    //上边界
   //img_bl_R_th.copyTo(th1(Rect(0, 1350, 150, 150)));     //下边界
   //img_tr_R_th.copyTo(th1(Rect(2850, 0, 150, 150)));                   //左边界
   //img_br_R_th.copyTo(th1(Rect(2849, 1349, 150, 150)));      //右边界

   //Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));//闭操作结构元素
   //morphologyEx(th1, th1, CV_MOP_CLOSE, element);   //闭运算形态学操作。可以减少噪点

       Mat th1_R;
       //做掩膜
       threshold(Original, th1_R, 25, 255, CV_THRESH_BINARY);
       //bitwise_and(th1_R, img_gray, img_gray);
       bitwise_and(th1_R, Original, Original);

//       Mat left_yamo(th1.rows, 70, th1.type(), Scalar(0));//全白图
//       left_yamo.copyTo(th1(Rect(0,0,70, th1.rows)));      //右边界
       //left_yamo.copyTo(th1(Rect(th1.cols-71, 0, th1.cols, th1.rows)));      //右边界

       vector<vector<Point>> contours;
       findContours(th1, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
       vector<Rect> boundRect(contours.size());
       vector<Rect> boundRect_area(contours.size());

       float w, h;
       int X_1, Y_1, X_2, Y_2;//矩形左上角X坐标值

       for (vector<int>::size_type i = 0; i < contours.size(); i++)
       {
           boundRect_area[i] = boundingRect(Mat(contours[i]));
           w = boundRect_area[i].width;
           h = boundRect_area[i].height;
           X_1 = boundRect_area[i].tl().x;//矩形左上角X坐标值
           Y_1 = boundRect_area[i].tl().y;//矩形左上角Y坐标值
           X_2 = boundRect_area[i].br().x;//矩形右下角X坐标值
           Y_2 = boundRect_area[i].br().y;//矩形右下角Y坐标值

           Mat Crop_Image = th1(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
           double area = countNonZero(Crop_Image);

           if (area > yiwu_area_lower && area < yiwu_area_upper/*&&i == 8*/)
           {
               double longShortRatio = max(h / w, w / h);
               if (longShortRatio < 5 && min(w, h) >= 2 && max(w, h) < 120)	//对异物最大最小直径,长宽之比做限制50
               {
                   if (area > 300)
                   {
                       if ((X_1 == 0 && Y_1 == 0) || (X_1 == 0 && Y_2 >= th1.rows-1) || (Y_1 == 0 && X_2 >= th1.cols-1) || (X_2 >= th1.cols-1 &&Y_2 >= th1.rows-1))
                           continue;
                   }
                   if (X_2 <= 4 || (th1.cols - X_1) <= 4 || Y_2 <= 4 || (th1.rows - Y_1 <= 4))
                       continue;
                   int Ceguang_Expand = 1;
                   int x_lt = X_1 - Ceguang_Expand;
                   if (x_lt < 0)
                   {
                       x_lt = 0;
                   }
                   int y_lt = Y_1 - Ceguang_Expand;
                   if (y_lt < 0)
                   {
                       y_lt = 0;
                   }
                   int x_rt = X_2 + Ceguang_Expand;
                   if (x_rt > gray_ceguang.cols - 1)
                   {
                       x_rt = gray_ceguang.cols - 1;
                       //x_rt = gray_ceguang.cols;
                   }
                   int y_rt = Y_2 + Ceguang_Expand;
                   if (y_rt > gray_ceguang.rows - 1)
                   {
                       //y_rt = gray_ceguang.rows;
                       y_rt = gray_ceguang.rows - 1;
                   }


                   //计算侧光图像缺陷中心和缺陷外围灰度差
                   Mat temp_ceguang = gray_ceguang(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                   double ceguangth_huahen, ceguangth_huichen, stddev;
                   int Qnum = 0;
                   double mean_temp = mean(temp_ceguang)[0];

                   //上下边缘时排灰尘与气泡
                   if (y_rt < 20 || (img_gray.rows - y_lt < 20))
                   {
                       Mat  col, row;
                       double m, n, p;
                       for (int col_line = 0; col_line < temp_ceguang.cols; col_line++)//列进行编列  行遍历
                       {
                           col = temp_ceguang.colRange(col_line, col_line + 1).clone();
                           m = mean(col)[0];

                           p = m - mean_temp;

                           if (p > 4)
                               Qnum++;
                       }
                   }

                   //左右边缘时排灰尘与气泡
                   if (x_rt < 20 || (img_gray.cols - x_lt < 20))
                   {
                       Mat  col, row;
                       double m, n, p;
                       for (int row_line = 0; row_line < temp_ceguang.rows; row_line++)//列进行编列  行遍历
                       {
                           row = temp_ceguang.rowRange(row_line, row_line + 1).clone();
                           m = mean(row)[0];

                           p = m - mean_temp;

                           if (p > 4)
                               Qnum++;
                       }
                   }

                   //屏幕内部时排灰尘与气泡
                   if ((y_rt >= 20 && (img_gray.rows - y_lt >= 20 && x_rt >= 20 && (img_gray.cols - x_lt >= 20))) || (Qnum > 0))
                   {
                       Mat mask = th1(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                       double mean_in = mean(temp_ceguang, mask)[0];
                       double mean_out = mean(temp_ceguang, ~mask)[0];
                       ceguangth_huahen = mean_in - mean_out;
                       ceguangth_huichen = mean_out - mean_in;
                       //侧光图上的灰度均值方差排除气泡等干扰
                       cv::Mat meanGray;
                       cv::Mat stdDev;
                       cv::meanStdDev(temp_ceguang, meanGray, stdDev);
                       stddev = stdDev.at<double>(0, 0);
                   }
                   else
                   {
                       ceguangth_huahen = 0;
                       stddev = 0;
                   }

                   //if (ceguangth <-5&&area>30)
                   //{
                      // continue;
                   //}
                   //if ((ceguangth <1.5 && stddev < 50) || (ceguangth <-5 && area>30))
                   if ((ceguangth_huahen >= 0 && ceguangth_huahen < 1.5 && area <= 50) ||(ceguangth_huahen >= 0 && ceguangth_huahen < 2.3 && area >50)|| (ceguangth_huichen >= 3 && area > 30) || (ceguangth_huichen >= 0 && ceguangth_huichen < 3))
                   {
                       int Secscreen_Expand = 20;
                       int x_lt_sec = X_1 - Secscreen_Expand;
                       if (x_lt_sec < 0)
                       {
                           x_lt_sec = 0;
                       }
                       int y_lt_sec = Y_1 - Secscreen_Expand;
                       if (y_lt_sec < 0)
                       {
                           y_lt_sec = 0;
                       }
                       int x_rt_sec = X_2 + Secscreen_Expand;
                       if (x_rt_sec > gray_ceguang.cols - 1)
                       {
                           //x_rt_sec = gray_ceguang.cols;
                           x_rt_sec = gray_ceguang.cols - 1;
                       }
                       int y_rt_sec = Y_2 + Secscreen_Expand;
                       if (y_rt_sec > gray_ceguang.rows - 1)
                       {
                           //y_rt_sec = gray_ceguang.rows;
                           y_rt_sec = gray_ceguang.rows - 1;
                       }

                       //Mat imageSuspected = img_gray(Rect(x_lt_sec, y_lt_sec, x_rt_sec - x_lt_sec, y_rt_sec - y_lt_sec));
                       Mat imageSuspected = Original(Rect(x_lt_sec, y_lt_sec, x_rt_sec - x_lt_sec, y_rt_sec - y_lt_sec));
                       Mat TempCeguang = gray_ceguang(Rect(x_lt_sec, y_lt_sec, x_rt_sec - x_lt_sec, y_rt_sec - y_lt_sec));
                       double mean_temp1 = mean(imageSuspected)[0];

                       int num_wu = 0;
                       //防止普通边缘误检——上下边缘
                       if (y_lt_sec < 20 || (img_gray.rows - y_rt_sec < 20))
                       {
                           Mat  col, row;
                           double m, n, p;
                           for (int col_line = 0; col_line < imageSuspected.cols; col_line++)//列进行编列  行遍历
                           {
                               col = imageSuspected.colRange(col_line, col_line + 1).clone();
                               m = mean(col)[0];

                               p = m - mean_temp1;

                               if (p > 4 || p < -4)
                                   num_wu++;	//真实白点区域
                           }
                       }
                       //左右边缘
                       if (x_lt_sec < 20 || (img_gray.cols - x_rt_sec < 20))
                       {
                           Mat  col, row;
                           double m, n, p;
                           for (int row_line = 0; row_line < imageSuspected.rows; row_line++)//列进行编列  行遍历
                           {
                               row = imageSuspected.rowRange(row_line, row_line + 1).clone();
                               m = mean(row)[0];

                               p = m - mean_temp1;

                               if (p > 3.3 || p < -3.3)
                                   num_wu++;	//真实白点区域
                           }
                       }


                       //if (y_rt_sec < 20 || (img_gray.rows - y_lt_sec<20))
                       //{
                          // Mat  col, row;
                          // double m, n, p, o, r;
                          // for (int col_line = 0; col_line < imageSuspected.cols - 2; col_line++)//列进行编列
                          // {
                             //  col = imageSuspected.colRange(col_line, col_line + 1).clone();
                             //  m = mean(col)[0];

                             //  col = imageSuspected.colRange(col_line + 1, col_line + 2).clone();
                             //  n = mean(col)[0];

                             //  p = abs(n - m);

                             //  if (p > 1)
                                //   num_wu++;
                          // }
                       //}

                       //if (x_rt_sec < 20 || (img_gray.cols - x_lt_sec<20))
                       //{
                          // Mat  col, row;
                          // double m, n, p, o, r;
                          // for (int col_line = 0; col_line < imageSuspected.rows - 2; col_line++)//列进行编列
                          // {
                             //  col = imageSuspected.rowRange(col_line, col_line + 1).clone();
                             //  m = mean(col)[0];

                             //  col = imageSuspected.rowRange(col_line + 1, col_line + 2).clone();
                             //  n = mean(col)[0];

                             //  p = abs(n - m);

                             //  if (p > 1)
                                //   num_wu++;
                          // }
                       //}

                       //int num_wu = 1;
                       if (num_wu > 0 || (y_lt_sec >= 20 && (img_gray.rows - y_rt_sec >= 20 && x_lt_sec >= 20 && (img_gray.cols - x_rt_sec >= 20))))
                       {
                           double Luminaceth;
                           Mat imageBinary;
                           adaptiveThreshold(imageSuspected, imageBinary, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, yiwu_sec_size, yiwu_sec_th);
                           if (x_lt < 200 || img_gray.cols - x_rt <= 200 || y_lt_sec <= 20 || img_gray.rows - y_rt <= 20)
                           {
                               //防止R角边缘误检
                               int grayValueSum = 0;
                               int pixelsNum = 0;
                               double meanIn, meanOut, meanAll;
                               Mat maskGray;
                               Mat mask1 = th1(Rect(x_lt_sec, y_lt_sec, x_rt_sec - x_lt_sec, y_rt_sec - y_lt_sec));
                               bitwise_and(imageSuspected, mask1, maskGray);
                               for (int i = 0; i < maskGray.cols; i++)
                               {
                                   for (int j = 0; j < maskGray.rows; j++)
                                   {
                                       if (maskGray.at<uchar>(j, i) > 0)
                                       {
                                           grayValueSum += maskGray.at<uchar>(j, i);
                                           pixelsNum++;
                                       }
                                   }
                               }
                               meanIn = grayValueSum / (float)pixelsNum;

                               grayValueSum = 0;
                               pixelsNum = 0;
                               bitwise_and(imageSuspected, ~mask1, maskGray);
                               for (int i = 0; i < maskGray.cols; i++)
                               {
                                   for (int j = 0; j < maskGray.rows; j++)
                                   {
                                       if (maskGray.at<uchar>(j, i) > 0)
                                       {
                                           grayValueSum += maskGray.at<uchar>(j, i);
                                           pixelsNum++;
                                       }
                                   }
                               }
                               meanOut = grayValueSum / (float)pixelsNum;

                               grayValueSum = 0;
                               pixelsNum = 0;
                               for (int i = 0; i < imageSuspected.cols; i++)
                               {
                                   for (int j = 0; j < imageSuspected.rows; j++)
                                   {
                                       if (imageSuspected.at<uchar>(j, i) > 0)
                                       {
                                           grayValueSum += imageSuspected.at<uchar>(j, i);
                                           pixelsNum++;
                                       }
                                   }
                               }
                               meanAll = grayValueSum / (float)pixelsNum;

                               Luminaceth = meanOut - meanIn;
                           }
                           //double  defect_areath = meanIn - meanOut;//缺陷区域与周围灰度差值(整体性)
                           else
                           {
                               //异物缺陷亮度阈值
                               Mat tt;
                               Mat mask1 = th1(Rect(x_lt_sec, y_lt_sec, x_rt_sec - x_lt_sec, y_rt_sec - y_lt_sec));
                               double meanGrayin_Suspect = mean(imageSuspected, mask1)[0];
                               bitwise_and(imageSuspected, mask1, tt);
                               double meanGrayout_Suspect = mean(imageSuspected, ~mask1)[0];
                               bitwise_and(imageSuspected, ~mask1, tt);
                               Luminaceth = meanGrayout_Suspect - meanGrayin_Suspect;
                           }

                           if (area < 25 && Luminaceth < 9 && ceguangth_huichen > 1.5)		//如果缺陷本身面积与灰度差都很大，不用排侧光黑色区域，否则进行侧光黑色区域排除
                               continue;

                           vector<vector<Point>> contours7;
                           vector<vector<Point>> contours8;
                           findContours(imageBinary, contours7, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
                           vector<Rect> BoundRect_Area_Sec(contours7.size());

                           Mat tempBinary;
                           if (imageBinary.rows > 4 && imageBinary.cols > 4)
                               tempBinary = imageBinary(Rect(2, 2, imageBinary.cols - 4, imageBinary.rows - 4));
                           else
                               tempBinary = imageBinary;
                           findContours(tempBinary, contours8, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);

                           if (area <= 300 && (contours7.size() < 15 || contours8.size() < 15) || area <= 500 && area > 300 && (contours7.size() < 20 || contours8.size() < 20) || area > 500 && (contours7.size() < 25 || contours8.size() < 25))//彩色相机网格效应对此处有影响
                           {
                               for (vector<int>::size_type i7 = 0; i7 < contours7.size(); i7++)
                               {
                                   BoundRect_Area_Sec[i7] = boundingRect(Mat(contours7[i7]));
                                   int w_last = BoundRect_Area_Sec[i7].width;
                                   int h_last = BoundRect_Area_Sec[i7].height;
                                   int X_1_last = BoundRect_Area_Sec[i7].tl().x;//矩形左上角X坐标值
                                   int Y_1_last = BoundRect_Area_Sec[i7].tl().y;//矩形左上角Y坐标值
                                   int X_2_last = BoundRect_Area_Sec[i7].br().x;//矩形右下角X坐标值
                                   int Y_2_last = BoundRect_Area_Sec[i7].br().y;//矩形右下角Y坐标值

                                   Mat Crop_Image_last = imageBinary(Rect(X_1_last, Y_1_last, X_2_last - X_1_last, Y_2_last - Y_1_last));

                                   //Mat TempImage = imageBinary.clone();
                                   //for (int i = 0; i < TempImage.cols; i++)//cols是列，rows是行
                                   //{
                                   //	for (int j = 0; j < TempImage.rows; j++)
                                   //	{
                                   //		if (i < X_2_last && i >= X_1_last && j < Y_2_last && j >= Y_1_last)
                                   //		{
                                   //			TempImage.at<uchar>(j, i) = imageBinary.at<uchar>(j, i);
                                   //		}
                                   //		else {
                                   //			TempImage.at<uchar>(j, i) = 0;
                                   //		}
                                   //	}
                                   //}
                                   int Secscreen_Expand = 1;
                                   int X_1_end = X_1_last - Secscreen_Expand;
                                   if (X_1_end < 0)
                                   {
                                       X_1_end = 0;
                                   }
                                   int Y_1_end = Y_1_last - Secscreen_Expand;
                                   if (Y_1_end < 0)
                                   {
                                       Y_1_end = 0;
                                   }
                                   int X_2_end = X_2_last + Secscreen_Expand;
                                   if (X_2_end > imageBinary.cols - 1)
                                   {
                                       //x_rt_sec = gray_ceguang.cols;
                                       X_2_end = imageBinary.cols - 1;
                                   }
                                   int Y_2_end = Y_2_last + Secscreen_Expand;
                                   if (Y_2_end > imageBinary.rows - 1)
                                   {
                                       //y_rt_sec = gray_ceguang.rows;
                                       Y_2_end = imageBinary.rows - 1;
                                   }
                                   Mat TempCeguang0 = TempCeguang(Rect(X_1_end, Y_1_end, X_2_end - X_1_end, Y_2_end - Y_1_end));
                                   Mat TempImage0 = imageBinary(Rect(X_1_end, Y_1_end, X_2_end - X_1_end, Y_2_end - Y_1_end));
//                                   Mat TempCeguang1;
//                                   bitwise_and(TempCeguang0, TempImage0, TempCeguang1);
                                   double ceguang1_In = mean(TempCeguang0, TempImage0)[0];
//                                   bitwise_and(TempCeguang0, ~TempImage0, TempCeguang1);
                                   double ceguang1_Out = mean(TempCeguang0, ~TempImage0)[0];
                                   double differ = ceguang1_Out - ceguang1_In;
                                   if (area < 130 && area >= 50 && differ <= 6.6 || area < 50 && area >= 10 && differ < 3.1 || area > 0 && area < 10 && differ < 1.8 || area >= 130 || Luminaceth > 11.2 && area > 80)
                                   {
                                       //double area7 = countNonZero(Crop_Image_last);

                                       //double meanGrayin_Suspect = mean(imageSuspected, TempImage)[0];
                                       //double meanGrayout_Suspect = mean(imageSuspected, ~TempImage)[0];
                                       //Luminaceth = meanGrayout_Suspect - meanGrayin_Suspect;

                                       //double area7 = contourArea(contours7[i7]);
                                       if (area > 25 && area < yiwu_sec_area_upper && Luminaceth >= 5.9 || area >15 && area <= 25 & Luminaceth >= 7.5 ||area <= 15 && area >= 12 && Luminaceth >= 10 || area < 12 && area >= yiwu_sec_area_lower && Luminaceth >= 13)
                                           //if (area7 > 25 && area7 < yiwu_sec_area_upper && Luminaceth >= yiwu_lighth_BigArea || area7 <= 25 && area7 >= yiwu_sec_area_lower && Luminaceth >= yiwu_lighth_SmallArea)
                                           //if (area7 >= yiwu_sec_area_lower && area7 < yiwu_sec_area_upper && Luminaceth >= yiwu_lighth_BigArea)
                                       {
                                           result = true;

                                           CvPoint top_lef44 = cvPoint(X_1, Y_1);
                                           CvPoint bottom_right44 = cvPoint(X_2, Y_2);
                                           rectangle(white_yiwu, top_lef44, bottom_right44, Scalar(0, 255, 0), 1, 8, 0);

                                           int x_lt_high = top_lef44.x - 50;
                                           if (x_lt_high < 0)
                                           {
                                               x_lt_high = 0;
                                           }
                                           int y_lt_high = top_lef44.y - 50;
                                           if (y_lt_high < 0)
                                           {
                                               y_lt_high = 0;
                                           }
                                           int x_rt_high = bottom_right44.x + 50;
                                           if (x_rt_high > white_yiwu.cols - 1)
                                           {
                                               x_rt_high = white_yiwu.cols;
                                           }
                                           int y_rt_high = bottom_right44.y + 50;
                                           if (y_rt_high > white_yiwu.rows - 1)
                                           {
                                               y_rt_high = white_yiwu.rows;
                                           }

                                           CvPoint top_lef4 = cvPoint(x_lt_high, y_lt_high);
                                           CvPoint bottom_right4 = cvPoint(x_rt_high, y_rt_high);

                                           rectangle(white_yiwu, top_lef4, bottom_right4, Scalar(0, 255, 0), 5, 8, 0);

                                           string information = "th:" + to_string(Luminaceth) + "area " + to_string(area);	//信息数据查看
                                           putText(white_yiwu, information, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 0), 2, 8, 0);
                                       }
                                   }
                               }
                           }

                       }

                   }
               }
           }
       }

       if (result == true)
       {
           *causecolor = "背光异物";
           *mresult = white_yiwu;
       }
       return result;
   }



  /*=========================================================
     *@函 数 名: adaptiveThresholdCustom
     *@功能描述: 自适应阈值分割实现图像二值化
     *@param src          输入灰度图像
     *@param dst          输出二值图像
     *@param maxValue     输入满足阈值条件时像素取值
     *@param method       计算局部均值方法
     *@param type         输入阈值判断类型
     *@param blockSize    卷积窗口大小(奇数)
     *@param delta        输入偏移常量
     *@param ratio        输入均值比例系数
     *@备注说明：
     =========================================================*/
     void adaptiveThresholdCustom(const cv::Mat &src, cv::Mat &dst, double maxValue, int method, int type, int blockSize, double delta, double ratio)
     {
         CV_Assert(src.type() == CV_8UC1);               // 原图必须是单通道无符号8位,CV_Assert（）若括号中的表达式值为false，则返回一个错误信息
         CV_Assert(blockSize % 2 == 1 && blockSize > 1);	// 块大小必须大于1，并且是奇数
         CV_Assert(maxValue > 0);                        //二值图像最大值
         CV_Assert(ratio > DBL_EPSILON);	                //输入均值比例系数
         Size size = src.size();							//源图像的尺寸
         Mat _dst(size, src.type());						//目标图像的尺寸
         Mat mean;	                                    //存放均值图像
         if (src.data != _dst.data)
             mean = _dst;


         int top = (blockSize - 1)*0.5;     //填充的上边界行数
         int bottom = (blockSize - 1)*0.5;  //填充的下边界行数
         int left = (blockSize - 1)*0.5;	   //填充的左边界行数
         int right = (blockSize - 1)*0.5;   //填充的右边界行数
         int border_type = BORDER_CONSTANT; //边界填充方式
         Mat src_Expand;	                   //对原图像进行边界扩充

         Mat topImage = src(Rect(0, 0, src.cols, 1));//上边界一行图像

         cv::Scalar color = cv::mean(topImage)*0.5;//35-80之间均可以  该值需要确定

         //Scalar color = Scalar(50);//35-80之间均可以
         copyMakeBorder(src, src_Expand, top, bottom, left, right, border_type, color);

         if (method == ADAPTIVE_THRESH_MEAN_C)
         {
             /*
             @param src 单通道灰度图
             @param dst 单通道处理后的图
             @param int类型的ddepth，输出图像的深度
             @param Size类型的ksize，内核的大小
             @param Point类型的anchor，表示锚点
             @param bool类型的normaliz,即是否归一化
             @param borderType 图像外部像素的某种边界模式
             */
             boxFilter(src_Expand, mean, src.type(), Size(blockSize, blockSize), Point(-1, -1), true, BORDER_CONSTANT);
         }
         else if (method == ADAPTIVE_THRESH_GAUSSIAN_C)
         {
             GaussianBlur(src, mean, Size(blockSize, blockSize), 0, 0, BORDER_DEFAULT);
         }
         else
             CV_Error(CV_StsBadFlag, "Unknown/unsupported adaptive threshold method");

         mean = mean(cv::Rect(top, top, src_Expand.cols - top * 2, src_Expand.rows - top * 2)); //删除扩充的图像边界

         int i, j;
         uchar imaxval = saturate_cast<uchar>(maxValue);	                       //将maxValue由double类型转换为uchar型
         int idelta = type == THRESH_BINARY ? cvCeil(delta) : cvFloor(delta);   //将idelta由double类型转换为int型
         if (src.isContinuous() && mean.isContinuous() && _dst.isContinuous())
         {
             size.width *= size.height;
             size.height = 1;
         }
         for (i = 0; i < size.height; i++)
         {
             const uchar* sdata = src.data + src.step * i;		   //指向源图像
             const uchar* mdata = mean.data + mean.step * i;		   //指向均值图
             uchar* ddata = _dst.data + _dst.step * i;	           //指向输出图
             for (j = 0; j < size.width; j++)
             {
                 double Thresh = mdata[j] * ratio - idelta;	        //阈值
                 if (CV_THRESH_BINARY == type)	                    //S>T时为imaxval
                 {
                     ddata[j] = sdata[j] > Thresh ? imaxval : 0;
                 }
                 else if (CV_THRESH_BINARY_INV == type)	            //S<T时为imaxval
                 {
                     ddata[j] = sdata[j] > Thresh ? 0 : imaxval;
                 }
                 else
                     CV_Error(CV_StsBadFlag, "Unknown/unsupported threshold type");
             }
         }
         dst = _dst.clone();
     }


  /*=========================================================
 * 函 数 名: Stripe
 * 功能描述: 条纹检测
 * 输入：    主相机灰底图像
 =========================================================*/
bool Stripe(Mat white, Mat *mresult, QString *causecolor)
   {
       bool result = false;
       Mat img_gray = white.clone();

       Mat col;

       int num = 0; //条纹出现次数
       int Num_Stripe = 0;
       //int col_line_last = 0;
       //double m, n, p;
       double m, n, p, o, r;    //二阶差分中间量
       double Diff_Thre = 0.6;    //二阶差分判断阈值

       for (int col_line = 100; col_line < img_gray.cols - 220; col_line++)//列进行编列**排除边界处突亮影响
       {
           col = img_gray.colRange(col_line, col_line + 5).clone();
           m = mean(col)[0];

           col = img_gray.colRange(col_line + 5, col_line + 10).clone();
           n = mean(col)[0];

           p = m - n;   //二阶差分减数

           col = img_gray.colRange(col_line + 10, col_line + 15).clone();
           m = mean(col)[0];

           col = img_gray.colRange(col_line + 15, col_line + 20).clone();
           n = mean(col)[0];

           o = m - n;   //二阶差分被减数

           r = o - p;   //改变二阶差分计算方式

           if (r < 0)
               r = 0;
           cout << r << endl;
           if (r > Diff_Thre)
           {
               num++;
           }
           if (r < Diff_Thre)
           {
               if (num <30 && num >5)  //尖峰的宽度特征
               {
                   Num_Stripe++;
                   CvPoint top_lef4 = cvPoint(col_line + 8, 0);
                   CvPoint bottom_right4 = cvPoint(col_line + 13, 1775);
                   rectangle(white, top_lef4, bottom_right4, Scalar(255, 255, 255), 1, 8, 0);
                   imwrite("D:\\Stripe-information.bmp", white);
               }
               num = 0;
           }


       }

       if (Num_Stripe > 1)
       {
           result = true;
       }
       if (result == true)
       {
           *mresult = white;
           *causecolor = "条纹";
       }
       return result;

   }

/*=========================================================
* 函 数 名: lack_line
* 功能描述: 少线缺陷判断
* 函数输入：主相机白底图像和主相机拍摄侧光图像
* 备注说明：2020年7月15日修改
=========================================================*/
bool lack_line(Mat white_yiwu, Mat ceguang, Mat *mresult, QString *causecolor)
{
    bool result = false;
    Mat img_gray = white_yiwu.clone();
    Mat gray_ceguang = ceguang.clone();
    Mat imageBinary;//分割二值图
    adaptiveThreshold(img_gray, imageBinary, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, lackline_bolckSize, lackline_delta);

    imageBinary(Rect(imageBinary.cols - 240, imageBinary.rows - 315, 240, 315)) = uchar(0);//去除易撕贴部分
    imageBinary(Rect(0, 0, imageBinary.cols - 1, 10)) = uchar(0);                          //去除四个边界的影响上边
    imageBinary(Rect(0, imageBinary.rows - 12, imageBinary.cols - 1, 12)) = uchar(0);      //下边
    imageBinary(Rect(0, 0, 40, imageBinary.rows - 1)) = uchar(0);                          //左边有误分割情况15
    imageBinary(Rect(imageBinary.cols - 11, 0, 11, imageBinary.rows - 1)) = uchar(0);      //右边

    vector<vector<Point>> contours_lackline;
    findContours(imageBinary, contours_lackline, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect_lackLine(contours_lackline.size());
    {
        for (vector<int>::size_type i = 0; i < contours_lackline.size(); i++)
        {
            double area = contourArea(contours_lackline[i]);
            if (area > 30 && area < 50000)
            {
                boundRect_lackLine[i] = boundingRect(Mat(contours_lackline[i]));
                float w = boundRect_lackLine[i].width;
                float h = boundRect_lackLine[i].height;
                int X_1 = boundRect_lackLine[i].tl().x;//矩形左上角X坐标值
                int Y_1 = boundRect_lackLine[i].tl().y;//矩形左上角Y坐标值
                int X_2 = boundRect_lackLine[i].br().x;//矩形右下角X坐标值
                int Y_2 = boundRect_lackLine[i].br().y;//矩形右下角Y坐标值
                Moments m = moments(contours_lackline[i]);
                int x_point = int(m.m10 / m.m00);
                int y_point = int(m.m01 / m.m00);
                double longShortRatio = max(h / w, w / h);
                if (longShortRatio > longShortlowerth && min(w, h) < shortHigherth && max(w, h) > longLowerth)//长宽比，长度，宽度限制
                {
                    int border = 3;//选定框边界宽度
                    int x_lt = X_1 - border;
                    //越界保护
                    if (x_lt < 0)
                    {
                        x_lt = 0;
                    }
                    int y_lt = Y_1 - border;
                    if (y_lt < 0)
                    {
                        y_lt = 0;
                    }
                    int x_rt = X_2 + border;
                    if (x_rt > img_gray.size[1] - 1)
                    {
                        x_rt = img_gray.size[1] - 1;
                    }
                    int y_rt = Y_2 + border;
                    if (y_rt > img_gray.size[0] - 1)
                    {
                        y_rt = img_gray.size[0] - 1;
                    }
                    //侧光图排除明显划痕
                    Mat sidelightSuspect = gray_ceguang(Rect(x_lt+1, y_lt+1, x_rt - x_lt-2, y_rt - y_lt-2));//侧光图像疑似贴膜划痕图像
                    Mat mask = imageBinary(Rect(x_lt+1, y_lt+1, x_rt - x_lt-2, y_rt - y_lt-2));             //侧光图像疑似贴膜划痕掩膜
                    double meanGrayin_Suspect = mean(sidelightSuspect, mask)[0];                            //缺陷中心灰度均值
                    double meanGrayout_Suspect = mean(sidelightSuspect, ~mask)[0];                          //缺陷外围灰度均值
                    double removeScratch = abs(meanGrayin_Suspect - meanGrayout_Suspect);                   //排除侧光图贴膜划痕的参数
                    if (removeScratch < lackscratchth)
                    {
                        result = true;
                        CvPoint top_lef4 = cvPoint(x_lt, y_lt);
                        CvPoint bottom_right4 = cvPoint(x_rt, y_rt);
                        rectangle(img_gray, top_lef4, bottom_right4, Scalar(0, 255, 0), 5, 8, 0);
                        string information = "th:" + to_string(removeScratch) + " area " + to_string(area) + " width:" + to_string(min(w, h)) + " length:" + to_string(max(w, h));
                        putText(img_gray, information, Point(40, 20), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 0), 2, 8, 0);
                        imwrite("D:\\lackline-information.bmp", white_yiwu);
                    }
                }
            }
        }
    }
    if (result == true)
    {
        *mresult = img_gray;
        *causecolor = "少线";
        result = true;
    }
    return result;
}

/*=========================================================
 * 函 数 名: Mura
 * 功能描述: 白色底下间检测圆形白点状缺陷,白点缺陷在主黑白相机中不可见
 * 输入: 左右侧相机拍摄白点图片和对应侧光图片，侧光图片排除划痕和气泡
 * 输出：白底下检测结果图和result
 * 修改时间：2021年1月12日
 * 其他：
 =========================================================*/
bool Mura_Decter(Mat imageGray, Mat *mresult, QString *causecolor)
{
       bool result = false;

       int length = 80;
       Mat th_result;
       Mat img_gray = imageGray.clone();
       adaptiveThreshold(img_gray, th_result, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 91, -3);

       int shuidi_length = 160;
       int part = img_gray.rows / 2 - shuidi_length;
       //针对边界位置取原图的边界
       Mat img_top = img_gray(Rect(0, 0, img_gray.cols - 1, length));
       Mat img_bottom = img_gray(Rect(0, img_gray.rows - length, img_gray.cols - 1, length));
       Mat img_left = img_gray(Rect(0, 0, length, img_gray.rows - 1));
       Mat img_right = img_gray(Rect(img_gray.cols - length, 0, length, img_gray.rows - 1));
       Mat img_right_light = img_gray(Rect(img_gray.cols - 150, part, 150, shuidi_length * 2));
       Mat img_tl_R = img_gray(Rect(0, 0, 150, 150));
       Mat img_bl_R = img_gray(Rect(0, 1350, 150, 150));
       Mat img_tr_R = img_gray(Rect(2850, 0, 150, 150));
       Mat img_br_R = img_gray(Rect(2849, 1349, 150, 150));

       Mat top_th, bottom_th, left_th, right_th, img_tl_R_th, img_bl_R_th, img_tr_R_th, img_br_R_th, img_right_light_th;

       //针对边界位置设置参数
       adaptiveThreshold(img_top, top_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, -1);
       adaptiveThreshold(img_bottom, bottom_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, -1);
       //adaptiveThreshold(img_left, left_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, -1);
       adaptiveThreshold(img_left, left_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
       adaptiveThreshold(img_right, right_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
       adaptiveThreshold(img_right_light, img_right_light_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 9, -3);
       adaptiveThreshold(img_tl_R, img_tl_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
       adaptiveThreshold(img_bl_R, img_bl_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
       adaptiveThreshold(img_tr_R, img_tr_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 21, -3);
       adaptiveThreshold(img_br_R, img_br_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 21, -3);

       //针对边界位置深拷贝
       top_th.copyTo(th_result(Rect(0, 0, th_result.cols - 1, length)));                    //上边界
       bottom_th.copyTo(th_result(Rect(0, th_result.rows - length, th_result.cols - 1, length)));     //下边界
       left_th.copyTo(th_result(Rect(0, 0, length, th_result.rows - 1)));                   //左边界
       right_th.copyTo(th_result(Rect(th_result.cols - length, 0, length, th_result.rows - 1)));      //右边界
       img_right_light_th.copyTo(th_result(Rect(img_gray.cols - 150, part, 150, shuidi_length * 2)));      //右边界
       img_tl_R_th.copyTo(th_result(Rect(0, 0, 150, 150)));                    //上边界
       img_bl_R_th.copyTo(th_result(Rect(0, 1350, 150, 150)));     //下边界
       img_tr_R_th.copyTo(th_result(Rect(2850, 0, 150, 150)));                   //左边界
       img_br_R_th.copyTo(th_result(Rect(2849, 1349, 150, 150)));      //右边界

       th_result(Rect(0, 0, 15, th_result.rows)) = uchar(0);            //屏蔽右侧15行，防止灯口误检白点
       th_result(Rect(th_result.cols - 10, 0, 10, th_result.rows)) = uchar(0);            //屏蔽左侧10行，防止头部亮边误检为白点

       Mat th1;
       //做掩膜
       threshold(img_gray, th1, 25, 255, CV_THRESH_BINARY);
       bitwise_and(th1, img_gray, img_gray);
       //闭运算,弥合内部空洞,连接相距很近的区域
       Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));//闭操作结构元素
       morphologyEx(th_result, th_result, CV_MOP_CLOSE, element);   //闭运算形态学操作。可以减少噪点

       vector<vector<Point>> contours;
       findContours(th_result, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
       std::sort(contours.begin(), contours.end(), compareContourAreas);
       vector<Rect> boundRect(contours.size());

       for (vector<int>::size_type i = 0; i < contours.size(); i++)
       {
           Mat temp_mask = Mat::zeros(th_result.rows, th_result.cols, CV_8UC1);
           boundRect[i] = boundingRect(Mat(contours[i]));
           drawContours(temp_mask, contours, i, 255, FILLED, 8);
           int w = boundRect[i].width;
           int h = boundRect[i].height;
           int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
           int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
           int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
           int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
           Mat Crop_Image_last = th_result(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
           double area = countNonZero(Crop_Image_last);
           //int x_point, y_point;

           if (area >= 100 && area < 60000)
           {
               RotatedRect rect = minAreaRect(contours[i]);
               double mw = rect.size.height;
               double mh = rect.size.width;
               double radio = max(mw / mh, mh / mw);

               //长宽比排除
               if (radio > whitePoint_w_h)
               {
                   continue;
               }

               int border = 30;
               int x_lt = X_1 - border;
               if (x_lt < 0)
               {
                   x_lt = 0;
               }
               int y_lt = Y_1 - border;
               if (y_lt < 0)
               {
                   y_lt = 0;
               }
               int x_rt = X_2 + border;
               if (x_rt > img_gray.size[1] - 1)
               {
                   x_rt = img_gray.size[1] - 1;
               }
               int y_rt = Y_2 + border;
               if (y_rt > img_gray.size[0] - 1)
               {
                   y_rt = img_gray.size[0] - 1;
               }

               Mat tempImage_Binary;
               Mat tempGray = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt)).clone();
               Mat tempBinary = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt)).clone();
               Mat tempBinary0 = th_result(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1)).clone();

               double Width_Average = 0.0;
               int row_line;
               for (row_line = 0; row_line < tempBinary0.rows; row_line++)//列进行编列  行遍历
               {
                   Mat row;
                   row = tempBinary0.rowRange(row_line, row_line + 1).clone();
                   double num = countNonZero(row);
                   Width_Average = Width_Average + num;
               }
               Width_Average = Width_Average / row_line;
               if (Width_Average < tempBinary0.cols / 4)
                   continue;

               int col_line;
               Width_Average = 0.0;
               for (col_line = 0; col_line < tempBinary0.cols; col_line++)//列进行编列  行遍历
               {
                   Mat col;
                   col = tempBinary0.colRange(col_line, col_line + 1).clone();
                   double num = countNonZero(col);
                   Width_Average = Width_Average + num;
               }
               Width_Average = Width_Average / row_line;
               if (Width_Average < tempBinary0.rows / 4)
                   continue;

               double mean_In = mean(tempGray, tempBinary)[0];
               threshold(tempGray, tempImage_Binary, 30, 255, CV_THRESH_BINARY);
               double mean_All = mean(tempGray, tempImage_Binary)[0];
               threshold(tempGray, tempImage_Binary, mean_All - 20, 255, CV_THRESH_BINARY);
               bitwise_and(tempImage_Binary, ~tempBinary, tempBinary);
               double mean_Out = mean(tempGray, tempBinary)[0];
               double differ = mean_In - mean_Out;
               if (differ >= 6.4)
               {
                   result = true;
                   CvPoint top_lef4 = cvPoint(X_1, Y_1);
                   CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                   rectangle(imageGray, top_lef4, bottom_right4, Scalar(0, 0, 0), 5, 8, 0);
               }
           }
       }
       if (result == true)
       {
           *causecolor = "白印";
           *mresult = imageGray;
       }
       return result;
}

/*=========================================================
* 函 数 名:WhiteDot_BackSide后相机白点检测
* 功能描述: 白点 有缺陷 '白点'
* 输入:侧面相机拍摄白点缺陷
* 3月12日修改白点缺陷检测程序
=========================================================*/
bool WhiteDot_BackSide(Mat white_yiwu, Mat ceguang, Mat *mresult, QString *causecolor)//灰度检测  Mat white_middle
{
    bool result = false;
    Mat img_gray = white_yiwu.clone();
    Mat img_ceguang = ceguang.clone();
    //cvtColor(white_yiwu, img_gray, CV_BGR2GRAY); //img_gray左相机白底图像
    Mat th1, th2;
    adaptiveThreshold(img_gray, th1, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
    threshold(img_gray, th2, 30, 255, CV_THRESH_BINARY);   //以一个较低的阈值对原图二值化,得出相与用的模板
    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(9, 9));
    Mat dilate2;
    erode(th2, dilate2, structure_element2);    //将模板腐蚀一下,为了去除边界影响,否则相与过后会有白边
    Mat th_result;
    bitwise_and(th1, dilate2, th_result);

    th_result(Rect(th_result.cols - 261, th_result.rows - 351, 260, 350)) = uchar(0);//易撕贴部分设置右下角
    th_result(Rect(0, 0, 200, th_result.rows - 1)) = uchar(0);          // 左边有误分割情况15
    //    th_result(Rect(0, 0, th_result.cols - 1, 10)) = uchar(0);          //去除四个边界的影响上边
    //    th_result(Rect(0, th_result.rows - 24, th_result.cols - 1, 24)) = uchar(0);//下边
    //    th_result(Rect(0, 0, 40, th_result.rows - 1)) = uchar(0);          // 左边有误分割情况15
    //    th_result(Rect(th_result.cols - 11, 0, 11, th_result.rows - 1)) = uchar(0);//右

    vector<vector<Point>> contours;
    findContours(th_result, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 18 && area < 165)//修改主相机面积上下限120
        {

            boundRect[i] = boundingRect(Mat(contours[i]));
            float w = boundRect[i].width;
            float h = boundRect[i].height;
            float changkuanbi = w / h;
            if (changkuanbi < 1)
            {
                changkuanbi = 1 / changkuanbi;
            }
            int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
            Moments m = moments(contours[i]);   //查找轮廓的重心
            int x_point = int(m.m10 / m.m00);
            int y_point = int(m.m01 / m.m00);
            if (true)
            {
                int border = 10;
                int x_lt = X_1 - border;
                if (x_lt < 0)
                {
                    x_lt = 0;
                }
                int y_lt = Y_1 - border;
                if (y_lt < 0)
                {
                    y_lt = 0;
                }
                int x_rt = X_2 + border;
                if (x_rt > img_gray.size[1] - 1)
                {
                    x_rt = img_gray.size[1] - 1;
                }
                int y_rt = Y_2 + border;
                if (y_rt > img_gray.size[0] - 1)
                {
                    y_rt = img_gray.size[0] - 1;
                }
                //颜色深浅判断
                Mat temp_gray = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                Mat mask = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                double mean_in = mean(temp_gray, mask)[0];
                double mean_out = mean(temp_gray, ~mask)[0];
                double  White_Huidu_Th = mean_in - mean_out;
                //排除划痕的干扰
                Mat temp_ceguang = img_ceguang(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                Mat mask_ceguang = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                double mean_in_ceguang = mean(temp_ceguang, mask_ceguang)[0];
                double mean_out_ceguang = mean(temp_ceguang, ~mask_ceguang)[0];
                double ceguang_th = mean_in_ceguang - mean_out_ceguang;

                string ceghaung = "cegaung:" + to_string(ceguang_th)+"th:"+to_string(White_Huidu_Th);
                putText(white_yiwu, ceghaung, Point(x_rt + 10, y_rt + 20), FONT_HERSHEY_SIMPLEX, 0.9, Scalar(0, 0, 0), 1, 8, 0);


                CvPoint top_lef = cvPoint(x_lt, y_lt);
                CvPoint bottom_right = cvPoint(x_rt, y_rt);
                rectangle(img_ceguang, top_lef, bottom_right, Scalar(255, 255, 255), 2, 8, 0);
                //排除划痕划痕跳过
                if (abs(ceguang_th) > 10)
                {
                    continue;
                }

                if (White_Huidu_Th >= 8.5 && ((X_2 - X_1)*(Y_2 - Y_1) > 10) && (max(w / h, h / w) < 2))//长宽比一般在1.8之内
                    // 7.85 6.8 4.6  if (x_data>0 && MSE >= 150 && ((X_2 - X_1)*(Y_2 - Y_1) > 10) && (max(w / h, h / w) < 5))
                {
                    Mat temp_gray = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                    Mat mask = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                    Mat image_highlight;
                    bitwise_and(temp_gray, mask, image_highlight);//提取高亮区域
                    double minStrength, maxStrength;
                    cv::minMaxLoc(image_highlight, &minStrength, &maxStrength);//查找此处的最亮点处灰度值
                    double x_data = maxStrength - mean_out;                    //用最亮点减去周围平均得到高度差
                    double MSE = pow(x_data, 2);
                    string c = "MSE:" + to_string(MSE) + "Th " + to_string(White_Huidu_Th);
                    //putText(white_yiwu, c, Point(x_point, y_point), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1, 8, 0);
                    if (MSE >= 46)
                    {
                        CvPoint top_lef4 = cvPoint(X_1 - 10, Y_1 - 10);
                        CvPoint bottom_right4 = cvPoint(X_2 + 20, Y_2 + 20);
                        rectangle(white_yiwu, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
                        result = true;
                    }
                }

            }
        }
    }
    if (result == true)
    {
        *causecolor = "白点";
        *mresult = white_yiwu;
    }
    return result;
}

/*=========================================================
* 函 数 名: WhiteDotLeft
* 功能描述: 白色底下间检测圆形白点状缺陷,白点缺陷在主黑白相机中不可见
* 输入: 左右侧相机拍摄白点图片和对应侧光图片，侧光图片排除划痕和气泡
* 输出：白底下检测结果图和result
* 修改时间：2020年12月15日
* 其他：
=========================================================*/
   bool WhiteDotLeft(Mat white_yiwu, Mat ceguang, Mat Original, Mat *mresult, QString *causecolor)//灰度检测  Mat white_middle
   {
       bool result = false;
       Mat img_gray = white_yiwu.clone();
       Mat img_ceguang = ceguang.clone();
       medianBlur(img_gray, img_gray, 3); //中值滤波滤除椒盐噪声,缺点耗时26毫秒 奇数半径越大效果越强
       Mat th_result;
       //adaptiveThresholdCustom_whitedot(img_gray, th_result, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, whitePoint_step, -3, 1);
       adaptiveThreshold(img_gray, th_result, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, whitePoint_step, -3);

       //针对边界位置取原图的边界
       Mat img_top = img_gray(Rect(0, 0, img_gray.cols - 1, 40));
       Mat img_bottom = img_gray(Rect(0, img_gray.rows - 40, img_gray.cols - 1, 40));
       Mat img_left = img_gray(Rect(0, 0, 40, img_gray.rows - 1));
       Mat img_right = img_gray(Rect(img_gray.cols - 40, 0, 40, img_gray.rows - 1));
       Mat img_right_light = img_gray(Rect(img_gray.cols - 15, 0, 15, img_gray.rows - 1));
       Mat img_tl_R = img_gray(Rect(0, 0, 150, 150));
       Mat img_bl_R = img_gray(Rect(0, 1350, 150, 150));
       Mat img_tr_R = img_gray(Rect(2850, 0, 150, 150));
       Mat img_br_R = img_gray(Rect(2849, 1349, 150, 150));

       Mat top_th, bottom_th, left_th, right_th, img_tl_R_th, img_bl_R_th, img_tr_R_th, img_br_R_th, img_right_light_th;

       //针对边界位置设置参数
       adaptiveThreshold(img_top, top_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, -1);
       adaptiveThreshold(img_bottom, bottom_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, -1);
       //adaptiveThreshold(img_left, left_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, -1);
       adaptiveThreshold(img_left, left_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 37, -3);
       adaptiveThreshold(img_right, right_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
       adaptiveThreshold(img_right_light, img_right_light_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 9, -3);
       adaptiveThreshold(img_tl_R, img_tl_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
       adaptiveThreshold(img_bl_R, img_bl_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
       adaptiveThreshold(img_tr_R, img_tr_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 21, -3);
       adaptiveThreshold(img_br_R, img_br_R_th, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 21, -3);

       //针对边界位置深拷贝
       top_th.copyTo(th_result(Rect(0, 0, th_result.cols - 1, 40)));                    //上边界
       bottom_th.copyTo(th_result(Rect(0, th_result.rows - 40, th_result.cols - 1, 40)));     //下边界
       left_th.copyTo(th_result(Rect(0, 0, 40, th_result.rows - 1)));                   //左边界
       right_th.copyTo(th_result(Rect(th_result.cols - 40, 0, 40, th_result.rows - 1)));      //右边界
       img_right_light_th.copyTo(th_result(Rect(img_gray.cols - 15, 0, 15, img_gray.rows - 1)));      //右边界
       img_tl_R_th.copyTo(th_result(Rect(0, 0, 150, 150)));                    //上边界
       img_bl_R_th.copyTo(th_result(Rect(0, 1350, 150, 150)));     //下边界
       img_tr_R_th.copyTo(th_result(Rect(2850, 0, 150, 150)));                   //左边界
       img_br_R_th.copyTo(th_result(Rect(2849, 1349, 150, 150)));      //右边界

       th_result(Rect(0, 0, 15, th_result.rows)) = uchar(0);            //屏蔽右侧15行，防止灯口误检白点
       th_result(Rect(th_result.cols - 10, 0, 10, th_result.rows)) = uchar(0);            //屏蔽左侧10行，防止头部亮边误检为白点

       Mat th1;
       //做掩膜
       threshold(img_gray, th1, 25, 255, CV_THRESH_BINARY);
       bitwise_and(th1, img_gray, img_gray);
       //闭运算,弥合内部空洞,连接相距很近的区域
       Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));//闭操作结构元素
       morphologyEx(th_result, th_result, CV_MOP_CLOSE, element);   //闭运算形态学操作。可以减少噪点
                                                                    //th_result(Rect(th_result.cols - 261, th_result.rows - 351, 260, 350)) = uchar(0);//易撕贴部分设置右下角
       vector<vector<Point>> contours;
       findContours(th_result, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
       vector<Rect> boundRect(contours.size());
       for (vector<int>::size_type i = 0; i < contours.size(); i++)
       {
           boundRect[i] = boundingRect(Mat(contours[i]));
           int w = boundRect[i].width;
           int h = boundRect[i].height;
           int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
           int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
           int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
           int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
           Mat Crop_Image_last = th_result(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
           double area = countNonZero(Crop_Image_last);
           //int x_point, y_point;

           if (area >= whitePoint_lowerArea && area < 600)
           {
               RotatedRect rect = minAreaRect(contours[i]);
               double mw = rect.size.height;
               double mh = rect.size.width;
               double radio = max(mw / mh, mh / mw);

               //长宽比排除
               if (radio > whitePoint_w_h)
               {
                   continue;
               }

               int x = boundRect[i].x + w / 2;
               int y = boundRect[i].y + h / 2;

               //Moments m = moments(contours[i]);//查找轮廓的重心
               //x_point = int(m.m10 / m.m00);
               //y_point = int(m.m01 / m.m00);
               //if (x_point > 840 && x_point < 930 && y_point > 730 && y_point < 830)
               //{
               // int A = 0;
               //}
               if (true)
               {
                   //粗筛选白点缺陷
                   int border = 15;
                   int x_lt = X_1 - border;
                   if (x_lt < 0)
                   {
                       x_lt = 0;
                   }
                   int y_lt = Y_1 - border;
                   if (y_lt < 0)
                   {
                       y_lt = 0;
                   }
                   int x_rt = X_2 + border;
                   if (x_rt > img_gray.size[1] - 1)
                   {
                       x_rt = img_gray.size[1] - 1;
                   }
                   int y_rt = Y_2 + border;
                   if (y_rt > img_gray.size[0] - 1)
                   {
                       y_rt = img_gray.size[0] - 1;
                   }
                   //排除屏幕上划痕的干扰,侧光图上检测
                   Mat temp_ceguang = img_ceguang(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                   //Mat mask_ceguang = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                   //double mean_in_ceguang = mean(temp_ceguang, mask_ceguang)[0];
                   //double mean_out_ceguang = mean(temp_ceguang, ~mask_ceguang)[0];
                   //double ceguang_th = mean_in_ceguang - mean_out_ceguang;
                   int Qnum1 = 0;
                   double mean_temp1 = mean(temp_ceguang)[0];
                   double ceguang_th, stddev;

                   //Mat  col, row;
                   //double m, n, p, o, r;
                   //ofstream oFile;
                   //oFile.open("D:\\Users\\秦立峰\\Desktop\\test.csv", ios::out | ios::trunc);
                   //////列灰度均值
                   ////for (int col_line = 0; col_line < temp_ceguang.cols; col_line++)//列进行编列
                   ////{
                   ////	col = temp_ceguang.colRange(col_line, col_line + 1).clone();
                   ////	m = mean(col)[0];

                   ////	oFile << m << "," << endl;
                   ////}
                   ////oFile << " " << "," << endl;
                   ////行灰度均值
                   //for (int row_line = 0; row_line < temp_ceguang.rows; row_line++)//列进行编列
                   //{
                   // row = temp_ceguang.rowRange(row_line, row_line + 1).clone();
                   // m = mean(col)[0];

                   // oFile << m << "," << endl;
                   //}
                   //oFile << " " << "," << endl;
                   ////一次差分
                   //for (int col_line = 5; col_line < white1.cols - 10; col_line++)//列进行编列
                   //{
                   // col = white1.colRange(col_line, col_line + 5).clone();
                   // m = mean(col)[0];

                   // col = white1.colRange(col_line + 5, col_line + 10).clone();
                   // n = mean(col)[0];

                   // p = n - m;

                   // oFile << p << "," << endl;
                   //}
                   //oFile << " " << "," << endl;


                   //上下边缘时排灰尘与气泡
                   if (y_rt < 40 || (img_gray.rows - y_lt < 40))
                   {
                       Mat  col, row;
                       double m, n, p;
                       for (int col_line = 0; col_line < temp_ceguang.cols; col_line++)//列进行编列  行遍历
                       {
                           col = temp_ceguang.colRange(col_line, col_line + 1).clone();
                           m = mean(col)[0];

                           p = m - mean_temp1;

                           if (p > 4)
                               Qnum1++;
                       }
                   }

                   //左右边缘时排灰尘与气泡
                   if (x_rt < 40 || (img_gray.cols - x_rt<40))
                   {
                       Mat  col, row;
                       double m, n, p;
                       for (int row_line = 0; row_line < temp_ceguang.rows; row_line++)//列进行编列  行遍历
                       {
                           row = temp_ceguang.rowRange(row_line, row_line + 1).clone();
                           m = mean(row)[0];

                           p = m - mean_temp1;

                           if (p > 4)
                               Qnum1++;
                       }
                   }

                   //屏幕内部时排灰尘与气泡
                   if ((y_rt >= 40 && (img_gray.rows - y_lt >= 40 && x_rt >= 40 && (img_gray.cols - x_rt >= 40))) || (Qnum1>0))
                   {
                       Mat mask_ceguang = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                       double mean_in_ceguang = mean(temp_ceguang, mask_ceguang)[0];
                       double mean_out_ceguang = mean(temp_ceguang, ~mask_ceguang)[0];
                       ceguang_th = mean_in_ceguang - mean_out_ceguang;

                       //侧光图上的灰度均值方差排除气泡等干扰
                       cv::Mat meanGray;
                       cv::Mat stdDev;
                       cv::meanStdDev(temp_ceguang, meanGray, stdDev);
                       double avg = meanGray.at<double>(0, 0);
                       stddev = stdDev.at<double>(0, 0);
                   }
                   else
                   {
                       ceguang_th = 0;
                       stddev = 0;
                   }


                   //排除贴膜划痕划痕跳过
                   if (ceguang_th > scratchth)
                   {
                       continue;
                   }
                   ////侧光图上的灰度均值方差排除气泡等干扰
                   //cv::Mat meanGray;
                   //cv::Mat stdDev;
                   //cv::meanStdDev(temp_ceguang, meanGray, stdDev);
                   //double avg = meanGray.at<double>(0, 0);
                   //double stddev = stdDev.at<double>(0, 0);
                   //排除贴膜表面气泡等跳过
                   if (stddev > bubbleth)
                   {
                       continue;
                   }

                   //颜色深浅判断2个指标  1:缺陷区域与周围灰度差值(整体性)  2:缺陷中心点与一次缺陷区域灰度差值(局部性)
                   //Mat temp_gray = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                   Mat temp_gray = Original(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                   Mat mask = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));

                   double minp;			//最小灰度值
                   double maxp;			//最大灰度值
                   Point low_gray, high_gray;	//正常取时灰度最大最小点

                   minMaxLoc(temp_gray, &minp, &maxp, &low_gray, &high_gray, mask);	//求最大最小灰度点
                   double spotpeak_temp = maxp - minp;

                   double mean_temp = mean(temp_gray)[0];

                   double meanOut;//缺陷外围灰度均值
                   double meanIn;//缺陷区域灰度均值
                   double meanAll;//整个区域的灰度均值
                   int Qnum = 0;

                   //防止普通边缘误检——上下边缘
                   if (y_rt < 40 || (img_gray.rows - y_lt < 40))
                   {
                       Mat  col, row;
                       double m, n, p;
                       for (int col_line = 0; col_line < temp_gray.cols; col_line++)//列进行编列  行遍历
                       {
                           col = temp_gray.colRange(col_line, col_line + 1).clone();
                           m = mean(col)[0];

                           p = m - mean_temp;

                           if (p > 4)
                               Qnum++;	//真实白点区域
                       }
                   }
                   //左右边缘
                   if (x_rt < 40 || (img_gray.cols - x_lt<40))
                   {
                       Mat  col, row;
                       double m, n, p;
                       for (int row_line = 0; row_line < temp_gray.rows; row_line++)//列进行编列  行遍历
                       {
                           row = temp_gray.rowRange(row_line, row_line + 1).clone();
                           m = mean(row)[0];

                           p = m - mean_temp;

                           if (p > 4)
                               Qnum++;	//真实白点区域
                       }
                   }

                   if (Qnum > 0 || (y_rt >= 40 && (img_gray.rows - y_lt >= 40 && x_rt >= 40 && (img_gray.cols - x_lt >= 40))))//边界处真实白点或屏内区域
                   {
                       //防止R角边缘误检
                       int grayValueSum = 0;
                       int pixelsNum = 0;
                       Mat maskGray;
                       bitwise_and(temp_gray, mask, maskGray);
                       for (int i = 0; i < maskGray.cols; i++)
                       {
                           for (int j = 0; j < maskGray.rows; j++)
                           {
                               if (maskGray.at<uchar>(j, i) > 0)
                               {
                                   grayValueSum += maskGray.at<uchar>(j, i);
                                   pixelsNum++;
                               }
                           }
                       }
                       meanIn = grayValueSum / (float)pixelsNum;

                       grayValueSum = 0;
                       pixelsNum = 0;
                       bitwise_and(temp_gray, ~mask, maskGray);
                       for (int i = 0; i < maskGray.cols; i++)
                       {
                           for (int j = 0; j < maskGray.rows; j++)
                           {
                               if (maskGray.at<uchar>(j, i) > 0)
                               {
                                   grayValueSum += maskGray.at<uchar>(j, i);
                                   pixelsNum++;
                               }
                           }
                       }
                       meanOut = grayValueSum / (float)pixelsNum;

                       grayValueSum = 0;
                       pixelsNum = 0;
                       for (int i = 0; i < temp_gray.cols; i++)
                       {
                           for (int j = 0; j < temp_gray.rows; j++)
                           {
                               if (temp_gray.at<uchar>(j, i) > 0)
                               {
                                   grayValueSum += temp_gray.at<uchar>(j, i);
                                   pixelsNum++;
                               }
                           }
                       }
                       meanAll = grayValueSum / (float)pixelsNum;

                       double  defect_areath = meanIn - meanOut;//缺陷区域与周围灰度差值(整体性)
                                                                //double coreth = img_gray.at<uchar>(y_point, x_point) - meanAll;
                                                                //double coreth = Original.at<uchar>(y_point, x_point) - meanAll;

                       if (img_gray.cols - x_rt < corewholeth)
                       {
                           //灰度差限制
                           if (defect_areath >= siecevariance && spotpeak_temp >= spotpeak && area <= 80|| area > 80 && defect_areath >= 4.8 && spotpeak_temp >= spotpeak)//这里的参数先写成定值
                           {
                               result = true;
                               CvPoint top_lef4 = cvPoint(X_1 - 10, Y_1 - 10);
                               CvPoint bottom_right4 = cvPoint(X_2 + 20, Y_2 + 20);
                               rectangle(white_yiwu, top_lef4, bottom_right4, Scalar(0, 0, 0), 5, 8, 0);
                               /*Point p3(x_point, y_point);
                               circle(white_yiwu, p3, 12, Scalar(0, 0, 255), 1, 8, 0);*/
                               string ceghaung = "cegaung:" + to_string(ceguang_th) + " " + "th:" + to_string(defect_areath) + " " /*+ "ceth:" + to_string(coreth) + " "*/ + "area:" + to_string(area) + " " + "stddev:" + to_string(stddev);
                               putText(white_yiwu, ceghaung, Point(30, 30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1, 8, 0);
                           }
                       }
                       else
                       {
                           //灰度差限制
                           if (defect_areath >= defectouterth && spotpeak_temp >= spotpeak && area <= 60|| area > 60 && defect_areath >= 5.5 && spotpeak_temp >= spotpeak)//这里的参数先写成定值
                           {
                               result = true;
                               CvPoint top_lef4 = cvPoint(X_1 - 10, Y_1 - 10);
                               CvPoint bottom_right4 = cvPoint(X_2 + 20, Y_2 + 20);
                               rectangle(white_yiwu, top_lef4, bottom_right4, Scalar(0, 0, 0), 5, 8, 0);
                               /*Point p3(x_point, y_point);
                               circle(white_yiwu, p3, 12, Scalar(0, 0, 255), 1, 8, 0);*/
                               string ceghaung = "cegaung:" + to_string(ceguang_th) + " " + "th:" + to_string(defect_areath) + " " /*+ "ceth:" + to_string(coreth) + " "*/ + "area:" + to_string(area) + " " + "stddev:" + to_string(stddev);
                               putText(white_yiwu, ceghaung, Point(30, 30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1, 8, 0);
                           }
                       }
                   }
               }
           }
       }
       if (result == true)
       {
           *causecolor = "白点";
           *mresult = white_yiwu;
       }
       return result;
   }

   /*=========================================================
   *@函 数 名: adaptiveThresholdCustom
   *@功能描述: 自适应阈值分割实现图像二值化
   *@param src          输入灰度图像
   *@param dst          输出二值图像
   *@param maxValue     输入满足阈值条件时像素取值
   *@param method       计算局部均值方法
   *@param type         输入阈值判断类型
   *@param blockSize    卷积窗口大小(奇数)
   *@param delta        输入偏移常量
   *@param ratio        输入均值比例系数
   *@修改时间：			2020年09月26日
   =========================================================*/
   void adaptiveThresholdCustom_whitedot(const cv::Mat &src, cv::Mat &dst, double maxValue, int method, int type, int blockSize, double delta, double ratio)
   {
       CV_Assert(src.type() == CV_8UC1);               // 原图必须是单通道无符号8位,CV_Assert（）若括号中的表达式值为false，则返回一个错误信息
       CV_Assert(blockSize % 2 == 1 && blockSize > 1);	// 块大小必须大于1，并且是奇数
       CV_Assert(maxValue > 0);                        //二值图像最大值
       CV_Assert(ratio > DBL_EPSILON);	                //输入均值比例系数
       Size size = src.size();							//源图像的尺寸
       Mat _dst(size, src.type());						//目标图像的尺寸
       Mat mean;	                                    //存放均值图像
       if (src.data != _dst.data)
           mean = _dst;


       int top = (blockSize - 1)*0.5;     //填充的上边界行数
       int bottom = (blockSize - 1)*0.5;  //填充的下边界行数
       int left = (blockSize - 1)*0.5;	   //填充的左边界行数
       int right = (blockSize - 1)*0.5;   //填充的右边界行数
       int border_type = BORDER_CONSTANT; //边界填充方式——常量填充
       //int border_type = BORDER_REPLICATE; //边界填充方式——复制最边缘像素
       //int border_type = BORDER_REFLECT_101;//边界填充方式——以最边缘像素为轴，对称
       //int border_type = BORDER_REFLECT;
       //int border_type = BORDER_WRAP;
       Mat src_Expand;	                   //对原图像进行边界扩充

       Mat topImage = src(Rect(0, 0, src.cols, 3));//上边界一行图像

       //cv::Scalar color = cv::mean(topImage)*0.5;//35-80之间均可以  该值需要确定

       //cv::Scalar color = cv::mean(topImage)*0.6;//35-80之间均可以  该值需要确定

       Scalar color = Scalar(200);//35-80之间均可以
       copyMakeBorder(src, src_Expand, top, bottom, left, right, border_type, color);

       if (method == ADAPTIVE_THRESH_MEAN_C)
       {
           /*
           @param src 单通道灰度图
           @param dst 单通道处理后的图
           @param int类型的ddepth，输出图像的深度
           @param Size类型的ksize，内核的大小
           @param Point类型的anchor，表示锚点
           @param bool类型的normaliz,即是否归一化
           @param borderType 图像外部像素的某种边界模式
           */
           boxFilter(src_Expand, mean, src.type(), Size(blockSize, blockSize), Point(-1, -1), true, BORDER_CONSTANT);
       }
       else if (method == ADAPTIVE_THRESH_GAUSSIAN_C)
       {
           GaussianBlur(src, mean, Size(blockSize, blockSize), 0, 0, BORDER_DEFAULT);
       }
       else
           CV_Error(CV_StsBadFlag, "Unknown/unsupported adaptive threshold method");

       mean = mean(cv::Rect(top, top, src_Expand.cols - top * 2, src_Expand.rows - top * 2)); //删除扩充的图像边界

       int i, j;
       uchar imaxval = saturate_cast<uchar>(maxValue);	                       //将maxValue由double类型转换为uchar型
       int idelta = type == THRESH_BINARY ? cvCeil(delta) : cvFloor(delta);   //将idelta由double类型转换为int型
       if (src.isContinuous() && mean.isContinuous() && _dst.isContinuous())
       {
           size.width *= size.height;
           size.height = 1;
       }
       for (i = 0; i < size.height; i++)
       {
           const uchar* sdata = src.data + src.step * i;		   //指向源图像
           const uchar* mdata = mean.data + mean.step * i;		   //指向均值图
           uchar* ddata = _dst.data + _dst.step * i;	           //指向输出图
           for (j = 0; j < size.width; j++)
           {
               double Thresh = mdata[j] * ratio - idelta;	        //阈值
               if (CV_THRESH_BINARY == type)	                    //S>T时为imaxval
               {
                   ddata[j] = sdata[j] > Thresh ? imaxval : 0;
               }
               else if (CV_THRESH_BINARY_INV == type)	            //S<T时为imaxval
               {
                   ddata[j] = sdata[j] > Thresh ? 0 : imaxval;
               }
               else
                   CV_Error(CV_StsBadFlag, "Unknown/unsupported threshold type");
           }
       }
       dst = _dst.clone();
   }
   /*=========================================================
   * 函 数 名：bool LessMaterial(Mat src_img)
   * 少料检测函数
   =========================================================*/
   bool LessMaterial(Mat src_img, Mat *mresult, QString *causecolor)
   {
       bool Test_Result = false;
       //灰度化
       Mat src_gray, src_bin;
       if (src_img.type() == CV_8UC1)
           src_gray = src_img.clone();
       else
           cvtColor(src_img, src_gray, CV_BGR2GRAY);

       //ROI区域提取
       Mat src_roi1, src_roi1_1, src_roi2, src_roi2_2;
       int Roi1_UpLeft_X = 0;
       int Roi2_UpLeft_X = src_gray.cols * 7 / 8;
       src_roi1 = src_gray(Rect(Roi1_UpLeft_X, 0, src_gray.cols / 8, src_gray.rows)).clone();
       src_roi2 = src_gray(Rect(Roi2_UpLeft_X, 0, src_gray.cols / 8, src_gray.rows)).clone();

       Mat src_roi_neg1, src_roi_neg2;
       Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(2,2));
       Mat src_roi_dilate1, src_roi_dilate2;
       vector<vector<Point>> contours1, contours2;
       vector<Vec4i> hierarchy2;
       int i;

       //ROI预处理
       threshold(src_roi1, src_roi1_1, 200, 255, CV_THRESH_BINARY);//灰度化
       bitwise_not(src_roi1_1, src_roi_neg1);//图像反色
       erode(src_roi_neg1, src_roi_neg1, structure_element1);//腐蚀操作
       bitwise_not(src_roi_neg1, src_roi_dilate1);//图像再次反色

       //缺陷检测
       findContours(src_roi_dilate1, contours1, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//轮廓检索(只检索最外层轮廓)
       findContours(src_roi_dilate1, contours2, hierarchy2, CV_RETR_CCOMP, CHAIN_APPROX_SIMPLE);//轮廓检索(检索所有轮廓)
       vector<Rect> boundRect1(contours2.size());
       vector<Rect> boundRect(contours2.size());

       if (contours2.size() - contours1.size() == 1)
       {
           //分割缺陷区域
           for (i = 0; i < contours2.size(); i++)
           {
               if (hierarchy2[i][2] != -1)
               {
                   boundRect1[i] = boundingRect(Mat(contours2[i]));
                   float w1 = boundRect1[i].width;
                   float h1 = boundRect1[i].height;
                   int X_UpLeft_1 = boundRect1[i].tl().x; //矩形左上角X坐标值
                   int Y_UpLeft_1 = boundRect1[i].tl().y; //矩形左上角Y坐标值
                   int X_UpLeft_2 = boundRect1[i].br().x; //矩形右下角X坐标值
                   int Y_UpLeft_2 = boundRect1[i].br().y; //矩形右下角Y坐标值

                   Mat srcImage_roi = src_roi1(Rect(X_UpLeft_1, Y_UpLeft_1, X_UpLeft_2 - X_UpLeft_1, Y_UpLeft_2 - Y_UpLeft_1));
                   Mat srcThreshold_roi = src_roi_dilate1(Rect(X_UpLeft_1, Y_UpLeft_1, X_UpLeft_2 - X_UpLeft_1, Y_UpLeft_2 - Y_UpLeft_1));
                   double area = countNonZero(srcThreshold_roi);
                   if (area < 6000 && area > 15)
                   {
                       Mat meanGray0, stdDev0, meanGray1, stdDev1;
                       cv::meanStdDev(srcImage_roi, meanGray0, stdDev0, srcThreshold_roi);
                       cv::meanStdDev(srcImage_roi, meanGray1, stdDev1, ~srcThreshold_roi);
                       double meanGray_0 = meanGray0.at<double>(0, 0);
                       double meanGray_1 = meanGray1.at<double>(0, 0);
                       if ((meanGray_0 - meanGray_1) > 60 && meanGray_1 > 0 && meanGray_0 > 0)
                       {
                           Test_Result = true;
                           CvPoint Point_UpLeft1 = cvPoint(X_UpLeft_1, Y_UpLeft_1);
                           CvPoint Point_DownRight1 = cvPoint(X_UpLeft_2, Y_UpLeft_2);
                           rectangle(src_img, Point_UpLeft1, Point_DownRight1, Scalar(0, 0, 0), 5, 8, 0);
                           break;
                       }
                   }
               }
           }
       }
       else {
           for (vector<int>::size_type i = 0; i < contours2.size(); i++)
           {
               boundRect[i] = boundingRect(Mat(contours2[i]));
               int w = boundRect[i].width;
               int h = boundRect[i].height;
               int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
               int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
               int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
               int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
               Mat srcImage_roi = src_roi1(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
               Mat srcThreshold_roi = src_roi_dilate1(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
               double area = countNonZero(srcThreshold_roi);
               if (area < 6000 && area > 15)
               {
                   Mat meanGray0, stdDev0, meanGray1, stdDev1;
                   cv::meanStdDev(srcImage_roi, meanGray0, stdDev0, srcThreshold_roi);
                   cv::meanStdDev(srcImage_roi, meanGray1, stdDev1, ~srcThreshold_roi);
                   double meanGray_0 = meanGray0.at<double>(0, 0);
                   double meanGray_1 = meanGray1.at<double>(0, 0);

                   if ((meanGray_0 - meanGray_1) > 80  && meanGray_1 > 0 && meanGray_0 > 0 )
                   {
                       Test_Result = true;
                       int border = 25;
                       X_1 = X_1 - border;
                       if (X_1 < 0) {
                           X_1 = 0;
                       }
                       Y_1 = Y_1 - border;
                       if (Y_1 < 0) {
                           Y_1 = 0;
                       }
                       X_2 = X_2 + border;
                       if (X_2 > src_gray.size[1] - 1)
                       {
                           X_2 = src_gray.size[1] - 1;
                       }
                       Y_2 = Y_2 + border;
                       if (Y_2 > src_gray.size[0] - 1) {
                           Y_2 = src_gray.size[0] - 1;
                       }
                       CvPoint Point_UpLeft1 = cvPoint(X_1, Y_1);
                       CvPoint Point_DownRight1 = cvPoint(X_2, Y_2);
                       rectangle(src_img, Point_UpLeft1, Point_DownRight1, Scalar(0, 0, 0), 5, 8, 0);
                       break;
                   }
               }
           }
       }

       //另外一个ROI区域的转换
       if (Test_Result == false)
       {
           //ROI预处理
           threshold(src_roi2, src_roi2_2, 200, 255, CV_THRESH_BINARY);//灰度化
           bitwise_not(src_roi2_2, src_roi_neg2);//图像反色
           erode(src_roi_neg2, src_roi_neg2, structure_element1);//腐蚀操作
           bitwise_not(src_roi_neg2, src_roi_dilate2);//图像再次反色

           //缺陷检测
           findContours(src_roi_dilate2, contours1, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//轮廓检索(只检索最外层轮廓)
           findContours(src_roi_dilate2, contours2, hierarchy2, CV_RETR_CCOMP, CHAIN_APPROX_SIMPLE);//轮廓检索(检索所有轮廓)
           vector<Rect> boundRect2(contours2.size());
           vector<Rect> boundRect3(contours2.size());

           if (contours2.size() - contours1.size() == 1)
           {
               //分割缺陷区域
               for (i = 0; i < contours2.size(); i++)
               {
                   if (hierarchy2[i][2] != -1)
                   {
                       boundRect2[i] = boundingRect(Mat(contours2[i]));
                       float w2 = boundRect2[i].width;
                       float h2 = boundRect2[i].height;
                       int X_UpLeft_2 = boundRect2[i].tl().x; //矩形左上角X坐标值
                       int Y_UpLeft_2 = boundRect2[i].tl().y; //矩形左上角Y坐标值
                       int X_DownRight_2 = boundRect2[i].br().x; //矩形右下角X坐标值
                       int Y_DownRight_2 = boundRect2[i].br().y; //矩形右下角Y坐标值

                       Mat srcImage_roi = src_roi1(Rect(X_UpLeft_2, Y_UpLeft_2, X_DownRight_2 - X_UpLeft_2, Y_DownRight_2 - Y_UpLeft_2));
                       Mat srcThreshold_roi = src_roi_dilate1(Rect(X_UpLeft_2, Y_UpLeft_2, X_DownRight_2 - X_UpLeft_2, Y_DownRight_2 - Y_UpLeft_2));
                       double area = countNonZero(srcThreshold_roi);
                       if (area < 6000 && area > 15)
                       {
                           Mat meanGray0, stdDev0, meanGray1, stdDev1;
                           cv::meanStdDev(srcImage_roi, meanGray0, stdDev0, srcThreshold_roi);
                           cv::meanStdDev(srcImage_roi, meanGray1, stdDev1, ~srcThreshold_roi);
                           double meanGray_0 = meanGray0.at<double>(0, 0);
                           double meanGray_1 = meanGray1.at<double>(0, 0);

                           if ((meanGray_0 - meanGray_1) > 60 && meanGray_1 > 0 && meanGray_0 > 0)
                           {
                               Test_Result = true;
                               CvPoint Point_UpLeft2 = cvPoint(X_UpLeft_2, Y_UpLeft_2);
                               CvPoint Point_DownRight2 = cvPoint(X_DownRight_2, Y_DownRight_2);
                               rectangle(src_img, Point_UpLeft2, Point_DownRight2, Scalar(0, 0, 0), 5, 8, 0);
                               break;
                           }
                       }
                   }
               }
           }
           else {
               for (vector<int>::size_type i = 0; i < contours2.size(); i++)
               {
                   boundRect3[i] = boundingRect(Mat(contours2[i]));
                   int w = boundRect3[i].width;
                   int h = boundRect3[i].height;
                   int X_1 = boundRect3[i].tl().x;//矩形左上角X坐标值
                   int Y_1 = boundRect3[i].tl().y;//矩形左上角Y坐标值
                   int X_2 = boundRect3[i].br().x;//矩形右下角X坐标值
                   int Y_2 = boundRect3[i].br().y;//矩形右下角Y坐标值
                   Mat srcImage_roi = src_roi1(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                   Mat srcThreshold_roi = src_roi_dilate2(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                   double area = countNonZero(srcThreshold_roi);
                   if (area < 6000 && area > 15)
                   {
                       Mat meanGray0, stdDev0, meanGray1, stdDev1;
                       cv::meanStdDev(srcImage_roi, meanGray0, stdDev0, srcThreshold_roi);
                       cv::meanStdDev(srcImage_roi, meanGray1, stdDev1, ~srcThreshold_roi);
                       double meanGray_0 = meanGray0.at<double>(0, 0);
                       double meanGray_1 = meanGray1.at<double>(0, 0);

                       if ((meanGray_0 - meanGray_1) > 80 && meanGray_1 > 0 && meanGray_0 > 0)
                       {
                           Test_Result = true;
                           int border = 25;
                           X_1 = X_1 - border;
                           if (X_1 < 0) {
                               X_1 = 0;
                           }
                           Y_1 = Y_1 - border;
                           if (Y_1 < 0) {
                               Y_1 = 0;
                           }
                           X_2 = X_2 + border;
                           if (X_2 > src_gray.size[1] - 1)
                           {
                               X_2 = src_gray.size[1] - 1;
                           }
                           Y_2 = Y_2 + border;
                           if (Y_2 > src_gray.size[0] - 1) {
                               Y_2 = src_gray.size[0] - 1;
                           }
                           CvPoint Point_UpLeft1 = cvPoint(X_1, Y_1);
                           CvPoint Point_DownRight1 = cvPoint(X_2, Y_2);
                           rectangle(src_img, Point_UpLeft1, Point_DownRight1, Scalar(0, 0, 0), 5, 8, 0);
                           break;
                       }
                   }
               }
           }
       }

       if (Test_Result == true)
       {
           *causecolor = "少料";
           *mresult = src_img;
       }

       return Test_Result;
   }

/*====================================================================
* 函 数 名: Shifting
* 功能描述:移位，表现为白底图象有一条亮线
* 输入：主相机白底图像
* 输出：主相机白底下检测结果图和result
* 其他：
======================================================================*/
bool Shifting(Mat white, Mat *mresult, QString *causecolor)
{
    bool result = false;
    Mat img_gray = white.clone();

    Mat ad_result, th1,th2, ImageBinary;
    //adaptiveThreshold(img_gray, ad_result, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 15, 3);

    adaptiveThresholdCustom(img_gray, th1, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 5.5, 1);

    Mat th_result = Mat::zeros(th1.rows-2*200, th1.cols - 2 * 300, img_gray.type());
    th_result.copyTo(th1(Rect(300, 200, th1.cols - 2 * 300, th1.rows - 2 * 200)));
    vector<vector<Point>> contours;

    threshold(img_gray, ImageBinary, 30, 255, CV_THRESH_BINARY);
    bitwise_and(ImageBinary, th1, th2);

    findContours(th1, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    std::sort(contours.begin(), contours.end(), compareContourAreas);

    vector<Rect> boundRect(contours.size());
    vector<RotatedRect>box(contours.size());
    Point2f rect[4];
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        Mat temp_mask = Mat::zeros(th1.rows, th1.cols, CV_8UC1);
        drawContours(temp_mask, contours, i, 255, FILLED, 8);
        double area = contourArea(contours[i]);
        if (area > 200 && area < 80000)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            box[i] = minAreaRect(Mat(contours[i]));
            box[i].points(rect);
            float Width = sqrt(abs(rect[0].x - rect[1].x) * abs(rect[0].x - rect[1].x) + abs(rect[0].y - rect[1].y) * abs(rect[0].y - rect[1].y));
            float Height = sqrt(abs(rect[1].x - rect[2].x) * abs(rect[1].x - rect[2].x) + abs(rect[1].y - rect[2].y) * abs(rect[1].y - rect[2].y));

            float w = boundRect[i].width;
            float h = boundRect[i].height;
            RotatedRect rect = minAreaRect(contours[i]);  //包覆轮廓的最小斜矩形 划伤缺陷有旋转特点
            Point p = rect.center;
            double w1 = rect.size.height;
            double h1 = rect.size.width;
            int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
            //int x_point = X_1 + round(w / 2);
            //int y_point = Y_1 + round(h / 2);
            double HeWid = max(Height / Width, Width / Height);
            if ((w < 5 && X_1 >= th1.cols - 4) || (w < 5 && X_2 <= 4))
            {
                continue;
            }
            if (min(Height, Width) >= 200 && ((X_1 == 0 && Y_1 == 0) || (X_1 == 0 && Y_2 >= th1.rows - 1) || (Y_1 == 0 && X_2 >= th1.cols - 1) || (X_2 >= th1.cols - 1 && Y_2 >= th1.rows - 1)))
            {
                continue;
            }
            if (HeWid >= 3.2 && max(Height,Width) >= 40)
            {
                X_1 = X_1 - 10;
                Y_1 = Y_1 - 10;
                X_2 = X_2 + 10;
                Y_2 = Y_2 + 10;
                if (X_1 < 0)
                {
                    X_1 = 0;
                }
                if (Y_1 < 0)
                {
                    Y_1 = 0;
                }
                if (X_2 > th1.cols - 1)
                {
                    X_2 = th1.cols - 1;
                }
                if (Y_2 > th1.rows - 1)
                {
                    Y_2 = th1.rows - 1;
                }
                Mat ImageOutBinary;
                Mat tempImage = ImageBinary(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                Mat tempBinary1 = th1(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1)).clone();
                Mat tempBinary2 = th2(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1)).clone();
                Mat tempGray = img_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                double mean_In = mean(tempGray, tempBinary1)[0];
                double mean_all = mean(tempGray, tempImage)[0];
                threshold(tempGray, ImageOutBinary, mean_all - 10, 255, CV_THRESH_BINARY);
                bitwise_and(ImageOutBinary, ~tempBinary2, tempBinary2);
                double mean_Out = mean(tempGray, tempBinary2)[0];
                double differ = mean_Out - mean_In;
                if (((HeWid <= 8 && differ >= 13) || (HeWid > 8 && differ >= 9.5)) && mean_In >= mean_all - 80)
                {
                    result = true;
                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                    rectangle(white, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
                    break;
                }
            }
        }
    }
    if (result == true)
    {
        *mresult = white;
        *causecolor = "移位";
        result = true;
    }
    return result;
}

/*=========================================================
* 函 数 名: GrayDotLeft
* 功能描述: 灰色底下间检测圆形白点状缺陷,白点缺陷在主黑白相机中不可见
* 输入: 左右侧相机拍摄白点图片和对应侧光图片，侧光图片排除划痕和气泡
* 输出：白底下检测结果图和result
* 时间：2020年05月16日
* 其他：其实就和WhiteDotLeft变了两个阈值
=========================================================*/
bool GrayDotLeft(Mat white_yiwu, Mat ceguang, Mat *mresult, QString *causecolor)//灰度检测
{
    bool result = false;
    Mat img_gray = white_yiwu.clone();
    Mat img_ceguang = ceguang.clone();
    medianBlur(img_gray, img_gray, 3); //中值滤波滤除椒盐噪声,缺点耗时26毫秒 奇数半径越大效果越强
    Mat th_result;
    adaptiveThreshold(img_gray, th_result, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, whitePoint_step, whitePoint_threshold);
    Mat th1;
    //做掩膜
    threshold(img_gray, th1, 25, 255, CV_THRESH_BINARY);
    bitwise_and(th1, img_gray, img_gray);
    //闭运算,弥合内部空洞,连接相距很近的区域
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));//闭操作结构元素
    morphologyEx(th_result, th_result, CV_MOP_CLOSE, element);   //闭运算形态学操作。可以减少噪点
    th_result(Rect(th_result.cols - 261, th_result.rows - 351, 260, 350)) = uchar(0);//易撕贴部分设置右下角
    vector<vector<Point>> contours;
    findContours(th_result, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        boundRect[i] = boundingRect(Mat(contours[i]));
        int w = boundRect[i].width;
        int h = boundRect[i].height;
        int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
        int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
        int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
        int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
        Mat Crop_Image_last = th_result(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
        double area = countNonZero(Crop_Image_last);
        if (area >= whitePoint_lowerArea && area < whitePoint_higherArea)
        {
            RotatedRect rect = minAreaRect(contours[i]);
            double mw = rect.size.height;
            double mh = rect.size.width;
            double radio = max(mw / mh, mh / mw);
            //长宽比排除
            if (radio > whitePoint_w_h)
            {
                continue;
            }

            int x = boundRect[i].x + w / 2;
            int y = boundRect[i].y + h / 2;

            Moments m = moments(contours[i]);//查找轮廓的重心
            int x_point = int(m.m10 / m.m00);
            int y_point = int(m.m01 / m.m00);

            if (true)
            {
                //粗筛选白点缺陷
                int border = 7;
                int x_lt = X_1 - border;
                if (x_lt < 0)
                {
                    x_lt = 0;
                }
                int y_lt = Y_1 - border;
                if (y_lt < 0)
                {
                    y_lt = 0;
                }
                int x_rt = X_2 + border;
                if (x_rt > img_gray.size[1] - 1)
                {
                    x_rt = img_gray.size[1] - 1;
                }
                int y_rt = Y_2 + border;
                if (y_rt > img_gray.size[0] - 1)
                {
                    y_rt = img_gray.size[0] - 1;
                }
                //排除屏幕上划痕的干扰,侧光图上检测
                Mat temp_ceguang = img_ceguang(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                Mat mask_ceguang = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                double mean_in_ceguang = mean(temp_ceguang, mask_ceguang)[0];
                double mean_out_ceguang = mean(temp_ceguang, ~mask_ceguang)[0];
                double ceguang_th = mean_in_ceguang - mean_out_ceguang;
                //排除贴膜划痕划痕跳过
                if (ceguang_th > scratchth)
                {
                    continue;
                }
                //侧光图上的灰度均值方差排除气泡等干扰
                cv::Mat meanGray;
                cv::Mat stdDev;
                cv::meanStdDev(temp_ceguang, meanGray, stdDev);
                double avg = meanGray.at<double>(0, 0);
                double stddev = stdDev.at<double>(0, 0);
                //排除贴膜表面气泡等跳过
                if (stddev > bubbleth)
                {
                    continue;
                }

                //颜色深浅判断2个指标  1:缺陷区域与周围灰度差值(整体性)  2:缺陷中心点与一次缺陷区域灰度差值(局部性)
                Mat temp_gray = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                Mat mask = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));

                double meanOut;//缺陷外围灰度均值
                double meanIn;//缺陷区域灰度均值
                double meanAll;//整个区域的灰度均值
                //if (x < 140 && y < 140 || x < 140 && y>1360 || x>2860 && y < 140 || x>2860 && y>1360||x>2880&&y<620&&y>880)//我感觉opencv的函数mean求均值可能比我写的求均值的函数快，所以只有这几个地方用自己写的求均值的函数。是否真的快，我也没测试过
                {
                    int grayValueSum = 0;
                    int pixelsNum = 0;
                    Mat maskGray;
                    bitwise_and(temp_gray, mask, maskGray);
                    for (int i = 0; i < maskGray.cols; i++)
                    {
                        for (int j = 0; j < maskGray.rows; j++)
                        {
                            if (maskGray.at<uchar>(j, i) > 0)
                            {
                                grayValueSum += maskGray.at<uchar>(j, i);
                                pixelsNum++;
                            }
                        }
                    }
                    meanIn = grayValueSum / (float)pixelsNum;

                     grayValueSum = 0;
                     pixelsNum = 0;
                     bitwise_and(temp_gray, ~mask, maskGray);
                     for (int i = 0; i < maskGray.cols; i++)
                     {
                         for (int j = 0; j < maskGray.rows; j++)
                         {
                             if (maskGray.at<uchar>(j, i) > 0)
                             {
                                 grayValueSum += maskGray.at<uchar>(j, i);
                                 pixelsNum++;
                             }
                         }
                     }
                     meanOut = grayValueSum / (float)pixelsNum;

                     grayValueSum = 0;
                     pixelsNum = 0;
                     for (int i = 0; i < temp_gray.cols; i++)
                     {
                         for (int j = 0; j < temp_gray.rows; j++)
                         {
                             if (temp_gray.at<uchar>(j, i) > 0)
                             {
                                 grayValueSum += temp_gray.at<uchar>(j, i);
                                 pixelsNum++;
                             }
                         }
                     }
                     meanAll = grayValueSum / (float)pixelsNum;
                }
                //else
                //{
                //	meanIn = mean(temp_gray, mask)[0];
                //	meanOut = mean(temp_gray, ~mask)[0];
                //	meanAll = mean(temp_gray)[0];  //缺陷中心点与一次缺陷区域灰度差值(局部性)
                //}

                double  defect_areath = meanIn - meanOut;//缺陷区域与周围灰度差值(整体性)
                double coreth = img_gray.at<uchar>(y_point, x_point) - meanAll;
                 //灰度差限制
                if (defect_areath >= defectouterth && coreth >= corewholeth )
                {
                        result = true;
                        CvPoint top_lef4 = cvPoint(X_1 - 10, Y_1 - 10);
                        CvPoint bottom_right4 = cvPoint(X_2 + 20, Y_2 + 20);
                        rectangle(white_yiwu, top_lef4, bottom_right4, Scalar(0, 0, 0), 5, 8, 0);
                        Point p3(x_point, y_point);
                        circle(white_yiwu, p3, 12, Scalar(0, 0, 255), 1, 8, 0);
                        string ceghaung = "cegaung:" + to_string(ceguang_th) + " " + "th:" + to_string(defect_areath) + " " + "ceth:" + to_string(coreth) + " " + "area:" + to_string(area) + " "+ "stddev:" + to_string(stddev);
                        putText(white_yiwu, ceghaung, Point(30, 30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1, 8, 0);
                        //break;
                }
            }
        }
    }
    if (result == true)
    {
        *causecolor = "白点";
        *mresult = white_yiwu;
    }
    return result;
}

//bool WhiteDotLeft(Mat white_yiwu, Mat ceguang, Mat *mresult, QString *causecolor)//20200718之前用的函数
//{
//    bool result = false;
//    Mat img_gray = white_yiwu.clone();
//    Mat img_ceguang = ceguang.clone();
//    medianBlur(img_gray, img_gray, 3); //中值滤波滤除椒盐噪声,缺点耗时26毫秒 奇数半径越大效果越强
//    Mat th_result;
//    adaptiveThreshold(img_gray, th_result, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, whitePoint_step, whitePoint_threshold);
//    Mat th1, dilate1;
//    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));//开操作结构元素
//    morphologyEx(th_result, th_result, CV_MOP_OPEN, element);   //去除噪点减少检测时间

//    th_result(Rect(th_result.cols - 261, th_result.rows - 351, 260, 350)) = uchar(0);//易撕贴部分设置右下角
//    th_result(Rect(0, 0, 55, th_result.rows - 1)) = uchar(0);                        // 左边有误分割情况15

//    // th_result(Rect(0, 0, th_result.cols - 1, 10)) = uchar(0);                  //上边
//    // th_result(Rect(0, th_result.rows - 24, th_result.cols - 1, 24)) = uchar(0);//下边
//    // th_result(Rect(th_result.cols - 11, 0, 11, th_result.rows - 1)) = uchar(0);//右

//    vector<vector<Point>> contours;
//    findContours(th_result, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//    vector<Rect> boundRect(contours.size());
//    for (vector<int>::size_type i = 0; i < contours.size(); i++)
//    {
//        double area = contourArea(contours[i]);
//        if (area > whitePoint_lowerArea && area < whitePoint_higherArea)
//        {

//            boundRect[i] = boundingRect(Mat(contours[i]));
//            float w = boundRect[i].width;
//            float h = boundRect[i].height;
//            float changkuanbi = w / h;
//            if (changkuanbi < 1)
//            {
//                changkuanbi = 1 / changkuanbi;
//            }
//            int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
//            int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
//            int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
//            int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
//            Moments m = moments(contours[i]);//查找轮廓的重心
//            int x_point = int(m.m10 / m.m00);
//            int y_point = int(m.m01 / m.m00);
//            if (true)
//            {
//                //粗筛选白点缺陷
//                int border = 10;
//                int x_lt = X_1 - border;
//                if (x_lt < 0)
//                {
//                    x_lt = 0;
//                }
//                int y_lt = Y_1 - border;
//                if (y_lt < 0)
//                {
//                    y_lt = 0;
//                }
//                int x_rt = X_2 + border;
//                if (x_rt > img_gray.size[1] - 1)
//                {
//                    x_rt = img_gray.size[1] - 1;
//                }
//                int y_rt = Y_2 + border;
//                if (y_rt > img_gray.size[0] - 1)
//                {
//                    y_rt = img_gray.size[0] - 1;
//                }
//                //排除屏幕上划痕的干扰
//                Mat temp_ceguang = img_ceguang(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
//                Mat mask_ceguang = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
//                double mean_in_ceguang = mean(temp_ceguang, mask_ceguang)[0];
//                double mean_out_ceguang = mean(temp_ceguang, ~mask_ceguang)[0];
//                double ceguang_th = mean_in_ceguang - mean_out_ceguang;
//                //排除贴膜划痕划痕跳过
//                if (abs(ceguang_th) >scratchth)
//                {
//                    continue;
//                }
//                //侧光图上的灰度均值方差排除气泡等干扰
//                cv::Mat meanGray;
//                cv::Mat stdDev;
//                cv::meanStdDev(temp_ceguang, meanGray, stdDev);
//                double avg = meanGray.at<double>(0, 0);
//                double stddev = stdDev.at<double>(0, 0);
//                //排除贴膜表面气泡等跳过
//                if (stddev>bubbleth)
//                {
//                    continue;
//                }
//                //颜色深浅判断2个指标  1:缺陷区域与周围灰度差值(整体性)  2:缺陷中心点与一次缺陷区域灰度差值(局部性)
//                Mat temp_gray = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
//                Mat mask = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
//                double mean_in = mean(temp_gray, mask)[0];
//                double mean_out = mean(temp_gray, ~mask)[0];
//                double  defect_areath = mean_in - mean_out;//缺陷区域与周围灰度差值(整体性)

//                double mean_ceguang = mean(temp_gray)[0];  //缺陷中心点与一次缺陷区域灰度差值(局部性)
//                double coreth = img_gray.at<uchar>(y_point, x_point) - mean_ceguang;

//                vector<vector<Point>> contours1;
//                findContours(mask, contours1, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
//                double domainnum = contours1.size();      //疑似白点缺陷连通域个数判断去除噪点干扰
//                if (domainnum > dotdomainnum)
//                {
//                    continue;
//                }
//                if (defect_areath >= defectouterth && coreth >= corewholeth && ((X_2 - X_1)*(Y_2 - Y_1) > 10) && (max(w / h, h / w) < whitePoint_w_h))//长宽比一般在1.8之内
//                {
//                    //细筛选白点缺陷
//                    double  minStrength, maxStrength;
//                    Mat image_highlight, image_lowlight;
//                    Mat filter = img_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1)); //提取最小正接白点缺陷区域
//                    Mat mask2 = th_result(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1)); //提取最小正接白点缺陷区域掩膜
//                    bitwise_and(filter, mask2, image_highlight);                 //提取白点的高亮度区域
//                    cv::minMaxLoc(image_highlight, &minStrength, &maxStrength);  //查找此处的最亮点处灰度值
//                    minStrength = mean(filter, ~mask2)[0];	                     //计算低亮度灰度均值
//                    double peakGray = maxStrength - minStrength;

//                    Mat sidefilter = img_ceguang(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1)); //提取正接侧光图缺陷区域
//                    cv::Mat meanGray1;
//                    cv::Mat stdDev1;
//                    cv::meanStdDev(sidefilter, meanGray1, stdDev1);
//                    double stddev1 = stdDev1.at<double>(0, 0);

//                    Mat ceguangDebug1 = img_ceguang.clone();
//                    rectangle(ceguangDebug1, cvPoint(X_1, Y_1), cvPoint(X_2, Y_2), Scalar(255, 255, 255), 2, 8, 0);

//                    if (peakGray > spotpeak && stddev1<siecevariance)
//                    {
//                        result = true;
//                        CvPoint top_lef4 = cvPoint(X_1 - 10, Y_1 - 10);
//                        CvPoint bottom_right4 = cvPoint(X_2 + 20, Y_2 + 20);
//                        rectangle(white_yiwu, top_lef4, bottom_right4, Scalar(0, 0, 0), 5, 8, 0);
//                        Point p3(x_point, y_point);
//                        circle(white_yiwu, p3, 12, Scalar(0, 0, 255), 1, 8, 0);

//                        string ceghaung = "cegaung:" + to_string(ceguang_th ) + " " + "th:" + to_string(defect_areath) + " " + "ceth:" + to_string(coreth) + " " + "peth:" + to_string(peakGray) + " " + "area:" + to_string(area) + " " + "ceVar:" + to_string(stddev1 ) + " " + "stddev:" + to_string(stddev);
//                        putText(white_yiwu, ceghaung, Point(30, 30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1, 8, 0);
//                    }
//                }

//            }
//        }
//    }
//    if (result == true)
//    {
//        *causecolor = "白点";
//        *mresult = white_yiwu;
//    }
//    return result;
//}

/*=========================================================
* 函 数 名: fanzhuan
* 功能描述: 镜像翻转 1上下 2左右
=========================================================*/
Mat fanzhuan(Mat src, int width, int height, int direction)
{
    vector<Point2f> src_corner(4);
    src_corner[0] = Point(0, 0);
    src_corner[1] = Point(0, height-1);
    src_corner[2] = Point(width-1, height-1);
    src_corner[3] = Point(width-1, 0);
    //透视变换后的四个点所在坐标
    vector<Point2f> dst_corner(4);
    if(direction == 1)
    {
        dst_corner[0] = Point(0, height-1);
        dst_corner[1] = Point(0, 0);
        dst_corner[2] = Point(width-1, 0);
        dst_corner[3] = Point(width-1, height-1);
    }
    if(direction == 2)
    {
        dst_corner[0] = Point(width-1, 0);
        dst_corner[1] = Point(width-1, height-1);
        dst_corner[2] = Point(0, height-1);
        dst_corner[3] = Point(0, 0);
    }

    Mat M = getPerspectiveTransform(src_corner, dst_corner);
    Mat resultImg;
    warpPerspective(src, resultImg, M, resultImg.size(), INTER_LINEAR);
    return resultImg;
}

/*=========================================================
* 函 数 名：bool Frame(Mat src_img)
* 边框检测函数
=========================================================*/
bool Frame(Mat src_img, Mat *mresult, QString *causecolor)
{
    bool TestResult_Frame = false;
    Mat img_gray, img_bin, img_FinalResult;
    img_FinalResult = src_img.clone();
    img_gray=img_FinalResult;
    //cvtColor(src_img, img_gray, CV_BGR2GRAY);
    Mat img_result(img_gray.size(), img_gray.type(), Scalar(0));
    Mat img_Rangle(180, 180, img_gray.type(), Scalar(0));                           //R角
    Mat img_WaterDrop(380, 380, img_gray.type(), Scalar(0));                        //水滴
    threshold(img_gray, img_bin, 30, 255, CV_THRESH_BINARY);
    medianBlur(img_bin, img_bin, 3);
    vector<vector<Point>> contours1, contours2;
    Rect rect;																        //最小正外接矩形
    findContours(img_bin, contours1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);       //轮廓检索

    for (vector<int>::size_type i = 0; i < contours1.size(); i++)
    {
        double area = contourArea(contours1[i]);
        if (area > 3000000 && area < 5000000)
        {
            rect = cv::boundingRect(contours1[i]);                                   //最小外接矩形提取
            Mat Cir_Rect(rect.height, rect.width, img_gray.type(), Scalar(255));
            Cir_Rect.copyTo(img_result(Rect(rect.tl().x, rect.tl().y, rect.width, rect.height)));
            drawContours(img_result, contours1, i, Scalar(0), -1);
            img_Rangle.copyTo(img_result(Rect(0, 0, 180, 180)));
            img_Rangle.copyTo(img_result(Rect(0, img_result.rows - 180, 180, 180)));
            img_Rangle.copyTo(img_result(Rect(img_result.cols - 180, 0, 180, 180)));
            img_Rangle.copyTo(img_result(Rect(img_result.cols - 180, img_result.rows - 180, 180, 180)));
            img_WaterDrop.copyTo(img_result(Rect(0, img_result.rows / 2 - 190, 380, 380)));
            break;
        }
    }

    findContours(img_result, contours2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    vector<RotatedRect> minRect(contours2.size());                                //最小斜外接矩形
    int Num_FrameDefect = 0;
    Point2f rect_points[4];
    for (vector<int>::size_type i = 0; i < contours2.size(); i++)
    {
        minRect[i] = minAreaRect(Mat(contours2[i]));
        if (minRect[i].size.height >= 7 && minRect[i].size.width >= 7)
        {
            Num_FrameDefect++;
            minRect[i].points(rect_points);
            for (int j = 0; j < 4; j++)
                line(img_FinalResult, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255), 4, 8);
        }
    }

    if (Num_FrameDefect != 0)
    {
        TestResult_Frame = true;
        *causecolor = "边框";
        *mresult = img_FinalResult;
    }

    return TestResult_Frame;
}

/*=========================================================
* 函 数 名: redBlueSpot
* 功能描述: 红蓝斑检测
=========================================================*/
bool redBlueSpot(Mat image_black1,Mat *mresult,QString *causecolor)
{
    bool result =false;
    Mat image_black=image_black1.clone();
    Mat img_hsv;
    cvtColor(image_black,img_hsv, COLOR_BGR2HSV);
    Mat img_out;
    Mat img_out_blue;

    inRange(img_hsv, Scalar(156, 40, 40), Scalar(180, 255, 255), img_out); //红色斑块提取
    inRange(img_hsv, Scalar(100, 43, 46), Scalar(124, 255, 255), img_out_blue);	//蓝色斑块提取
    bitwise_or(img_out, img_out_blue, img_out);

    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(5,5));
    img_out(Rect(0,0,img_out.cols-1,10))=uchar(0);        //去除四个边界的影响
    img_out(Rect(0,img_out.rows-11,img_out.cols-1,10))=uchar(0);
    img_out(Rect(0,0,150,img_out.rows-1))=uchar(0);
    img_out(Rect(img_out.cols-11,0,10,img_out.rows-1))=uchar(0);
    Mat dilate1;
    dilate(img_out, dilate1, structure_element1);

    vector<vector<Point>> contours;
    findContours( dilate1, contours,  CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    int count_spot =0;
    for(vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area>redBlue_area_lower and area<redBlue_area_upper)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            float w=boundRect[i].width;
            float h=boundRect[i].height;
            int X_1=boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1=boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2=boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2=boundRect[i].br().y;//矩形右下角Y坐标值
            X_1=X_1-25-int(w);
            Y_1=Y_1-25-int(h);
            X_2=X_2+25+int(w);
            Y_2=Y_2+25+int(h);
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

            CvPoint top_lef4 = cvPoint(X_1, Y_1);
            CvPoint bottom_right4 = cvPoint(X_2, Y_2);
            rectangle(image_black , top_lef4, bottom_right4, Scalar(0,0,255), 5, 8, 0);
            int x_point = image_black.cols / 1.2;
            int y_point = image_black.rows /10;
            count_spot++;

        }
    }
    if(count_spot>=redBlue_numupper)
    {
        result =true;
        *causecolor="红蓝斑";
        *mresult=image_black;
    }
    //image_black.release();
    return result;
}

/*=========================================================
* 函 数 名: 黑屏大块屏幕漏光
* 功能描述: 大块漏光检测
* 输入：    主相机黑白相机黑底图像
=========================================================*/
bool Leakage(Mat image_black, Mat *mresult, QString *causecolor)
{
    bool result = false;
    Mat img_gray;
    img_gray=image_black.clone();
    Mat th1,th2;
    threshold(img_gray, th1, 50, 255, CV_THRESH_BINARY);
    adaptiveThreshold(img_gray, th2, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, leakage_bolckSize, leakage_delta);
    vector<vector<Point>> contours;
    th2(Rect(70, 70, th2.cols - 140, th2.rows-140)) = uchar(0);
    findContours(th2, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    //异形屏幕检测外边缘处
    if(Model_Mod_type=="R角水滴屏")
    {
        bitwise_and(louguang_mask, th2, th2);
    }
    vector<Rect> boundRect(contours.size());
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > leakage_area_lower && area < leakage_area_upper)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            float w = boundRect[i].width;
            float h = boundRect[i].height;
            int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
            Moments m = moments(contours[i]);
            int x_point = int(m.m10 / m.m00);//X_1+round(w/2);
            int y_point = int(m.m01 / m.m00);
            if (true)
            {
                int x_lt = X_1 - 5;
                if (x_lt < 0)
                {
                    x_lt = 0;
                }
                int y_lt = Y_1 - 5;
                if (y_lt < 0)
                {
                    y_lt = 0;
                }
                int x_rt = X_2 + 5;
                if (x_rt > img_gray.size[1] - 1)
                {
                    x_rt = img_gray.size[1] - 1;
                }
                int y_rt = Y_2 + 5;
                if (y_rt > img_gray.size[0] - 1)
                {
                    y_rt = img_gray.size[0] - 1;
                }
                Mat IMG_TEMP_SHIAIXUAN = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                Mat mask = th1(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                double mean_in_gray = mean(IMG_TEMP_SHIAIXUAN, mask)[0];
                double mean_out_gray = mean(IMG_TEMP_SHIAIXUAN, ~mask)[0];
                double intensity = mean_in_gray - mean_out_gray;
                if (intensity >= leakageth  && (8< max(w / h, h / w) < leakage_w_h))//长线状PCB处漏光
                {
                    CvPoint top_lef4 = cvPoint(x_lt, y_lt);
                    CvPoint bottom_right4 = cvPoint(x_rt, y_rt);
                    rectangle(image_black, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
                    string b = "Leakage";
                    //putText(white_yiwu, b, Point(x_point, y_point), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 255, 0), 1);
                    result = true;
                }
            }
        }
    }
    if (result == true)
    {
        *causecolor = "黑底边角漏光";
        *mresult = image_black;
    }
    return result;
}

/*=========================================================
* 函 数 名: Scratch
* 功能描述: 屏幕划伤缺陷判断
* 输入：左右侧面相机白底图像及侧光整张图像
* 输出：划伤缺陷检测结果图像和检测Result
* 时间：2020年5月18日
* 其他：
=========================================================*/
bool Scratch(Mat white, Mat ceguang, Mat *mresult, QString *causecolor)
{
    bool result = false;
    Mat img_gray = white.clone();
    Mat img_ceguang = ceguang.clone();
    Mat Filer;
    medianBlur(img_gray, Filer, 3);//中值滤波滤除椒盐噪声,缺点耗时26毫秒 奇数半径越大效果越强
    Mat th1;
    adaptiveThresholdCustom(Filer, th1, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, scratch__bolckSize, scratch__delta,1);
    Mat th_mask = th1.clone();
    Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));
    morphologyEx(th1, th1, CV_MOP_OPEN, element);

    //易撕贴以及边缘部分屏蔽
    //th1(Rect(th1.cols - 261, th1.rows - 351, 260, 350)) = uchar(0);//易撕贴部分设置右下角
    th_mask(Rect(0, 0, th1.cols - 1, 51)) = uchar(0);                  //上
    th_mask(Rect(0, th1.rows - 48, th1.cols - 1, 48)) = uchar(0);      //下
    th_mask(Rect(0, 0, 55, th1.rows - 1)) = uchar(0);                 //左   180
    th_mask(Rect(th1.cols - 50, 0, 50, th1.rows - 1)) = uchar(0);      //右

    th1(Rect(0, 0, th1.cols - 1, 51)) = uchar(0);                  //上
    th1(Rect(0, th1.rows - 48, th1.cols - 1, 48)) = uchar(0);      //下
    th1(Rect(0, 0, 55, th1.rows - 1)) = uchar(0);                 //左   180
    th1(Rect(th1.cols - 50, 0, 50, th1.rows - 1)) = uchar(0);      //右

    vector<vector<Point>> contours;
    findContours(th1, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());      //Rect类型的vector容器boundRect存放正外接矩形
    vector<RotatedRect> roRect(contours.size());  //定义Rect类型的vector容器roRect存放最小外接矩形
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > scratch_lowerArea && area < scratch_higherArea)//scratch_lowerArea
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            RotatedRect rect = minAreaRect(contours[i]); //包覆轮廓的最小斜矩形 划伤缺陷有旋转特点
            double w1 = rect.size.height;
            double h1 = rect.size.width;
            double longShortRatio = max(w1 / h1, h1 / w1); //防止旋转特点正好45度为1
            float w = boundRect[i].width;
            float h = boundRect[i].height;
            int X_1 = boundRect[i].tl().x; //矩形左上角X坐标值
            int Y_1 = boundRect[i].tl().y; //矩形左上角Y坐标值
            int X_2 = boundRect[i].br().x; //矩形右下角X坐标值
            int Y_2 = boundRect[i].br().y; //矩形右下角Y坐标值

            Moments m = moments(contours[i]);
            int x_point = int(m.m10 / m.m00);
            int y_point = int(m.m01 / m.m00);
            if (longShortRatio > scratch_lowerLongShort && longShortRatio<scratch_higherLongShort && min(w1, h1)>scratch_lowerWidth && min(w1, h1) < scratch_higherWidth && max(w1, h1) > scratch_lowerLength && max(w1, h1) < scratch_higherLength)
            {
                int border = 8;
                int x_lt = X_1 - border;
                if (x_lt < 0)
                {
                    x_lt = 0;
                }
                int y_lt = Y_1 - border;
                if (y_lt < 0)
                {
                    y_lt = 0;
                }
                int x_rt = X_2 + border;
                if (x_rt > img_gray.size[1] - 1)
                {
                    x_rt = img_gray.size[1] - 1;
                }
                int y_rt = Y_2 + border;
                if (y_rt > img_gray.size[0] - 1)
                {
                    y_rt = img_gray.size[0] - 1;
                }
                //扩大候选范围来对气泡进行排除，2种气泡1:白底下可见侧光不可见 2：白底侧光均可见
                int border1 = 15;
                int x_lt1 = X_1 - border1;
                if (x_lt1 < 0)
                {
                    x_lt1 = 0;
                }
                int y_lt1 = Y_1 - border1;
                if (y_lt1 < 0)
                {
                    y_lt1 = 0;
                }
                int x_rt1 = X_2 + border1;
                if (x_rt1 > img_gray.size[1] - 1)
                {
                    x_rt1 = img_gray.size[1] - 1;
                }
                int y_rt1 = Y_2 + border1;
                if (y_rt1 > img_gray.size[0] - 1)
                {
                    y_rt1 = img_gray.size[0] - 1;
                }
                //排除划痕的干扰，划伤缺陷具有旋转性需提取最小外接矩形
                Point2f center = rect.center; //最小外接矩形中心点坐标
                Mat rot_mat = getRotationMatrix2D(center, rect.angle, 1.0);//求旋转矩阵
                Mat rot_image;
                Size dst_sz(white.size());
                warpAffine(white, rot_image, rot_mat, dst_sz);//原图像旋转rot_image 仿射变换

                float rect_width;
                float rect_height;

                float rect_w;
                float rect_h;
                if (rect.angle == -90)  //旋转角度-90时会有长宽互换
                {
                    rect_width = rect.size.height;
                    rect_height = rect.size.width;
                    rect_w = rect_width;
                    rect_h = rect_height;
                }
                else
                {
                    rect_width = rect.size.width;
                    rect_height = rect.size.height;
                    rect_w = rect_width;
                    rect_h = rect_height;
                }
                int leftTopx = 0;
                int leftTopy = 0;
                if (center.x - (rect_w / 2) < 0)
                    leftTopx = 0;
                else
                    leftTopx = center.x - (rect_w / 2);
                if (center.y - (rect_h / 2) < 0)
                    leftTopy = 0;
                else
                    leftTopy = center.y - (rect_h / 2);

                if (center.x + (rect_width / 2) > white.cols - 1) //防止越界
                    rect_width = rect_width / 2 + white.cols - 1 - center.x;
                if (center.y + (rect_height / 2) > white.rows - 1)
                    rect_height = rect_height / 2 + white.rows - 1 - center.y;

                Mat result1 = rot_image(Rect(leftTopx, leftTopy, rect_width, rect_height));
                //侧光图像最小外接矩形提取
                Mat rot_ceguang;
                warpAffine(ceguang, rot_ceguang, rot_mat, dst_sz);//原侧光图像旋转rot_image
                Mat temp_ceguang = rot_ceguang(Rect(leftTopx, leftTopy, rect_width, rect_height));
                //二值图掩膜
                Mat rot_th1;
                //warpAffine(th1, rot_th1, rot_mat, dst_sz); //原侧光图像旋转rot_image
                warpAffine(th_mask, rot_th1, rot_mat, dst_sz); //原侧光图像旋转rot_image
                Mat mask_ceguang = rot_th1(Rect(leftTopx, leftTopy, rect_width, rect_height));

                double mean_in_ceguang = mean(temp_ceguang, mask_ceguang)[0];
                double mean_out_ceguang = mean(temp_ceguang, ~mask_ceguang)[0];
                double ceguang_th = mean_in_ceguang - mean_out_ceguang;
                //排除划痕干扰限定条件1
                if (abs(ceguang_th) >= filmscratch)
                {
                    continue;
                }
                //排除划痕干扰限定条件2
                cv::Mat meanGray;
                cv::Mat stdDev;
                cv::meanStdDev(temp_ceguang, meanGray, stdDev);
                double stddev = stdDev.at<double>(0, 0);
                if (stddev >= scratchbubbleth)
                {
                    continue;
                }
                //排除白底图像上的明显气泡，计算缺陷外围的灰度方差，若存在气泡则灰度方差明显较大
                Mat image_bubule = img_gray(Rect(x_lt1, y_lt1, x_rt1 - x_lt1, y_rt1 - y_lt1));
                //Mat mask_bubble = th1(Rect(x_lt1, y_lt1, x_rt1 - x_lt1, y_rt1 - y_lt1));
                Mat mask_bubble = th_mask(Rect(x_lt1, y_lt1, x_rt1 - x_lt1, y_rt1 - y_lt1));
                cv::Mat meanGray1;
                cv::Mat stdDev1;
                cv::meanStdDev(image_bubule, meanGray1, stdDev1, ~mask_bubble);
                double stddev1 = stdDev1.at<double>(0, 0);
                if (stddev1 >= scratchbubbleth1)
                {
                    continue;
                }
                //划伤缺陷颜色筛选
                //Mat mask = th1(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                Mat mask = th_mask(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                Mat Image_Gray = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                double mean_in_gray = mean(Image_Gray, mask)[0];
                double mean_out_gray = mean(Image_Gray, ~mask)[0];
                double defect_areath = mean_in_gray - mean_out_gray;

                double mean_in_gray1 = mean(result1, mask_ceguang)[0];
                double mean_out_gray1 = mean(result1, ~mask_ceguang)[0];
                double mindefect_areath = mean_in_gray1 - mean_out_gray1;

                //由最小外接矩形4个点坐标绘图
                roRect[i] = minAreaRect(Mat(contours[i])); //由轮廓（点集）确定出最小外接矩形并绘制
                Point2f pts[4] = { };
                roRect[i].points(pts); //旋转矩形的四个端点存储进pts

                if (defect_areath >= scratchbrighth && mindefect_areath >= minscratchbrighth) //划伤亮度筛选  minscratchbrighth
                {
                    result = true;
                    CvPoint top_lef4 = cvPoint(x_lt, y_lt);
                    CvPoint bottom_right4 = cvPoint(x_rt, y_rt);
                    rectangle(white, top_lef4, bottom_right4, Scalar(0, 0, 0), 5, 8, 0);

                    //用line方法，根据旋转矩形的四个角点画出矩形
                    line(white, pts[0], pts[1], Scalar(100, 100, 100), 1, 8, 0);
                    line(white, pts[0], pts[3], Scalar(100, 100, 100), 1, 8, 0);
                    line(white, pts[2], pts[1], Scalar(100, 100, 100), 1, 8, 0);
                    line(white, pts[2], pts[3], Scalar(100, 100, 100), 1, 8, 0);

                    string ceghaung = "cegaung:" + to_string(ceguang_th) + " " + "th:" + to_string(defect_areath) + " " + " " + "area:" + to_string(area) + " " + " " + "stddev:" + to_string(stddev);
                    putText(white, ceghaung, Point(30, 30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1, 8, 0);
                    imwrite("D:\\Scratch_result-0410.bmp", white);

                }
            }
        }
    }
    if (result == true)
    {
        *causecolor = "划伤";
        *mresult = white;
    }
    return result;
}

/*=========================================================
* 函 数 名: StickerLocate
* 功能描述: 易撕贴定位子模块，若检测到易撕贴返回
* 功能描述: 定位模板，未检测到易撕贴返回空白模板
* 输入：    相机拍摄侧管图像
* 输出：    定位易撕贴位置的模板
=========================================================*/
Mat StickerLocate(Mat ceguang)
{
    bool IsSticker = false;  //检测是否存在易撕贴标志位是存在易撕贴
    int sticker_width = 230; //易撕贴的宽度
    int sticker_height = 380;//易撕贴的高度
    Mat StickerLocate;       //返回定位易撕贴的定位模板
    Mat Left_Upper = ceguang(Rect(0, 0, sticker_width, sticker_height));                                                        //侧光左上角位置
    Mat Left_Down = ceguang(Rect(0, ceguang.rows - sticker_height, sticker_width, sticker_height));                             //侧光左下角位置
    Mat Right_Down = ceguang(Rect(ceguang.cols - sticker_width, ceguang.rows - sticker_height, sticker_width, sticker_height)); //侧光右下角位置
    Mat Right_Upper = ceguang(Rect(ceguang.cols - sticker_width, 0, sticker_width, sticker_height));                            //侧光右上角位置
    Mat Cut_Left_Upper, Cut_Left_Down, Cut_Right_Down, CutRight_Upper;	                                                        //定义侧光的分割图像
    //分割图像二值化
    adaptiveThreshold(Left_Upper, Cut_Left_Upper, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15, -3);
    adaptiveThreshold(Left_Down, Cut_Left_Down, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15, -3);
    adaptiveThreshold(Right_Down, Cut_Right_Down, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15, -3);
    adaptiveThreshold(Right_Upper, CutRight_Upper, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15, -3);
    Mat Cut_Compound = Mat::zeros(ceguang.size(), CV_8UC1);//生成分割空白图像备用
    //分割图像拷贝到空白图像
    Cut_Left_Upper.copyTo(Cut_Compound(Rect(0, 0, sticker_width, sticker_height)));
    Cut_Left_Down.copyTo(Cut_Compound(Rect(0, Cut_Compound.rows - sticker_height, sticker_width, sticker_height)));
    Cut_Right_Down.copyTo(Cut_Compound(Rect(Cut_Compound.cols - sticker_width, Cut_Compound.rows - sticker_height, sticker_width, sticker_height)));
    CutRight_Upper.copyTo(Cut_Compound(Rect(Cut_Compound.cols - sticker_width, 0, sticker_width, sticker_height)));
    //膨胀扩大信息
    Mat structure_element = getStructuringElement(MORPH_RECT, Size(4, 4));
    Mat Binaryzation;
    dilate(Cut_Compound, Binaryzation, structure_element);
    //轮廓遍历检测是否存在易撕贴
    vector<vector<Point>> contours;
    findContours(Binaryzation, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 2000 && area < 10000)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            float w = boundRect[i].width;
            float h = boundRect[i].height;
            float longShortRatio_Standard = max(w / h, h / w);
            int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
            circle(ceguang, Point(X_1, Y_1), 0, Scalar(255, 255, 255), 5, 8, 0);
            circle(ceguang, Point(X_2, Y_2), 0, Scalar(255, 255, 255), 5, 8, 0);
            int border = 5;//125
            if (w > 125 && w < 220 && h>290 && h < 375 && longShortRatio_Standard>1.5 && longShortRatio_Standard < 2.4)
            {
                int x_lt = X_1 - border;
                if (x_lt < 0)
                {
                    x_lt = 0;
                }
                int y_lt = Y_1 - border;
                if (y_lt < 0)
                {
                    y_lt = 0;
                }
                int x_rt = X_2 + border;
                if (x_rt > ceguang.size[1] - 1)
                {
                    x_rt = ceguang.size[1] - 1;
                }
                int y_rt = Y_2 + border;
                if (y_rt > ceguang.size[0] - 1)
                {
                    y_rt = ceguang.size[0] - 1;
                }
                Mat BlankImage = Mat::zeros(ceguang.size(), CV_8UC1);         //生成空白图像
                BlankImage(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt)) = uchar(255);//生成易撕贴处模板
                bitwise_not(BlankImage, StickerLocate);                       //对模板取反
                IsSticker = true;                                             //检测到易撕贴
                break;
            }
        }
    }
    if (!IsSticker)                                                          //若条件不符合面积长度宽度要求,或者未检测到易撕贴
    {
        Mat BlankImage = Mat::zeros(ceguang.size(), CV_8UC1);                //生成空白图像
        bitwise_not(BlankImage, StickerLocate);                              //对模板取反
        return StickerLocate;
    }
    else return StickerLocate;
}

/*=========================================================
* 函 数 名: louye7
* 功能描述: 漏液检测
=========================================================*/
bool louye7(Mat grayImg,Mat *mresult ,QString *causecolor)
{
    bool result =false;
    Mat img_gray;
    cvtColor(grayImg, img_gray, CV_BGR2GRAY);
    Mat th1;
    threshold(img_gray, th1, 10, 255, CV_THRESH_BINARY);//100->60
    for (int i=0;i<th1.rows;i=i+150) {
        for (int j = 0;j<th1.cols;j=j+150) {
            int width=151,height=151;
            if(j+width>th1.cols-1)
                width =th1.cols-1 -j;
            if(i+height>th1.rows-1)
                height=th1.rows-1-i;
            Mat cut_image=th1(Rect(j,i,width,height));
            int mean_cut = mean(cut_image)[0];
            //qDebug()<<mean_cut<<endl;
            if(mean_cut+gray_thre < 255)
            {
                //                 qDebug()<<mean_cut<<endl;
                result =true;
                rectangle(grayImg ,Rect(j,i,width,height), Scalar(0,0,255), 2, 8, 0);
            }
        }
    }
    if(result == true)
    {
        *mresult=grayImg;
        *causecolor ="漏液";
        return result;
    }
    return result;
}

/*=========================================================
* 函 数 名: TransverseLine
* 功能描述: 横线检测属于少线的一种类似于少线
* 输入：    主相机白底图像
=========================================================*/
bool TransverseLine(Mat white, Mat *mresult, QString *causecolor)
{
    bool result = false;
    Mat img_gray = white.clone();
    Mat th_result;
    adaptiveThreshold(img_gray, th_result, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 5.1);
    vector<vector<Point>> contours;
    th_result(Rect(0, th_result.rows - 358, 246, 358)) = uchar(0);	             //易撕贴部分
    th_result(Rect(0, 0, th_result.cols - 1, 5)) = uchar(0);                     //上边
    th_result(Rect(0, th_result.rows - 5, th_result.cols - 1, 5)) = uchar(0);    //下边
    th_result(Rect(0, 0, 4, th_result.rows - 1)) = uchar(0);                     // 左边
    th_result(Rect(th_result.cols - 5, 0, 5, th_result.rows - 1)) = uchar(0);    //右边
    findContours(th_result, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    {
        for (vector<int>::size_type i = 0; i < contours.size(); i++)
        {
            //======================判断是否为少线情况=======================
            double area = contourArea(contours[i]);
            if (area > 20 && area < 200000)
            {
                boundRect[i] = boundingRect(Mat(contours[i]));
                float w = boundRect[i].width;
                float h = boundRect[i].height;
                int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
                int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
                int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
                int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
                Moments m = moments(contours[i]);
                int x_point = int(m.m10 / m.m00);
                int y_point = int(m.m01 / m.m00);
                int long_lackLine = boundRect[i].width;
                int short_lackLine = boundRect[i].height;
                if (short_lackLine > long_lackLine) //长宽对位
                {
                    int value = short_lackLine;
                    short_lackLine = long_lackLine;
                    long_lackLine = value;
                }
                int longShortRatio = long_lackLine / short_lackLine; //长宽比
                int border = 3;
                int x_lt = X_1 - border;
                if (x_lt < 0)
                {
                    x_lt = 0;
                }
                int y_lt = Y_1 - border;
                if (y_lt < 0)
                {
                    y_lt = 0;
                }
                int x_rt = X_2 + border;
                if (x_rt > img_gray.size[1] - 1)
                {
                    x_rt = img_gray.size[1] - 1;
                }
                int y_rt = Y_2 + border;
                if (y_rt > img_gray.size[0] - 1)
                {
                    y_rt = img_gray.size[0] - 1;
                }

                //边界位置处不进行检测，防止检测到外边界处
                int corner=10;
                if((x_point<corner and y_point<corner) or (th_result.cols-x_point<corner and th_result.rows-y_point<corner) or(x_point<corner and th_result.rows-y_point<corner) or (th_result.cols-x_point<corner and y_point<corner))
                {
                    continue;
                }

                Mat imagedoubt = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                Mat mask = th_result(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                double mean_in_gray = mean(imagedoubt, mask)[0];
                double mean_out_gray = mean(imagedoubt, ~mask)[0];
                double intensity = mean_out_gray-mean_in_gray;

                string a = "long:" + to_string(long_lackLine) + "short:" + to_string(short_lackLine)+"th:"+to_string(intensity);
                //putText(white, a, Point(X_1 - 400, Y_1), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 5, 8, 0);

                if (longShortRatio >= 25 && short_lackLine < 30 && long_lackLine >= 800 && intensity>20)
                {
                    result = true;
                    CvPoint top_lef4 = cvPoint(x_lt, y_lt);
                    CvPoint bottom_right4 = cvPoint(x_rt, y_rt);
                    rectangle(white, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
                }
            }
        }
    }
    if (result == true)
    {
        *mresult = white;
        *causecolor = "横线";
        result = true;
    }
    return result;
}


/*=========================================================
* 函 数 名: Abshow_White_Detect
* 功能描述: 主黑白相机白底下显示异常检测
* 输入：    主黑白相机白底图像
* 编制人：秦立峰
* 修改日期：2020年07月06日
=========================================================*/
bool Abshow_White_Detect(Mat white1, Mat *mresult, QString *causecolor)
{
    int abshow_neigh_size = 10;
    bool result = false;
    //白底亮度异常
    cv::Mat abshow_white, abshow_white1;
    abshow_white1 = white1;

    Mat th1;
    threshold(abshow_white1, th1, 20, 255, CV_THRESH_BINARY);

    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(15, 15));
    erode(th1, th1, structure_element2);

    bitwise_and(abshow_white1, th1, abshow_white);

    //Mat Slid_Mlow, Slid_Mhigh;
    double Slid_Low, Slid_High, Slid_Current;
    Mat Sliding_Win;
    Sliding_Win = abshow_white(Rect(500, 500, abshow_neigh_size, abshow_neigh_size));
    Mat mask1 = th1(Rect(500, 500, abshow_neigh_size, abshow_neigh_size));
    Slid_Current = mean(Sliding_Win, mask1)[0];
    Slid_Low = Slid_Current;
    Slid_High= Slid_Current;
    CvPoint Slid_Low_Win, Slid_High_Win;
    for (int i = 0; i < (abshow_white.cols- abshow_neigh_size); i=i+ abshow_neigh_size)
    {
        for (int j = 0; j < abshow_white.rows; j=j+ abshow_neigh_size)
        {
            Sliding_Win = abshow_white(Rect(i, j, abshow_neigh_size, abshow_neigh_size));
            //double meanGrayin_Suspect = mean(imageSuspected, mask1)[0];
            Mat mask=th1(Rect(i, j, abshow_neigh_size, abshow_neigh_size));
            Slid_Current = mean(Sliding_Win, mask)[0];
            if (Slid_Low >= Slid_Current&&Slid_Current !=0)
            {
                Slid_Low = Slid_Current;
                Slid_Low_Win = cvPoint(i, j);
            }
            else if (Slid_High <= Slid_Current&&Slid_Current != 0)
            {
                Slid_High = Slid_Current;
                Slid_High_Win = cvPoint(i, j);
            }
        }
    }
    double RD_U = Slid_Low / Slid_High;

    if (RD_U < white_gray_lower|| Slid_High<100)
    {
        result = true;

        CvPoint Slid_Low_Win_br = cvPoint(Slid_Low_Win.x + 10, Slid_Low_Win.y + 10);
        CvPoint Slid_High_Win_br = cvPoint(Slid_High_Win.x + 10, Slid_High_Win.y + 10);

        //判断缺陷得区域框得大些
        //低灰度区域
        int x_lt_high = Slid_Low_Win.x - 100;
        if (x_lt_high<0)
        {
            x_lt_high = 0;
        }
        int y_lt_high = Slid_Low_Win.y - 100;
        if (y_lt_high<0)
        {
            y_lt_high = 0;
        }
        int x_rt_high = Slid_Low_Win_br.x + 100;
        if (x_rt_high>white1.cols - 1)
        {
            x_rt_high = white1.cols;
        }
        int y_rt_high = Slid_Low_Win_br.y + 100;
        if (y_rt_high>white1.rows - 1)
        {
            y_rt_high = white1.rows;
        }

        //高灰度区域
        int x_lt_low = Slid_High_Win.x - 100;
        if (x_lt_low<0)
        {
            x_lt_low = 0;
        }
        int y_lt_low = Slid_High_Win.y - 100;
        if (y_lt_low<0)
        {
            y_lt_low = 0;
        }
        int x_rt_low = Slid_High_Win_br.x + 100;
        if (x_rt_low>white1.cols - 1)
        {
            x_rt_low = white1.cols;
        }
        int y_rt_low = Slid_High_Win_br.y + 100;
        if (y_rt_low>white1.rows - 1)
        {
            y_rt_low = white1.rows;
        }

        CvPoint High_Region_TL = cvPoint(x_lt_high, y_lt_high);
        CvPoint High_Region_BT = cvPoint(x_rt_high, y_rt_high);
        CvPoint Low_Region_TL = cvPoint(x_lt_low, y_lt_low);
        CvPoint Low_Region_BT = cvPoint(x_rt_low, y_rt_low);

        rectangle(white1, Low_Region_TL, Low_Region_BT, Scalar(255, 255, 255), 8, 8, 0);//绿色异物
        rectangle(white1, High_Region_TL, High_Region_BT, Scalar(0, 0, 0), 8, 8, 0);//绿色异物
    }
    if (result == true)
    {
        *causecolor = "显示异常";
        *mresult = white1;
    }
    return result;
}



/*=========================================================
* 函 数 名: ColorLine
* 功能描述: 彩线检测
* 功能描述: 黑底下彩色线条检测
* 输入：    相机拍摄黑底图像
* 输出：    彩线检测结果
=========================================================*/
bool ColorLine(Mat black, Mat *mresult, QString *causecolor)
{
    bool result = false;
    Mat img_gray = black.clone();;
    Mat th2,th1;
    adaptiveThreshold(img_gray, th2, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, -3);
    vector<vector<Point>> contours;
    //异形屏幕去除刘海干扰
    if(Model_Mod_type=="R角水滴屏")
    {
        bitwise_and(main_mask, th2, th2);
    }
    th2(Rect(th2.cols - 240, th2.rows - 315, 240, 315)) = uchar(0);	     //去除易撕贴部分
    th2(Rect(0, 0, th2.cols - 1, 30)) = uchar(0);                         //上边
    th2(Rect(0, th2.rows - 30, th2.cols - 1, 30)) = uchar(0);              //下边
    th2(Rect(0, 0, 30, th2.rows - 1)) = uchar(0);                         // 左边
    th2(Rect(th2.cols - 30, 0, 30, th2.rows - 1)) = uchar(0);              //右边

    findContours(th2, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    {
        for (vector<int>::size_type i = 0; i < contours.size(); i++)
        {
            double area = contourArea(contours[i]);
            if (area > 500 && area < 2000)
            {
                boundRect[i] = boundingRect(Mat(contours[i]));
                float w = boundRect[i].width;
                float h = boundRect[i].height;
                int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
                int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
                int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
                int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
                Moments m = moments(contours[i]);
                int x_point = int(m.m10 / m.m00);
                int y_point = int(m.m01 / m.m00);
                int long_lackLine = boundRect[i].width;
                int short_lackLine = boundRect[i].height;
                if (short_lackLine > long_lackLine) //长宽对位
                {
                    int value = short_lackLine;
                    short_lackLine = long_lackLine;
                    long_lackLine = value;
                }
                int longShortRatio = long_lackLine / short_lackLine; //长宽比
                if (longShortRatio >= 30 && short_lackLine < 10 && long_lackLine > 500)
                {
                    int border = 5;
                    int x_lt = X_1 - border;
                    if (x_lt < 0)
                    {
                        x_lt = 0;
                    }
                    int y_lt = Y_1 - border;
                    if (y_lt < 0)
                    {
                        y_lt = 0;
                    }
                    int x_rt = X_2 + border;
                    if (x_rt > img_gray.size[1] - 1)
                    {
                        x_rt = img_gray.size[1] - 1;
                    }
                    int y_rt = Y_2 + border;
                    if (y_rt > img_gray.size[0] - 1)
                    {
                        y_rt = img_gray.size[0] - 1;
                    }
                    Mat imagedoubt = img_gray(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                    Mat mask = th2(Rect(x_lt, y_lt, x_rt - x_lt, y_rt - y_lt));
                    double mean_in_gray = mean(imagedoubt, mask)[0];
                    double mean_out_gray = mean(imagedoubt, ~mask)[0];
                    double intensity = mean_in_gray - mean_out_gray;
                    string a = "long:" + to_string(long_lackLine) + "short:" + to_string(short_lackLine) + "th:" + to_string(intensity);
                    //putText(img_gray, a, Point(X_1 - 400, Y_1 - 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 3, 8, 0);
                    if (intensity > 3)
                    {
                        result = true;
                        CvPoint top_lef4 = cvPoint(x_lt, y_lt-5);
                        CvPoint bottom_right4 = cvPoint(x_rt, y_rt+5);
                        rectangle(black, top_lef4, bottom_right4, Scalar(255, 255, 255), 5, 8, 0);
                    }
                }
            }
        }
    }
    if (result == true)
    {
        *mresult = black;
        *causecolor = "彩线";
        result = true;
    }
    return result;
}

/*=========================================================
* 函 数 名: 灯眼缺陷
* 功能描述: 灯眼缺陷检测
* 输入：    白底图像
=========================================================*/
//             输入图片 ，  距上下边缘宽度，     距右边缘宽度   计算行均值的区域宽度，数据做差求斜率的步长                斜率小于graTh为极点  ，滤波范围         极点之间x方向间隔最小值                        极点之间斜率                     极点前后斜率绝对值均值        x方向间隔相似度阈值         相似的间隔的个数     斜率满足阈值的个数
bool LampEye(Mat white, int topBottomEdgeWidth,int rightEdgeWidth,double regionWidth,       int step,      int gradSpan ,      double graTh,  int fliterWidth, double intervalBetweenExtremePointsTh,double gradBetweenExtremePointsTh1, double gradBetweenExtremePointsTh2, double intervalSimilarityTh,   int numIntervalTh,   int numSlopeTh,Mat *mresult, QString *causecolor)
{                                                                                                         //极点前后做差，看极点
                                                                                                          //前后的斜率是否相反
    bool result = false;
    Mat thResult;
    threshold(white, thResult, 30, 255, CV_THRESH_BINARY);
    Mat structure_element = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat  erode1;
    erode(thResult, erode1, structure_element);
    bitwise_and(erode1, white, white);//将增强后的白底图做掩膜

    double rowGraySum = 0;
    vector<double> yArray(white.rows - topBottomEdgeWidth*2);//y轴
    vector<double> xArray(white.rows - topBottomEdgeWidth*2);//x轴
    Fit fit;
    //获得行灰度分布
    for (int i = topBottomEdgeWidth; i < white.rows - topBottomEdgeWidth; i++)//topBottomEdgeWidth默认是5
    {
        xArray[i - topBottomEdgeWidth] = i - topBottomEdgeWidth;
        int num = 0;
        int j = 0;
        while (num < regionWidth + rightEdgeWidth && j <= white.cols-1)//regionWidth默认是15，rightEdgeWidth默认是2
        {
            if (white.at<uchar>(i, j) > 0)
            {
                num++;
                if (num >= rightEdgeWidth)
                {
                    rowGraySum += white.at<uchar>(i, j);
                }
            }
            j++;
        }
        yArray[i - topBottomEdgeWidth] = rowGraySum / regionWidth;
        //cout << rowGraySum / regionWidth << endl;
        rowGraySum = 0;
    }
    //均值滤波
    vector<double> AverageFliter;
    int count ;
    double Sum ;
    for (int j = 0; j < yArray.size() ; j ++)//
    {
        count = 0;
        Sum = 0;
        if (j < fliterWidth)
        {
            for (int k = 0; k <= j+ fliterWidth; k++)
            {
                Sum += yArray[k];
                count++;
            }
        }
        else if (j > yArray.size() - (fliterWidth + 1))
        {
            for (int k = j- fliterWidth; k < yArray.size(); k++)
            {
                Sum += yArray[k];
                count++;
            }
        }
        else
        {
            for (int k = j - fliterWidth; k <= j+ fliterWidth; k++)
            {
                Sum += yArray[k];
                count++;
            }

        }
        AverageFliter.push_back(Sum / count);
        //cout << Sum / count<<endl;
    }

    vector<double>  extremePointsGrad;//非常小的斜率
    vector<double> realExtremePoints;
    vector<double> gradBetweenExtremePoints;
    int late = 0;
    double extremePoint = 0;
    double extremePoint_before = 0;
    double extremePoint_after = 0;

    //获得拟合函数导数小于graTh的点，并排除同一极点附近的其他点
    for (double j = 0; j < AverageFliter.size()- step; j += step)
    {
        //拟合函数求导
        extremePoint = AverageFliter[j + step] - AverageFliter[j];
        if (j < gradSpan)//
        {
            extremePoint_before= AverageFliter[j ] - AverageFliter[0];
            extremePoint_after = AverageFliter[j + step + gradSpan] - AverageFliter[j + step];
        }
        else if (j >= AverageFliter.size() - step- gradSpan && j < AverageFliter.size() - step)
        {
            extremePoint_before = AverageFliter[j] - AverageFliter[j- gradSpan];
            extremePoint_after = AverageFliter[AverageFliter.size()-1] - AverageFliter[j + step];
        }
        else
        {
            extremePoint_before = AverageFliter[j] - AverageFliter[j - gradSpan];
            extremePoint_after = AverageFliter[j + step+ gradSpan] - AverageFliter[j + step];
        }

        if (abs(extremePoint) <= graTh&& extremePoint_before*extremePoint_after <0&&j-late>intervalBetweenExtremePointsTh)
        {
            extremePointsGrad.push_back(extremePoint);
            realExtremePoints.push_back(j);
            gradBetweenExtremePoints.push_back((AverageFliter[j] - AverageFliter[late]) / (j - late));
            late = j;
        }
    }
    //打印计算得到的极值点所在位置
    //int a = 0;
    //for (int i = 0; i < AverageFliter.size(); i++)
    //{
    //	if (a < realExtremePoints.size())
    //	{
    //		if (i == realExtremePoints[a])
    //		{
    //			cout << 120 << endl;
    //			a++;
    //		}
    //		else
    //		{
    //			cout << 0 << endl;
    //		}
    //	}
    //	else
    //	{
    //		cout << 0 << endl;
    //	}
    //}
    //计算每个极点之间的间隔大致一样的个数
    vector<double> intervals;
    double aveInterval;
    double sumInterval=0;
    for (int i = 0; i < realExtremePoints.size()-1; i++)
    {
        intervals.push_back(realExtremePoints[i + 1] - realExtremePoints[i]);
        sumInterval += (realExtremePoints[i + 1] - realExtremePoints[i]);
    }
    aveInterval = sumInterval / intervals.size();
    int numInterval=0;
    for (int i = 0; i < intervals.size(); i++)
    {
        if (abs(intervals[i] - aveInterval) < intervalSimilarityTh)
        {
            numInterval++;
        }
    }
    //计算相邻极点之间的斜率满足阈值的个数
    int numSlope1=0;
    for (int i = 0; i < gradBetweenExtremePoints.size()-1; i++)
    {
        if (abs(gradBetweenExtremePoints[i]) >= gradBetweenExtremePointsTh1&&abs(gradBetweenExtremePoints[i+1]) >= gradBetweenExtremePointsTh1&& (abs(gradBetweenExtremePoints[i + 1])+ abs(gradBetweenExtremePoints[i]))/2.0>= gradBetweenExtremePointsTh2&& gradBetweenExtremePoints[i + 1]* gradBetweenExtremePoints[i]<0)
        {
            numSlope1++;
        }
    }
    //判断是否为灯眼
    if (numInterval >= numIntervalTh &&numSlope1 >= numSlopeTh)
    {
        rectangle(white, cvPoint(white.cols - 60, 10), cvPoint(white.cols - 1, white.rows - 10), Scalar(255, 255, 255), 5, 8, 0);
        *mresult = white;
        *causecolor = "灯眼";
        result= true;
    }
    cout << "numInterval:" << numInterval << "		" << "numSlope1:" << numSlope1 << "		"  << endl;
    return result;
}
/*=========================================================
* 函 数 名: SunSpots
* 功能描述: 太阳斑缺陷，白底下类似于色斑的一种缺陷
=========================================================*/
bool SunSpots(Mat image_white_src, Mat *mresult, QString *causecolor)
{
    bool result = false;
    Mat image_white = image_white_src.clone();
    Mat blur;
    cv::blur(image_white, blur, Size(15, 15));
    Mat hsv;
    cvtColor(blur, hsv, COLOR_BGR2HSV);             //RGB转HSV空间
    vector<Mat> channels;                           //存储三通道数据
    split(hsv, channels);                           //分离HSV三通道
    Mat H = channels.at(0);                         //H通道
    Mat S = channels.at(1);	                        //S通道
    Mat V = channels.at(2);                         //V通道
    Mat gray_clone = Mat::zeros(H.size(), H.type());//二值化图像
    int co, ro;
    for (int row = 0; row < hsv.rows; row = row + white_seban_step)//滑动窗口分块检测
    {
        for (int col = 0; col < hsv.cols; col = col + white_seban_step)
        {
            co = col;
            ro = row;
            if (ro + white_seban_size > hsv.rows - 1)
            {
                ro = hsv.rows - white_seban_size - 1;
                row = hsv.rows;
            }
            if (co + white_seban_size2 > hsv.cols - 1)
            {
                co = hsv.cols - white_seban_size2 - 1;
                col = hsv.cols;
            }
            Rect rect_wp(co, ro, white_seban_size2, white_seban_size);
            Mat temp_wp = hsv(rect_wp);

            int matMean_h = mean(H(rect_wp)).val[0];
            int matMean_s = mean(S(rect_wp)).val[0];
            int matMean_v = mean(V(rect_wp)).val[0];
            Mat mask;
            inRange(temp_wp, Scalar(matMean_h - white_seban_th_h_low, matMean_s - white_seban_th_s_low, matMean_v - white_seban_th_v_low), Scalar(matMean_h + white_seban_th_h_up, matMean_s + white_seban_th_s_up, matMean_v + white_seban_th_v_up), mask);
            mask = ~mask;
            mask.copyTo(gray_clone(rect_wp));
        }
    }
    Mat th_result;
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));                            //开操作结构元素
    morphologyEx(gray_clone, th_result, CV_MOP_OPEN, element);                              //先腐蚀，再膨胀，清除噪点
    Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));                           //膨胀结构元素
    dilate(th_result, th_result, element1);	                                                //扩扩大缺陷信息
    th_result(Rect(th_result.cols - 240, th_result.rows - 315, 240, 315)) = uchar(0);	    //去除易撕贴部分

    if(Model_Mod_type=="R角水滴屏")
    {
        bitwise_and(main_mask, th_result, th_result);
    }

    vector<vector<Point>> contours;
    findContours(th_result, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > white_seban_area_lower && area < white_seban_area_upper)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            float w = boundRect[i].width;
            float h = boundRect[i].height;
            int X_1 = boundRect[i].tl().x;//矩形左上角X坐标值
            int Y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
            int X_2 = boundRect[i].br().x;//矩形右下角X坐标值
            int Y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
            int brder = 5;
            X_1 = X_1 - brder - int(w);
            Y_1 = Y_1 - brder - int(h);
            X_2 = X_2 + brder + int(w);
            Y_2 = Y_2 + brder + int(h);
            Moments m = moments(contours[i]);
            int x_point = int(m.m10 / m.m00);
            int y_point = int(m.m01 / m.m00);
            double longShortRatio = max(h / w, w / h);
            //越界处理
            if (X_1 < 0)
            {
                X_1 = 0;
            }

            if (Y_1 < 0)
            {
                Y_1 = 0;
            }
            if (X_2 > th_result.cols - 1)
            {
                X_2 = th_result.cols - 1;
            }

            if (Y_2 > th_result.rows - 1)
            {
                Y_2 = th_result.rows - 1;
            }
            if (longShortRatio < 2 && min(w, h)>15 && min(w, h) < 50)
            {
                bool hsv_flage = false;	                                                            //参数成立标志位
                bool defect_flage = false;                                                          //缺陷亮度标志位
                int corner_Width = 30;
                double deviation_H,deviation_S,deviation_V,parah_Threshold,paras_Threshold,parav_Threshold;
                //==========================图像边界处参数单独计算====================
                if (x_point < corner_Width || y_point < corner_Width || th_result.cols - x_point < corner_Width || th_result.rows - y_point < corner_Width)
                {
                    Mat temp_Mask = th_result(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                //二值图像掩膜图
                    Mat temp_Hsv = hsv(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                       //疑似缺陷HSV图像
                    Mat mean_In, stddev_In;
                    meanStdDev(temp_Hsv, mean_In, stddev_In, temp_Mask);
                    double meanH_in = mean_In.at<double>(0, 0);	                                    //H空间缺陷处均值
                    double meanS_in = mean_In.at<double>(1, 0);	                                    //S空间缺陷处均值
                    double meanV_in = mean_In.at<double>(2, 0);	                                    //V空间缺陷处均值

                    Mat temp_H = H(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                           //H空间原图
                    Mat temp_S = S(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                           //H空间原图
                    Mat temp_V = V(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                           //H空间原图

                    Mat mask_H, mask_S, mask_V;
                    Mat maskH_out, maskS_out, maskV_out;

                    threshold(temp_H, mask_H, 25, 255, CV_THRESH_BINARY);
                    bitwise_and(~temp_Mask, mask_H, maskH_out);

                    threshold(temp_S, mask_S, 25, 255, CV_THRESH_BINARY);
                    bitwise_and(~temp_Mask, mask_S, maskS_out);

                    threshold(temp_V, mask_V, 25, 255, CV_THRESH_BINARY);
                    bitwise_and(~temp_Mask, mask_V, maskV_out);

                    double meanH_out = mean(temp_H, maskH_out)[0];                                 //H空间缺陷外围均值
                    double meanS_out = mean(temp_S, maskS_out)[0];                                 //S空间缺陷外围均值
                    double meanV_out = mean(temp_V, maskV_out)[0];                                 //V空间缺陷外围均值

                    Point minPoint, maxPoint;
                    double minStrength, maxStrength;
                    minMaxLoc(temp_H, &minStrength, &maxStrength, &minPoint, &maxPoint, temp_Mask);
                    deviation_H = maxStrength - minStrength;	                                  //缺陷处H最高值和最低值的差值

                    minMaxLoc(temp_S, &minStrength, &maxStrength, &minPoint, &maxPoint, temp_Mask);
                    deviation_S = maxStrength - minStrength;							          //缺陷处S最高值和最低值的差值

                    minMaxLoc(temp_V, &minStrength, &maxStrength, &minPoint, &maxPoint, temp_Mask);
                    deviation_V = maxStrength - minStrength;					                  //缺陷处V最高值和最低值的差值

                    parah_Threshold = abs(meanH_in - meanH_out);                                  //H空间判定参数阈值
                    paras_Threshold = abs(meanS_in - meanS_out);                                  //S空间判定参数阈值
                    parav_Threshold = abs(meanV_in - meanV_out);                                  //S空间判定参数阈值
                }
                //=========================非图像边界处参数计算====================
                Mat temp_Mask = th_result(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                    //二值图像掩膜图
                Mat temp_Hsv = hsv(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                           //疑似缺陷HSV图像
                Mat mean_In, stddev_In;
                meanStdDev(temp_Hsv, mean_In, stddev_In, temp_Mask);
                double meanH_in = mean_In.at<double>(0, 0);	                                        //H空间缺陷处均值
                double meanS_in = mean_In.at<double>(1, 0);	                                        //S空间缺陷处均值
                double meanV_in = mean_In.at<double>(2, 0);	                                        //V空间缺陷处均值

                Mat temp_H = H(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                               //H空间原图
                Mat temp_S = S(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                               //H空间原图
                Mat temp_V = V(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));                               //H空间原图

                Point minPoint, maxPoint;
                double minStrength, maxStrength;
                minMaxLoc(temp_H, &minStrength, &maxStrength, &minPoint, &maxPoint, temp_Mask);
                deviation_H = maxStrength - minStrength;	                                       //缺陷处H最高值和最低值的差值

                minMaxLoc(temp_S, &minStrength, &maxStrength, &minPoint, &maxPoint, temp_Mask);
                deviation_S = maxStrength - minStrength;							               //缺陷处S最高值和最低值的差值

                minMaxLoc(temp_V, &minStrength, &maxStrength, &minPoint, &maxPoint, temp_Mask);
                deviation_V = maxStrength - minStrength;					                       //缺陷处V最高值和最低值的差值

                Mat result1, result2, result3;
                bitwise_and(temp_H, temp_Mask, result1);
                bitwise_and(temp_S, temp_Mask, result2);
                bitwise_and(temp_V, temp_Mask, result3);

                Mat mean_Outer, stddev_Outer;
                meanStdDev(temp_Hsv, mean_Outer, stddev_Outer, ~temp_Mask);
                double meanH_out = mean_Outer.at<double>(0, 0);	                                    //H空间缺陷外围均值
                double meanS_out = mean_Outer.at<double>(1, 0);	                                    //S空间缺陷外围均值
                double meanV_out = mean_Outer.at<double>(2, 0);	                                    //V空间缺陷外围均值

                parah_Threshold = abs(meanH_in - meanH_out);                                        //H空间判定参数阈值
                paras_Threshold = abs(meanS_in - meanS_out);                                        //S空间判定参数阈值
                parav_Threshold = abs(meanV_in - meanV_out);                                        //S空间判定参数阈值

                if (parah_Threshold > 1.2 && parah_Threshold <10 && paras_Threshold > 13 && parav_Threshold > 1.5 && parav_Threshold < 10)
                {
                    hsv_flage = true;
                }
                if (deviation_H > 2 && deviation_H < 20 && deviation_S > 15 && deviation_V > 2 && deviation_V < 20)
                {
                    defect_flage = true;
                }
                if (hsv_flage && defect_flage)
                {
                    result = true;
                    CvPoint top_lef4 = cvPoint(X_1, Y_1);
                    CvPoint bottom_right4 = cvPoint(X_2, Y_2);
                    rectangle(image_white, top_lef4, bottom_right4, Scalar(0, 255, 0), 5, 8, 0);

                    string text = "H:" + to_string(parah_Threshold) + " S:" + to_string(paras_Threshold) + " V:" + to_string(parav_Threshold) ;
                    putText(image_white, text, Point(40, 40), FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0, 0, 255), 2, 8, 0);
                }
            }
        }
    }
    if (result == true)
    {
        *mresult = image_white;
        *causecolor = "太阳斑";
    }
    return result;
}

/*=========================================================
* 函 数 名: ScreenFlash
* 功能描述: 屏幕闪烁检测
* 函数输入：主彩色相机灰底两帧图片
* 备注说明：2020年7月16日修改
 =========================================================*/
bool ScreenFlash(Mat grayColor1, Mat grayColor2,Mat *mresult, QString *causecolor)
{
    bool result = false;
    Mat screenflashHsv1,screenflashHsv2;                  //每帧灰底图像转为HSV空间图
    cvtColor(grayColor1, screenflashHsv1, COLOR_BGR2HSV); //第1帧灰底图像转为HSV空间图
    cvtColor(grayColor2, screenflashHsv2, COLOR_BGR2HSV); //第2帧灰底图像转为HSV空间图
    Mat frameDifference=screenflashHsv1-screenflashHsv2;  //两帧图像差分
    imwrite("D:\\frameDifference.bmp",frameDifference);

    Mat meanGray, stddevGray;
    meanStdDev(frameDifference, meanGray, stddevGray);     //计算差分HSV图像的S的方差
    double stddevSaturation = stddevGray.at<double>(1, 0); //计算S图像的方差
    double meanSaturation = meanGray.at<double>(1, 0);     //计算S图像的均值

    vector<Mat> channels;
    split(frameDifference, channels);
    Mat S = channels.at(1);
    Mat Binarytion, Binarytion1;
    Canny(S, Binarytion1, 80, 160, 3);
    vector<Vec4i>lines;
    HoughLinesP(Binarytion1, lines, 1, CV_PI / 180, 40, 80, 10);

    if (stddevSaturation > 6.3 && meanSaturation > 3.5 && lines.size() > 480)
    {
        result = true;
        *mresult = grayColor1;
        *causecolor = "屏闪";

        string information = "stddev: " + to_string(stddevSaturation) + " mean: " + to_string(meanSaturation)+ " size: " + to_string(lines.size());
        putText(grayColor1, information, Point(40, 40), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 4, 8, 0);
        imwrite("D:\\screenflashResult.bmp",grayColor1);

    }
    return result;
}

/*=========================================================
* 函 数 名: DarkCorner
* 功能描述暗角缺陷检测
* 函数输入：主彩色相机白底图片
* 备注说明：2020年9月9日修改
 =========================================================*/
bool DarkCorner(Mat white,Mat *mresult,QString* causecolor)
{
    bool result = false;

    Mat yanmo;
    threshold(white, yanmo, 60, 255, CV_THRESH_BINARY);
    Mat result11 = white & yanmo;
    Mat dst2;
    //构建CLAHE对象
    Ptr<CLAHE> clahe = createCLAHE(5.0, Size(8, 8));
    //限制对比度的自适应直方图均衡化
    clahe->apply(result11, dst2);
    Mat dst3;
    threshold(dst2, dst3, 70, 255, CV_THRESH_BINARY);

    Mat dst4;
    dst4 = dst3 ^ yanmo;
    dst4(Rect(120,0,dst4.cols - 240, dst4.rows)) = uchar(0);

    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat  erode1;
    erode(dst4, dst4, structure_element1);

    vector<vector<Point>> contours;

    findContours(dst4, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> boundRect(contours.size());
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);


        cout << "缺陷面积" << area << endl;
        if (area > 5000 && area < 30000)
        {

            RotatedRect rect = minAreaRect(contours[i]);
            double w1 = rect.size.height;
            double h1 = rect.size.width;
            cout << "缺陷宽" << w1 << "缺陷长" << h1 << endl;
            cout << "缺陷长宽比" << max(w1 / h1,h1 / w1) << endl;
            if (max(w1 / h1, h1 / w1) >= 1 && max(w1 / h1, h1 / w1) < 7.5)
            {
                //cout << "缺陷区域面积" << area << endl;
                //RotatedRect rect = minAreaRect(contours[i]); //包覆轮廓的最小斜矩形
                //double w1 = rect.size.height;
                //double h1 = rect.size.width;

                boundRect[i] = boundingRect(Mat(contours[i]));
                float w = boundRect[i].width;
                float h = boundRect[i].height;
                int x_1 = boundRect[i].tl().x;//矩形左上角X坐标值
                int y_1 = boundRect[i].tl().y;//矩形左上角Y坐标值
                int x_2 = boundRect[i].br().x;//矩形右下角X坐标值
                int y_2 = boundRect[i].br().y;//矩形右下角Y坐标值
                int x_point = x_1 + round(w / 2);
                int y_point = y_1 + round(h / 2);

                //将区域扩大
                int	X_1 = x_1 - int(w);
                int	Y_1 = y_1 - int(h);
                int	X_2 = x_2 + int(w);
                int	Y_2 = y_2 + int(h);
                if (X_1 < 0)
                {
                    X_1 = 0;
                }
                if (Y_1 < 0)
                {
                    Y_1 = 0;
                }
                if (X_2 > white.cols - 1)
                {
                    X_2 = white.cols - 1;
                }
                if (Y_2 > white.rows - 1)
                {
                    Y_2 = white.rows - 1;
                }

                Mat temp_gray = white(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                Mat mask = dst4(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                Mat mask1 = dst3(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                double meanOut;
                double meanIn;
                double meanAll;
                int grayValueSum = 0;
                int pixelsNum = 0;
                Mat maskGray;
                bitwise_and(temp_gray, mask, maskGray);
                for (int i = 0; i < maskGray.cols; i++)
                {
                    for (int j = 0; j < maskGray.rows; j++)
                    {
                        if (maskGray.at<uchar>(j, i) > 0)
                        {
                            grayValueSum += maskGray.at<uchar>(j, i);
                            pixelsNum++;
                        }
                    }
                }
                meanIn = grayValueSum / (float)pixelsNum;

                grayValueSum = 0;
                pixelsNum = 0;
                bitwise_and(temp_gray, mask1, maskGray);
                for (int i = 0; i < maskGray.cols; i++)
                {
                    for (int j = 0; j < maskGray.rows; j++)
                    {
                        if (maskGray.at<uchar>(j, i) > 0)
                        {
                            grayValueSum += maskGray.at<uchar>(j, i);
                            pixelsNum++;
                        }
                    }
                }
                meanOut = grayValueSum / (float)pixelsNum;

                grayValueSum = 0;
                pixelsNum = 0;
                for (int i = 0; i < temp_gray.cols; i++)
                {
                    for (int j = 0; j < temp_gray.rows; j++)
                    {
                        if (temp_gray.at<uchar>(j, i) > 0)
                        {
                            grayValueSum += temp_gray.at<uchar>(j, i);
                            pixelsNum++;
                        }
                    }
                }
                meanAll = grayValueSum / (float)pixelsNum;

                double  threshold = meanOut - meanIn;//缺陷区域与周围灰度差值(整体性)
                double chazhi = meanAll - white.at<uchar>(y_point, x_point);

                if (threshold > 20 || meanIn < 90)//满足灰度差值阈值就将其当作亮点
                {
                    drawContours(white, contours, i, cv::Scalar(255, 0, 0), 3, 8);
                    result = true;
                    *mresult = white;
                    *causecolor = "暗角";
                }

            }
        }
    }
    return  result;
}

/*=========================================================
* 函 数 名: Brightedge
* 功能描述：亮边
* 函数输入：主彩色相机左右边界外扩后图片
* 备注说明：2020年9月9日修改
 =========================================================*/
bool Brightedge(Mat src_white,Mat Ceguang, Mat* Mwhite,QString* causecolor,int Flag_L_R)
{
    double enlarge_num = 1.12;    //刘海部分的拉伸
    int reduce_num = 30;  //往里缩的像素值
    int reduce_num_out = 20;//计算外围灰度时向外扩的像素行数

    double canny_low_limit = 8;//8
    //int reduce_num = 50;  //往里缩的像素值
    int Border_remove_highlimit = 1200; //去除边框的轮廓size大小
    int contours_min_limit1 = 50;  //边缘ROI检测的轮廓最小size
    int contours_min_limit2 = 30;  //候选框内检测的轮廓最小size
    int small_rect_filter = 25;//最小矩形框长宽限制
    int mean_value_defect = 120;//缺陷处的亮度最低阈值
    int mean_value_defect_sum_out = 25;//候选框内缺陷处与框内其他区域的灰度差值
    int mean_value_defect_sum_rect_out = 15;//候选框内缺陷处与框外其他区域的灰度差值
    int mean_value_defect_sum_rect_out_nearlight = 25; //靠近灯部的候选框内缺陷处与框外其他区域的灰度差值

    Mat Mask_img;
    Mat img1;
    Mat M;
    vector<vector<Point>> contours;
    vector<vector<Point>> contours1;
    Mat img = src_white.clone();
    Mat src = src_white.clone();
    //Canny(img, img1, 20, 60, 3);
    //threshold(img, Mask_img, 20, 255, CV_THRESH_BINARY);
    adaptiveThreshold(img, img, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 57, -3.95);
    findContours(img, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 100000)
        {
            drawContours(img, contours, i, Scalar(255), -1, 8);
            break;
        }
        else
            drawContours(img, contours, i, Scalar(0), -1, 8);
    }

    int y_part1 = int(img.rows / 5 - reduce_num);
    int y_part2 = y_part1 + reduce_num;
    int y_part3 = int(img.rows / 5 * 4 - reduce_num);
    int y_part4 = y_part3 + reduce_num;

    int x_part1 = int(img.cols / 2);
    int x_part2 = x_part1 + 2 * reduce_num;

    Mat desImg1(img.rows, img.cols, CV_8UC1, Scalar(0, 0, 0));  //创建和原图相同大小的黑色图片
    Mat desImg = desImg1(Rect(reduce_num, reduce_num, img.cols - 2 * reduce_num, img.rows - 2 * reduce_num)); //长和宽缩小固定像素后的区域

    //将原图分分为6部分
    Mat desImgROI1 = desImg(Rect(0, 0, x_part1, y_part1));
    Mat desImgROI2 = desImg(Rect(x_part1, 0, desImg.cols - x_part1, y_part1));
    Mat desImgROI3 = desImg(Rect(0, y_part1, x_part1, y_part3 - y_part2));
    Mat desImgROI4 = desImg(Rect(x_part1, y_part1, desImg.cols - x_part1, y_part3 - y_part2));
    Mat desImgROI5 = desImg(Rect(0, y_part1 + y_part3 - y_part2, x_part1, desImg.rows + 2 * reduce_num - y_part4));
    Mat desImgROI6 = desImg(Rect(x_part1, y_part1 + y_part3 - y_part2, desImg.cols - x_part1 - 1, desImg.rows + 2 * reduce_num - y_part4 - 1));

    Mat imgROI1 = img(Rect(0, 0, x_part1, y_part1));
    Mat imgROI2 = img(Rect(x_part2, 0, desImg.cols - x_part1, y_part1));
    Mat imgROI3 = img(Rect(0, y_part2, x_part1, y_part3 - y_part2));
    Mat imgROI4 = img(Rect(x_part2, y_part2, desImg.cols - x_part1, y_part3 - y_part2));
    Mat imgROI5 = img(Rect(0, y_part4, x_part1, desImg.rows + 2 * reduce_num - y_part4));
    Mat imgROI6 = img(Rect(x_part2, y_part4, desImg.cols - x_part1 - 1, desImg.rows + 2 * reduce_num - y_part4 - 1));

    //调整刘海部分边缘
    Mat imgROI3_Enlarge;
    resize(imgROI3, imgROI3_Enlarge, Size(imgROI3.cols, imgROI3.rows * enlarge_num), 0, 0, INTER_LINEAR);
    Mat imgROI3_Enlarge_ROI = imgROI3_Enlarge(Rect(0, imgROI3.rows * abs(1 - enlarge_num) / 2, imgROI3.cols, imgROI3.rows));

    Mat imgROI4_Enlarge;
    resize(imgROI4, imgROI4_Enlarge, Size(imgROI4.cols, imgROI4.rows * enlarge_num), 0, 0, INTER_LINEAR);
    Mat imgROI4_Enlarge_ROI = imgROI4_Enlarge(Rect(0, imgROI4.rows * abs(1 - enlarge_num) / 2, imgROI4.cols, imgROI4.rows));

    imgROI1.copyTo(desImgROI1);
    imgROI2.copyTo(desImgROI2);
    imgROI3_Enlarge_ROI.copyTo(desImgROI3);
    imgROI4_Enlarge_ROI.copyTo(desImgROI4);
    imgROI5.copyTo(desImgROI5);
    imgROI6.copyTo(desImgROI6);

    //得到边缘区域的掩膜
    Mat img2;
    bitwise_not(desImg1, desImg1);
    bitwise_and(img, desImg1, img2);

    //在原图上得到边缘区域
    Mat img3 = src_white.clone();
    Mat img4;
    bitwise_and(img3, img2, img4);


    int start_row = 0;
    int end_row = 0;
    if (Flag_L_R == 0)//右相机屏蔽对应不聚焦区域
    {
        start_row = 70;
        end_row = img4.rows - 550;
        img4(Rect(0, img4.rows - 100, img4.cols - 1, 100)) = uchar(0);
    }
    else if (Flag_L_R == 1)//左相机屏蔽对应不聚焦区域
    {
        start_row = 550;
        end_row = img4.rows - 70;
        img4(Rect(0, 0, img4.cols - 1, 100)) = uchar(0);
    }

    Mat img_result = img4.clone();

    double m;
    Canny(img_result, img_result, canny_low_limit, canny_low_limit * 3, 3);
    findContours(img_result, contours, CV_RETR_TREE, CHAIN_APPROX_NONE);

    for (vector<int>::size_type i = 0; i < contours.size(); i++)  // 去掉边框的轮廓
    {
        if (contours[i].size() > Border_remove_highlimit)
        {
            drawContours(img_result, contours, i, Scalar(0), 2, 8);
        }
    }

    //去掉边框轮廓后再膨胀
    Mat element = getStructuringElement(MORPH_RECT, Size(6, 6));
    morphologyEx(img_result, img_result, CV_MOP_CLOSE, element);
    //dilate(img_result, img_result, element);

    findContours(img_result, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE);
    std::sort(contours.begin(), contours.end(), compareContourAreas);
    vector<Rect> boundRect(contours.size());//Rect类型的vector容器boundRect存放正外接矩形
    vector<RotatedRect> roRect(contours.size());  //定义Rect类型的vector容器roRect存放最小外接矩形

    int num = 0;
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        if (contours[i].size() > contours_min_limit1)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            int X_1 = boundRect[i].tl().x; //矩形左上角X坐标值
            int Y_1 = boundRect[i].tl().y; //矩形左上角Y坐标值
            int X_2 = boundRect[i].br().x; //矩形右下角X坐标值
            int Y_2 = boundRect[i].br().y; //矩形右下角Y坐标值
            Mat boundrect;
            Mat b_2;
            int b_x1;
            int b_y1;
            int b_wdith;
            int b_height;

            if ((X_2 - X_1) < small_rect_filter && (Y_2 - Y_1) < small_rect_filter)  //滤掉一些干扰点
                continue;


            //需要在侧光图上排掉是否是膜贴歪出现的边缘
            int Ce_x1;
            int Ce_y1;
            int Ce_wdith;
            int Ce_height;

            Ce_x1 = X_1;
            Ce_y1 = Y_1;
            Ce_wdith = X_2 - X_1;
            Ce_height = Y_2 - Y_1;

            int mean_value_sum_out = mean_value_defect_sum_rect_out;

            if (X_1 < 200)  //头部
            {
                Ce_x1 = Ce_x1 + reduce_num_out;
                b_x1 = 0;
                b_y1 = Y_1;
                b_wdith = X_2;
                b_height = Y_2 - Y_1;
            }
            else if (X_1 > 2800) //尾部
            {
                Ce_x1 = Ce_x1 - reduce_num_out;
                b_x1 = X_1;
                b_y1 = Y_1;
                b_wdith = img4.cols - 1 - X_1;
                b_height = Y_2 - Y_1;
            }
            else  //两侧
            {
                if (X_1 > 2500 && X_1 < 2800)
                    mean_value_sum_out = mean_value_defect_sum_rect_out_nearlight;
                if (Flag_L_R == 0)
                {
                    Ce_y1 = Ce_y1 + reduce_num_out;
                    b_x1 = X_1;
                    b_y1 = 0;
                    b_wdith = X_2 - X_1;
                    b_height = Y_2;
                }
                else
                {
                    Ce_y1 = Ce_y1 - reduce_num_out;
                    b_x1 = X_1;
                    b_y1 = Y_1;
                    b_wdith = X_2 - X_1;
                    b_height = img4.rows - Y_1 - 1;
                }
            }


            boundrect = img4(Rect(b_x1, b_y1, b_wdith, b_height));
            Mat b_1 = boundrect.clone();
            b_2 = boundrect.clone();
            Mat b_3 = boundrect.clone();


            Mat img_out = src_white(Rect(Ce_x1, Ce_y1, Ce_wdith, Ce_height));
            Mat img_out_mask = img(Rect(Ce_x1, Ce_y1, Ce_wdith, Ce_height));
            double mean_out = mean(img_out, img_out_mask)[0];



            //查找候选框内的轮廓形状
            m = mean(b_1, b_1)[0];  //计算候选框的灰度均值
            threshold(b_1, b_1, m, 255, CV_THRESH_BINARY);  //用该灰度均值为阈值进行二值化

            //求取割出的轮廓之外的灰度均值
            threshold(b_3, b_3, m, 255, CV_THRESH_TOZERO_INV);  //计算除分割出的轮廓之外的其余部分的灰度均值
            double m_anti = mean(b_2, b_3)[0];

            findContours(b_1, contours1, CV_RETR_LIST, CHAIN_APPROX_NONE);
            std::sort(contours1.begin(), contours1.end(), compareContourAreas);
            vector<Rect> boundRect1(contours1.size());//Rect类型的vector容器boundRect存放正外接矩形
            for (vector<int>::size_type i1 = 0; i1 < contours1.size(); i1++)
            {
                if (contours1[i1].size() > contours_min_limit2)
                {
                    boundRect1[i1] = boundingRect(Mat(contours1[i1]));
                    int X_1_lo = boundRect1[i1].tl().x; //矩形左上角X坐标值
                    int Y_1_lo = boundRect1[i1].tl().y; //矩形左上角Y坐标值
                    int X_2_lo = boundRect1[i1].br().x; //矩形右下角X坐标值
                    int Y_2_lo = boundRect1[i1].br().y; //矩形右下角Y坐标值

                    Mat boundrect_local = b_2(Rect(X_1_lo, Y_1_lo, X_2_lo - X_1_lo, Y_2_lo - Y_1_lo));
                    Mat mask_lo = b_1(Rect(X_1_lo, Y_1_lo, X_2_lo - X_1_lo, Y_2_lo - Y_1_lo));
                    double m_lo = mean(boundrect_local, mask_lo)[0];
                    //if (m_lo > mean_value_defect && (m_lo - m_anti) > mean_value_defect_sum_out && (m_lo - mean_out) > mean_value_sum_out)
                    if (m_lo > mean_value_defect && (m_lo - m_anti) > mean_value_defect_sum_out)
                    {
                        num++;
                        rectangle(src, boundRect[i].tl(), boundRect[i].br(), Scalar(255), 5, 8, 0);
                        break;
                    }
                }
            }
            if (num > 0)
            {
                break;
            }
        }

    }
    if (num != 0)
    {
        *Mwhite = src;
        *causecolor = "亮边";
        return true;
    }

    else
        return false;
}

/*=========================================================
* 函 数 名: Light_leakage
* 功能描述：漏光
* 函数输入：主彩色相机左右边界外扩后图片
* 备注说明：2020年9月25日修改
 =========================================================*/
bool Light_leakage(Mat white, Mat ceguang, Mat* mresult, QString* causecolor)
{
    int enlarge_num =40; //挖孔处外扩的像素行数
        double enlargr_size = 1.5;//挖孔处外扩的倍数
        int ROI_X_limit = 300;  //头部孔的最大横轴值
        int interfere_point_filter_low_limit = 100;  //滤掉干扰点的最低像素阈值
        int ROI_expand_figuremean =5;  //计算候选框的灰度均值时外扩的像素行数
        int ROI_Out_expand_figuremean = 8;  //计算外围灰度时宽和高扩的像素行数
        int ROI_Out_expand_figuremean1 = 0;//计算外围灰度时坐标扩的像素行数
        int increase_mean = 8;
        int canny_low_limit = 10;
        int Light_low_limit = 8;  //疑似缺陷区域与外围区域的最小亮度阈值差
        int num=0;  //缺陷数量

        Mat src = white.clone();
        Mat src_gray = white.clone();
        Mat src_1 = white.clone();
        Mat src_2 = white.clone();
        threshold(src, src_1, 40, 255, CV_THRESH_BINARY_INV);
        vector<vector<Point>> contours;
        vector<vector<Point>> contours1;
        findContours(src_1, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        vector<Rect> boundRect(contours.size());
        int src_width = src.cols;
        int src_height = src.rows;
        for (vector<int>::size_type i = 0; i < contours.size(); i++)
        {
            if (contours[i].size() < interfere_point_filter_low_limit)  //滤掉干扰点
            {
                drawContours(src_1, contours, i, Scalar(0), -1, 8);
            }
            else
            {
                boundRect[i] = boundingRect(Mat(contours[i]));
                int X_1 = boundRect[i].tl().x; //矩形左上角X坐标值
                int Y_1 = boundRect[i].tl().y; //矩形左上角Y坐标值
                int X_2 = boundRect[i].br().x; //矩形右下角X坐标值
                int Y_2 = boundRect[i].br().y; //矩形右下角Y坐标值

                if (X_1 > 0 && X_1 < src_width && Y_1>0 && Y_1 < src_height && X_2>0 && X_2 < ROI_X_limit && Y_2>0 && Y_2 < src_height)   //排除四个边角以及边界外的干扰区域
                {
                    X_1 = X_1 - enlarge_num;
                    Y_1 = Y_1 - enlarge_num;
                    X_2 = X_2 + enlarge_num;
                    Y_2 = Y_2 + enlarge_num;
                    if (X_1 < 0)
                        X_1 = 0;
                    if (Y_1 < 0)
                        Y_1 = 0;
                    if (X_2 > src_1.cols - 1)
                        X_2 = src_1.cols - 1;
                    if (Y_2 > src_1.rows - 1)
                        Y_2 = src_1.rows - 1;
                    Mat boundrect;
                    boundrect = src_1(Rect(X_1, Y_1, X_2-X_1, Y_2-Y_1));
                    Mat src_rect;
                    src_rect = src_gray(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                    Mat src_rect_out = src(Rect(X_1, Y_1, X_2 - X_1, Y_2 - Y_1));
                    Mat src_rect_lo = src_rect.clone();
                    Mat src_rect_lo1 = src_rect.clone();
                    Mat b_1 = boundrect.clone();
                    resize(b_1, b_1, Size(b_1.cols * enlargr_size, b_1.rows * enlargr_size), 0, 0, INTER_LINEAR);
                    Mat boundrect_Enlarge_ROI = b_1(Rect(boundrect.cols * abs(1 - enlargr_size) / 2, boundrect.rows * abs(1 - enlargr_size) / 2, boundrect.cols, boundrect.rows));

                    bitwise_not(boundrect, boundrect);
                    bitwise_and(boundrect_Enlarge_ROI, boundrect, boundrect);
                    bitwise_and(src_rect, boundrect, src_rect);

                    Mat src_rect1 = src_rect.clone();
                    //GaussianBlur(src_rect1, src_rect1, Size(3, 3), 0, 0);
                    Canny(src_rect1, src_rect1, canny_low_limit, 3*canny_low_limit, 3);
                    findContours(src_rect1, contours1,CV_RETR_LIST, CHAIN_APPROX_NONE);
                    //drawContours(src_rect1,contours1, -1, Scalar(100, 100, 100), 1, 8);
                    for (vector<int>::size_type i_lo = 0; i_lo < contours1.size(); i_lo++)
                    {
                        if(contours1[i_lo].size()>100)//去掉边界轮廓
                            drawContours(src_rect1, contours1, i_lo, Scalar(0), 1, 8);
                    }
                    Mat src_rect1_dilate;
                    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
                    morphologyEx(src_rect1, src_rect1_dilate, CV_MOP_CLOSE, element);
                    //dilate(src_rect1, src_rect1_dilate, element);
                    vector<vector<Point>> contours2;
                    findContours(src_rect1_dilate, contours2, CV_RETR_LIST, CHAIN_APPROX_NONE);
                    vector<Rect> boundRect_lo(contours2.size());
                    //vector<RotatedRect> roRect(contours2.size());
                    for (vector<int>::size_type i_lo = 0; i_lo < contours2.size(); i_lo++)
                    {
                        if (contours2[i_lo].size() < 5)
                            continue;
                        boundRect_lo[i_lo] = boundingRect(Mat(contours2[i_lo]));
                        RotatedRect rect = minAreaRect(contours2[i_lo]);
                        int X_1_lo = boundRect_lo[i_lo].tl().x; //矩形左上角X坐标值
                        int Y_1_lo = boundRect_lo[i_lo].tl().y; //矩形左上角Y坐标值
                        int X_2_lo = boundRect_lo[i_lo].br().x; //矩形右下角X坐标值
                        int Y_2_lo = boundRect_lo[i_lo].br().y; //矩形右下角Y坐标值



                        Point2f center = rect.center; //最小外接矩形中心点坐标
                        Mat rot_mat = getRotationMatrix2D(center, rect.angle, 1.0);//求旋转矩阵
                        Mat rot_image;
                        Mat rot_image_lo;
                        Size dst_sz(src_rect.size());
                        warpAffine(src_rect, rot_image, rot_mat, dst_sz);//原图像旋转rot_image 仿射变换
                        warpAffine(src_rect_lo, rot_image_lo, rot_mat, dst_sz);

                        float rect_width;
                        float rect_height;
                        float rect_w;
                        float rect_h;
                        if (rect.angle == -90)  //旋转角度-90时会有长宽互换
                        {
                            rect_width = rect.size.height;
                            rect_height = rect.size.width;
                            rect_w = rect_width;
                            rect_h = rect_height;
                        }
                        else
                        {
                            rect_width = rect.size.width;
                            rect_height = rect.size.height;
                            rect_w = rect_width;
                            rect_h = rect_height;
                        }
                        if (center.x + (rect_width / 2) > src_rect.cols - 1) //防止越界
                            rect_width = rect_width / 2 + src_rect.cols - 1 - center.x;
                        if (center.y + (rect_height / 2) > src_rect.rows - 1)
                            rect_height = rect_height / 2 + src_rect.rows - 1 - center.y;
                        int X_1_result1 = center.x - (rect_w / 2) - ROI_expand_figuremean;
                        if (X_1_result1 <= 0)
                            X_1_result1 = 1;
                        int Y_1_result1 = center.y - (rect_h / 2)-ROI_expand_figuremean;
                        if (Y_1_result1 <= 0)
                            Y_1_result1 = 1;
                        int wdith_result1 =rect_width+2* ROI_expand_figuremean;
                        if (X_1_result1 + wdith_result1 >= rot_image.cols - 1)
                            wdith_result1 = rot_image.cols - 1 - X_1_result1;
                        int height_result1 = rect_height + 2 * ROI_expand_figuremean;
                        if (Y_1_result1 + height_result1 >= rot_image.rows - 1)
                            height_result1 = rot_image.rows - 1 - Y_1_result1;
                        //Mat result1 = rot_image(Rect(center.x - (rect_w / 2), center.y - (rect_h / 2), rect_width, rect_height));
                        Mat result1_pre;
                        result1_pre = rot_image_lo(Rect(X_1_result1, Y_1_result1, wdith_result1, height_result1));
                        Mat binary_result1;
                        threshold(result1_pre, binary_result1, 120, 255, CV_THRESH_BINARY);
                        Mat result1;
                        bitwise_and(result1_pre, binary_result1, result1);
                        double m1 = mean(result1, result1)[0];
                        Mat result2 = result1.clone();
                        if (m1 + increase_mean >= 255)
                            m1 = 254 - increase_mean;
                        threshold(result2, result2, m1+ increase_mean, 255, CV_THRESH_BINARY);
                        //threshold(result2, result2, m1 + increase_mean, 255, CV_THRESH_BINARY);
                        vector<vector<Point>> contours3;
                        findContours(result2, contours3, CV_RETR_LIST, CHAIN_APPROX_NONE);
                        vector<Rect> boundRect_lo1(contours3.size());
                        for (vector<int>::size_type i_lo1 = 0; i_lo1 < contours3.size(); i_lo1++)
                        {
                            /*if (contours3[i_lo1].size() > 300)
                                continue;*/

                            if (contours3[i_lo1].size() < 10)
                                continue;

                            int Out_X1;
                            int Out_Y1;
                            int Out_wdith = X_2_lo - X_1_lo + ROI_Out_expand_figuremean;
                            int Out_height = Y_2_lo - Y_1_lo + ROI_Out_expand_figuremean;
                            //取候选框向外延伸的部分区域作为外围灰度值
                            if (X_1_lo <= int(src_rect1_dilate.cols / 2))  //左半区
                            {
                                if (Y_1_lo <= int(src_rect1_dilate.rows / 2)) //左上
                                {
                                    Out_X1 = X_1_lo -Out_wdith- ROI_Out_expand_figuremean1;
                                    Out_Y1 = Y_1_lo - Out_height-ROI_Out_expand_figuremean1;
                                }
                                else
                                {
                                    Out_X1 = X_1_lo - Out_wdith-ROI_Out_expand_figuremean1;
                                    //Out_Y1 = Y_1_lo + Out_height+ ROI_Out_expand_figuremean1;
                                    Out_Y1 = Y_1_lo ;
                                }
                            }
                            else  //右半区
                            {
                                if (Y_1_lo <= int(src_rect1_dilate.rows / 2)) //右上
                                {
                                    Out_X1 = X_1_lo + Out_wdith+ROI_Out_expand_figuremean1;
                                    Out_Y1 = Y_1_lo - Out_height- ROI_Out_expand_figuremean1;
                                }
                                else     //右下
                                {
                                    Out_X1 = X_1_lo + Out_wdith+ ROI_Out_expand_figuremean1;
                                    Out_Y1 = Y_1_lo + Out_height+ ROI_Out_expand_figuremean1;
                                }
                            }
                            if (Out_X1 <= 0)
                                Out_X1 = 1;
                            if (Out_Y1 <= 0)
                                Out_Y1 = 1;
                            if (Out_X1 + Out_wdith > src_rect1_dilate.cols)
                                Out_wdith = src_rect1_dilate.cols-1 - Out_X1;
                            if (Out_Y1 + Out_height > src_rect1_dilate.rows)
                                Out_height = src_rect1_dilate.rows-1 - Out_Y1;
                            Mat mean_out = src_rect_lo(Rect(Out_X1,Out_Y1, Out_wdith, Out_height));
                            Mat mask;
                            threshold(mean_out, mask, 120, 255, CV_THRESH_BINARY);
                            double m_out = mean(mean_out, mask)[0];

                            boundRect_lo1[i_lo1] = boundingRect(Mat(contours3[i_lo1]));
                            int X_1_lo1 = boundRect_lo1[i_lo1].tl().x; //矩形左上角X坐标值
                            int Y_1_lo1 = boundRect_lo1[i_lo1].tl().y; //矩形左上角Y坐标值
                            int X_2_lo1 = boundRect_lo1[i_lo1].br().x; //矩形右下角X坐标值
                            int Y_2_lo1 = boundRect_lo1[i_lo1].br().y; //矩形右下角Y坐标值

                            Mat r_1 = result1(Rect(X_1_lo1, Y_1_lo1, X_2_lo1 - X_1_lo1, Y_2_lo1 - Y_1_lo1));
                            Mat r_2 = result2(Rect(X_1_lo1, Y_1_lo1, X_2_lo1 - X_1_lo1, Y_2_lo1 - Y_1_lo1));

                            double m_r = mean(r_1, r_2)[0];
                            if (m_r-m_out< Light_low_limit)
                                continue;
                            rectangle(src_rect_out, boundRect_lo[i_lo].tl(), boundRect_lo[i_lo].br(), Scalar(100, 100, 100), 3, 8, 0);
                            num++;

                            /*if (X_1_lo1 > ROI_expand_figuremean - 10 && X_2_lo1 < X_1_lo1 + rect_width + 10)
                            {
                                Mat r_1 = result1(Rect(X_1_lo1, Y_1_lo1, X_2_lo1 - X_1_lo1, Y_2_lo1 - Y_1_lo1));
                                Mat r_2 = result2(Rect(X_1_lo1, Y_1_lo1, X_2_lo1 - X_1_lo1, Y_2_lo1 - Y_1_lo1));

                                double m_r = mean(r_1, r_2)[0];
                                if (m_r - m1 > 10)
                                {
                                    rectangle(src_rect_out, boundRect_lo[i_lo].tl(), boundRect_lo[i_lo].br(), Scalar(100, 100, 100), 3, 8, 0);
                                    num++;
                                }
                            }*/

                        }

                    }
                }
            }
        }
        if (num != 0)
        {
            *mresult = src;
            *causecolor = "漏光";
            return true;
        }
        else
            return false;
}

/*====================================================================
* 函 数 名: display_defect
* 功能描述: 对提取不到ROI的屏幕进行检查，若是有部分屏幕发亮，则认为是显示异常，否则就是无显示
* 输入：白底原图
* 输出：
* 时间：2020.9.20
* 其他：
======================================================================*/
bool display_defect(Mat src_white)
{
    bool result_display = false;

    double minp;			//最小灰度值
    double maxp;			//最大灰度值
    Point low_gray, high_gray;	//正常取时灰度最大最小点

    minMaxLoc(src_white, &minp, &maxp, &low_gray, &high_gray);	//求最大最小灰度点

    if(maxp<20)
        result_display=true;

    return result_display;
}

void debug_msg1(QVariant msg)
{
    QFile file("D:\\thread_time_after.txt");

    file.open(QFile::WriteOnly | QFile::Append | QFile::Text);

    file.write(msg.toString().toStdString().c_str());
    file.write(QString("\n").toStdString().c_str());
    file.close();
}

/*====================================================================
* 函 数 名: white_defect1
* 功能描述: 对白屏下的图片进行检测 包括异物 划伤 漏光 变形
* 输入：主前后左右白底和侧光图像
* 输出：白底下检测结果图和result
* 时间
* 其他：
======================================================================*/
bool white_defect1(Mat white,Mat mainfilter,Mat leftfilter,Mat rightfilter,Mat ceguang,Mat ceguangleft,Mat ceguangright,int num,Mat biankuangfilter)
{
    bool result=false;

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg1("刚进入检测子线程1"+current_date);
    //==========================提取ROI异常报显示异常==============================
    if(Ext_Result_BlackWhite==true||Ext_Result_Left==true||Ext_Result_Right==true)
    {
        Mresult_1_white=white;
        bool result_dispplay = display_defect(white);
        if(result_dispplay == true)
            causeColor_1_white="无显示";
        else
            causeColor_1_white="显示异常";  //白底没提取到屏幕报屏幕无显示
        result = true;
    }
//    current_date_time =QDateTime::currentDateTime();
//    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
//    debug_msg1("无显示检测完毕"+current_date);
    //==========================异物检测==============================
    if(result==false&&ForeignBody_Checked)
    {
        result=ForeignBody(mainfilter,ceguang,white,&Mresult_1_white,&causeColor_1_white);
    }

//    current_date_time =QDateTime::currentDateTime();
//    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
//    debug_msg1("异物检测完毕"+current_date);
    //==========================划伤检测==============================
    if(result==false&&Scratch_Checked)
    {
            //先对左右整张图片进行检测，然后对前后相机的半张图片进行检测即总共检测3次划伤缺陷
            result=Scratch(leftfilter,ceguangleft,&Mresult_1_white,&causeColor_1_white);
            if(!result)
            {
                result=Scratch(rightfilter,ceguangright,&Mresult_1_white,&causeColor_1_white);
            }
    }

    //==========================漏光检测==============================
    if(result==false&&Light_leakage_Checked)
    {
//        result=Light_leakage(mainfilter,ceguang,&Mresult_1_white,&causeColor_1_white);

    }

    //==========================死灯检测==============================
    if(result==false&&LampEye_Checked)
    {
//        result=LampEye(mainfilter, 5, 2, 15, 1, 6, 0.1, 8, 35, 0.1,0.12, 20, 9, 9, &Mresult_3_white, &causeColor_3_white);
        result = Dead_light(mainfilter, ceguang, &Mresult_1_white,&causeColor_1_white);
    }

    //==========================变形检测==============================
    if(result==false&&Frame_Checked)
    {
//        result = Frame(biankuangfilter, &Mresult_1_white, &causeColor_1_white);
    }

    return result;
}

/*====================================================================
* 函 数 名: white_defect2
* 功能描述: 对白屏下的图片进行检测 包括亮边 白点 暗角 爆灯
* 输入：主前后左右白底和侧光图像
* 输出：白底下检测结果图和result
* 时间
* 其他：
======================================================================*/
bool white_defect2(Mat white,Mat mainfilter,Mat ceguang,Mat CeL1_Enlarge_Fliter,Mat ceguangleft_enlarge,Mat CeR1_Enlarge_Fliter,Mat ceguangright_enlarge, int num)
{
    bool result=false;
    //==========================亮边检测==============================
    if(result==false&&Brightedge_Checked)
    {
            result=Brightedge(CeL1_Enlarge_Fliter,ceguangleft_enlarge,&Mresult_2_white, &causeColor_2_white,0);
            if(!result)
                result=Brightedge(CeR1_Enlarge_Fliter,ceguangright_enlarge,&Mresult_2_white, &causeColor_2_white,1);
    }

    //==========================白点检测==============================
    if(result==false&&WhiteDot_Checked)
    {
            result=WhiteDotLeft(mainfilter,ceguang,white,&Mresult_2_white,&causeColor_2_white);//主相机白点检测
    }
    //==========================暗角检测==============================
    if(result==false&&DarkCorner_Checked)
    {
//        switch(num)
//        {
//        case 1:result=DarkCorner(mainfilter,&Mresult_2_white,&causeColor_2_white);
//            break;
//        case 2:result=DarkCorner(mainfilter,&Mresult_2_white,&causeColor_2_white);
//            break;
//        }
    }
    //==========================爆灯检测==============================
    if(result==false&&boom_light_Checked)
    {
        result=boom_light(mainfilter,ceguang,&Mresult_2_white,&causeColor_2_white);
    }

//    if(result == false)
//        result=BoomDead_light(mainfilter,ceguang,&Mresult_2_white,&causeColor_2_white);

    return result;
}

/*====================================================================
* 函 数 名: white_defect3
* 功能描述: 对白屏下的图片进行检测 包括暗区 灯眼 少料 白印
* 输入：主前后左右白底和侧光图像
* 输出：白底下检测结果图和result
* 时间
* 其他：
======================================================================*/
bool white_defect3(Mat white_abshow1,Mat mainfilter,Mat leftfilter,Mat rightfilter,Mat ceguangleft,Mat ceguangright,int num)
{
    bool result=false;

    //delay(5000);

    //==========================移位检测==============================
    if(result==false&&Abshow_White_Detect_Checked)
    {
//        result=Abshow_White_Detect(white_abshow1,&Mresult_3_white,&causeColor_3_white);//主黑白相机白底显异检测
        result = Shifting(mainfilter, &Mresult_3_white, &causeColor_3_white);
    }

    //==========================少料检测==============================
    if(result==false&&LessMaterial_Checked)
    {
//        result = LessMaterial(mainfilter, &Mresult_3_white, &causeColor_3_white);
    }
    //==========================白印、膜拱、白团检测==============================
    if(result==false&&mura_Checked)
    {
        result = Mura_Decter(mainfilter, &Mresult_3_white, &causeColor_3_white);//主相机白印检测
//        Mat fit_resultL = fitL(leftfilter);
//        Mat fit_resultR = fitR(rightfilter);

//            QString imageType = "右";
//            bool result = mura_1(mura_imgR, ceguangright, fit_resultR, &Mresult_3_white, &MURA1, &causeColor_3_white, imageType);
//            if (result == false) {
//                imageType = "左";
//                result = mura_1(mura_imgL, ceguangleft, fit_resultL, &Mresult_3_white, &MURA1, &causeColor_3_white, imageType);
//            }
    }
    return result;
}

//void delay(int MSecs)
//{
//    QTime dieTime= QTime::currentTime().addMSecs(MSecs);
//    while (QTime::currentTime() < dieTime)
//        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
//}

/*====================================================================
* 函 数 名: white_defect
* 功能描述: 对白屏下的图片进行检测 包括少线 横线 异物 白印 灯眼 白点 划伤
* 输入：主前后左右白底和侧光图像
* 输出：白底下检测结果图和result
* 时间
* 其他：
======================================================================*/
bool white_defect(Mat white,Mat whiteleft,Mat whiteright,Mat whitecolor,Mat ceguang,Mat back,Mat ceguangback,Mat front,Mat ceguangfront,Mat ceguangleft,Mat ceguangright,int num,Mat white_abshow1,Mat CeL1_Enlarge, Mat CeR1_Enlarge,Mat white_biankuang,Mat ceguangleft_enlarge,Mat ceguangright_enlarge)
{
    //正常检测屏幕主黑白相机灰度均值在160上下 左相机在180上下 右侧相机在200上下
    bool result=false;
    Mat mainfilter=Gabor7(white);           //滤波去除水平和竖直方向的纹理
    Mat leftfilter=Gabor7(whiteleft);       //左侧白底滤波
    Mat rightfilter=Gabor7(whiteright);     //右侧白底滤波
    Mat biankuangfilter=Gabor7(white_biankuang);
    Mat CeL1_Enlarge_Fliter=Gabor7(CeL1_Enlarge);
    Mat CeR1_Enlarge_Fliter=Gabor7(CeR1_Enlarge);

    //==========================异物检测==============================
    if(result==false&&ForeignBody_Checked)
    {
        switch(num)
        {
        case 1:result=ForeignBody(mainfilter,ceguang,white,&Mresult_1_white,&causeColor_1_white);
            break;
        case 2:result=ForeignBody(mainfilter,ceguang,white,&Mresult_2_white,&causeColor_2_white);
            break;
        }
    }

    //==========================主黑白相机R角屏与直角屏白底显异检测/暗区检测==============================
    if(result==false&&Abshow_White_Detect_Checked)
    {
        switch(num)
        {
        case 1:result=Abshow_White_Detect(white_abshow1,&Mresult_1_white,&causeColor_1_white);//主黑白相机白底显异检测
            break;
        case 2:result=Abshow_White_Detect(white_abshow1,&Mresult_1_white,&causeColor_1_white);//主黑白相机白底显异检测
            break;
        }
    }

    //==========================灯眼检测==============================
    if(result==false&&LampEye_Checked)
    {
        switch(num)
        {
        case 1:result=LampEye(mainfilter, 5, 2, 15, 1, 6, 0.1, 8, 35, 0.1,0.12, 20, 9, 9, &Mresult_1_white, &causeColor_1_white);
            break;
        case 2:result=LampEye(mainfilter, 5, 2, 15, 1, 6, 0.1, 8, 35, 0.1,0.12, 20, 9, 9, &Mresult_2_white, &causeColor_2_white);
            break;
        }
    }

    //==========================变形检测==============================
    if(result==false&&Frame_Checked)
    {
        //result = Frame(mainfilter, &Mresult_1_white, &causeColor_1_white);
        result = Frame(biankuangfilter, &Mresult_1_white, &causeColor_1_white);
    }

    //亮边拿到白点之前，防止头部和尾部的亮边误检为白点，被检测人员当作误检
    //==========================亮边检测==============================
    if(result==false&&Brightedge_Checked)
    {
        switch(num)
        {
        case 1:
        {
            result=Brightedge(CeL1_Enlarge_Fliter,ceguangleft_enlarge,&Mresult_1_white, &causeColor_1_white,0);
            if(!result)
                result=Brightedge(CeR1_Enlarge_Fliter,ceguangright_enlarge,&Mresult_1_white, &causeColor_1_white,1);
            break;
        }
        case 2:result=Brightedge(CeR1_Enlarge,ceguangright_enlarge,&Mresult_1_white, &causeColor_1_white,1);
            break;
        }
    }

    //==========================白点检测==============================
    if(result==false&&WhiteDot_Checked)
    {
        switch(num)
        {
        case 1:
        {
            //先对左右整张图片进行检测，然后对前后相机的半张图片进行检测即总共检测3次白点缺陷
            result=WhiteDotLeft(mainfilter,ceguang,white,&Mresult_1_white,&causeColor_1_white);//主相机白点检测
//            if(!result)
//                result=WhiteDotLeft(rightfilter,ceguangright,whiteright,&Mresult_1_white,&causeColor_1_white);//右相机白点检测
//            if(!result)
//                result=WhiteDotLeft(leftfilter,ceguangleft,whiteleft,&Mresult_1_white,&causeColor_1_white);//左相机白点检测
            //对前后相机拍摄半张屏幕进行检测没写
             break;
        }
        case 2:result=WhiteDotLeft(mainfilter,ceguangback,white,&Mresult_1_white,&causeColor_1_white);//主相机白点检测
            break;
        }
    }

    //==========================白印、膜拱、白团检测==============================
    if(result==false&&mura_Checked)
    {
        Mat fit_resultL = fitL(leftfilter);
        Mat fit_resultR = fitR(rightfilter);

        switch(num)
        {
        case 1:
        {
            QString imageType = "右";
            bool result = mura_1(mura_imgR, ceguangright, fit_resultR, &Mresult_1_white, &MURA1, &causeColor_1_white, imageType);
            if (result == false) {
                imageType = "左";
                result = mura_1(mura_imgL, ceguangleft, fit_resultL, &Mresult_1_white, &MURA1, &causeColor_1_white, imageType);
            }
            break;
        }
        case 2:result=mura(leftfilter,rightfilter,&Mresult_2_white,&MURA2,&causeColor_2_white);
            break;
        }
    }

    //==========================暗角检测==============================
    if(result==false&&DarkCorner_Checked)
    {
//        switch(num)
//        {
//        case 1:result=DarkCorner(mainfilter,&Mresult_1_white,&causeColor_1_white);
//            break;
//        case 2:result=DarkCorner(mainfilter,&Mresult_1_white,&causeColor_1_white);
//            break;
//        }
    }

    //==========================划伤检测==============================
    if(result==false&&Scratch_Checked)
    {
        switch(num)
        {
        case 1:
        {
            //先对左右整张图片进行检测，然后对前后相机的半张图片进行检测即总共检测3次划伤缺陷
            result=Scratch(leftfilter,ceguangleft,&Mresult_1_white,&causeColor_1_white);
            if(!result)
            {
                result=Scratch(rightfilter,ceguangright,&Mresult_1_white,&causeColor_1_white);
            }
            break;
        }
        case 2:result=Scratch(rightfilter,ceguangright,&Mresult_2_white,&causeColor_2_white);
            break;
        }
    }

    //==========================漏光检测==============================
    if(result==false&&Light_leakage_Checked)
    {
        switch(num)
        {
        case 1:result=Light_leakage(mainfilter,ceguang,&Mresult_1_white,&causeColor_1_white);
            break;
        case 2:result=Light_leakage(mainfilter,ceguang,&Mresult_1_white,&causeColor_1_white);
            break;
        }
    }



    //==========================爆灯检测==============================
    if(result==false&&boom_light_Checked)
    {
        switch(num)
        {
        case 1:result=boom_light(mainfilter,ceguang,&Mresult_1_white,&causeColor_1_white);
            break;
        case 2:result=boom_light(mainfilter,ceguang,&Mresult_2_white,&causeColor_2_white);
            break;
        }
    }

    //==========================十字检测==============================
    if(result==false&&CrossLine_Checked)
    {
//        switch(num)
//        {
//        case 1:result=CrossLine(mainfilter,&Mresult_1_white,&causeColor_1_white);
//            break;
//        case 2:result=CrossLine(mainfilter,&Mresult_2_white,&causeColor_2_white);
//            break;
//        }
    }

    //==========================少料检测==============================
    if(result==false&&LessMaterial_Checked)
    {
        result = LessMaterial(mainfilter, &Mresult_1_white, &causeColor_1_white);
    }

    //==========================色差检测==============================
    if(result==false&&SunSpots_Checked)
    {
//        switch(num)
//        {
//        case 1:result=SunSpots(whitecolor,&Mresult_1_white,&causeColor_1_white);
//            break;
//        case 2:result=SunSpots(whitecolor,&Mresult_1_white,&causeColor_1_white);
//            break;
//        }
    }

    return result;

}

/*=========================================================================================
* 函 数 名: black_defect
* 功能描述: 对黑屏下的图片进行检测   包括:显示异常,漏光，红蓝斑，亮点，彩线
* 输入：主黑白相机黑底 主彩色相机黑底 主黑白相机向外扩大ROI区域图像
* 输出：黑底下检测结果图和result
* 时间
* 其他：
===========================================================================================*/
bool black_defect(Mat img_black, Mat img_black1,  Mat img_black2, Mat white,int border,int num)
{
    bool result=false;
    int white_trans_num=0;
    Mat black = img_black.clone();                        //黑白黑底图像
    Mat black1 = img_black1.clone();                      //彩色黑底图像
    cv::Mat black_gram=gamma(black,black_light_gram);     //亮点用参数
    cv::Mat black_gram1=gamma(black1,black_light_red);    //红蓝斑用参数
    cv::Mat black_color_line=gamma(black,black_colorline);//彩线用参数

     //==========================未提取到屏幕显示异常=================================
    if(isArea_2==true&&num==2)//不用工位2的
    {
        Mresult_2_black=black;
        causeColor_2_black="显示异常";
        return true;
    }
    if(isArea_1==true&&num==1)
    {
        Mresult_1_black=black;
        causeColor_1_black="显示异常";  //白底没提取到屏幕报屏幕无显示
        return true;
    }
    //==========================提取到屏幕白底亮度异常认为显示异常=====================
    if(num==1)
    {
        cv::Mat gray17;
        Mat im7=white(Rect(1600, 1200, 100, 100));
        gray17=im7.clone();
        //cvtColor(im7, gray17, CV_RGB2GRAY);
        Mat R_m1;
        Mat R_sd1;
        meanStdDev(gray17, R_m1, R_sd1);
        if (R_m1.at<double>(0,0) <white_gray_lower)//白底下存在异常，包括亮度偏低等各种情况
        {
            Mresult_1_black=im7;
            causeColor_1_black="显示异常";
            white_trans_num++;
            result=true;
        }
        if (white_trans_num>=5)//白底没切换过来报错
        {
            causeColor_1_black="显示异常";
        }
    }
    if(num==2)//工位2不用
    {   cv::Mat gray27;
        Mat im7=white(Rect(1600, 1200, 100, 100));
        cvtColor(im7, gray27, CV_RGB2GRAY);
        Mat R_m2;
        Mat R_sd2;
        meanStdDev(gray27, R_m2, R_sd2);
        if (R_m2.at<double>(0, 0) <white_gray_lower)
        {
            Mresult_2_black=im7;
            causeColor_2_black="显示异常";
            result=true;
        }
    }
    //==========================提取到屏幕白底亮度异常认为显示异常=====================
    if(result==false)
    {
        if(num==1)
        {
            cv::Mat gray1;
            gray1=black.clone();
            Mat R_m1;
            Mat R_sd1;
            meanStdDev(gray1, R_m1, R_sd1);
            if (R_m1.at<double>(0, 0) >black_gray_upper)//黑底没切换过来
            {
                Mresult_1_black=black;
                causeColor_1_black="显示异常";
                result=true;
            }
        }
        if(num==2)
        {
            cv::Mat gray2;
            cvtColor(black, gray2, CV_RGB2GRAY);
            Mat R_m2;
            Mat R_sd2;
            meanStdDev(gray2, R_m2, R_sd2);
            if (R_m2.at<double>(0, 0) >black_gray_upper)
            {
                Mresult_2_black=black;
                causeColor_2_black="显示异常";
                result=true;
            }
        }
        //==========================黑屏边角处大量漏光检测=====================
        if(result==false)
        {
            switch(num)
            {
            case 1:result=Leakage(img_black2,&Mresult_1_black,&causeColor_1_black);//边缘大量漏光检测
                break;
            case 2:result=Leakage(img_black2,&Mresult_2_black,&causeColor_2_black);//边缘大量漏光检测
                break;
            }
        }
        //==========================红蓝斑检测==============================
        if(result==false)
        {
            switch(num)
            {
            case 1:result=redBlueSpot(black_gram1,&Mresult_1_black,&causeColor_1_black);
                break;
            case 2:result=redBlueSpot(black_gram1,&Mresult_2_black,&causeColor_2_black);
                break;
            }
        }
        //==========================亮点检测==============================
        if(result==false)
        {
            switch(num)
            {
            case 1:result=light_point7(black_gram,white,&Mresult_1_black,&causeColor_1_black);
                break;
            case 2:result=light_point7(black_gram,white,&Mresult_2_black,&causeColor_2_black);
                break;
            }
        }    
        //==========================彩线检测====================================
        if(result==false)
        {
            switch(num)
            {
            case 1:result=ColorLine(black_color_line,&Mresult_1_black,&causeColor_1_black);//边缘大量漏光检测
                break;
            case 2:result=ColorLine(black_color_line,&Mresult_2_black,&causeColor_2_black);//边缘大量漏光检测
                break;
            }
        }
    }
    return result;
}

/*=========================================================================================
* 函 数 名: gray_detect
* 功能描述: 对灰屏下的图片进行检测
* 输入：主黑白拍摄灰底
* 输出：
* 时间
* 其他：没写空着的
===========================================================================================*/
bool gray_detect(Mat gray,Mat gray2,Mat gray3,Mat cg_main,Mat cg_left,Mat cg_right,Mat grayColor1,Mat grayColor2)
{
    bool result =false;

    Mat mainfilter=Gabor9(gray);       //主相机白底滤波
    Mat leftfilter=Gabor9(gray2);      //左侧白底滤波
    Mat rightfilter=Gabor9(gray3);     //右侧白底滤波

    //==========================条纹检测==============================
    result = Stripe(gray, &Mresult_gray, &causeColor_gray);

    //==========================白点检测==============================
    if(!result)
    {
        result=GrayDotLeft(leftfilter,cg_left,&Mresult_gray,&causeColor_gray);//左相机白点检测
        result=GrayDotLeft(rightfilter,cg_right,&Mresult_gray,&causeColor_gray);//右相机白点检测
        result=GrayDotLeft(mainfilter,cg_main,&Mresult_gray,&causeColor_gray);//主相机白点检测
    }

    //==========================闪屏检测==============================
    if(!result)
    {
        result=ScreenFlash(grayColor1,grayColor2,&Mresult_gray,&causeColor_gray);
    }
    return result;
}

/*=========================================================
*@函 数 名: fitL
*@功能描述: 左黑白相机开线程实现列拟合
*@param src 输入左相机灰度图像
*@修改时间：
*@备注说明：
=========================================================*/
Mat fitL(Mat left)
{
    Mat img_gray = left.clone();
    medianBlur(img_gray, img_gray, 3);//中值滤波滤除椒盐噪声,缺点耗时26毫秒 奇数半径越大效果越强
    Mat img1 = img_gray.clone();
    Mat img2;
    threshold(img1, img2, 30, 255, CV_THRESH_BINARY);//固定阈值二值化
    Mat img3 = ~img2;
    Mat structure_element_temp = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat img4;
    dilate(img3, img4, structure_element_temp);

    vector<vector<Point>> contours;
    findContours(img4, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);

    vector<Rect> boundRect(contours.size());
    double max_area = 0.0;
    int index = 0;
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > max_area)
        {
            max_area = area;
            index = i;
        }
    }
    Mat par_mask = Mat::zeros(img4.rows, img4.cols, CV_8UC1);
    drawContours(par_mask, contours, index, 255, FILLED, 8);


    if (Model_Mod_type != "矩形屏")
    {
        if (index >= contours.size())
        {
            mura_imgL = Mat::zeros(img_gray.rows, img_gray.cols, CV_8UC1);
            Mat error_result = Mat::zeros(img_gray.rows, img_gray.cols, CV_8UC1);
            return error_result;
        }
    }
    drawContours(par_mask, contours, index, 255, FILLED, 8);
    //imwrite("C:\\Users\\ch\\Desktop\\par_mask.bmp", par_mask);
    double area = contourArea(contours[index]);
    RotatedRect rect = minAreaRect(contours[index]);  //包覆轮廓的最小斜矩形
    float w = rect.size.width;
    float h = rect.size.height;
    CvPoint p = rect.center;
    int bangs_x = p.x - w / 2;
    int bangs_y = p.y + h / 2;
    //par_mask(Rect(x, y, w, h)) = uchar(255);
    //bitwise_and(img_gray,~par_mask,img_gray);

    if (Model_Mod_type != "矩形屏")
    {
        if (max_area > 50000 || bangs_x < 2700 || bangs_y>730)
        {
            mura_imgL = Mat::zeros(img_gray.rows, img_gray.cols, CV_8UC1);
            Mat error_result = Mat::zeros(img_gray.rows, img_gray.cols, CV_8UC1);
            return error_result;
        }
    }


    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(7, 7));
    Mat structure_element3 = getStructuringElement(MORPH_RECT, Size(5, 5));
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(3, 3));
    Mat binaryzation = Mat::zeros(img_gray.size(), img_gray.type());//二值化图像
    Mat dilate1 = Mat::zeros(img_gray.size(), img_gray.type());
    if (Model_Mod_type == "矩形屏")
    {
        Mat img_center_mid = img_gray(Rect(200, 100, img_gray.cols - 400, img_gray.rows - 200));//中间部分(中)
        Mat img_CenM_mask = Ployfit_Col7_1(img_center_mid, 3, 0, black_mura_gray_th);

        Mat img_center_left = img_gray(Rect(0, 100, 200, img_gray.rows - 200));//中间部分(左)
        Mat img_CenL_mask = Ployfit_Col7_1(img_center_left, 3, 0, black_mura_gray_th + 7);

        Mat img_center_right = img_gray(Rect(img_gray.cols - 200, 100, 200, img_gray.rows - 200));//中间部分(右)
        Mat img_CenR_mask = Ployfit_Col7_1(img_center_right, 3, 0, black_mura_gray_th);

        Mat img_top = img_gray(Rect(0, 0, img_gray.cols, 100));//上面部分
        img_top = matRotateClockWise90(img_top);
        Mat img_top_mask = Ployfit_Col7_1(img_top, 3, 0, black_mura_gray_th + 7);
        img_top_mask = matRotateClockWisefu90(img_top_mask);

        Mat img_bottom = img_gray(Rect(0, img_gray.rows - 100, img_gray.cols, 100));//下面部分
        img_bottom = matRotateClockWise90(img_bottom);//旋转顺时针90度
        Mat img_bottom_mask = Ployfit_Col7_1(img_bottom, 3, 0, black_mura_gray_th + 7);
        img_bottom_mask = matRotateClockWisefu90(img_bottom_mask);//旋转逆时针90度

        img_top_mask.copyTo(binaryzation(Rect(0, 0, binaryzation.cols, 100)));
        img_bottom_mask.copyTo(binaryzation(Rect(0, binaryzation.rows - 100, binaryzation.cols, 100)));
        img_CenM_mask.copyTo(binaryzation(Rect(200, 100, binaryzation.cols - 400, binaryzation.rows - 200)));
        img_CenL_mask.copyTo(binaryzation(Rect(0, 100, 200, binaryzation.rows - 200)));
        img_CenR_mask.copyTo(binaryzation(Rect(binaryzation.cols - 200, 100, 200, binaryzation.rows - 200)));
        //imwrite("D:\\binaryzation.bmp",binaryzation);
        mura_imgL = img_gray.clone();
    }
    else {
        //************************************************************************************************************************
        Mat temp = Mat::zeros(img_gray.size(), img_gray.type());
        Mat img_left_down = img_gray(Rect(0, img_gray.rows - 200, 200, 200));//中间部分(中)
        Mat img_left_mask = Ployfit_Col7_1(img_left_down, 3, 0, black_mura_gray_th - 2);
        Mat img_center_mid = img_gray(Rect(200, 0, img_gray.cols - 400, img_gray.rows - 200));//中间部分(中)
        Mat img_CenM_mask;
        Mat image_cen_1;
        img_CenM_mask = Ployfit_Col7_1(img_center_mid, 3, 0, black_mura_gray_th - 2);
        //if (Whiteprint_Detect_Flag == 2)
        //{
        //	img_CenM_mask = Ployfit_Col7(img_center_mid, 3, 0, black_mura_gray_th - 2);
        //	image_cen_1 = Ployfit_Col7_1(img_center_mid, 3, 0, black_mura_gray_th - 2);
        //}
        //else
        //{
        //	img_CenM_mask = Ployfit_Col7(img_center_mid, 3, 0, black_mura_gray_th + 2);
        //	morphologyEx(img_CenM_mask, img_CenM_mask, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
        //	dilate(img_CenM_mask, img_CenM_mask, structure_element2);                  //膨胀扩大缺陷面积
        //	image_cen_1 = Ployfit_Col7_1(img_center_mid, 3, 0, black_mura_gray_th - 2);
        //}
        Mat img_center_left = img_gray(Rect(0, 0, 200, img_gray.rows));//中间部分(左)
        Mat img_CenL_mask = Ployfit_Col7_1(img_center_left, 3, 0, black_mura_gray_th + 3);
        //Mat img_CenL_mask = Ployfit_Col7(img_center_left, 3, 0, black_mura_gray_th + 3);
        //morphologyEx(img_CenL_mask, img_CenL_mask, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
        //dilate(img_CenL_mask, img_CenL_mask, structure_element2);                  //膨胀扩大缺陷面积

        //针对左侧相机左下方的R角位置处 修改时间：2020年10月21日 修改人：HY
        //Mat image_left_down = img_center_left(Rect(0, img_gray.rows - 200, 200, 200));
        //Mat image_left_down_mask = Ployfit_Col7(image_left_down, 3, 0, black_mura_gray_th-1);
        //morphologyEx(image_left_down_mask, image_left_down_mask, CV_MOP_OPEN, structure_element1);
        //image_left_down_mask.copyTo(img_CenL_mask(Rect(0, img_gray.rows - 200, 200, 200)));


        Mat img_center_right = img_gray(Rect(img_gray.cols - 200, bangs_y, 200, 735 - bangs_y));//中间部分(右)  刘海->易撕贴
                                                                                                //Mat img_center_right=img_gray(Rect(img_gray.cols-200,200,200,img_gray.rows-200));//中间部分(右)
        Mat img_CenR_mask = Ployfit_Col7_1(img_center_right, 3, 0, black_mura_gray_th + 1);//2
                                                                                           //Mat img_CenR_mask = Ployfit_Col7(img_center_right, 3, 0, black_mura_gray_th + 1);//2
                                                                                           //morphologyEx(img_CenR_mask, img_CenR_mask, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
                                                                                           //dilate(img_CenR_mask, img_CenR_mask, structure_element2);                  //膨胀扩大缺陷面积

        Mat img_bottom_M = img_gray(Rect(200, img_gray.rows - 200, img_gray.cols - 400, 200));//下面部分(中)
        img_bottom_M = matRotateClockWise90(img_bottom_M);//旋转顺时针90度
        Mat img_bottomM_mask = Ployfit_Col7_1(img_bottom_M, 3, 0, black_mura_gray_th + 4);
        img_bottomM_mask = matRotateClockWisefu90(img_bottomM_mask);//旋转逆时针90度
        img_bottom_M = matRotateClockWisefu90(img_bottom_M);//
                                                            //morphologyEx(img_bottomM_mask, img_bottomM_mask, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
                                                            //dilate(img_bottomM_mask, img_bottomM_mask, structure_element2);                  //膨胀扩大缺陷面积
                                                            //dilate1 = binaryzation.clone();

        img_bottomM_mask.copyTo(binaryzation(Rect(200, binaryzation.rows - 200, binaryzation.cols - 400, 200)));
        //img_bottomR_mask.copyTo(binaryzation( Rect(binaryzation.cols-200,binaryzation.rows-200,200,200)));
        //img_bottomL_mask.copyTo(binaryzation( Rect(0,binaryzation.rows-200,200,200)));

        img_CenM_mask.copyTo(binaryzation(Rect(200, 0, binaryzation.cols - 400, binaryzation.rows - 200)));
        img_CenL_mask.copyTo(binaryzation(Rect(0, 0, 200, binaryzation.rows)));
        //img_CenR_mask.copyTo(binaryzation( Rect(binaryzation.cols-200,0,200,binaryzation.rows)));
        img_CenR_mask.copyTo(binaryzation(Rect(binaryzation.cols - 200, bangs_y, 200, 735 - bangs_y)));
        //imwrite("C:\\Users\\ch\\Desktop\\binaryzation.bmp", binaryzation);

        img_bottom_M.copyTo(temp(Rect(200, temp.rows - 200, temp.cols - 400, 200)));
        //img_bottom_R.copyTo(temp( Rect(temp.cols-200,temp.rows-200,200,200)));
        //img_bottom_L.copyTo(temp( Rect(0,temp.rows-200,200,200)));

        img_center_mid.copyTo(temp(Rect(200, 0, temp.cols - 400, temp.rows - 200)));
        img_center_left.copyTo(temp(Rect(0, 0, 200, temp.rows)));
        img_center_right.copyTo(temp(Rect(temp.cols - 200, bangs_y, 200, 735 - bangs_y)));
        //2个R角区域分块处理
        //        binaryzation(Rect(0, binaryzation.rows-200, 200, 1)) = uchar(0);
        //        binaryzation(Rect(199, binaryzation.rows-200, 1, 200)) = uchar(0);

        //        binaryzation(Rect(binaryzation.cols-200, binaryzation.rows-200, 200, 1)) = uchar(0);
        //        binaryzation(Rect(binaryzation.cols-200, binaryzation.rows-200, 1, 200)) = uchar(0);

        temp(Rect(0, 0, temp.cols, 5)) = uchar(0);
        temp(Rect(0, 0, 5, temp.rows)) = uchar(0);
        temp(Rect(temp.cols - 5, 0, 5, temp.rows)) = uchar(0);

        mura_imgL = temp.clone();
    }
    return binaryzation;
}

/*=========================================================
*@函 数 名: fitR
*@功能描述: 右黑白相机开线程实现列拟合
*@param src 输入右相机灰度图像
*@修改时间：
*@备注说明：
=========================================================*/
Mat fitR(Mat right)
{
    Mat img_gray = right.clone();
    medianBlur(img_gray, img_gray, 3);//中值滤波滤除椒盐噪声,缺点耗时26毫秒 奇数半径越大效果越强
    Mat img1 = img_gray.clone();
    Mat img2;
    threshold(img1, img2, 30, 255, CV_THRESH_BINARY);//固定阈值二值化
    Mat img3 = ~img2;
    Mat structure_element_temp = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat img4;
    dilate(img3, img4, structure_element_temp);

    vector<vector<Point>> contours;
    findContours(img4, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);

    vector<Rect> boundRect(contours.size());
    double max_area = 0.0;
    int index = 0;
    for (vector<int>::size_type i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > max_area)
        {
            max_area = area;
            index = i;
        }
    }
    Mat par_mask = Mat::zeros(img4.rows, img4.cols, CV_8UC1);

    if (Model_Mod_type != "矩形屏")
    {
        if (index >= contours.size())
        {
            mura_imgR = Mat::zeros(img_gray.rows, img_gray.cols, CV_8UC1);
            Mat error_result = Mat::zeros(img_gray.rows, img_gray.cols, CV_8UC1);
            return error_result;
        }
    }

    drawContours(par_mask, contours, index, 255, FILLED, 8);
    double c = contourArea(contours[index]);
    RotatedRect rect = minAreaRect(contours[index]);  //包覆轮廓的最小斜矩形
    float w = rect.size.width;
    float h = rect.size.height;
    CvPoint p = rect.center;
    int x = p.x - w / 2;
    int y = p.y - h / 2;
    int bangs_y_up = p.y - h / 2;
    int bangs_y_down = p.y + h / 2;
    if (Model_Mod_type != "矩形屏")
    {
        //        par_mask(Rect(x, y, w, h)) = uchar(255);
        //        bitwise_and(img_gray,~par_mask,img_gray);
        if (max_area < 10000 || max_area>50000 || x < 2700 || bangs_y_up < 250 || bangs_y_down>999)
        {
            mura_imgR = Mat::zeros(img_gray.rows, img_gray.cols, CV_8UC1);
            Mat error_result = Mat::zeros(img_gray.rows, img_gray.cols, CV_8UC1);
            return error_result;
        }
    }

    Mat structure_element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat structure_element2 = getStructuringElement(MORPH_RECT, Size(7, 7));
    Mat structure_element3 = getStructuringElement(MORPH_RECT, Size(5, 5));
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(3, 3));
    Mat binaryzation = Mat::zeros(img_gray.size(), img_gray.type());//二值化图像
    Mat dilate1 = Mat::zeros(img_gray.size(), img_gray.type());
    if (Model_Mod_type == "矩形屏")
    {
        Mat img_center_mid = img_gray(Rect(200, 100, img_gray.cols - 400, img_gray.rows - 200));//中间部分(中)
        Mat img_CenM_mask = Ployfit_Col7(img_center_mid, 3, 0, black_mura_gray_th);

        Mat img_center_left = img_gray(Rect(0, 100, 200, img_gray.rows - 200));//中间部分(左)
        Mat img_CenL_mask = Ployfit_Col7(img_center_left, 3, 0, black_mura_gray_th + 7);

        Mat img_center_right = img_gray(Rect(img_gray.cols - 200, 100, 200, img_gray.rows - 200));//中间部分(右)
        Mat img_CenR_mask = Ployfit_Col7(img_center_right, 3, 0, black_mura_gray_th);

        Mat img_top = img_gray(Rect(0, 0, img_gray.cols, 100));//上面部分
        img_top = matRotateClockWise90(img_top);
        Mat img_top_mask = Ployfit_Col7(img_top, 3, 0, black_mura_gray_th + 7);
        img_top_mask = matRotateClockWisefu90(img_top_mask);

        Mat img_bottom = img_gray(Rect(0, img_gray.rows - 100, img_gray.cols, 100));//下面部分
        img_bottom = matRotateClockWise90(img_bottom);//旋转顺时针90度
        Mat img_bottom_mask = Ployfit_Col7(img_bottom, 3, 0, black_mura_gray_th + 7);
        img_bottom_mask = matRotateClockWisefu90(img_bottom_mask);//旋转逆时针90度

        img_top_mask.copyTo(binaryzation(Rect(0, 0, binaryzation.cols, 100)));
        img_bottom_mask.copyTo(binaryzation(Rect(0, binaryzation.rows - 100, binaryzation.cols, 100)));
        img_CenM_mask.copyTo(binaryzation(Rect(200, 100, binaryzation.cols - 400, binaryzation.rows - 200)));
        img_CenL_mask.copyTo(binaryzation(Rect(0, 100, 200, binaryzation.rows - 200)));
        img_CenR_mask.copyTo(binaryzation(Rect(binaryzation.cols - 200, 100, 200, binaryzation.rows - 200)));
        mura_imgR = img_gray.clone();
    }
    else {
        //************************************************************************************************************************

        Mat temp = Mat::zeros(img_gray.size(), img_gray.type());
        Mat img_center_mid = img_gray(Rect(200, 200, img_gray.cols - 400, img_gray.rows - 200));//中间部分(中)
        Mat img_CenM_mask;
        Mat img_cen_1;
        img_cen_1 = Ployfit_Col7_1(img_center_mid, 3, 0, black_mura_gray_th - 2);
        Mat img_center_left = img_gray(Rect(0, 0, 200, img_gray.rows));//中间部分(左)
        Mat img_CenL_mask = Ployfit_Col7_1(img_center_left, 3, 0, black_mura_gray_th);
        Mat img_center_right = img_gray(Rect(img_gray.cols - 200, 0, 200, bangs_y_up));//中间部分(右)
                                                                                       //Mat img_CenR_mask = Ployfit_Col7(img_center_right, 3, 0, black_mura_gray_th);
        Mat img_CenR_mask_1 = Ployfit_Col7_1(img_center_right, 3, 0, black_mura_gray_th);
        //morphologyEx(img_CenR_mask, img_CenR_mask, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
        //dilate(img_CenR_mask, img_CenR_mask, structure_element2);                  //膨胀扩大缺陷面积

        Mat img_center_right1 = img_gray(Rect(img_gray.cols - 200, bangs_y_down, 200, img_gray.rows - (bangs_y_down + 1)));//中间部分(右)

        Mat img_CenR_mask1 = Ployfit_Col7_1(img_center_right1, 3, 0, black_mura_gray_th);
        //morphologyEx(img_CenR_mask1, img_CenR_mask1, CV_MOP_OPEN, structure_element1);      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
        //dilate(img_CenR_mask1, img_CenR_mask1, structure_element2);                  //膨胀扩大缺陷面积

        Mat img_top_M = img_gray(Rect(200, 0, img_gray.cols - 400, 200));//上面部分(中)
        img_top_M = matRotateClockWise90(img_top_M);
        Mat img_topM_mask = Ployfit_Col7_1(img_top_M, 3, 0, black_mura_gray_th + 1);//-1
        img_topM_mask = matRotateClockWisefu90(img_topM_mask);
        img_top_M = matRotateClockWisefu90(img_top_M);//

                                                      //erode(img_topM_mask, img_topM_mask, structure_element2);
        morphologyEx(img_topM_mask, img_topM_mask, CV_MOP_OPEN, structure_element3);//1      //先腐蚀，再膨胀，可清除一些小亮点，放大局部低亮度的区域
        dilate(img_topM_mask, img_topM_mask, structure_element2);                  //膨胀扩大缺陷面积

        dilate1 = binaryzation.clone();
        //二值图
        img_topM_mask.copyTo(binaryzation(Rect(200, 0, binaryzation.cols - 400, 200)));
        img_cen_1.copyTo(binaryzation(Rect(200, 200, binaryzation.cols - 400, binaryzation.rows - 200)));
        img_CenL_mask.copyTo(binaryzation(Rect(0, 0, 200, binaryzation.rows)));
        img_CenR_mask_1.copyTo(binaryzation(Rect(binaryzation.cols - 200, 0, 200, bangs_y_up)));
        img_CenR_mask1.copyTo(binaryzation(Rect(binaryzation.cols - 200, bangs_y_down, 200, binaryzation.rows - (bangs_y_down + 1))));

        //暂时屏蔽左上R角
        //binaryzation(Rect(0,0,200,200)) = uchar(0);
        //imwrite("C:\\Users\\ch\\Desktop\\binaryzation_L.bmp", binaryzation);
        //测试
        img_top_M.copyTo(temp(Rect(200, 0, temp.cols - 400, 200)));
        img_center_mid.copyTo(temp(Rect(200, 200, temp.cols - 400, temp.rows - 200)));
        img_center_left.copyTo(temp(Rect(0, 0, 200, temp.rows)));
        //img_center_right.copyTo(temp( Rect(temp.cols-200,0,200,temp.rows)));
        img_center_right.copyTo(temp(Rect(temp.cols - 200, 0, 200, bangs_y_up)));
        img_center_right1.copyTo(temp(Rect(temp.cols - 200, bangs_y_down, 200, temp.rows - (bangs_y_down + 1))));

        temp(Rect(0, temp.rows - 5, temp.cols, 5)) = uchar(0);
        temp(Rect(0, 0, 5, temp.rows)) = uchar(0);
        temp(Rect(temp.cols - 5, 0, 5, temp.rows)) = uchar(0);
        mura_imgR = temp.clone();
    }
    return binaryzation;
}

/*========================================================================================================
* 函 数 名: Ployfit_Col
* 功能描述: 对”列“数据进行多项式拟合，并返回拟合结果
* 输入变量: 1.输入图片 2.拟合的阶数（若poly_n=2，则y=a0+a1*x+a2*x^2 ）3.是否保存数据（FLASE）
4.图片行数 5.图片列数 6.行拟合阈值
==========================================================================================================*/
Mat Ployfit_Col7_1(Mat img_col, int poly_n, bool isSaveOrNot, double Scoral)
{
    Fit fit1;
    Mat mask;
    Mat img_temp = img_col.clone();
    blur(img_temp, img_temp, Size(15, 15));
    threshold(img_temp, mask, 50, 255, CV_THRESH_BINARY);
    //Mat structure_element = getStructuringElement(MORPH_RECT, Size(0, 0));
    //erode(mask, mask, structure_element);
    Mat temp1;
    Mat temp2;
    meanStdDev(img_temp, temp1, temp2, mask);
    double agv = temp1.at<double>(0, 0);//灰度均值
    double std = temp2.at<double>(0, 0);//灰度方差
    int nRows = img_col.rows;
    int nCols = img_col.cols;
    int sigma = 0;

    vector<vector<int>> array(nRows);
    vector<vector<int>> array_col_diff(nRows);
    vector<vector<int>> array_col_diff_cols(nCols);
    vector<vector<uchar>> array_clone2(nRows);
    vector<vector<uchar>> array_clone2_cols(nCols);
    vector<int> array_clone_cols;
    vector<int> array_clone_rows;
    vector<int> array_rows;
    vector<int> array_cols(nCols);
    int count = 0;
    for (int i = 0; i < nCols; i++)
    {
        array_col_diff_cols[i].resize(nRows);
        array_clone2_cols[i].resize(nRows);
    }
    for (int i = 0; i < nRows; i++) {
        array_col_diff[i].resize(nCols);
        array_clone2[i].resize(nCols);
        array[i].resize(nCols);
    }
    array = Read_Mat7(array, img_col, nRows, nCols);  //将图像数据存入vector
    for (int i = 0; i < nRows; i += 2)
    {
        for (int j = 0; j < nCols; j++)
        {
            if (mask.at<uchar>(i, j) != 0)
            {
                if (array[i][j] > agv - std)
                {
                    array_clone_rows.push_back(array[i][j]);
                    //array_clone_rows[j] = array[i][j];
                }
                else {
                    array_clone_rows.push_back(agv);
                    //array_clone_rows[j] = agv;
                    //array[m][i]=agv;
                }

                array_rows.push_back(j);
            }
        }
        if (array_rows.size() != 0)
        {
            fit1.polyfit(array_rows, array_clone_rows, poly_n, isSaveOrNot);
        }


        count = 0;
        for (int j = 0; j < nCols; j++)
        {
            if (mask.at<uchar>(i, j) != 0)
            {
                int nihe = fit1.getFactor(0) + fit1.getFactor(1) * (j + 1) + fit1.getFactor(2)  * pow(j + 1, 2) + fit1.getFactor(3)  * pow(j + 1, 3);
                array_col_diff[i][j] = array[i][j] - nihe;
                if (array_col_diff[i][j] > sigma) {
                    sigma = array_col_diff[i][j];
                }
            }

        }
        array_rows.clear();
        array_clone_rows.clear();
    }
    /* 对行拟合进行阈值选择 */
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
            //if (array_col_diff[i][j] < sigma1 - 2)
        {
            if (array_col_diff[i][j] < 5 || array_col_diff[i][j] >  12)
                array_clone2[i][j] = 0;
            else
                array_clone2[i][j] = 255;
        }
    }
    //Mat img_2 = Vec2Mat7(array_col_diff, nRows, nCols);
    Mat img_1 = Vec2Mat7(array_clone2, nRows, nCols);
    array_clone2.clear();
    array_col_diff.clear();
    array_col_diff.resize(nRows);
    array_clone2.resize(nRows);
    for (int i = 0; i < nRows; i++) {
        array_col_diff[i].resize(nCols);
        array_clone2[i].resize(nCols);
    }
    array_rows.clear();
    for (int i = 0; i < nCols; i += 2)
    {
        for (int m = 0; m < nRows; m++)
        {
            if (mask.at<uchar>(m, i) != 0)
            {
                if (array[m][i] > agv - std)
                {
                    array_clone_cols.push_back(array[m][i]);
                    //array_clone_cols[m] = array[m][i];
                }
                else {
                    array_clone_cols.push_back(agv);
                    //array_clone_cols[m] = agv;
                    //array[m][i]=agv;
                }
                array_rows.push_back(m);

            }
        }
        if (array_rows.size() != 0)
        {
            fit1.polyfit(array_rows, array_clone_cols, poly_n, isSaveOrNot);
        }
        for (int j = 0; j < nRows; j++)
        {
            if (mask.at<uchar>(j, i) != 0)
            {
                array_col_diff[j][i] = array[j][i] - (fit1.getFactor(0) + fit1.getFactor(1) * (j + 1) + fit1.getFactor(2)  * pow(j + 1, 2) + fit1.getFactor(3)  * pow(j + 1, 3));;
                if (array_col_diff[j][i] > sigma) {
                    sigma = array_col_diff[j][i];
                }
            }

        }
        array_rows.clear();
        array_clone_cols.clear();
    }
    /* 对列列拟合进行阈值选择 */
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
            //if (array_col_diff[i][j] < sigma1 - 2)
        {
            if (array_col_diff[i][j] < 5 || array_col_diff[i][j] >  12)
                array_clone2[i][j] = 0;
            else
                array_clone2[i][j] = 255;
        }
    }
    Mat structureElement = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));

    Mat img = Vec2Mat7(array_clone2, nRows, nCols);
    dilate(img, img, structureElement, Point(-1, -1), 1);
    dilate(img_1, img_1, structureElement, Point(-1, -1), 1);
    erode(img, img, structureElement, Point(-1, -1), 1);
    erode(img_1, img_1, structureElement, Point(-1, -1), 1);
    Mat outputImage;
    Mat outputImage_1;
    Mat outputImage_2;
    bitwise_and(img_1, img, outputImage);
    bitwise_or(img_1, img, outputImage_2);
    structureElement = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
    dilate(outputImage, outputImage_1, structureElement, Point(-1, -1), 1);

    return outputImage;
}
