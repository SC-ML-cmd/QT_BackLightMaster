#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>//Ui::MainWindow，通过这个ui成员去访问界面上的元素
#include "camera.h"
#include <QModbusDataUnit>
#include "datastatistics_win.h"
#include "QTimer"
#include "dbhelper.h"
#include "detector.h"
#include <QNetworkInterface>
#include <QHostInfo>
#include<hikvisionsdk.h>
#include<save_images.h>

class QModbusClient;
extern QString production_lot_code;
static QModbusClient *modbusDevice;
static int Data_Form_Plc;

extern HWND appwnd1;
extern HWND appwnd2;
extern HWND appwnd3;
extern HWND appwnd4;
extern HWND appwnd5;
extern HWND appwnd6;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //bool write_Modbus(int address,int value);
    bool write_Modbus(int address,int value,QModbusClient *modbusDevice_temp);
    void readReady();
    void read_Modbus(int address,QModbusClient *modbusDevice_temp);
    //void read_Modbus(int address);
    void InitTimer();
    void display();
    int detect();
    int detect_offine();
    void myFunc1();//,cv::Mat* ceL_1,cv::Mat* ceR_1,cv::Mat* ceL_2,cv::Mat* ceR_2
    void myFunc2();
    void myFunc3();
    void White_Thread1(QList <Mat> img_white);
    void White_Thread2(QList <Mat> img_white);
    void White_Thread3(QList <Mat> img_white);
    void test(Mat white);
    int Photograph();//拍照线程
    HikvisionSDK *camera=HikvisionSDK::getInstance();
    void updata_database(QString defect_type);
    void debug_msg(QVariant msg);
    void main_msg(QVariant msg);
    void left_msg(QVariant msg);
    void right_msg(QVariant msg);
    void ini_state();

signals:
    void read_Modbus_Num(int);
    void read_Modbus_Num1(int);
    bool write_Modbus_Num(int,int);
    void TimerTimeOut_Signal();
    void Restart_Timer_Signal();
    void Write_Photo_Signal();
    void Dectect_Result();

public slots:
    void read_Modbus(int address);
    bool write_Modbus(int address,int value);
    void TimerTimeOut();
    void Restart_Timer();
    void Dect_Result();
    void White1_Thread_Finish();
    void White2_Thread_Finish();
    void White3_Thread_Finish();

    void Force_Save(QString Path);
    void Write_Photo_Finish();

private slots:
    void on_pushButton_2_clicked();
    void on_action_triggered();
    void on_actionPLC_2_triggered();
    //void TimerTimeOut();
    void TimerTimeOut_temp();
    void TimerTimeOut_Offine();
    QImage MatToQImage(const cv::Mat& mat);
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_action_PLC_triggered();
    void on_pushButton_10_clicked();
    void on_label_10_linkActivated(const QString &link);
    void receiveData(QString e,QString a);
    void closeEvent(QCloseEvent *event);


    void on_action_13_triggered();

    void on_action_5_triggered();

    void on_action_7_triggered();

    void on_action_12_triggered();

//    void on_actionPLC_triggered();

    void on_action_11_triggered();

    void on_action_6_triggered();

    void on_action_8_triggered();

    void on_actionPLC_3_triggered();

    void on_action_10_triggered();

    void on_action_9_triggered();

    void on_action_15_triggered();

    void on_action_17_triggered();

    void on_action_18_triggered();

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_action_14_triggered();

    void on_action_clear_triggered();

private:
    Ui::MainWindow *ui;
    QTimer m_timer;
    QTimer m_timer_temp;
    cv::Mat ceguag_1;
    cv::Mat ceguag_2;
    cv::Mat white_1;
    cv::Mat white_2;
    cv::Mat Image_1_black;
    cv::Mat Image_2_black;
    cv::Mat Image_1_white;
    cv::Mat Image_2_white;
    cv::Mat Image_1_Right;
    cv::Mat Image_2_Right;
    cv::Mat Image_1_Left;
    cv::Mat Image_2_Left;
    cv::Mat Image_1_blue;
    cv::Mat Image_2_blue;
    cv::Mat Image_1_red;
    cv::Mat Image_2_red;
    cv::Mat Image_1_green;
    cv::Mat Image_2_green;
    cv::Mat Image_1_gray;
    cv::Mat Image_2_gray;
    bool F=true;
    void initial();
};

#endif // MAINWINDOW_H
