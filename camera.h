#ifndef FORM_CAMERA_H
#define FORM_CAMERA_H
#include <QWidget>
#include "hikvisionsdk.h"
#include "QTimer"
#include<QCloseEvent>
namespace Ui {
class Form_Camera;
}

class Form_Camera : public QWidget
{
    Q_OBJECT

public:
    explicit Form_Camera(QWidget *parent = nullptr);
    ~Form_Camera();
    // HikvisionSDK camera;

     HikvisionSDK *camera=HikvisionSDK::getInstance();

signals:
 void send();
private slots:
    void on_pushButton_clicked();

    void on_pushButton_Add_View_1_clicked();

    void on_pushButton_Remove_View_1_clicked();

    void on_pushButton_Add_View_2_clicked();

    void on_pushButton_Remove_View_2_clicked();

    void on_pushButton_Add_View_3_clicked();

    void on_pushButton_Remove_View_3_clicked();

    void on_pushButton_Add_View_4_clicked();

    void on_pushButton_Remove_View_4_clicked();

    void on_pushButton_Add_View_5_clicked();

    void on_pushButton_Remove_View_5_clicked();

    void on_pushButton_Add_View_6_clicked();

    void on_pushButton_Remove_View_6_clicked();

    void on_pushButton_Full_Screen_1_clicked();

    void on_pushButton_Full_Screen_2_clicked();

    void on_pushButton_Full_Screen_3_clicked();

    void on_pushButton_Full_Screen_4_clicked();

    void on_pushButton_Full_Screen_5_clicked();

    void on_pushButton_Full_Screen_6_clicked();

    void on_horizontalSlider_sliderMoved(int position);

   // void on_spinBox_valueChanged(int arg1);

    void on_spinBox_1_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

    void on_horizontalSlider_2_sliderMoved(int position);

    void on_horizontalSlider_3_sliderMoved(int position);

    void on_spinBox_3_valueChanged(int arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_horizontalSlider_4_sliderMoved(int position);

    void on_spinBox_5_valueChanged(int arg1);

    void on_horizontalSlider_5_sliderMoved(int position);

    void on_spinBox_6_valueChanged(int arg1);

    void on_horizontalSlider_6_sliderMoved(int position);

    void on_pushButton_8_clicked();

    void on_pushButton_Adaptive_exposure_clicked();

//    void Control();

    void Adaptive_exposure();

    bool Adaptive_exposure_main_ROI(int C_index);

    double Adaptive_exposure_PID(double meangray,int Set_gray,double I_limit);

    double Constrain_Float(double amt, double low, double high);
    cv::Mat PerspectiveTransformation(cv::Mat image,int C_index);

    void on_pushButton_10_clicked();

    void on_pushButton_manualExposure_clicked();

    double GetMeanGrayValue(int i);
    double Half_Control(double meangray,int Set_gray,double Current);
    void on_comboBox_Pro_Mode_currentTextChanged(const QString &arg1);

    void on_pushButton_3_clicked();

    //void on_pushButton_2_clicked();

    void closeEvent(QCloseEvent *event);

    void on_comboBox_Adaptive_exposure_list_currentTextChanged(const QString &arg1);

    void on_pushButton_2_clicked();

private:
     char strUserName[256] = {0};

     HWND appwnd_1;
     HWND appwnd_2;
     HWND appwnd_3;
     HWND appwnd_4;
     HWND appwnd_5;
     HWND appwnd_6;
     QTimer m_timer;

public:

    Ui::Form_Camera *ui;
    void display();
//protected:

//     void closeEvent(QCloseEvent *event);
};

#endif // FORM_CAMERA_H
