#include "camera_test.h"
#include "ui_camera_test.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

camera_test::camera_test(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::camera_test)
{
    ui->setupUi(this);
}

camera_test::~camera_test()
{
    delete ui;
}


int test()

{

    Mat imageSource = imread("2.jpg");
    Mat imageGrey;
    cvtColor(imageSource, imageGrey, CV_RGB2GRAY);
    Mat imageSobel;
    Sobel(imageGrey, imageSobel, CV_16U, 1, 1);
    //图像的平均灰度
    double meanValue = 0.0;
    meanValue = mean(imageSobel)[0];
    //double to string
    stringstream meanValueStream;
    string meanValueString;

    meanValueStream << meanValue;

    meanValueStream >> meanValueString;

    meanValueString = "Articulation(Sobel Method): " + meanValueString;

    putText(imageSource, meanValueString, Point(20, 50), CV_FONT_HERSHEY_COMPLEX, 0.8, Scalar(255, 255, 25), 2);

    imshow("Articulation", imageSource);

    waitKey(0);
    return 0;

}
