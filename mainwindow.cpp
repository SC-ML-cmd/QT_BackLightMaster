#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSplitter>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QUrl>
#include<QtConcurrent>
#include<QElapsedTimer>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include "parameter.h"
#include"global_variable.h"
#include "batch_manage.h"
#include "datastatistics_win.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QFileDialog>
#include "parameter_control.h"
#include "promodel.h"                    //产品型号界面头文件
#include<production_lot_information_win.h>
#include<hikvisionsdk.h>
#include"detector_choose.h"
#include"history_data.h"

bool First_PLC_Result;
bool Flag_Dec_Complete=false;
bool Flag_First_Photo=true;
bool Flag_First_Func=true;

QString Flag_Running_State="Online";

struct Dect_Photo
   {
       //测光
       Mat SideLight_Main;      //主相机测光
       Mat SideLight_Left;      //左相机测光
       Mat SideLight_Right;
//       Mat SideLight_Front;
//       Mat SideLight_Back;
       //白底
       Mat White_Main;
       Mat White_Left;
       Mat White_Right;
       //Mat White_Front;
//       Mat White_Back;
//       Mat White_Color;
//       //黑底
//       Mat Black_Main;
//       Mat Black_Color;
//       //灰底
//       Mat Gray_Main;
//       Mat Gray_Left;
//       Mat Gray_Right;
//       Mat Gray_Color1;
//       Mat Gray_Color2;
   };

QQueue<Dect_Photo> Pic;       //定义一个结构体队列

int Exposure_Ceguang_Front;              //主黑白相机侧光曝光值
int Exposure_Ceguang_Back;               //主黑白相机侧光曝光值
int Exposure_Ceguang_Left;               //主黑白相机侧光曝光值
int Exposure_Ceguang_Right;              //主黑白相机侧光曝光值

int Exposure_Ceguang_Main;        //主黑白相机侧光曝光值
int Exposure_White_Black_Left;    //左黑白白底相机曝光值
int Exposure_White_Black_Middle;  //主黑白白底相机曝光值
int Exposure_White_Color_Right;   //主彩色白底相机曝光值
int Exposure_White_Black_Front;
int Exposure_White_Black_Back;
int Exposure_White_Color_Middle;

int Exposure_Black_Middle;       //主黑白相机黑色底色曝光值
int Exposure_Black_Color;        //主彩色相机黑色底色曝光值
int Exposure_Gray_Left;          //左相机灰底曝光值
int Exposure_Gray_Right;         //右相机灰底曝光值
int Exposure_Gray_Main;          //主相机灰底曝光值

int Num_1_L_Exposure_white;
int Num_1_R_Exposure_white;
int Num_1_L_Exposure_black;
int Num_1_R_Exposure_black;

int Num_2_L_Exposure_white;
int Num_2_R_Exposure_white;
int Num_2_L_Exposure_black;
int Num_2_R_Exposure_black;

int Num_1_ceLL_Exposure_white;
int Num_1_ceLR_Exposure_white;
int Num_1_ceRL_Exposure_white;
int Num_1_ceRR_Exposure_white;

int Num_2_ceLL_Exposure_white;
int Num_2_ceLR_Exposure_white;
int Num_2_ceRL_Exposure_white;
int Num_2_ceRR_Exposure_white;

int BoxOpenTime;
int BlackSwitchTime;
int GraySwitchTime;
int BoxSwitchTime;
int SideLightOpenTime;
int Std;
int yiwu_pre_size;
double yiwu_pre_th;
int yiwu_area_upper;
int yiwu_area_lower;
int yiwu_k_w_h;
int yiwu_sec_th;
int yiwu_sec_size;
int yiwu_sec_area_lower;
int yiwu_sec_area_upper;
double yiwu_lighth;
double white_gray_lower;
int black_gray_upper;
double black_light_gram;
double black_light_red;
double black_colorline;
int black_light_th1;
int black_light_th2;
int black_lightPoint_step;
int black_lightPoint_size;
int black_lightPoint_gray_th;
double black_lightPoint_area_lower;
int black_lightPoint_area_upper;
int black_lightPoint_diff;
double black_lightPoint_w_h;

int redBlue_area_lower;
int redBlue_area_upper;
int redBlue_numupper;

int black_mura_step;
int black_mura_size;
double black_mura_gray_th;
int black_mura_area_lower;
int black_mura_area_upper;
int black_mura_boder;
int black_mura_corner;
int black_mura_area_1;
int black_mura_th_1;
int black_mura_area_2;
int black_mura_th_2;
double mura_bubbleth;
double mura_centerlighth;
double mura_lighth;
int modbus_time;
int yiwu_ceguang_th;
static QStatusBar* bar;
static bool F_start=false;
float time_1;

int white_seban_size;
int white_seban_step;
int white_seban_th_h_low;
int white_seban_th_s_low;
int white_seban_th_v_low;
int white_seban_th_h_up;
int white_seban_th_s_up;
int white_seban_th_v_up;
int white_seban_area_lower;
int white_seban_area_upper;
int white_seban_size2;

int lackline_bolckSize;    //少线分割邻域块大小
double lackline_delta;     //少线分割邻域块阈值
double longShortlowerth;   //少线长宽比下限
int shortHigherth;         //少线宽度上限
int longLowerth;
double lackscratchth;

double whitePoint_threshold;
int whitePoint_step;         //步长
int whitePoint_lowerArea;    //面积下限
int whitePoint_higherArea;   //面积上限
double scratchth;            //白点排除划痕阈值大于则认为贴膜划痕
double bubbleth;             //白点排除气泡阈值大于则认为贴膜气泡
int dotdomainnum;            //白点连通域个数大于则认为噪点
double defectouterth;	     //白点缺陷处灰度均值与缺陷外围灰度差
double corewholeth;          //白点缺陷中心点灰度值与整块疑似缺陷灰度均值差
double spotpeak;	         //白点缺陷最亮点与最暗点灰度差
double siecevariance;	     //侧光图中排除划痕参数
double whitePoint_w_h;

int scratch__bolckSize;               //划伤自适应分割领域块大小
double scratch__delta;                //划伤自适应分割领阈值大小
int scratch_lowerArea;                //划伤面积下限
int scratch_higherArea;               //划伤面积上限
double scratch_lowerLongShort;      //划伤长宽比下限
double scratch_higherLongShort;    //划伤长宽比上限
int scratch_lowerWidth;               //划伤宽度下限
int scratch_higherWidth;              //划伤宽度上限
int scratch_lowerLength;              //划伤长度下限
int scratch_higherLength;             //划伤长度上限
double filmscratch;                   //贴膜划痕系数
double scratchbubbleth;               //贴膜排除气泡系数
double scratchbubbleth1;              //贴膜排除气泡系数2
double scratchbrighth;                //划伤亮度阈值
double minscratchbrighth;             //最小划伤亮度阈值

int leakage_bolckSize;      //黑底漏光适应分割领域块大小
double leakage_delta;       //黑底漏光适应分割阈值大小
int leakage_area_lower;
int leakage_area_upper;
double leakageth;
double leakage_w_h;

double size_long;
double size_width;
double roi_proportion;
int pixel_num;

int G_R_diff;
int gray_thre;
int num7;
double main_whitemean; //主相机白底灰度均值
double left_whitemean; //左相机白底灰度均值
double right_whitemean;//右相机白点灰度均值

double test_num;        //测试总数目
double qualified_num;   //合格总数目
int yiwu_num;        //异物缺陷数目
int wuxian_num=0;        //异物缺陷数目
int liangdian_num;   //亮点
int white_num;       //白点数目
int mura_num;        //白印数目
int lackline_num;    //少线数目
int leakage_num;     //漏光数目
int redBlue_num;     //红蓝斑数目
int dispalyError_num;//显示异常数目
int scratch_num;     //划伤数目
int dengyan_num=0;     //划伤数目
int liangbian_num=0;     //划伤数目
int louguang_num=0;     //划伤数目
int biankuang_num=0;     //划伤数目
int anjiao_num=0;     //划伤数目
int shaoliao_num=0;     //划伤数目
int baodeng_num=0;     //划伤数目
int shizi_num=0;     //划伤数目
int secha_num=0;     //划伤数目
int dazhe_num=0;     //划伤数目
double pass_per;     //合格率

//************相机亮度参数*****************//
double light_num = 0.0;//可以点亮的屏幕的数量
double main_light;
double right_light;
double left_light;
double Average_mainlight = 0.0;  //主相机平均亮度
double Average_rightlight = 0.0;//右相机平均亮度
double Average_leftlight = 0.0;//左相机平均亮度
double Max_mainlight = 0.0;
double Min_mainlight = 0.0;
double Max_rightlight = 0.0;
double Min_rightlight = 0.0;
double Max_leftlight = 0.0;
double Min_leftlight = 0.0;
//************相机亮度参数*****************//

bool Form_Camera_Show;//相机窗口是否已经存在的标志位
bool production_lot_information_win_show;//批次窗口是否已经存在的标志位
bool promodel_show;//型号窗口是否存在的标志位
bool parameter_control_show;//参数窗口是否已经存在的标志位
bool save_images_show;//保存窗口是否已经存在的标志位

QString imageSavePath;//图片保存路径
bool autoSave;//是否自动保存标志位

QElapsedTimer mstimer;
QString sq;
Mat M_white_1;
Mat M_white_abshow;
Mat M_biankuang;
Mat M_black_1;
Mat M_louguang_1;
Mat M_louguang_abshow;
Mat M_louguang_2;
Mat M_white_2;
Mat M_white_3; //右侧相机透视变换矩阵
Mat M_black_2;
Mat M_black_abshow;
Mat M_abshow_white;
Mat Mresult_1_white;
Mat Mresult_2_white;
Mat Mresult_3_white;
Mat Mresult_1_black;
Mat Mresult_2_black;
Mat Mresult_gray;
Mat M_L_1;
Mat M_L_1_E;
Mat M_L_2;
Mat M_R_1;
Mat M_R_1_E;
Mat M_R_2;
Mat MURA1;
Mat MURA2;
Mat M_Back;
Mat M_Front;

Mat main_mask;
Mat left_mask;
Mat right_mask;
Mat front_mask;
Mat back_mask;
Mat color_mask;
Mat louguang_mask;


cv::Mat src_L1;
cv::Mat src_R1;
cv::Mat src_L2;
cv::Mat src_R2;
cv::Mat src_white1 ;
cv::Mat src_white2 ;
cv::Mat src_ceguang1;
cv::Mat src_ceguang2;
cv::Mat src_ceguang_back;
cv::Mat src_ceguang_left;
cv::Mat src_ceguang_right;
cv::Mat src_ceguang_front;
cv::Mat src_front; //黑白相机前面白底图像
cv::Mat src_back;  //黑白相机会面白底图像
cv::Mat src_black1;//黑白相机黑底下图像
cv::Mat src_color; //彩色相机白底下图像
cv::Mat src_black2;//彩色相机黑底下图像
cv::Mat src_black_high;//高曝光图像
cv::Mat whiteshow1;
cv::Mat whiteshow2;
cv::Mat src_gray1;
cv::Mat srcgrayColor1;//灰底彩色相机第一帧
cv::Mat srcgrayColor2;//灰底彩色相机第二帧
cv::Mat src_gray_left;
cv::Mat src_gray_right;
cv::Mat src_gray_stripe1;
cv::Mat src_red1;
cv::Mat src_green1;
cv::Mat src_blue1;

QString causeColor;
QString causeColor_1_white;
QString causeColor_2_white;
QString causeColor_3_white;
QString causeColor_1_black;
QString causeColor_2_black;
QString causeColorshow1;
QString causeColorshow2;
QString causeColor_gray;
int screen_type;
Mat Mresult;

bool result_white1;
bool result_white2;
bool result_white3;
bool result_white_1;
bool result_white_2;
bool result_black_1;
bool result_black_2;
bool result_gray;
QString paraConfigOk;

QFutureWatcher<void> Watcher_White1; //白底线程监视
QFutureWatcher<void> Watcher_White2; //白底线程监视
QFutureWatcher<void> Watcher_White3; //白底线程监视
QFutureWatcher<void> Watcher_d1; //黑底线程监视
QFutureWatcher<void> Watcher_Gray;  //灰底线程监视

int Flag_Three_Thread_Finsh;

cv::Mat src_L1_Temp;
cv::Mat src_R1_Temp;
cv::Mat src_white1_Temp ;
cv::Mat src_ceguang1_Temp;
cv::Mat src_ceguang_back_Temp;
cv::Mat src_ceguang_left_Temp;
cv::Mat src_ceguang_right_Temp;
cv::Mat src_ceguang_front_Temp;
cv::Mat src_front_Temp; //黑白相机前面白底图像
cv::Mat src_back_Temp;  //黑白相机会面白底图像
cv::Mat src_black1_Temp;//黑白相机黑底下图像
cv::Mat src_color_Temp; //彩色相机白底下图像
cv::Mat src_black2_Temp;//彩色相机黑底下图像
cv::Mat src_gray1_Temp;
cv::Mat srcgrayColor1_Temp;//灰底彩色相机第一帧
cv::Mat srcgrayColor2_Temp;//灰底彩色相机第二帧
cv::Mat src_gray_left_Temp;
cv::Mat src_gray_right_Temp;
String markSavePath;

//强制保存所需变量
QString Force_savePath;
bool isExist;

//图片缓存所需变量
QString Cache_savePath;
bool Cache_isExist;

QFuture<void> d1;

bool Flag_White1_Finish;
bool Flag_White2_Finish;
bool Flag_White3_Finish;

static bool store_flag=false;//第一节拍和第二节拍标志位

bool Ext_Result_BlackWhite;
bool Ext_Result_Left;
bool Ext_Result_Right;//屏幕提取异常标志

enum ModbusConnection
{
    Serial,
    Tcp
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);//最大化窗体
    bar = ui->statusBar;
    bar->setStyleSheet("color:white;");
    appwnd1 = (HWND)ui->label->winId();
    appwnd2 = (HWND)ui->label_2->winId();
    appwnd3 = (HWND)ui->label_3->winId();
    appwnd4 = (HWND)ui->label_4->winId();
    appwnd5 = (HWND)ui->label_5->winId();
    appwnd6 = (HWND)ui->label_6->winId();


    ui->tabWidget->setStyleSheet("QTabWidget:pane{ \
    border: 1px solid white; top: -1px;background-color:grey;}\
    QTabBar::tab:selected{border:1px solid white;border-bottom-color: none;background-color:rgb(0, 0, 0);font:20px;color:rgb(255, 255, 0);}}\
    QTabBar::tab:!selected{border-bottom: 1px solid white;background-color:rgb(0, 0, 0);font:20px;color:rgb(0, 0, 0);}\
    ");

    ui->widget_4->hide();

    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd");
    ui->label_32->setText(str);//日期时间

    QString str_ver ="1.0.34.101";       //版本号
    this->setWindowTitle("背光源缺陷检测系统"+str_ver);

    connect(this, SIGNAL(read_Modbus_Num(int)), this, SLOT(read_Modbus(int)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(write_Modbus_Num(int,int)), this, SLOT(write_Modbus(int,int)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(Write_Photo_Signal()), this, SLOT(Write_Photo_Finish()), Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(Restart_Timer_Signal()) ,this,SLOT(Restart_Timer()), Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(Dectect_Result()) ,this,SLOT(Dect_Result()), Qt::BlockingQueuedConnection);

    //    connect(this, SIGNAL(read_Modbus_Num(int)), this, SLOT(read_Modbus(int)), Qt::DirectConnection);
    //    connect(this, SIGNAL(write_Modbus_Num(int,int)), this, SLOT(write_Modbus(int,int)), Qt::DirectConnection);

    connect(&Watcher_White1, SIGNAL(finished()), this, SLOT(White1_Thread_Finish()));
    connect(&Watcher_White2, SIGNAL(finished()), this, SLOT(White2_Thread_Finish()));
    connect(&Watcher_White3, SIGNAL(finished()), this, SLOT(White3_Thread_Finish()));

    connect(&Watcher_d1, SIGNAL(finished()), this, SLOT(Restart_Timer()));


    //数据库读取状态显示在下拉框
    QSqlQuery *sqlQuery = new QSqlQuery;
    sqlQuery->exec("select run_state from Running_State");      //屏幕类型表
    while(sqlQuery->next())
    {
        QString text = sqlQuery->value(0).toString();
        ui->comboBox->setCurrentText(text);
    }

    if(ui->comboBox->currentText()=="在线")
    {
        Flag_Running_State="Online";
    }
    else if(ui->comboBox->currentText()=="离线")
    {
        Flag_Running_State="Offline";
    }
    ini_state();  //获取缺陷的检测状态


    //创建强制保存文件夹
    QDateTime Force_current_date_time =QDateTime::currentDateTime();
    QString Force_current_date =Force_current_date_time.toString("yyyyMMddhhmm");

    Force_savePath="D:\\samp";
    //创建样本图片文件夹
    QDir *sampleFolder = new QDir;
    isExist = sampleFolder->exists(Force_savePath);
    if(!isExist)
    {
        sampleFolder->mkdir(Force_savePath);
    }
    //创建工位拍摄图片子文件夹
    QDir *takenPictures=new QDir;
    Force_savePath = Force_savePath + "\\" + Force_current_date;
    isExist = takenPictures->exists(Force_savePath);
    if(!isExist)
    {
        takenPictures->mkdir(Force_savePath);
    }



    //创建缓存文件夹
    Cache_savePath="D:\\Cache_Picture";
    //创建样本图片文件夹
    QDir *Cache_sampleFolder = new QDir;
    Cache_isExist = Cache_sampleFolder->exists(Cache_savePath);
    if(!Cache_isExist)
    {
        Cache_sampleFolder->mkdir(Cache_savePath);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*====================================================================
* 类型：receiveData
* 功能：接收用户输入信息槽函数
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::receiveData(QString e,QString a)
{
    ui->label_105->setText(e); //产品型号
    ui->label_31->setText(a); //产品批次


    ui->label_52->setText(QString("%1%").arg(pass_per));

    ui->label_48->setText(QString("%1").arg(test_num));       //已经检测总数目
    ui->label_50->setText(QString("%1").arg(qualified_num));  //合格品总数目

    ui->label_34->setText(QString("%1").arg(yiwu_num));
    ui->label_36->setText(QString("%1").arg(white_num));
    ui->label_38->setText(QString("%1").arg(mura_num));
    ui->label_42->setText(QString("%1").arg(lackline_num));
    ui->label_69->setText(QString("%1").arg(scratch_num));
    ui->label_40->setText(QString("%1").arg(dengyan_num));
    ui->label_44->setText(QString("%1").arg(liangbian_num));
//    ui->label_46->setText(QString("%1").arg(biankuang_num));
//    ui->label_54->setText(QString("%1").arg(anjiao_num));
//    ui->label_63->setText(QString("%1").arg(shaoliao_num));
    ui->label_65->setText(QString("%1").arg(baodeng_num));

}

void MainWindow::on_pushButton_2_clicked()//工位1主相机全屏显示
{

}

/*=========================================================
* 函 数 名: 屏幕划伤
* 功能描述: 延时函数
* 说明：
=========================================================*/
void delay(int MSecs)
{
    QTime dieTime= QTime::currentTime().addMSecs(MSecs);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

/*=========================================================
* 函 数 名: 屏幕划伤
* 功能描述: 延时函数
* 说明：
=========================================================*/
void delay_msec(int msec)
{
    //定义一个新的事件循环
    QEventLoop loop;
    //创建单次定时器,槽函数为事件循环的退出函数
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    //事件循环开始执行,程序会卡在这里,直到定时时间到,本循环被退出
    loop.exec();
}
/*====================================================================
* 类型：菜单栏PLC连接
* 功能：
======================================================================*/
void MainWindow::on_actionPLC_2_triggered()//PLC连接
{
    //关闭6个相机
//    camera->Find_Device();
//    if(camera->m_hDevHandle_1)
//        camera->closeCamera(camera->m_hDevHandle_1);
//    if(camera->m_hDevHandle_2)
//        camera->closeCamera(camera->m_hDevHandle_2);
//    if(camera->m_hDevHandle_3)
//        camera->closeCamera(camera->m_hDevHandle_3);
//    if(camera->m_hDevHandle_4)
//        camera->closeCamera(camera->m_hDevHandle_4);
//    if(camera->m_hDevHandle_5)
//        camera->closeCamera(camera->m_hDevHandle_5);
//    if(camera->m_hDevHandle_6)
//        camera->closeCamera(camera->m_hDevHandle_6);
    if (modbusDevice)//删除modbus设备对象
    {
        modbusDevice->disconnectDevice();//断开连接
        delete modbusDevice;
        modbusDevice = nullptr;
    }
    modbusDevice = new QModbusTcpClient();//创建服务端对象
    if (!modbusDevice)//没有PLC设备的话进行返回
        return;
    if (modbusDevice->state() != QModbusDevice::ConnectedState)//如果处在非连接状态，进行连接
    {
        const QUrl url = QUrl::fromUserInput("192.168.1.5:8580");
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());

        modbusDevice->setTimeout(1000);//连接超时1000毫秒
        modbusDevice->setNumberOfRetries(3);//连接失败重试3次连接
    }
    if (modbusDevice->connectDevice())//如果处在连接状态的话
    {
        int Num_Conne=0;
        while(Num_Conne<6)
        {
            if((modbusDevice->state() == QModbusDevice::ConnectedState))//如果处在连接状态，连接成功
            {
                bar->showMessage("PLC连接成功");
                break;
            }
            if((modbusDevice->state() == QModbusDevice::ConnectingState))
            {
                bar->showMessage("PLC连接中");
                Num_Conne++;
                delay(100);
            }
            if(Num_Conne==5)//连接次数超过5次算作连接失败
            {
                bar->showMessage("PLC连接失败");
                break;
            }
        }
    }
    else //如果处在非连接状态的话
    {
        bar->showMessage("PLC连接失败");
    }
}

/*====================================================================
* 类型：工具栏PLC连接
* 功能：
======================================================================*/
void MainWindow::on_actionPLC_3_triggered()
{
    //点击PLC连接在log日志中标记
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("点击连接相机"+current_date);
    //关闭6个相机
    camera->Find_Device();
    if(camera->m_hDevHandle_1)
        camera->closeCamera(camera->m_hDevHandle_1);
    if(camera->m_hDevHandle_2)
        camera->closeCamera(camera->m_hDevHandle_2);
    if(camera->m_hDevHandle_3)
        camera->closeCamera(camera->m_hDevHandle_3);
    if(camera->m_hDevHandle_4)
        camera->closeCamera(camera->m_hDevHandle_4);
    if(camera->m_hDevHandle_5)
        camera->closeCamera(camera->m_hDevHandle_5);
    if(camera->m_hDevHandle_6)
        camera->closeCamera(camera->m_hDevHandle_6);
    if (modbusDevice)//删除modbus设备对象
    {
        modbusDevice->disconnectDevice();//断开连接
        delete modbusDevice;
        modbusDevice = nullptr;
    }
    modbusDevice = new QModbusTcpClient();//创建服务端对象
    if (!modbusDevice)//没有PLC设备的话进行返回
        return;
    if (modbusDevice->state() != QModbusDevice::ConnectedState)//如果处在非连接状态，进行连接
    {
        const QUrl url = QUrl::fromUserInput("192.168.1.5:8580");
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());

        modbusDevice->setTimeout(1000);//连接超时1000毫秒
        modbusDevice->setNumberOfRetries(3);//连接失败重试3次连接
    }
    if (modbusDevice->connectDevice())//如果处在连接状态的话
    {
        int Num_Conne=0;
        while(Num_Conne<6)
        {
            if((modbusDevice->state() == QModbusDevice::ConnectedState))//如果处在连接状态，连接成功
            {
                bar->showMessage("PLC连接成功");
                break;
            }
            if((modbusDevice->state() == QModbusDevice::ConnectingState))
            {   bar->showMessage("PLC连接中");
                Num_Conne++;
                delay(100);
            }
            if(Num_Conne==5)//连接次数超过5次算作连接失败
            {
                bar->showMessage("PLC连接失败");
                break;
            }
        }
    }
    else //如果处在非连接状态的话
    {
        bar->showMessage("PLC连接失败");
    }
}

/*====================================================================
* 类型：写入PLC数值结果，1写入 0清空
* 功能：
* 输入：
* 输出：
* 其他：
======================================================================*/
bool MainWindow::write_Modbus(int address,int value)//写入PLC
{
    //给QModbusDataUnit赋好值后发送写请求sendWriteRequest，写到相应寄存器中的相应位置
    //如果没有发生错误的话sendWriteRequest将返回一个QModbusReply，否则返回空指针。
    //当reply完成或放弃后reply->isFinished()将返回true
    //发送的数据信息（数据类型 ，起始地址，个数）
    QModbusDataUnit writeUnit(QModbusDataUnit::Coils, address, 1); //往address里面写入一个数字。注意只写一个数字
    writeUnit.setValue(0, value);                         //将value写入地址中，其中value是一个十六进制的数值
    //这里先建好QModbusDataUnit
    for(int i=0;i<3;i++)
    {
        if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, 1))           //1是服务器的地址
            //发送写请求
        {
            for(int Write_Num=0;Write_Num<1000;Write_Num++)
            {
                if (!reply->isFinished())
                {
                    delay(1);
                }
                else if(reply->isFinished())
                {
                    if (reply->error() == QModbusDevice::NoError)
                    {
                        ui->label_48->setText("写入成功");
                        delete reply;
                        return true;
                    }
                    else
                    {
                        ui->label_48->setText("写入失败");
                        delete reply;
                        return false;
                    }
                }
            }
            delete reply;
        }
        else
        {
            ui->label_48->setText("写入失败");
            return false;
        }
    }
    ui->label_48->setText("三次写入都失败");

    return false;
}

/*====================================================================
* 类型：读取plc地址结果
* 功能：
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::read_Modbus(int address)//读取plc
{ 
    if (!modbusDevice)
        return;
    //QModbusDataUnit 存储接收和发送数据的类，数据类型为1bit和16bit
    QModbusDataUnit writeUnit1(QModbusDataUnit::Coils, address, 1);
    if (auto *reply = modbusDevice->sendReadRequest(writeUnit1, 1))
    {
        if (!reply->isFinished()) //eply在回复完成或中止时返回true
        {
            //QModbusReply客户端访问服务器后得到的回复（如客户端读服务器数据时包含数据信息）
            //QModbusReply这个类存储了来自client的数据
            connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
        }
        else
        {
            delete reply;
        }
    }
}

/*====================================================================
* 类型：PLC读取地址结果槽函数的处理请求
* 功能：
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::readReady()
{
    //QModbusReply这个类存储了来自client的数据,sender()返回发送信号的对象的指针
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;
    if (reply->error() == QModbusDevice::NoError)
    {
        //数据从QModbusReply这个类的resuil方法中获取,也就是本程序中的reply->result()
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++)
        {
            Data_Form_Plc=unit.value(i);
            statusBar()->showMessage(tr("读取成功：%1").arg(Data_Form_Plc),300);

        }
    }
    else if (reply->error() == QModbusDevice::ProtocolError)
    {
        statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->rawResult().exceptionCode(), -1, 16), 100);
    }
    else if(reply->error() == QModbusDevice::ReadError)
    {
        statusBar()->showMessage(tr("ReadError: %1 (code: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->error(), -1, 16), 100);
    }
    else if(reply->error() == QModbusDevice::WriteError)
    {
        statusBar()->showMessage(tr("WriteError: %1 (code: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->error(), -1, 16), 100);
    }
    else if(reply->error() == QModbusDevice::ConnectionError)
    {
        statusBar()->showMessage(tr("ConnectionError: %1 (code: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->error(), -1, 16), 100);
    }
    else if(reply->error() == QModbusDevice::ConfigurationError)
    {
        statusBar()->showMessage(tr("ConfigurationError: %1 (code: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->error(), -1, 16), 100);
    }
    else if(reply->error() == QModbusDevice::TimeoutError)
    {
        statusBar()->showMessage(tr("TimeoutError: %1 (code: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->error(), -1, 16), 100);
    }
    else if(reply->error() == QModbusDevice::ReplyAbortedError)
    {
        statusBar()->showMessage(tr("ReplyAbortedError: %1 (code: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->error(), -1, 16), 100);
    }
    else if(reply->error() == QModbusDevice::UnknownError)
    {
        statusBar()->showMessage(tr("UnknownError: %1 (code: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->error(), -1, 16), 100);
    }
    reply->deleteLater();
}


/*====================================================================
* 功能：定时器触发槽函数
* 其他：
======================================================================*/
void MainWindow::InitTimer()
{
//    int w=ui->label_2->width();
//    int l=ui->label_2->height();

    m_timer.setSingleShot(false);
    m_timer.start(100);                       //启动或重启定时器, 并设置定时器时间：毫秒
    //connect(&m_timer, SIGNAL(timeout()), this, SLOT(TimerTimeOut()));//定时器触发信号槽
    if(Flag_Running_State!="Offline")
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(TimerTimeOut()));//定时器触发信号槽
    else if(Flag_Running_State=="Offline")
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(TimerTimeOut_Offine()));//定时器触发信号槽
}

/*====================================================================*
 *  功能：重启定时器*
 * 其他：
 * ======================================================================*/
void MainWindow::Restart_Timer()
{
    m_timer.start(50);
}
/*====================================================================
* 功能：定时器m_timer触发槽函数
* 其他：
======================================================================*/
void MainWindow::TimerTimeOut()
{
    if(F)//判断定时器是否运行,执行定时器触发时需要处理的业务
    {
        read_Modbus(1600);//位置到达，开测光《拍测光
        delay(modbus_time);

        if(Data_Form_Plc==1)
        {
            Data_Form_Plc=0;
            if(m_timer.isActive())
            {
                m_timer.stop();//停止定时器
            }
            detect();//冠华半自动机器3月30日开始修改 该版本可以使用
        }
    }
    m_timer.start(50);
}

/*====================================================================
* 功能：定时器m_timer触发槽函数
* 其他：
======================================================================*/
void MainWindow::TimerTimeOut_Offine()
{
    if(F)//判断定时器是否运行,执行定时器触发时需要处理的业务
    {
        Data_Form_Plc=1;//位置到达，开测光《拍测光
        delay(50);

        if(Data_Form_Plc==1)
        {
            Data_Form_Plc=0;
            if(m_timer.isActive())
            {
                m_timer.stop();//停止定时器
            }
            detect_offine();//冠华半自动机器3月30日开始修改 该版本可以使用
        }
    }
}

void MainWindow::TimerTimeOut_temp()
{
    if(Data_Form_Plc==1)
    {  Data_Form_Plc=0;
        if(m_timer_temp.isActive())
        {
            m_timer_temp.stop();//停止定时器
            QFuture<void> f5 =QtConcurrent::run(this,&MainWindow::display);
        }
    }
}

/*====================================================================
* 功能：调试过程输出时间
* 其他：
======================================================================*/
void MainWindow::main_msg(QVariant msg)
{
    QFile file("D:\\main_record.txt");

    file.open(QFile::WriteOnly | QFile::Append | QFile::Text);

    file.write(msg.toString().toStdString().c_str());
    file.write(QString("\n").toStdString().c_str());
    file.close();
}

/*====================================================================
* 功能：调试过程输出时间
* 其他：
======================================================================*/
void MainWindow::left_msg(QVariant msg)
{
    QFile file("D:\\left_record.txt");

    file.open(QFile::WriteOnly | QFile::Append | QFile::Text);

    file.write(msg.toString().toStdString().c_str());
    file.write(QString("\n").toStdString().c_str());
    file.close();
}

/*====================================================================
* 功能：调试过程输出时间
* 其他：
======================================================================*/
void MainWindow::right_msg(QVariant msg)
{
    QFile file("D:\\right_record.txt");

    file.open(QFile::WriteOnly | QFile::Append | QFile::Text);

    file.write(msg.toString().toStdString().c_str());
    file.write(QString("\n").toStdString().c_str());
    file.close();
}
/*====================================================================
* 功能：调试过程输出时间
* 其他：
======================================================================*/
void MainWindow::debug_msg(QVariant msg)
{
    QFile file("D:\\thread_time_after.txt");

    file.open(QFile::WriteOnly | QFile::Append | QFile::Text);

    file.write(msg.toString().toStdString().c_str());
    file.write(QString("\n").toStdString().c_str());
    file.close();
}

/*====================================================================
* 功能：拍照（离线）
* 其他：
======================================================================*/
int MainWindow::detect_offine()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("拍照线程开始"+current_date);
    //offline_wsc
    std::string SRC_PATH = "D:\\graduateStudent\\Project\\BacklightTest20210118\\O_YW_0101_1\\01_20200927165_44_";
    src_ceguang1_Temp = cv::imread(SRC_PATH + "210.bmp", -1);
    src_ceguang_left_Temp=cv::imread(SRC_PATH + "110.bmp", -1);
    src_ceguang_right_Temp=cv::imread(SRC_PATH + "010.bmp", -1);
//    src_ceguang1_Temp = cv::imread("D:\\graduateStudent\\Project\\BacklightTest20210118\\O_YW_0101_1\\01_20200927165_44_210.bmp", -1);
//    src_ceguang_left_Temp=cv::imread("D:\\graduateStudent\\Project\\BacklightTest20210118\\O_YW_0101_1\\01_20200927165_44_110.bmp", -1);
//    src_ceguang_right_Temp=cv::imread("D:\\graduateStudent\\Project\\BacklightTest20210118\\O_YW_0101_1\\01_20200927165_44_010.bmp", -1);
    //离线陈
//    src_ceguang1_Temp = cv::imread("F:\\35\\00_20200927165_35_210.bmp", -1);
//    src_ceguang_left_Temp=cv::imread("F:\\35\\00_20200927165_35_110.bmp", -1);
//    src_ceguang_right_Temp=cv::imread("F:\\35\\00_20200927165_35_010.bmp", -1);
    //离线秦
//    src_ceguang1_Temp = cv::imread("D:\\quexian\\quexian13\\tiaoshi1\\src_ceguang10418.bmp", -1);
//    src_ceguang_left_Temp=cv::imread("D:\\quexian\\quexian13\\tiaoshi1\\src_ceguang_left0418.bmp", -1);
//    src_ceguang_right_Temp=cv::imread("D:\\quexian\\quexian13\\tiaoshi1\\src_ceguang_right0418.bmp", -1);

    QString s = " ";
    if(!store_flag)
    {
        s = "_1.bmp";
        store_flag  = true;
    }
    else
    {
        s = "_2.bmp";
        store_flag = false;
    }

    QString Cache_savePath_ceguang_left = Cache_savePath + "\\src_ceguang_left"+s;
    string Cache_Save_ceguang_left=Cache_savePath_ceguang_left.toStdString();
    imwrite(Cache_Save_ceguang_left,src_ceguang_left_Temp);

    QString Cache_savePath_ceguang1 = Cache_savePath + "\\src_ceguang1"+s;
    string Cache_Save_ceguang1=Cache_savePath_ceguang1.toStdString();
    imwrite(Cache_Save_ceguang1,src_ceguang1_Temp);

    QString Cache_savePath_ceguang_right = Cache_savePath + "\\src_ceguang_right"+s;
    string Cache_Save_ceguang_right=Cache_savePath_ceguang_right.toStdString();
    imwrite(Cache_Save_ceguang_right,src_ceguang_right_Temp);

    ui->label_30->setText("开始检测");

    ui->label_30->setText("读到平台到达");
    delay_msec(SideLightOpenTime);                 //等待侧光灯打开延时

    ui->label_30->setText("侧光拍摄完成");

    ui->label_30->setText("侧光完成信号");


    delay_msec(BoxOpenTime);//打开测试盒点亮为白底屏幕拍照延时 

    //offline_wsc
    src_white1_Temp = cv::imread(SRC_PATH + "212.bmp", -1);
    src_L1_Temp=cv::imread(SRC_PATH + "112.bmp", -1);
    src_R1_Temp=cv::imread(SRC_PATH + "012.bmp", -1);
//    src_white1_Temp = cv::imread("D:\\graduateStudent\\Project\\BacklightTest20210118\\O_YW_0101_1\\01_20200927165_44_212.bmp", -1);
//    src_L1_Temp=cv::imread("D:\\graduateStudent\\Project\\BacklightTest20210118\\O_YW_0101_1\\01_20200927165_44_112.bmp", -1);
//    src_R1_Temp=cv::imread("D:\\graduateStudent\\Project\\BacklightTest20210118\\O_YW_0101_1\\01_20200927165_44_012.bmp", -1);

    //离线陈
//    src_white1_Temp = cv::imread("F:\\35\\00_20200927165_35_212.bmp", -1);
//    src_L1_Temp= cv::imread("F:\\35\\00_20200927165_35_112.bmp", -1);
//    src_R1_Temp= cv::imread("F:\\35\\00_20200927165_35_012.bmp", -1);
    //离线秦
//    src_white1_Temp = cv::imread("D:\\quexian\\quexian13\\tiaoshi1\\src_white1_2.bmp", -1);
//    src_L1_Temp= cv::imread("D:\\quexian\\quexian13\\tiaoshi1\\src_R1_2.bmp", -1);
//    src_R1_Temp= cv::imread("D:\\quexian\\quexian13\\tiaoshi1\\src_L1_2.bmp", -1);
    QString Cache_savePath_white1 = Cache_savePath + "\\src_white1"+s;
    string Cache_Save_white1=Cache_savePath_white1.toStdString();
    //保存图片到指定路径
    imwrite(Cache_Save_white1,src_white1_Temp);

    QString Cache_savePath_L1 = Cache_savePath + "\\src_L1"+s;
    string Cache_Save_L1=Cache_savePath_L1.toStdString();
    imwrite(Cache_Save_L1,src_L1_Temp);

    QString Cache_savePath_R1 = Cache_savePath + "\\src_R1"+s;
    string Cache_Save_R1=Cache_savePath_R1.toStdString();
    imwrite(Cache_Save_R1,src_R1_Temp);

    if(src_ceguang1_Temp.channels() == 3)
        cvtColor(src_ceguang1_Temp,src_ceguang1_Temp,CV_BGR2GRAY);
    if(src_ceguang_left_Temp.channels() == 3)
        cvtColor(src_ceguang_left_Temp,src_ceguang_left_Temp,CV_BGR2GRAY);
    if(src_ceguang_right_Temp.channels() == 3)
        cvtColor(src_ceguang_right_Temp,src_ceguang_right_Temp,CV_BGR2GRAY);
    if(src_white1_Temp.channels() == 3)
        cvtColor(src_white1_Temp,src_white1_Temp,CV_BGR2GRAY);
    if(src_L1_Temp.channels() == 3)
        cvtColor(src_L1_Temp,src_L1_Temp,CV_BGR2GRAY);
    if(src_R1_Temp.channels() == 3)
        cvtColor(src_R1_Temp,src_R1_Temp,CV_BGR2GRAY);

    //拍摄图片入队
    struct Dect_Photo Dect_Photo1;
    Dect_Photo1.SideLight_Main=src_ceguang1_Temp;
    Dect_Photo1.SideLight_Left=src_ceguang_left_Temp;
    Dect_Photo1.SideLight_Right=src_ceguang_right_Temp;

    Dect_Photo1.White_Main=src_white1_Temp;
    Dect_Photo1.White_Left=src_L1_Temp;
    Dect_Photo1.White_Right=src_R1_Temp;

    Pic.enqueue(Dect_Photo1);

    ui->label_30->setText("拍照完成");

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("isfinished前"+current_date);

    while(!(d1.isFinished()))
        delay(50);

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("myfunc1前"+current_date);

    d1 =QtConcurrent::run(this,&MainWindow::myFunc1);     //开白底检测线程

    //if(!(d1.isCanceled()))
//    current_date_time =QDateTime::currentDateTime();
//    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
//    debug_msg("waitforfinished前"+current_date);

//    d1.waitForFinished();

//    current_date_time =QDateTime::currentDateTime();
//    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
//    debug_msg("myfunc1前"+current_date);

//    d1 =QtConcurrent::run(this,&MainWindow::myFunc1);     //开白底检测线程

    if(autoSave==true && SaveSize == true)
    {
        debug_msg("保存检测原图像1_DefectNum1 == 0");
        QString savePath=imageSavePath;
        //创建样本图片文件夹
        QDir *sampleFolder = new QDir;
        bool isExist = sampleFolder->exists(savePath);
        if(!isExist)
        {
            sampleFolder->mkdir(savePath);
        }
        //创建工位拍摄图片子文件夹
        QDir *takenPictures=new QDir;
        savePath+="\\station";
        isExist = takenPictures->exists(savePath);
        if(!isExist)
        {
            takenPictures->mkdir(savePath);
        }
        //创建以某天命名的子文件夹
        QDir *dayFolder=new QDir;
        savePath = savePath + "\\" + product_Lot;
        isExist=dayFolder->exists(savePath);
        if(!isExist)
        {
            dayFolder->mkdir(savePath);
        }
        //创建以序号命名的文件夹
        QDir *numFolder=new QDir;
        QString serialNum="\\"+QString::number(num7+1);
        savePath+=serialNum;
        isExist=numFolder->exists(savePath);
        if(!isExist)
        {
            numFolder->mkdir(savePath);
        }
        markSavePath.clear();
        markSavePath = savePath.toStdString();
        QString save_ceguang1_1;
        string save_ceguang1;
        save_ceguang1_1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_210.bmp";//主相机侧光灯图像
        save_ceguang1_1=savePath+save_ceguang1_1;
        save_ceguang1=save_ceguang1_1.toStdString();

        QString save_ceguang_left1;
        string save_ceguang_left;
        save_ceguang_left1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_010.bmp";//左相机侧光灯图像
        save_ceguang_left1 = savePath + save_ceguang_left1;
        save_ceguang_left=save_ceguang_left1.toStdString();

        QString save_ceguang_right1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_110.bmp";
        save_ceguang_right1 = savePath + save_ceguang_right1;//右相机侧光灯图像
        string save_ceguang_right=save_ceguang_right1.toStdString();


        QString save_white1_1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_212.bmp";
        save_white1_1 = savePath + save_white1_1 ;//主黑白相机白底
        string save_white1=save_white1_1.toStdString();

        QString save_L1_1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_012.bmp";
        save_L1_1 = savePath + save_L1_1 ;//左侧相机拍摄白底
        string save_L1=save_L1_1.toStdString();

        QString save_R1_1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_112.bmp";
        save_R1_1 = savePath + save_R1_1;//右侧相机拍摄白底
        string save_R1=save_R1_1.toStdString();

        imwrite(save_ceguang1,src_ceguang1_Temp);//主相机侧光灯图像
        imwrite(save_ceguang_left,src_ceguang_left_Temp);//左相机侧光灯图像
        imwrite(save_ceguang_right,src_ceguang_right_Temp);//右相机侧光灯图像

        imwrite(save_white1,src_white1_Temp);//白底
        imwrite(save_L1,src_L1_Temp);//左侧相机拍摄白底
        imwrite(save_R1,src_R1_Temp);//右侧相机拍摄白底

    }

//        current_date_time =QDateTime::currentDateTime();
//        current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
//        debug_msg("waitforfinished前"+current_date);

//        d1.waitForFinished();

//        current_date_time =QDateTime::currentDateTime();
//        current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
//        debug_msg("isfinished前"+current_date);

//        while(!(d1.isFinished()))
//            delay(50);

//        current_date_time =QDateTime::currentDateTime();
//        current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
//        debug_msg("myfunc1前"+current_date);

//        d1 =QtConcurrent::run(this,&MainWindow::myFunc1);     //开白底检测线程
        //Watcher_d1.setFuture(d1);

    //d1.waitForFinished();
//    d1 =QtConcurrent::run(this,&MainWindow::myFunc1);     //开白底检测线程
//

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("拍照线程结束"+current_date);

    m_timer.start(50);

//    while(F_start==true)
//    {
//        if(Flag_First_Photo==true)
//        {
//            Flag_First_Photo=false;
//            Write_Photo_Finish();
//            ui->label_30->setText("本节拍拍照完成");
//            m_timer.start(50);
//            break;
//        }
//        if(Flag_Dec_Complete==true)
//        {
//            Flag_Dec_Complete=false;
//            Write_Photo_Finish();
//            ui->label_30->setText("本节拍拍照完成");
//            m_timer.start(50);
//            break;
//        }
//        delay(50);
//    }

    return 0;
}

/*====================================================================
* 功能：拍照（在线）
* 其他：
======================================================================*/
int MainWindow::detect()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("拍照线程开始"+current_date);

    MV_CC_SetFloatValue(camera->m_hDevHandle_1,"ExposureTime", Exposure_Ceguang_Left); //左黑白主相机拍摄
    MV_CC_SetFloatValue(camera->m_hDevHandle_2,"ExposureTime", Exposure_Ceguang_Main); //主黑白主相机拍摄
    MV_CC_SetFloatValue(camera->m_hDevHandle_3,"ExposureTime", Exposure_Ceguang_Right);//右黑白主相机拍摄

    ui->label_30->setText("开始检测");

    ui->label_30->setText("读到平台到达");
    //delay(SideLightOpenTime);                 //等待侧光灯打开延时
    delay_msec(SideLightOpenTime);                 //等待侧光灯打开延时
    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("开始获取测光图"+current_date);

//    ui->label_74->setText(causeColor_1_white);//上边显示清除上次白底的缺陷类型
//    ui->label_10->setText("");                //上边显示清除上次的检测结果
    //============================拍摄侧光图片并进行显示================================
    QFuture<cv::Mat> s1 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, int(1));//开线程左相机拍照侧光
    QFuture<cv::Mat> s2 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, int(2));//开线程主相机拍照侧光
    QFuture<cv::Mat> s3 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, int(3));//开线程右相机拍照侧光
    s1.waitForFinished();
    s2.waitForFinished();
    s3.waitForFinished();

//    while(!(s1.isFinished()))
//        delay(10);
//    while(!(s2.isFinished()))
//        delay(10);
//    while(!(s3.isFinished()))
//        delay(10);

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("测光图拍照完毕"+current_date);

    src_ceguang_left_Temp=s1.result().clone();
    //imwrite("D:\\src_ceguang_left0418.bmp",src_ceguang_left);
    //右相机侧光图像
    src_ceguang_right_Temp=s3.result().clone();
    //imwrite("D:\\src_ceguang_right0418.bmp",src_ceguang_right);
    //主相机侧光图像
    src_ceguang1_Temp=s2.result().clone();
    //imwrite("D:\\src_ceguang10418.bmp",src_ceguang1);

//    QString s = " ";
//    if(!store_flag)
//    {
//        s = "_1.bmp";
//        store_flag  = true;
//    }
//    else
//    {
//        s = "_2.bmp";
//        store_flag = false;
//    }

//    QString Cache_savePath_ceguang_left = Cache_savePath + "\\src_ceguang_left"+s;
//    string Cache_Save_ceguang_left=Cache_savePath_ceguang_left.toStdString();
//    imwrite(Cache_Save_ceguang_left,src_ceguang_left_Temp);

//    QString Cache_savePath_ceguang1 = Cache_savePath + "\\src_ceguang1"+s;
//    string Cache_Save_ceguang1=Cache_savePath_ceguang1.toStdString();
//    imwrite(Cache_Save_ceguang1,src_ceguang1_Temp);

//    QString Cache_savePath_ceguang_right = Cache_savePath + "\\src_ceguang_right"+s;
//    string Cache_Save_ceguang_right=Cache_savePath_ceguang_right.toStdString();
//    imwrite(Cache_Save_ceguang_right,src_ceguang_right_Temp);

    MV_CC_SetFloatValue(camera->m_hDevHandle_1,"ExposureTime", Exposure_White_Black_Left);   //左侧黑白相机白底曝光值
    MV_CC_SetFloatValue(camera->m_hDevHandle_2,"ExposureTime", Exposure_White_Black_Middle); //黑白主相机拍摄2
    MV_CC_SetFloatValue(camera->m_hDevHandle_3,"ExposureTime", Exposure_White_Color_Right);  //右侧黑白相机白底曝光值


    ui->label_30->setText("侧光拍摄完成");

    //================通知PLC关闭侧光源打开测试盒============================
    if(write_Modbus(1616,1))//拍照完成1<白
    {
        //delay(modbus_time);
        Data_Form_Plc=0;
        int i;
        for(i=0;i<=50;i++)
        {
            read_Modbus(1601);//通知拍2<黑
            delay(modbus_time);
            if(Data_Form_Plc==1)
            {
                Data_Form_Plc=0;
                break;
            }
            else
            {
                delay(modbus_time);
                continue;
            }
        }
        if(i >= 50&&Data_Form_Plc!=2)
        {
            QMessageBox box(QMessageBox::Warning,"提示","白底切换信号读取失败");
            box.setWindowIcon(QIcon(":/resourse/tishi.png"));
            box.addButton(QMessageBox::Ok);
            box.button(QMessageBox::Ok)->hide();
            box.show();
            box.exec ();
        }
    }
    else
    {
        QMessageBox box(QMessageBox::Warning,"提示","侧光完成信号写入失败");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.addButton(QMessageBox::Ok);
        box.button(QMessageBox::Ok)->hide();
        box.show();
        box.exec ();
    }

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("测试盒已经打开"+current_date);

    if(F_start==false)
        return 0;

    ui->label_30->setText("侧光完成信号");

//    delay(BoxOpenTime);//打开测试盒点亮为白底屏幕拍照延时
    delay_msec(BoxOpenTime);//打开测试盒点亮为白底屏幕拍照延时
    //================拍摄白底图像========================
    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("开始拍摄白底图"+current_date);

    QFuture<cv::Mat> c1 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, int(1));//开线程左侧相机拍照
    QFuture<cv::Mat> c3 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, int(3));//开线程右侧相机拍照
    QFuture<cv::Mat> c2 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, int(2));//开线程中间相机拍照
    c1.waitForFinished();  //等待线程关闭
    c3.waitForFinished();
    c2.waitForFinished();

    src_L1_Temp=c1.result().clone();
    //imwrite("D:\\src_L10418.bmp",src_L1);

    src_R1_Temp=c3.result().clone();
    //imwrite("D:\\src_R10418.bmp",src_R1);

    src_white1_Temp =c2.result().clone();
    //imwrite("D:\\src_white10418.bmp",src_white1);

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("白底图拍照完毕"+current_date);

//    QString Cache_savePath_white1 = Cache_savePath + "\\src_white1"+s;
//    string Cache_Save_white1=Cache_savePath_white1.toStdString();
//    imwrite(Cache_Save_white1,src_white1_Temp);

//    QString Cache_savePath_L1 = Cache_savePath + "\\src_L1"+s;
//    string Cache_Save_L1=Cache_savePath_L1.toStdString();
//    imwrite(Cache_Save_L1,src_L1_Temp);

//    QString Cache_savePath_R1 = Cache_savePath + "\\src_R1"+s;
//    string Cache_Save_R1=Cache_savePath_R1.toStdString();
//    imwrite(Cache_Save_R1,src_R1_Temp);

    if(src_ceguang1_Temp.channels() == 3)
        cvtColor(src_ceguang1_Temp,src_ceguang1_Temp,CV_BGR2GRAY);
    if(src_ceguang_left_Temp.channels() == 3)
        cvtColor(src_ceguang_left_Temp,src_ceguang_left_Temp,CV_BGR2GRAY);
    if(src_ceguang_right_Temp.channels() == 3)
        cvtColor(src_ceguang_right_Temp,src_ceguang_right_Temp,CV_BGR2GRAY);
    if(src_white1_Temp.channels() == 3)
        cvtColor(src_white1_Temp,src_white1_Temp,CV_BGR2GRAY);
    if(src_L1_Temp.channels() == 3)
        cvtColor(src_L1_Temp,src_L1_Temp,CV_BGR2GRAY);
    if(src_R1_Temp.channels() == 3)
        cvtColor(src_R1_Temp,src_R1_Temp,CV_BGR2GRAY);

    //拍摄图片入队
    struct Dect_Photo Dect_Photo1;
    Dect_Photo1.SideLight_Main=src_ceguang1_Temp;
    Dect_Photo1.SideLight_Left=src_ceguang_left_Temp;
    Dect_Photo1.SideLight_Right=src_ceguang_right_Temp;

    Dect_Photo1.White_Main=src_white1_Temp;
    Dect_Photo1.White_Left=src_L1_Temp;
    Dect_Photo1.White_Right=src_R1_Temp;

    Pic.enqueue(Dect_Photo1);

    ui->label_30->setText("拍照完成");

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("waitforfinished前"+current_date);

    //d1.waitForFinished();
    while(!(d1.isFinished()))
        delay(10);

    //=====================通知PLC转动转盘，开启下一个拍照节拍===================
    while(true)
    {
        Data_Form_Plc=0;
        if(write_Modbus(1619,1))//拍照结束,转动转盘
        {
            break;
//            delay(modbus_time);
//            read_Modbus(1619);
        }
        else
        {
            delay(modbus_time);
            continue;
        }
//        delay(modbus_time);
//        if(Data_Form_Plc==1)
//        {
//            Data_Form_Plc=0;
//            break;
//        }
    }

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("myfunc1前"+current_date);

    d1 =QtConcurrent::run(this,&MainWindow::myFunc1);     //开白底检测线程

    if(autoSave==true && SaveSize == true)
    {
        debug_msg("保存检测原图像1_DefectNum1 == 0");
        QString savePath=imageSavePath;
        //创建样本图片文件夹
        QDir *sampleFolder = new QDir;
        bool isExist = sampleFolder->exists(savePath);
        if(!isExist)
        {
            sampleFolder->mkdir(savePath);
        }
        //创建工位拍摄图片子文件夹
        QDir *takenPictures=new QDir;
        savePath+="\\station";
        isExist = takenPictures->exists(savePath);
        if(!isExist)
        {
            takenPictures->mkdir(savePath);
        }
        //创建以某天命名的子文件夹
        QDir *dayFolder=new QDir;
        savePath = savePath + "\\" + product_Lot;
        isExist=dayFolder->exists(savePath);
        if(!isExist)
        {
            dayFolder->mkdir(savePath);
        }
        //创建以序号命名的文件夹
        QDir *numFolder=new QDir;
        QString serialNum="\\"+QString::number(num7+1);
        savePath+=serialNum;
        isExist=numFolder->exists(savePath);
        if(!isExist)
        {
            numFolder->mkdir(savePath);
        }
        markSavePath.clear();
        markSavePath = savePath.toStdString();
        QString save_ceguang1_1;
        string save_ceguang1;
        save_ceguang1_1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_210.bmp";//主相机侧光灯图像
        save_ceguang1_1=savePath+save_ceguang1_1;
        save_ceguang1=save_ceguang1_1.toStdString();

        QString save_ceguang_left1;
        string save_ceguang_left;
        save_ceguang_left1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_010.bmp";//左相机侧光灯图像
        save_ceguang_left1 = savePath + save_ceguang_left1;
        save_ceguang_left=save_ceguang_left1.toStdString();

        QString save_ceguang_right1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_110.bmp";
        save_ceguang_right1 = savePath + save_ceguang_right1;//右相机侧光灯图像
        string save_ceguang_right=save_ceguang_right1.toStdString();


        QString save_white1_1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_212.bmp";
        save_white1_1 = savePath + save_white1_1 ;//主黑白相机白底
        string save_white1=save_white1_1.toStdString();

        QString save_L1_1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_012.bmp";
        save_L1_1 = savePath + save_L1_1 ;//左侧相机拍摄白底
        string save_L1=save_L1_1.toStdString();

        QString save_R1_1 = "\\_" + product_Lot + "_" +QString::number(num7+1) + "_112.bmp";
        save_R1_1 = savePath + save_R1_1;//右侧相机拍摄白底
        string save_R1=save_R1_1.toStdString();

        imwrite(save_ceguang1,src_ceguang1_Temp);//主相机侧光灯图像
        imwrite(save_ceguang_left,src_ceguang_left_Temp);//左相机侧光灯图像
        imwrite(save_ceguang_right,src_ceguang_right_Temp);//右相机侧光灯图像

        imwrite(save_white1,src_white1_Temp);//白底
        imwrite(save_L1,src_L1_Temp);//左侧相机拍摄白底
        imwrite(save_R1,src_R1_Temp);//右侧相机拍摄白底

    }

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("拍照线程结束"+current_date);

//    m_timer.start(50);

//    while(F_start==true)
//    {
//        if(Flag_First_Photo==true)
//        {
//            Flag_First_Photo=false;
//            Write_Photo_Finish();
//            ui->label_30->setText("本节拍拍照完成");
//            m_timer.start(50);
//            break;
//        }
//        if(Flag_Dec_Complete==true)
//        {
//            Flag_Dec_Complete=false;
//            Write_Photo_Finish();
//            ui->label_30->setText("本节拍拍照完成");
//            m_timer.start(50);
//            break;
//        }
//        delay(50);
//    }

    return 0;
}


/*====================================================================
* 功能：强制保存使用
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::Force_Save(QString Path)
{
    QString Force_savePath_ceguang_left = Path + "\\src_ceguang_left.bmp";
    string Force_Save_ceguang_left=Force_savePath_ceguang_left.toStdString();
    imwrite(Force_Save_ceguang_left,src_ceguang_left);

    QString Force_savePath_ceguang_right = Path + "\\src_ceguang_right.bmp";
    string Force_Save_ceguang_right=Force_savePath_ceguang_right.toStdString();
    imwrite(Force_Save_ceguang_right,src_ceguang_right);

    QString Force_savePath_ceguang1 = Path + "\\src_ceguang1.bmp";
    string Force_Save_ceguang1=Force_savePath_ceguang1.toStdString();
    imwrite(Force_Save_ceguang1,src_ceguang1);

    QString Force_savePath_white1 = Path + "\\src_white1.bmp";
    string Force_Save_white1=Force_savePath_white1.toStdString();
    imwrite(Force_Save_white1,src_white1);

    QString Force_savePath_L1 = Path + "\\src_L1.bmp";
    string Force_Save_L1=Force_savePath_L1.toStdString();
    imwrite(Force_Save_L1,src_L1);

    QString Force_savePath_R1 = Path + "\\src_R1.bmp";
    string Force_Save_R1=Force_savePath_R1.toStdString();
    imwrite(Force_Save_R1,src_R1);

    if(result_white1==true)//白底子线程1存在缺陷
    {
        QString Force_savePath_Result = Path + "\\white_result1.bmp";
        string Force_Save_Result=Force_savePath_Result.toStdString();
        imwrite(Force_Save_Result,Mresult_1_white);
    }
    if(result_white2==true)//白底子线程1存在缺陷
    {
        QString Force_savePath_Result = Path + "\\white_result2.bmp";
        string Force_Save_Result=Force_savePath_Result.toStdString();
        imwrite(Force_Save_Result,Mresult_2_white);
    }
    if(result_white3==true)//白底子线程1存在缺陷
    {
        QString Force_savePath_Result = Path + "\\white_result3.bmp";
        string Force_Save_Result=Force_savePath_Result.toStdString();
        imwrite(Force_Save_Result,Mresult_3_white);
    }
}

/*====================================================================
* 功能：检测结果显示及处理
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::Dect_Result()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("进入检测结果处理"+current_date);

    if(Flag_Running_State!="Offline")
    //写检测结果
    Write_Photo_Finish();

    //==========================检测结果逻辑判断==============================
    QString DefectNum[20];
    int DefectNum1 = 0;

    QImage image2;
    QMatrix matrix,matrix1,matrix2,matrix3;

    ui->label_34->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
    ui->label_36->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
    ui->label_38->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
    ui->label_40->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
    ui->label_42->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
    ui->label_44->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
//    ui->label_46->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
//    ui->label_54->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
//    ui->label_63->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
    ui->label_65->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
//    ui->label_67->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
    ui->label_69->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
    ui->label_71->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
//    ui->label_100->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");

    if(result_white_1==true)//白底下存在缺陷
    {
        if(result_white1==true)//白底子线程1存在缺陷
        {
            QString Cache_savePath_white_result = Cache_savePath + "\\white_result1.bmp";
            string Cache_Save_white_result=Cache_savePath_white_result.toStdString();
            imwrite(Cache_Save_white_result,Mresult_1_white);

            image2 =MatToQImage(Mresult_1_white);//显示白底下的检测结果
            matrix1.rotate(90.0);
            image2=image2.transformed(matrix1,Qt::FastTransformation);
            if(Flag_Running_State=="Online")
                ui->label->setFixedSize(543,806);
            image2=image2.scaled(ui->label->size(),Qt::KeepAspectRatio);
            //ui->label_2->setScaledContents(true);
            ui->label->setAlignment(Qt::AlignCenter);
            ui->label->setPixmap(QPixmap::fromImage(image2));
        }
        else
        {
            image2 =MatToQImage(whiteshow1);//显示白底下的检测结果
            matrix1.rotate(90.0);
            image2=image2.transformed(matrix1,Qt::FastTransformation);
            if(Flag_Running_State=="Online")
                ui->label->setFixedSize(543,806);
            image2=image2.scaled(ui->label->size(),Qt::KeepAspectRatio);
            //ui->label_2->setScaledContents(true);
            ui->label->setAlignment(Qt::AlignCenter);
            ui->label->setPixmap(QPixmap::fromImage(image2));
        }

        if(result_white2==true)//白底子线程2存在缺陷
        {
            QString Cache_savePath_white_result = Cache_savePath + "\\white_result2.bmp";
            string Cache_Save_white_result=Cache_savePath_white_result.toStdString();
            imwrite(Cache_Save_white_result,Mresult_2_white);

            image2 =MatToQImage(Mresult_2_white);//显示白底下的检测结果
            matrix2.rotate(90.0);
            image2=image2.transformed(matrix2,Qt::FastTransformation);
            if(Flag_Running_State=="Online")
                ui->label_2->setFixedSize(543,806);
            image2=image2.scaled(ui->label_2->size(),Qt::KeepAspectRatio);
            //ui->label_2->setScaledContents(true);
            ui->label_2->setAlignment(Qt::AlignCenter);
            ui->label_2->setPixmap(QPixmap::fromImage(image2));
        }
        else
        {
            image2 =MatToQImage(whiteshow1);//显示白底下的检测结果
            matrix2.rotate(90.0);
            image2=image2.transformed(matrix2,Qt::FastTransformation);
            if(Flag_Running_State=="Online")
                ui->label_2->setFixedSize(543,806);
            image2=image2.scaled(ui->label_2->size(),Qt::KeepAspectRatio);
            //ui->label_2->setScaledContents(true);
            ui->label_2->setAlignment(Qt::AlignCenter);
            ui->label_2->setPixmap(QPixmap::fromImage(image2));
        }

        if(result_white3==true)//白底子线程3存在缺陷
        {
            QString Cache_savePath_white_result = Cache_savePath + "\\white_result3.bmp";
            string Cache_Save_white_result=Cache_savePath_white_result.toStdString();
            imwrite(Cache_Save_white_result,Mresult_3_white);

            image2 =MatToQImage(Mresult_3_white);//显示白底下的检测结果
            matrix3.rotate(90.0);
            image2=image2.transformed(matrix3,Qt::FastTransformation);
            if(Flag_Running_State=="Online")
                ui->label_3->setFixedSize(543,806);
            image2=image2.scaled(ui->label_3->size(),Qt::KeepAspectRatio);
            //ui->label_2->setScaledContents(true);
            ui->label_3->setAlignment(Qt::AlignCenter);
            ui->label_3->setPixmap(QPixmap::fromImage(image2));
        }
        else
        {
            image2 =MatToQImage(whiteshow1);//显示白底下的检测结果
            matrix3.rotate(90.0);
            image2=image2.transformed(matrix3,Qt::FastTransformation);
            if(Flag_Running_State=="Online")
                ui->label_3->setFixedSize(543,806);
            image2=image2.scaled(ui->label_3->size(),Qt::KeepAspectRatio);
            ui->label_3->setAlignment(Qt::AlignCenter);
            ui->label_3->setPixmap(QPixmap::fromImage(image2));
        }
        ui->label_10->setStyleSheet("color:red;font: 14pt;");
        ui->label_10->setText("NG");//中间红色检测结果
        ui->label_14->setText(QString("%1 %2 %3").arg(causeColor_1_white).arg(causeColor_2_white).arg(causeColor_3_white));
        ui->label_57->setStyleSheet("color:red;font: 44pt;QFont::StyleOblique;");
        ui->label_57->setText("NG");

        //==========================缺陷信息统计==============================
        //白底下缺陷
        if(causeColor_1_white=="无显示")
        {
            wuxian_num++;

            ui->label_71->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
            ui->label_71->setText(QString("%1").arg(wuxian_num));        //异物数目
        }
        if(causeColor_1_white=="背光异物")
        {
            yiwu_num++;
            DefectNum[DefectNum1] = "01";
            DefectNum1++;
            ui->label_34->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
            ui->label_34->setText(QString("%1").arg(yiwu_num));        //异物数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\yiwu";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }

            if(yiwu_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\yiwu" + QString::number(yiwu_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }

        }
        if(causeColor_2_white=="白点")
        {
            white_num++;
            DefectNum[DefectNum1] = "02";
            DefectNum1++;
            ui->label_36->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
            ui->label_36->setText(QString("%1").arg(white_num));        //异物数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\white_point";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }

            if(white_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\white_point" + QString::number(white_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }
        }
        if(causeColor_3_white=="白印")
        {
            mura_num++;
            DefectNum[DefectNum1] = "03";
            DefectNum1++;
            ui->label_38->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
            ui->label_38->setText(QString("%1").arg(mura_num));        //异物数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\baiyin";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }
            if(mura_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\baiyin" + QString::number(mura_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }
        }
        if(causeColor_3_white=="移位")
        {
            lackline_num++;
            DefectNum[DefectNum1] = "10";
            DefectNum1++;
            ui->label_42->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
            ui->label_42->setText(QString("%1").arg(lackline_num));        //异物数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\xianyi";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }
            if(lackline_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\xianyi" + QString::number(lackline_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }
        }
        if(causeColor_1_white=="划伤")
        {
            scratch_num++;
            DefectNum[DefectNum1] = "04";
            DefectNum1++;
            ui->label_69->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
            ui->label_69->setText(QString("%1").arg(scratch_num));        //异物数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\huashang";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }
            if(scratch_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\huashang" + QString::number(scratch_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }
        }
        if(causeColor_1_white=="死灯")
        {
            dengyan_num++;
            DefectNum[DefectNum1] = "06";
            DefectNum1++;
            ui->label_40->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
            ui->label_40->setText(QString("%1").arg(dengyan_num));        //异物数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\dengyan";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }
            if(dengyan_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\dengyan" + QString::number(dengyan_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }
        }
        if(causeColor_2_white=="亮边")
        {
            liangbian_num++;
            DefectNum[DefectNum1] = "06";
            DefectNum1++;
            ui->label_44->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
            ui->label_44->setText(QString("%1").arg(liangbian_num));        //异物数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\liangbian";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }
            if(liangbian_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\liangbian" + QString::number(liangbian_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }
        }
        if(causeColor_1_white=="漏光")
        {
            louguang_num++;
            DefectNum[DefectNum1] = "06";
            DefectNum1++;
//            ui->label_44->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
//            ui->label_44->setText(QString("%1").arg(louguang_num));        //异物数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\louguang";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }
            if(louguang_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\louguang" + QString::number(louguang_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }
        }
        if(causeColor_1_white=="边框")
        {
            biankuang_num++;
            DefectNum[DefectNum1] = "06";
            DefectNum1++;
//            ui->label_46->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
//            ui->label_46->setText(QString("%1").arg(biankuang_num));        //边框数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\biankuang";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }
            if(biankuang_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\biankuang" + QString::number(biankuang_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }
        }
        if(causeColor_2_white=="暗角")
        {
            anjiao_num++;
            DefectNum[DefectNum1] = "06";
            DefectNum1++;
//            ui->label_54->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
//            ui->label_54->setText(QString("%1").arg(anjiao_num));        //暗角数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\anjiao";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }
            if(anjiao_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\anjiao" + QString::number(anjiao_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }
        }
        if(causeColor_3_white=="少料")
        {
            shaoliao_num++;
            DefectNum[DefectNum1] = "06";
            DefectNum1++;
//            ui->label_63->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
//            ui->label_63->setText(QString("%1").arg(shaoliao_num));        //少料数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\shaoliao";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }
            if(shaoliao_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\shaoliao" + QString::number(shaoliao_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }
        }
        if(causeColor_2_white=="爆灯")
        {
            baodeng_num++;
            DefectNum[DefectNum1] = "06";
            DefectNum1++;
            ui->label_65->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
            ui->label_65->setText(QString("%1").arg(baodeng_num));        //异物数目

            QDir *dayFolder=new QDir;
            QString Force_savePath_local= Force_savePath;
            Force_savePath_local= Force_savePath_local + "\\baodeng";
            isExist=dayFolder->exists(Force_savePath_local);
            if(!isExist)
            {
                dayFolder->mkdir(Force_savePath_local);
            }
            if(baodeng_num<=10)
            {
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                //QString Force_savePath_local= Force_savePath;
                Force_savePath_local= Force_savePath_local + "\\baodeng" + QString::number(baodeng_num);
                isExist=dayFolder->exists(Force_savePath_local);
                if(!isExist)
                {
                    dayFolder->mkdir(Force_savePath_local);
                }
                Force_Save(Force_savePath_local);
            }
        }
    }

    if(result_white_1==false)
    {
        ui->label_10->setStyleSheet("color:green;font: 14pt;font: 黑体;");
        ui->label_10->setText("OK");
        ui->label_14->setText("合格");
        image2 =MatToQImage(whiteshow1); //白底透视变换后的图像
        matrix.rotate(90.0);
        image2=image2.transformed(matrix,Qt::FastTransformation);
        if(Flag_Running_State=="Online")
        ui->label_2->setFixedSize(543,806);
        image2=image2.scaled(ui->label_2->size(),Qt::KeepAspectRatio);
        //ui->label_2->setScaledContents(true);
        ui->label_2->setAlignment(Qt::AlignCenter);
        ui->label_2->setPixmap(QPixmap::fromImage(image2));
        ui->label_57->setText(" ");
        ui->label_57->setStyleSheet("color:green;font: 44pt;font: 黑体;");
        ui->label_57->setText("OK");
        qualified_num++;
        //updata_database("合格");
    }

    //==========================拍摄图像亮度异常提醒======================
    if(main_whitemean<100 && causeColor_1_white=="异物")
    {
        ui->label_10->setStyleSheet("color:yellow;font: 14pt;font: 黑体;");
        ui->label_10->setText("主相机亮度过低");
    }
    if(main_whitemean>210 && causeColor_1_white=="异物")
    {
        ui->label_10->setStyleSheet("color:yellow;font: 14pt;font: 黑体;");
        ui->label_10->setText("主相机亮度过高");
    }
    if(left_whitemean<100 && causeColor_1_white=="白点")
    {
        ui->label_10->setStyleSheet("color:yellow;font: 14pt;font: 黑体;");
        ui->label_10->setText("左相机亮度过低");
    }
    if(left_whitemean>210 && causeColor_1_white=="白点")
    {
        ui->label_10->setStyleSheet("color:yellow;font: 14pt;font: 黑体;");
        ui->label_10->setText("左相机亮度过高");
    }
    test_num++;
    //==========================统计屏幕亮度信息===========================
    //计算三个相机平均亮度
    if(main_light != 0.0)
    {
        light_num++;
        Average_mainlight = (Average_mainlight * (light_num-1) + main_light)/light_num;
        Average_rightlight = (Average_rightlight * (light_num-1) + right_light)/light_num;
        Average_leftlight = (Average_leftlight * (light_num-1) + left_light)/light_num;
    }

    ui->label_101->setText( QString("%1").arg(Average_mainlight));       //主相机平均亮度
    ui->label_15->setText(QString("%1").arg(Average_rightlight));       //右相机平均亮度
    ui->label_86->setText(QString("%1").arg(Average_leftlight));       //左相机平均亮度
    //计算三个相机的最大亮度
    if(Max_mainlight < main_light)
    {
        Max_mainlight = main_light;
    }
    if(Max_rightlight < right_light)
    {
        Max_rightlight = right_light;
    }
    if(Max_leftlight < left_light)
    {
        Max_leftlight = left_light;
    }

    ui->label_102->setText( QString("%1").arg(Max_mainlight));       //主相机最大亮度
    ui->label_82->setText( QString("%1").arg(Max_rightlight));       //右相机最大亮度
    ui->label_83->setText( QString("%1").arg(Max_leftlight));       //左相机最大亮度
    //计算三个相机的最小亮度
    if(Min_mainlight == 0.0)
    {
        Min_mainlight = main_light;
    }
    else if(Min_mainlight > main_light && main_light != 0.0)
    {
        Min_mainlight = main_light;
    }
    if(Min_rightlight == 0.0)
    {
        Min_rightlight = right_light;
    }
    else if(Min_rightlight > right_light && right_light != 0.0)
    {
        Min_rightlight = right_light;
    }
    if(Min_leftlight == 0.0)
    {
        Min_leftlight = left_light;
    }
    else if(Min_leftlight > left_light && left_light != 0.0)
    {
        Min_leftlight = left_light;
    }
    ui->label_103->setText( QString("%1").arg(Min_mainlight));       //主相机最小亮度
    ui->label_84->setText( QString("%1").arg(Min_rightlight));       //右相机最小亮度
    ui->label_85->setText( QString("%1").arg(Min_leftlight));       //左相机最小亮度
    //==========================缺陷信息显示==============================
    ui->label_47->setText("已检总数：");
    ui->label_49->setText("合格品数：");
    ui->label_51->setText("当前良率：");
    pass_per=100*(qualified_num/test_num);
    ui->label_48->setText( QString("%1").arg(test_num));       //已经检测总数目
    ui->label_50->setText( QString("%1").arg(qualified_num));  //合格品总数目
    //String numPersentage = pass_per.ToString("P");
    ui->label_52->setText( QString("%1%").arg(pass_per));       //当前良率

//    if(result_white1||result_white2||result_white3)
//        updata_database("不合格");
//    else
//        updata_database("合格");
    //==========================保存检测原图像=============================
    num7++;
    //获取当前时间
    QDateTime currentTime = QDateTime::currentDateTime();
    QString currentDay = currentTime.toString("\\yyyyMMdd");

    if(autoSave==true && SaveSize == true)
    {
        if(DefectNum1 == 0)
        {
            QString savePath=imageSavePath;
            savePath+="\\station";
            savePath = savePath + "\\" + product_Lot;
            QString serialNum="\\"+QString::number(num7);
            savePath+=serialNum;
            markSavePath.clear();
            markSavePath = savePath.toStdString();
            QString save_ceguang1_1;
            string save_ceguang1;
            save_ceguang1_1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_210.bmp";//主相机侧光灯图像
            save_ceguang1_1=savePath+save_ceguang1_1;
            save_ceguang1=save_ceguang1_1.toStdString();

            QString save_ceguang_left1;
            string save_ceguang_left;
            save_ceguang_left1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_010.bmp";//左相机侧光灯图像
            save_ceguang_left1 = savePath + save_ceguang_left1;
            save_ceguang_left=save_ceguang_left1.toStdString();

            QString save_ceguang_right1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_110.bmp";
            save_ceguang_right1 = savePath + save_ceguang_right1;//右相机侧光灯图像
            string save_ceguang_right=save_ceguang_right1.toStdString();

            QString save_white1_1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_212.bmp";
            save_white1_1 = savePath + save_white1_1 ;//主黑白相机白底
            string save_white1=save_white1_1.toStdString();

            QString save_L1_1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_012.bmp";
            save_L1_1 = savePath + save_L1_1 ;//左侧相机拍摄白底
            string save_L1=save_L1_1.toStdString();

            QString save_R1_1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_112.bmp";
            save_R1_1 = savePath + save_R1_1;//右侧相机拍摄白底
            string save_R1=save_R1_1.toStdString();

            QFile file(save_ceguang1_1);
            if (file.exists())
            {
                Mat temp_Mat = cv::imread(save_ceguang1,-1);
                save_ceguang1_1 = "\\00_" + product_Lot + "_" +QString::number(num7) + "_210.bmp";//主相机侧光灯图像
                save_ceguang1_1=savePath+save_ceguang1_1;
                save_ceguang1=save_ceguang1_1.toStdString();
                imwrite(save_ceguang1,temp_Mat);//主相机侧光灯图像
                file.remove();
            }
            QFile file1(save_ceguang_left1);
            if (file1.exists())
            {
                Mat temp_Mat = cv::imread(save_ceguang_left,-1);
                save_ceguang_left1 = "\\00_" + product_Lot + "_" +QString::number(num7) + "_010.bmp";//左相机侧光灯图像
                save_ceguang_left1 = savePath + save_ceguang_left1;
                save_ceguang_left=save_ceguang_left1.toStdString();
                imwrite(save_ceguang_left,temp_Mat);//左相机侧光灯图像
                file1.remove();
            }
            QFile file2(save_ceguang_right1);
            if (file2.exists())
            {
                Mat temp_Mat = cv::imread(save_ceguang_right,-1);
                save_ceguang_right1 = "\\00_" + product_Lot + "_" +QString::number(num7) + "_110.bmp";
                save_ceguang_right1 = savePath + save_ceguang_right1;//右相机侧光灯图像
                save_ceguang_right=save_ceguang_right1.toStdString();
                imwrite(save_ceguang_right,temp_Mat);//左相机侧光灯图像
                file2.remove();
            }

            QFile file5(save_white1_1);
            if (file5.exists())
            {
                Mat temp_Mat = cv::imread(save_white1,-1);
                save_white1_1 = "\\00_" + product_Lot + "_" +QString::number(num7) + "_212.bmp";
                save_white1_1 = savePath + save_white1_1 ;//主黑白相机白底
                save_white1=save_white1_1.toStdString();
                imwrite(save_white1,temp_Mat);//左相机侧光灯图像
                file5.remove();
            }
            QFile file6(save_L1_1);
            if (file6.exists())
            {
                Mat temp_Mat = cv::imread(save_L1,-1);
                save_L1_1 = "\\00_" + product_Lot + "_" +QString::number(num7) + "_012.bmp";
                save_L1_1 = savePath + save_L1_1 ;//左侧相机拍摄白底
                save_L1=save_L1_1.toStdString();
                imwrite(save_L1,temp_Mat);//左相机侧光灯图像
                file6.remove();
            }
            QFile file7(save_R1_1);
            if (file7.exists())
            {
                Mat temp_Mat = cv::imread(save_R1,-1);
                save_R1_1 = "\\00_" + product_Lot + "_" +QString::number(num7) + "_112.bmp";
                save_R1_1 = savePath + save_R1_1;//右侧相机拍摄白底
                save_R1=save_R1_1.toStdString();
                imwrite(save_R1,temp_Mat);//左相机侧光灯图像
                file7.remove();
            }

            string save_qualified_result="\\qualified_result.bmp";
            save_qualified_result=savePath.toStdString()+save_qualified_result;

                imwrite(save_qualified_result,whiteshow1);  //合格结果图
        }
        else {
            for(int lackNum = 0; lackNum < DefectNum1; lackNum++)
            {
                QString savePath=imageSavePath;

                savePath+="\\station";
                savePath = savePath + "\\" + product_Lot;

                QString serialNum="\\"+QString::number(num7);
                savePath+=serialNum;
                markSavePath.clear();
                markSavePath = savePath.toStdString();
                QString save_ceguang1_1;
                string save_ceguang1;
                save_ceguang1_1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_210.bmp";//主相机侧光灯图像
                save_ceguang1_1=savePath+save_ceguang1_1;
                save_ceguang1=save_ceguang1_1.toStdString();

                QString save_ceguang_left1;
                string save_ceguang_left;
                save_ceguang_left1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_010.bmp";//左相机侧光灯图像
                save_ceguang_left1 = savePath + save_ceguang_left1;
                save_ceguang_left=save_ceguang_left1.toStdString();

                QString save_ceguang_right1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_110.bmp";
                save_ceguang_right1 = savePath + save_ceguang_right1;//右相机侧光灯图像
                string save_ceguang_right=save_ceguang_right1.toStdString();


                QString save_white1_1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_212.bmp";
                save_white1_1 = savePath + save_white1_1 ;//主黑白相机白底
                string save_white1=save_white1_1.toStdString();

                QString save_L1_1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_012.bmp";
                save_L1_1 = savePath + save_L1_1 ;//左侧相机拍摄白底
                string save_L1=save_L1_1.toStdString();

                QString save_R1_1 = "\\_" + product_Lot + "_" +QString::number(num7) + "_112.bmp";
                save_R1_1 = savePath + save_R1_1;//右侧相机拍摄白底
                string save_R1=save_R1_1.toStdString();


                QFile file(save_ceguang1_1);
                if (file.exists())
                {
                    Mat temp_Mat = cv::imread(save_ceguang1,-1);
                    save_ceguang1_1 = "\\"+DefectNum[lackNum]+"_" + product_Lot + "_" +QString::number(num7) + "_210.bmp";//主相机侧光灯图像
                    save_ceguang1_1=savePath+save_ceguang1_1;
                    save_ceguang1=save_ceguang1_1.toStdString();
                    imwrite(save_ceguang1,temp_Mat);//主相机侧光灯图像
                    file.remove();
                }
                QFile file1(save_ceguang_left1);
                if (file1.exists())
                {
                    Mat temp_Mat = cv::imread(save_ceguang_left,-1);
                    save_ceguang_left1 = "\\"+DefectNum[lackNum]+"_" + product_Lot + "_" +QString::number(num7) + "_010.bmp";//左相机侧光灯图像
                    save_ceguang_left1 = savePath + save_ceguang_left1;
                    save_ceguang_left=save_ceguang_left1.toStdString();
                    imwrite(save_ceguang_left,temp_Mat);//左相机侧光灯图像
                    file1.remove();
                }
                QFile file2(save_ceguang_right1);
                if (file2.exists())
                {
                    Mat temp_Mat = cv::imread(save_ceguang_right,-1);
                    save_ceguang_right1 = "\\"+DefectNum[lackNum]+"_" + product_Lot + "_" +QString::number(num7) + "_110.bmp";
                    save_ceguang_right1 = savePath + save_ceguang_right1;//右相机侧光灯图像
                    save_ceguang_right=save_ceguang_right1.toStdString();
                    imwrite(save_ceguang_right,temp_Mat);//左相机侧光灯图像
                    file2.remove();
                }

                QFile file5(save_white1_1);
                if (file5.exists())
                {
                    Mat temp_Mat = cv::imread(save_white1,-1);
                    save_white1_1 = "\\"+DefectNum[lackNum]+"_" + product_Lot + "_" +QString::number(num7) + "_212.bmp";
                    save_white1_1 = savePath + save_white1_1 ;//主黑白相机白底
                    save_white1=save_white1_1.toStdString();
                    imwrite(save_white1,temp_Mat);//左相机侧光灯图像
                    file5.remove();
                }
                QFile file6(save_L1_1);
                if (file6.exists())
                {
                    Mat temp_Mat = cv::imread(save_L1,-1);
                    save_L1_1 = "\\"+DefectNum[lackNum]+"_" + product_Lot + "_" +QString::number(num7) + "_012.bmp";
                    save_L1_1 = savePath + save_L1_1 ;//左侧相机拍摄白底
                    save_L1=save_L1_1.toStdString();
                    imwrite(save_L1,temp_Mat);//左相机侧光灯图像
                    file6.remove();
                }
                QFile file7(save_R1_1);
                if (file7.exists())
                {
                    Mat temp_Mat = cv::imread(save_R1,-1);
                    save_R1_1 = "\\"+DefectNum[lackNum]+"_" + product_Lot + "_" +QString::number(num7) + "_112.bmp";
                    save_R1_1 = savePath + save_R1_1;//右侧相机拍摄白底
                    save_R1=save_R1_1.toStdString();
                    imwrite(save_R1,temp_Mat);//左相机侧光灯图像
                    file7.remove();
                }

                string save_white_result="\\white_result.bmp";
                save_white_result=savePath.toStdString()+save_white_result;

                //如果白底同时存在缺陷
                if(result_white_1==true)
                {
                    imwrite(save_white_result,Mresult_1_white); //白底下存在缺陷图
                }

            }
        }

    }

    if(autoSave==true && SaveSize==false)
    {
        debug_msg("保存检测原图像2");
        if(DefectNum1 != 0)
        {
            debug_msg("保存检测原图像2_DefectNum1 != 0");
            for(int lackNum = 0; lackNum < DefectNum1; lackNum++)
            {
                QString savePath=imageSavePath;
                //创建样本图片文件夹
                QDir *sampleFolder = new QDir;
                bool isExist = sampleFolder->exists(savePath);
                if(!isExist)
                {
                    sampleFolder->mkdir(savePath);
                }
                //创建工位拍摄图片子文件夹
                QDir *takenPictures=new QDir;
                savePath+="\\station";
                isExist = takenPictures->exists(savePath);
                if(!isExist)
                {
                    takenPictures->mkdir(savePath);
                }
                //创建以某天命名的子文件夹
                QDir *dayFolder=new QDir;
                savePath = savePath + "\\" + product_Lot;
                isExist=dayFolder->exists(savePath);
                if(!isExist)
                {
                    dayFolder->mkdir(savePath);
                }
                //创建以序号命名的文件夹
                QDir *numFolder=new QDir;
                QString serialNum="\\"+QString::number(num7);
                savePath+=serialNum;
                isExist=numFolder->exists(savePath);
                if(!isExist)
                {
                    numFolder->mkdir(savePath);
                }
                markSavePath.clear();
                markSavePath = savePath.toStdString();

                QString save_ceguang1_1;
                string save_ceguang1;
                save_ceguang1_1 = "\\" + DefectNum[DefectNum1] +"_" + product_Lot + "_" +QString::number(num7) + "_210.bmp";//主相机侧光灯图像
                save_ceguang1_1=savePath+save_ceguang1_1;
                save_ceguang1=save_ceguang1_1.toStdString();

                QString save_ceguang_left1;
                string save_ceguang_left;
                save_ceguang_left1 = "\\" + DefectNum[DefectNum1] +"_" + product_Lot + "_" +QString::number(num7) + "_010.bmp";//左相机侧光灯图像
                save_ceguang_left1 = savePath + save_ceguang_left1;
                save_ceguang_left=save_ceguang_left1.toStdString();

                QString save_ceguang_right1 = "\\" + DefectNum[DefectNum1] +"_" + product_Lot + "_" +QString::number(num7) + "_110.bmp";
                save_ceguang_right1 = savePath + save_ceguang_right1;//右相机侧光灯图像
                string save_ceguang_right=save_ceguang_right1.toStdString();

                QString save_white1_1 = "\\" + DefectNum[DefectNum1] +"_" + product_Lot + "_" +QString::number(num7) + "_212.bmp";
                save_white1_1 = savePath + save_white1_1 ;//主黑白相机白底
                string save_white1=save_white1_1.toStdString();

                QString save_L1_1 = "\\" + DefectNum[DefectNum1] +"_" + product_Lot + "_" +QString::number(num7) + "_012.bmp";
                save_L1_1 = savePath + save_L1_1 ;//左侧相机拍摄白底
                string save_L1=save_L1_1.toStdString();

                QString save_R1_1 = "\\" + DefectNum[DefectNum1] +"_" + product_Lot + "_" +QString::number(num7) + "_112.bmp";
                save_R1_1 = savePath + save_R1_1;//右侧相机拍摄白底
                string save_R1=save_R1_1.toStdString();

                imwrite(save_ceguang1,src_ceguang1);//主相机侧光灯图像
                imwrite(save_ceguang_left,src_ceguang_left);//左相机侧光灯图像
                imwrite(save_ceguang_right,src_ceguang_right);//右相机侧光灯图像
                imwrite(save_white1,src_white1);//白底
                imwrite(save_L1,src_L1);//左侧相机拍摄白底
                imwrite(save_R1,src_R1);//右侧相机拍摄白底

                string save_white_result="\\white_result.bmp";
                save_white_result=savePath.toStdString()+save_white_result;

                //如果黑白底同时存在缺陷
                if(result_white_1==true)
                {
                    imwrite(save_white_result,Mresult_1_white); //白底下存在缺陷图
                }
            }
        }
    }

    Flag_Dec_Complete=true;

    causeColor_1_white="";
    causeColor_2_white="";
    causeColor_3_white="";
    //m_timer.start(50);

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("检测结果处理完成"+current_date);

}



/*====================================================================
* 功能：向PLC写入拍照完成信号
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::Write_Photo_Finish()
{

//    if(First_PLC_Result==true)
//    {
//        First_PLC_Result=false;
//        //拍照第一节拍未进行检测，给PLC写入NG

//        while(F_start==true)
//        {
//            Data_Form_Plc=0;
//            if(write_Modbus(8705,1))//检测NG标志位8705
//            {
//                delay(modbus_time);

//                //emit(read_Modbus_Num(8705));

//                read_Modbus(8705);
//            }
//            else
//            {
//                delay(modbus_time);
//                continue;
//            }
//            delay(modbus_time);
//            if(Data_Form_Plc==1)
//            {
//                Data_Form_Plc=0;
//                break;
//            }
//        }
//    }
//    else
//    {
        if(result_white_1==false)
        {
            while(F_start==true)
            {   Data_Form_Plc=0;
                if(write_Modbus(1617,1))//检测OK标志位8704
                {
                    break;
                    //delay(modbus_time);
                    //read_Modbus(1617);
                }
                else
                {
                    delay(modbus_time/2);
                    continue;
                }
//                delay(modbus_time);
//                if(Data_Form_Plc==1)
//                {
//                    Data_Form_Plc=0;
//                    break;
//                }
            }
        }
        else
        {
            //ui->label_30->setText("37");
            while(F_start==true)
            {   Data_Form_Plc=0;
                if(write_Modbus(1618,1))//检测NG标志位8705
                {
                    break;
//                    delay(modbus_time);
//                    read_Modbus(1618);
                }
                else
                {
                    delay(modbus_time/2);
                    continue;
                }
//                delay(modbus_time);
//                if(Data_Form_Plc==1)
//                {
//                    Data_Form_Plc=0;
//                    break;
//                }
            }
            //ui->label_30->setText("38");
        }
    //}

}



/*====================================================================
* 函数：myFunc1
* 功能：白色底色检测子线程分别对不同型号的屏幕进行ROI提取和检测,直角工业屏和异形屏进行不同的编码
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::myFunc1()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("进入白底线程"+current_date);

    if(Pic.size()>0)
        {
            struct Dect_Photo Dect_Photo_Dect=Pic.dequeue();

            src_ceguang1=Dect_Photo_Dect.SideLight_Main;
            src_ceguang_left=Dect_Photo_Dect.SideLight_Left;
            src_ceguang_right=Dect_Photo_Dect.SideLight_Right;

            src_white1=Dect_Photo_Dect.White_Main;
            src_L1=Dect_Photo_Dect.White_Left;
            src_R1=Dect_Photo_Dect.White_Right;

//            if(src_ceguang1.channels() == 3)
//                cvtColor(src_ceguang1,src_ceguang1,CV_BGR2GRAY);
//            if(src_ceguang_left.channels() == 3)
//                cvtColor(src_ceguang_left,src_ceguang_left,CV_BGR2GRAY);
//            if(src_ceguang_right.channels() == 3)
//                cvtColor(src_ceguang_right,src_ceguang_right,CV_BGR2GRAY);
//            if(src_white1.channels() == 3)
//                cvtColor(src_white1,src_white1,CV_BGR2GRAY);
//            if(src_L1.channels() == 3)
//                cvtColor(src_L1,src_L1,CV_BGR2GRAY);
//            if(src_R1.channels() == 3)
//                cvtColor(src_R1,src_R1,CV_BGR2GRAY);
        }

    //图像声明
    Mat white1,whitecolor,white_abshow,ceL,ceR,ROI_Ceguang_left,ROI_Ceguang_right,ceL1,ceL1_Enlarge,ceR1,ceR1_Enlarge,ceguang1,Src_BackImage,Src_BackCeGuang,Src_FrontImage,Src_FrontCeGuang,LeftCeGuang,RightCeGuang,white_biankuang,LeftCeGuang_enlarge,RightCeGuang_enlarge;
    //==========================标准的直角屏幕的编码10==============================
    if(Model_Mod_type=="矩形屏")
    {
        //==========================主相机白底侧光透视变换矩阵计算==============================
        ui->label_30->setText("白色");
        roi_white(src_white1,-2,0,70,&M_white_1,&M_black_1,&M_louguang_1,&M_abshow_white,1);//黑白相机ROI变换矩阵
        ceguang1=toushi_white(src_ceguang1,M_white_1,-1,3000,1775);                         //透视变换校正后的黑白测光图
        white1=toushi_white(src_white1,M_white_1,-1,3000,1775);                             //透视变换校正后的黑白白底图
        white_abshow = toushi_white(src_white1, M_abshow_white, -1, 3000, 1775);            //透视变换校正后的黑白白底图
        white_biankuang = white1.clone();

        whiteshow1=white1.clone();

        //==========================主相机显示白底图像==============================
        QImage image2 =MatToQImage(white1);
        QMatrix matrix;
        matrix.rotate(90.0);
        image2=image2.transformed(matrix,Qt::FastTransformation);
        if(Flag_Running_State=="Online")
        ui->label_2->setFixedSize(543,806);
        image2=image2.scaled(ui->label_2->size(),Qt::KeepAspectRatio);
        //ui->label_2->setScaledContents(true);
        ui->label_2->setAlignment(Qt::AlignCenter);
        ui->label_2->setPixmap(QPixmap::fromImage(image2));

        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
        debug_msg("主相机ROI提取完毕"+current_date);

        roi_ceshi(src_L1, -10, &M_L_1);//左侧黑白相机透视变换矩阵M_L_1
        roi_ceshi(src_R1, -10, &M_R_1); //右侧黑白相机透视变换矩阵M_R_1

        ceL1=toushi_white(src_L1,M_L_1,-5,3000,1775);                       //左相机校正后图像
        ceR1=toushi_white(src_R1,M_R_1,-5,3000,1775);                       //右相机校正后图像
        ceL1_Enlarge = ceL1.clone();
        ceR1_Enlarge = ceR1.clone();
        LeftCeGuang=toushi_white(src_ceguang_left,M_L_1,-5,3000,1775);      //左相机侧光校正图
        RightCeGuang=toushi_white(src_ceguang_right,M_R_1,-5,3000,1775);    //右相机侧光校正图


        //==========================左右相机拍照取帧显示==============================
        if(isCeshi == false)//侧相机是否没有拍到.拍到false 没拍到true，拍到提取到进行显示左右界面显示
        {
            QImage image1;
            image1=MatToQImage(ceL1);//左侧面相机显示白底图像
            image1=image1.transformed(matrix,Qt::FastTransformation);
            if(Flag_Running_State=="Online")
            ui->label->setFixedSize(543,806);
            image1=image1.scaled(ui->label->size(),Qt::KeepAspectRatio);
            //ui->label->setScaledContents(true);
            ui->label->setAlignment(Qt::AlignCenter);
            ui->label->setPixmap(QPixmap::fromImage(image1));

            QImage image3;
            image3=MatToQImage(ceR1);//右侧面相机显示彩色图像
            image3=image3.transformed(matrix,Qt::FastTransformation);
            if(Flag_Running_State=="Online")
            ui->label_3->setFixedSize(543,806);
            image3=image3.scaled(ui->label_3->size(),Qt::KeepAspectRatio);
            //ui->label_3->setScaledContents(true);
            ui->label_3->setAlignment(Qt::AlignCenter);
            ui->label_3->setPixmap(QPixmap::fromImage(image3));
        }

        //相机拍摄亮度提醒
        Mat mat_mean1, mat_stddev1;
        meanStdDev(white1, mat_mean1, mat_stddev1,main_mask);
        main_light = mat_mean1.at<double>(0, 0);
        //ui->label_47->setText("主相机亮度：");
        //ui->label_101->setText (QString("%1").arg(main_light));//主相机亮度在160左右
        QString mainlight_value = QString("%1").arg(main_light);
        main_msg(mainlight_value);
        //左相机亮度
        Mat mat_mean2, mat_stddev2;
        meanStdDev(ceL1, mat_mean2, mat_stddev2,left_mask);
        left_light = mat_mean2.at<double>(0, 0);
        //ui->label_49->setText("左相机亮度：");
        //ui->label_102->setText(QString("%1").arg(left_light));
        QString leftlght_value = QString("%1").arg(left_light);
        left_msg(leftlght_value);
        //右相机亮度
        Mat mat_mean3, mat_stddev3;
        meanStdDev(ceR1, mat_mean3, mat_stddev3,right_mask);
        right_light = mat_mean3.at<double>(0, 0);
        //ui->label_51->setText("右相机亮度：");
        //ui->label_103->setText(QString("%1").arg(right_light));
        QString rightlight_value = QString("%1").arg(right_light);
        right_msg(rightlight_value);

        current_date_time =QDateTime::currentDateTime();
        current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
        debug_msg("左右相机ROI提取完毕"+current_date);

    }
    //==========================异形屏幕的编码11==============================
    if(Model_Mod_type=="R角水滴屏")
    {
        //==========================主相机白底侧光透视变换矩阵计算====================
        ui->label_30->setText("白色");
        //RoiWhite_Arcangle(src_white1,-2,10,70,&M_white_1,&M_black_1,&M_louguang_1,1);//黑白相机ROI变换矩阵
        Ext_Result_BlackWhite=f_MainCam_PersTransMatCal(src_white1, -2, 10, &M_white_1, &M_biankuang, &M_white_abshow, 1, Model_Mod_type);
        white1=toushi_white(src_white1,M_white_1,-1,pixel_num,1500);                     //透视变换校正后的黑白白底图
        white_biankuang=toushi_white(src_white1,M_biankuang,-1,pixel_num,1500);
        ceguang1=toushi_white(src_ceguang1,M_white_1,-1,pixel_num,1500);                 //透视变换校正后的黑白测光图
        //imwrite("D:\\white1.bmp",white1);
        //imwrite("D:\\ceguang1.bmp",ceguang1);
//        cvtColor(white1,white1,CV_BGR2GRAY);
//        cvtColor(ceguang1,ceguang1,CV_BGR2GRAY);
//        cvtColor(white_biankuang,white_biankuang,CV_BGR2GRAY);

        //RoiWhite_Arcangle_Abshow(src_white1, -2, 0, 70, &M_white_abshow, &M_black_abshow, &M_louguang_abshow, 1);//主黑白相机白底显异ROI变换矩阵
        white_abshow = toushi_white(src_white1, M_white_abshow, -1, pixel_num, 1500);                     //透视变换校正后的黑白白底图
//        cvtColor(white_abshow,white_abshow,CV_BGR2GRAY);

        whiteshow1=white1.clone();
        //==========================主相机显示白底图像==============================
        QImage image2 =MatToQImage(white1);
        QMatrix matrix;
        matrix.rotate(90.0);
        image2=image2.transformed(matrix,Qt::FastTransformation);

        if(Flag_Running_State=="Online")
        ui->label_2->setFixedSize(543,806);
        image2=image2.scaled(ui->label_2->size(),Qt::KeepAspectRatio);  //按图片真实比例显示
        //ui->label_2->setScaledContents(true);
        ui->label_2->setAlignment(Qt::AlignCenter);
        ui->label_2->setPixmap(QPixmap::fromImage(image2));

        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
        debug_msg("主相机ROI提取完毕"+current_date);

        //==========================侧左右相机ROI提取==============================
        Ext_Result_Left=f_LeftRightCam_PersTransMatCal(src_L1, &M_L_1, &M_L_1_E, Model_Mod_type,15);
        Ext_Result_Right=f_LeftRightCam_PersTransMatCal(src_R1, &M_R_1, &M_R_1_E, Model_Mod_type,15);

        ceL1=toushi_white(src_L1,M_L_1,-5,pixel_num,1500);                       //左相机校正后图像
        ceR1=toushi_white(src_R1,M_R_1,-5,pixel_num,1500);                       //右相机校正后图像
        ceL1_Enlarge=toushi_white(src_L1,M_L_1_E,-5,pixel_num,1500);
        ceR1_Enlarge=toushi_white(src_R1,M_R_1_E,-5,pixel_num,1500);
        LeftCeGuang=toushi_white(src_ceguang_left,M_L_1,-5,pixel_num,1500);      //左相机侧光校正图
        RightCeGuang=toushi_white(src_ceguang_right,M_R_1,-5,pixel_num,1500);    //右相机侧光校正图
        LeftCeGuang_enlarge=toushi_white(src_ceguang_left,M_L_1_E,-5,pixel_num,1500);
        RightCeGuang_enlarge=toushi_white(src_ceguang_right,M_R_1_E,-5,pixel_num,1500);

//        cvtColor(ceL1,ceL1,CV_BGR2GRAY);
//        cvtColor(ceR1,ceR1,CV_BGR2GRAY);
//        cvtColor(ceL1_Enlarge,ceL1_Enlarge,CV_BGR2GRAY);
//        cvtColor(ceR1_Enlarge,ceR1_Enlarge,CV_BGR2GRAY);
//        cvtColor(LeftCeGuang,LeftCeGuang,CV_BGR2GRAY);
//        cvtColor(RightCeGuang,RightCeGuang,CV_BGR2GRAY);
//        cvtColor(LeftCeGuang_enlarge,LeftCeGuang_enlarge,CV_BGR2GRAY);
//        cvtColor(RightCeGuang_enlarge,RightCeGuang_enlarge,CV_BGR2GRAY);

        Mat src_L1_gray,src_R1_gray;
        src_L1_gray = src_L1.clone();
        src_R1_gray = src_R1.clone();
//        cvtColor(src_L1,src_L1_gray,CV_BGR2GRAY);
//        cvtColor(src_R1,src_R1_gray,CV_BGR2GRAY);

        Mat th2,th3;
        threshold(src_L1_gray, th2, 20, 255, CV_THRESH_BINARY);
        threshold(src_R1_gray, th3, 20, 255, CV_THRESH_BINARY);
        left_mask = toushi_white(th2, M_L_1, -1, pixel_num, 1500);
        right_mask = toushi_white(th3, M_R_1, -1, pixel_num, 1500);
        bitwise_and(left_mask, ceL1, ceL1);
        bitwise_and(right_mask, ceR1, ceR1);
        bitwise_and(left_mask, LeftCeGuang, LeftCeGuang);
        bitwise_and(right_mask, RightCeGuang, RightCeGuang);

        current_date_time =QDateTime::currentDateTime();
        current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
        debug_msg("左右相机ROI提取完毕"+current_date);

        //==========================左右相机拍照取帧显示==============================
        if(isCeshi == false)//侧相机是否没有拍到.拍到false 没拍到true，拍到提取到进行显示左右界面显示
        {
            QImage image1;
            image1=MatToQImage(ceL1);//左侧面相机显示白底图像
            image1=image1.transformed(matrix,Qt::FastTransformation);
            if(Flag_Running_State=="Online")
            ui->label->setFixedSize(543,806);
            image1=image1.scaled(ui->label->size(),Qt::KeepAspectRatio);
            //ui->label->setScaledContents(true);
            ui->label->setAlignment(Qt::AlignCenter);
            ui->label->setPixmap(QPixmap::fromImage(image1));

            QImage image3;
            image3=MatToQImage(ceR1);//右侧面相机显示彩色图像
            image3=image3.transformed(matrix,Qt::FastTransformation);
            if(Flag_Running_State=="Online")
            ui->label_3->setFixedSize(543,806);
            image3=image3.scaled(ui->label_3->size(),Qt::KeepAspectRatio);
            //ui->label_3->setScaledContents(true);
            ui->label_3->setAlignment(Qt::AlignCenter);
            ui->label_3->setPixmap(QPixmap::fromImage(image3));
        }

        //相机拍摄亮度提醒
        Mat mat_mean1, mat_stddev1;
        meanStdDev(white1, mat_mean1, mat_stddev1,main_mask);
        main_light = mat_mean1.at<double>(0, 0);
        //ui->label_47->setText("主相机亮度：");
        //ui->label_101->setText (QString("%1").arg(main_light));//主相机亮度在160左右
//        QString mainlight_value = QString("%1").arg(main_light);
//        main_msg(mainlight_value);
        //左相机亮度
        Mat mat_mean2, mat_stddev2;
        meanStdDev(ceL1, mat_mean2, mat_stddev2,left_mask);
        left_light = mat_mean2.at<double>(0, 0);
        //ui->label_49->setText("左相机亮度：");
        //ui->label_102->setText(QString("%1").arg(left_light));
//        QString leftlght_value = QString("%1").arg(left_light);
//        left_msg(leftlght_value);
        //右相机亮度
        Mat mat_mean3, mat_stddev3;
        meanStdDev(ceR1, mat_mean3, mat_stddev3,right_mask);
        right_light = mat_mean3.at<double>(0, 0);
        //ui->label_51->setText("右相机亮度：");
        //ui->label_103->setText(QString("%1").arg(right_light));
//        QString rightlight_value = QString("%1").arg(right_light);
//        right_msg(rightlight_value);
    }

    Mat mainfilter=Gabor7(white1);           //滤波去除水平和竖直方向的纹理
    Mat leftfilter=Gabor7(ceL1);       //左侧白底滤波
    Mat rightfilter=Gabor7(ceR1);     //右侧白底滤波
    Mat biankuangfilter=Gabor7(white_biankuang);
    Mat CeL1_Enlarge_Fliter=Gabor7(ceL1_Enlarge);
    Mat CeR1_Enlarge_Fliter=Gabor7(ceR1_Enlarge);

    QList <Mat> img_white1;
    QList <Mat> img_white2;
    QList <Mat> img_white3;

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("开始赋值各变量"+current_date);

    img_white1.append(white1);
    img_white1.append(mainfilter);
    img_white1.append(leftfilter);
    img_white1.append(rightfilter);
    img_white1.append(ceguang1);
    img_white1.append(LeftCeGuang);
    img_white1.append(RightCeGuang);
    img_white1.append(biankuangfilter);
//    img_white1.append(CeL1_Enlarge_Fliter);
//    img_white1.append(LeftCeGuang_enlarge);
//    img_white1.append(CeR1_Enlarge_Fliter);
//    img_white1.append(RightCeGuang_enlarge);

    img_white2.append(white1);
    img_white2.append(mainfilter);
    img_white2.append(ceguang1);
    img_white2.append(CeL1_Enlarge_Fliter);
    img_white2.append(LeftCeGuang_enlarge);
    img_white2.append(CeR1_Enlarge_Fliter);
    img_white2.append(RightCeGuang_enlarge);

    img_white3.append(white_abshow);
    img_white3.append(mainfilter);
    img_white3.append(leftfilter);
    img_white3.append(rightfilter);
    img_white3.append(LeftCeGuang);
    img_white3.append(RightCeGuang);

    Flag_White1_Finish=false;
    Flag_White2_Finish=false;
    Flag_White3_Finish=false;

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("白底分线程前"+current_date);

    //白底下开启三个子线程
    QFuture<void> White_Thr1 =QtConcurrent::run(this,&MainWindow::White_Thread1,img_white1);                     //开白底检测线程
    img_white1.clear();
    Watcher_White1.setFuture(White_Thr1);
    QFuture<void> White_Thr2 =QtConcurrent::run(this,&MainWindow::White_Thread2,img_white2);                     //开黑底检测线程
    img_white2.clear();
    Watcher_White2.setFuture(White_Thr2);
    QFuture<void> White_Thr3 =QtConcurrent::run(this,&MainWindow::White_Thread3,img_white3);                     //开灰底检测线程
    img_white3.clear();
    Watcher_White3.setFuture(White_Thr3);

    while(!(Flag_White1_Finish==true&&Flag_White2_Finish==true&&Flag_White3_Finish==true))
    {
        delay(modbus_time);
        if(result_white1 == true)
        {
            Watcher_White2.cancel();
            Watcher_White3.cancel();
            break;
        }
        if(result_white2 == true)
        {
            Watcher_White1.cancel();
            Watcher_White3.cancel();
            break;
        }
        if(result_white3 == true)
        {
            Watcher_White1.cancel();
            Watcher_White2.cancel();
            break;
        }
    }

    //白底下检测函数进行检测，之前预留双工位，现在保留单工位
    //result_white_1=white_defect(white1,ceL1,ceR1,whitecolor,ceguang1,Src_BackImage,Src_BackCeGuang,Src_FrontImage,Src_FrontCeGuang,LeftCeGuang,RightCeGuang,1,white_abshow,ceL1_Enlarge,ceR1_Enlarge,white_biankuang,LeftCeGuang_enlarge,RightCeGuang_enlarge);

    if(result_white1==false&&result_white2==false&&result_white3==false)
        result_white_1=false;
    else
        result_white_1=true;

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("白底检测完成"+current_date);

    debug_msg("该节拍是否为良品"+QString::number(result_white_1));

    emit(Dectect_Result());
}


/*====================================================================
* 函数：White_Thread1
* 功能：白底子线程1，包含缺陷算法有异物，划伤，漏光，变形
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::White_Thread1(QList <Mat> img_white1)
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("进入白底子线程1"+current_date);

    Mat white=img_white1.at(0);
    Mat mainfilter=img_white1.at(1);
    Mat leftfilter=img_white1.at(2);
    Mat rightfilter=img_white1.at(3);
    Mat ceguang=img_white1.at(4);
    Mat ceguangleft=img_white1.at(5);
    Mat ceguangright=img_white1.at(6);
    Mat biankuangfilter=img_white1.at(7);
//    Mat CeL1_Enlarge_Fliter=img_white1.at(8);
//    Mat ceguangleft_enlarge=img_white1.at(9);
//    Mat CeR1_Enlarge_Fliter=img_white1.at(10);
//    Mat ceguangright_enlarge=img_white1.at(11);
    result_white1=false;
    result_white1=white_defect1(white,mainfilter,leftfilter,rightfilter,ceguang,ceguangleft,ceguangright,1,biankuangfilter);

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("白底子线程1结束"+current_date);
    Flag_White1_Finish=true;
}
/*====================================================================
* 函数：White_Thread2
* 功能：白底子线程2，包含缺陷算法有亮边，白点，暗角，爆灯
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::White_Thread2(QList <Mat> img_white2)
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("进入白底子线程2"+current_date);

    Mat white=img_white2.at(0);
    Mat mainfilter=img_white2.at(1);
    Mat ceguang=img_white2.at(2);
    Mat CeL1_Enlarge_Fliter=img_white2.at(3);
    Mat ceguangleft_enlarge=img_white2.at(4);
    Mat CeR1_Enlarge_Fliter=img_white2.at(5);
    Mat ceguangright_enlarge=img_white2.at(6);
    result_white2=false;
    result_white2=white_defect2(white,mainfilter,ceguang,CeL1_Enlarge_Fliter,ceguangleft_enlarge,CeR1_Enlarge_Fliter,ceguangright_enlarge, 1);

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("白底子线程2结束"+current_date);
    Flag_White2_Finish=true;
}
/*====================================================================
* 函数：White_Thread3
* 功能：白底子线程3，包含缺陷算法有暗区，灯眼，少料，白印
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::White_Thread3(QList <Mat> img_white3)
{  
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("进入白底子线程3"+current_date);

    Mat white_abshow1=img_white3.at(0);
    Mat mainfilter=img_white3.at(1);
    Mat leftfilter=img_white3.at(2);
    Mat rightfilter=img_white3.at(3);
    Mat ceguangleft=img_white3.at(4);
    Mat ceguangright=img_white3.at(5);
    result_white3=false;
    result_white3=white_defect3(white_abshow1,mainfilter,leftfilter,rightfilter,ceguangleft,ceguangright,1);

    current_date_time =QDateTime::currentDateTime();
    current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("白底子线程3结束"+current_date);
    Flag_White3_Finish=true;
}

/*====================================================================
* 函数：White_Thread3
* 功能：白底子线程3，包含缺陷算法有
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::White1_Thread_Finish()
{
    Flag_White1_Finish=true;
}

/*====================================================================
* 函数：White_Thread3
* 功能：白底子线程3，包含缺陷算法有
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::White2_Thread_Finish()
{
    Flag_White2_Finish=true;
}

/*====================================================================
* 函数：White_Thread3
* 功能：白底子线程3，包含缺陷算法有
* 输入：
* 输出：
* 其他：
======================================================================*/
void MainWindow::White3_Thread_Finish()
{
    Flag_White3_Finish=true;
}
/*=========================================================
* 函 数 名: 相机拍摄图像显示槽函数
* 功能描述:
* 输入：
=========================================================*/
void MainWindow::display()
{

    MV_CC_Display(camera->m_hDevHandle_1,appwnd1);
    MV_CC_Display(camera->m_hDevHandle_2,appwnd2);
    MV_CC_Display(camera->m_hDevHandle_3,appwnd3);
//    MV_CC_Display(camera->m_hDevHandle_4,appwnd4);
//    MV_CC_Display(camera->m_hDevHandle_5,appwnd5);
//    MV_CC_Display(camera->m_hDevHandle_6,appwnd6);
}

/*=========================================================
* 函 数 名: updata_database
* 功能描述: 数据库信息更新
* 输入：
=========================================================*/
void MainWindow::updata_database(QString defect_type)
 {

    qDebug()<<"缺陷类型"<<defect_type<<endl;
    db.transaction();   //开启Sqlite启动事务
    QSqlQuery query(db);

    QString productionName=product_Type+product_Lot;
    if(defect_type=="合格")
    {
        QString sq0=QStringLiteral("SELECT 缺陷编号 from defect_type where 缺陷名称='%1'").arg(defect_type);
        query.exec(sq0);
        query.next();
        QString defect_code=query.value(0).toString();
        qDebug()<<sq0<<endl;
        qDebug()<<defect_code<<endl;
        QString sq1=QStringLiteral("insert into '%1'(样本编号,生产批次,生产批次编码,产品型号,产品型号编码,是否缺陷,缺陷类型名称,缺陷类型编码,是否保存样本) values('%2', '%3','%4','%5','%6','%7','%8','%9','%10')").arg(productionName).
                arg(test_num).arg(product_Customize_Lot).arg(product_Lot).arg(Pro_Model_Mod).arg(Model_Code).arg("否").arg(defect_type).arg(defect_code).arg("否");
        qDebug()<<sq1<<endl;
        query.exec(sq1);
        QString sq3=QStringLiteral("update history_batch set 良品总数='%1' where 生产批次编码='%2'").arg(qualified_num).arg(product_Lot);
        query.exec(sq3);
    }
    else
    {
        QString sq3=QStringLiteral("update history_batch set 缺陷总数='%1' where 生产批次编码='%2'").arg((test_num-qualified_num)).arg(product_Lot);
        query.exec(sq3);
    }
    if(result_white1==true)//白底线程1下存在缺陷
    {
        QString sq0=QStringLiteral("SELECT 缺陷编号 from defect_type where 缺陷名称='%1'").arg(causeColor_1_white);
        query.exec(sq0);
        query.next();
        QString defect_code=query.value(0).toString();
        qDebug()<<sq0<<endl;
        qDebug()<<defect_code<<endl;
        QString sq1=QStringLiteral("insert into '%1'(样本编号,生产批次,生产批次编码,产品型号,产品型号编码,是否缺陷,缺陷类型名称,缺陷类型编码,是否保存样本) values('%2', '%3','%4','%5','%6','%7','%8','%9','%10')").arg(productionName).
                arg(test_num).arg(product_Customize_Lot).arg(product_Lot).arg(Pro_Model_Mod).arg(Model_Code).arg("是").arg(causeColor_1_white).arg(defect_code).arg("否");
        qDebug()<<sq1<<endl;
        query.exec(sq1);

    }
    else if(result_white2==true) //白底线程2下存在缺陷
    {
        QString sq0=QStringLiteral("SELECT 缺陷编号 from defect_type where 缺陷名称='%1'").arg(causeColor_2_white);
        query.exec(sq0);
        query.next();
        QString defect_code=query.value(0).toString();
        qDebug()<<sq0<<endl;
        qDebug()<<defect_code<<endl;
        QString sq1=QStringLiteral("insert into '%1'(样本编号,生产批次,生产批次编码,产品型号,产品型号编码,是否缺陷,缺陷类型名称,缺陷类型编码,是否保存样本) values('%2', '%3','%4','%5','%6','%7','%8','%9','%10')").arg(productionName).
                arg(test_num).arg(product_Customize_Lot).arg(product_Lot).arg(Pro_Model_Mod).arg(Model_Code).arg("是").arg(causeColor_2_white).arg(defect_code).arg("否");
        qDebug()<<sq1<<endl;
        query.exec(sq1);
    }
    if(result_white3==true) //白底线程3下存在缺陷
    {
        QString sq0=QStringLiteral("SELECT 缺陷编号 from defect_type where 缺陷名称='%1'").arg(causeColor_3_white);
        query.exec(sq0);
        query.next();
        QString defect_code=query.value(0).toString();
        qDebug()<<sq0<<endl;
        qDebug()<<defect_code<<endl;
        QString sq1=QStringLiteral("insert into '%1'(样本编号,生产批次,生产批次编码,产品型号,产品型号编码,是否缺陷,缺陷类型名称,缺陷类型编码,是否保存样本) values('%2', '%3','%4','%5','%6','%7','%8','%9','%10')").arg(productionName).
                arg(test_num).arg(product_Customize_Lot).arg(product_Lot).arg(Pro_Model_Mod).arg(Model_Code).arg("是").arg(causeColor_3_white).arg(defect_code).arg("否");
        qDebug()<<sq1<<endl;
        query.exec(sq1);
    }

    QString sq2=QStringLiteral("update history_batch set 测试总数='%1' where 生产批次编码='%2'").arg(test_num).arg(product_Lot);
    query.exec(sq2);

    QString sq5=QStringLiteral("update history_batch set 良品率='%0' where 生产批次编码='%1'").arg(pass_per).arg(product_Lot);
    query.exec(sq5);

    bool update_signal=db.commit();//提交事务执行SQL语句
    if(!update_signal)
    {
        QMessageBox::warning(this,tr("警告"),tr("数据库参数更新失败!"),QMessageBox::Yes,QMessageBox::No);
    }
 }

/*====================================================================
* 类型：MatToQImage图像格式转换
* 功能：
* 输入：
* 输出：
* 其他：
======================================================================*/
QImage MainWindow::MatToQImage(const cv::Mat& mat)
{
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    else if(mat.type() == CV_8UC3)
    {
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void MainWindow::on_pushButton_8_clicked()//单步调试
{
}

void MainWindow::on_pushButton_9_clicked()
{
//    parameter* d=new parameter();
//    d->show();
}

void MainWindow::on_action_PLC_triggered()//断开PLC
{
    modbusDevice->disconnectDevice();//断开PLC的连接
}

void MainWindow::on_pushButton_10_clicked()
{
    MV_CC_SetFloatValue(camera->m_hDevHandle_1,"ExposureTime", Num_1_ceLL_Exposure_white);
    MV_CC_SetFloatValue(camera->m_hDevHandle_3,"ExposureTime", Num_1_ceLR_Exposure_white);
}

void MainWindow::on_label_10_linkActivated(const QString &link)
{

}

/*=========================================================
* 函 数 名: closeEvent
* 功能描述: 系统关闭提示
* 输入：
=========================================================*/
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox *messageBox = new QMessageBox(this);
    messageBox->setStyleSheet("font: 9pt;background-color:rgb(230 ,230, 230)");
    messageBox->setIcon(QMessageBox::Warning);
    messageBox->setWindowTitle("提示");
    messageBox->setText("确定退出系统？");
    messageBox->addButton("取消", QMessageBox::RejectRole); //自定义取消按钮
    messageBox->addButton("确定", QMessageBox::AcceptRole); //自定义确定按钮
    if(messageBox->exec() == QDialog::Accepted)
    {
        if(modbusDevice)
            modbusDevice->disconnect();
        //关闭6个相机
        camera->Find_Device();
        if(camera->m_hDevHandle_1)
            camera->closeCamera(camera->m_hDevHandle_1);
        if(camera->m_hDevHandle_2)
            camera->closeCamera(camera->m_hDevHandle_2);
        if(camera->m_hDevHandle_3)
            camera->closeCamera(camera->m_hDevHandle_3);

        this->destroy(true,true);
    }
    else
    {
        event->ignore();
    }
}

/*=========================================================
* 函 数 名: 工具栏系统参数配置
* 功能描述:
* 输入：
=========================================================*/
void MainWindow::on_action_13_triggered()
{
    if(ui->label_9->text()=="正在检测")
    {
        QMessageBox box(QMessageBox::Warning,"提示","正在检测，参数不可修改！");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.setIcon(QMessageBox::Warning);
        box.show();
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
    }
    else if(ui->label_9->text()=="停止检测"&&parameter_control_show==false)
    {
        parameter_control* d=new parameter_control();
        //d->setWindowModality(Qt::ApplicationModal);
        d->setWindowFlags(Qt::WindowStaysOnTopHint);
        d->show();
        parameter_control_show=true;
    }

}

/*=========================================================
* 函 数 名: 菜单栏系统参数配置
* 功能描述:
* 输入：
=========================================================*/
void MainWindow::on_action_10_triggered()
{
    if(ui->label_9->text()=="正在检测")
    {
        QMessageBox box(QMessageBox::Warning,"提示","正在检测，参数不可修改！");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.setIcon(QMessageBox::Warning);
        box.show();
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
    }
    else if(ui->label_9->text()=="停止检测"&&parameter_control_show==false)
    {
        parameter_control* d=new parameter_control();
        //d->setWindowModality(Qt::ApplicationModal);
        d->setWindowFlags(Qt::WindowStaysOnTopHint);
        d->show();
        parameter_control_show=true;
    }
}

/*=========================================================
* 函 数 名: 系统运行启动
* 功能描述:
* 输入：
=========================================================*/
void MainWindow::on_action_5_triggered()
{
    if(ui->action_5->text()=="运行")
    {
        if(product_Type!=""&&product_Lot!=""&&paraConfigOk=="Y")//产品型号和批次号非空才可进行测试
        {
            if(Flag_First_Func==true)
            {
                Flag_First_Func=false;
                InitTimer();
            }
            else
            {
                Restart_Timer();
            }
            F_start=true;
            F=true;
            First_PLC_Result=true;
            Flag_First_Photo=true;
            Flag_Dec_Complete=false;
            ui->action_5->setText("停止");
            QIcon icon(":///resourse/tingzhi.png");
            ui->action_5->setIcon(icon);
            ui->label_9->setText("正在检测");
            ui->label_9->setStyleSheet("color:red;font: 14pt;font: 黑体;");
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"严重警告","请输入批次号和产品型号，初始化产品型号参数！");
            box.setWindowIcon(QIcon(":/resourse/xitong.png"));
            box.setIcon(QMessageBox::Warning);
            box.show();
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
        }
     }
    else
    {
        if(m_timer.isActive())
        {
            m_timer.stop();
        }
        F_start=false;
        F=false;
        ui->action_5->setText("运行");

        QIcon icon(":///resourse/yunhang-.png");
        ui->action_5->setIcon(icon);
        ui->label_9->setText("停止检测");
        ui->label_9->setStyleSheet("color:yellow;font: 14pt;font: 黑体;");
    }
}

/*=========================================================
* 函 数 名: 菜单栏产品型号配置
* 功能描述:
* 输入：
=========================================================*/
void MainWindow::on_action_7_triggered()
{
    if(ui->label_9->text()=="正在检测")
    {
        QMessageBox box(QMessageBox::Warning,"提示","正在检测，型号不可修改！");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.setIcon(QMessageBox::Warning);
        box.show();
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
    }
    else if(ui->label_9->text()=="停止检测"&&promodel_show==false)
    {
        promodel* d=new promodel();
        d->setWindowTitle("产品型号配置");
        //d->setWindowModality(Qt::ApplicationModal);
        d->show();
        promodel_show=true;
    }
}

/*=========================================================
* 函 数 名: 工具栏产品型号配置
* 功能描述:
* 输入：
=========================================================*/
void MainWindow::on_action_12_triggered()
{
    if(ui->label_9->text()=="正在检测")
    {
        QMessageBox box(QMessageBox::Warning,"提示","正在检测，型号不可修改！");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.setIcon(QMessageBox::Warning);
        box.show();
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
    }
    else if(ui->label_9->text()=="停止检测"&&promodel_show==false)
    {
        promodel* d=new promodel();
        d->setWindowTitle("产品型号配置");
        //d->setWindowFlags(Qt::WindowStaysOnTopHint);
        //d->setWindowModality(Qt::ApplicationModal);
        d->show();
        promodel_show=true;
    }

}

/*=========================================================
* 函 数 名: 工具栏批次配置
* 功能描述:
* 输入：
=========================================================*/
void MainWindow::on_action_11_triggered()
{
    //点击批次设置在log日志中标记
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
    debug_msg("点击批次设置"+current_date);
    if(ui->label_9->text()=="正在检测")
    {
        QMessageBox box(QMessageBox::Warning,"提示","正在检测，批次不可修改！");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.setIcon(QMessageBox::Warning);
        box.show();
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
    }
    else if(ui->label_9->text()=="停止检测"&&production_lot_information_win_show==false)
    {
        production_lot_information_win* d=new production_lot_information_win();
        connect(d,SIGNAL(infoSend(QString,QString)) ,this,SLOT(receiveData(QString,QString)));
        d->setWindowModality(Qt::ApplicationModal);
        d->show();
        production_lot_information_win_show=true;
    }
}

/*=========================================================
* 函 数 名: 菜单栏批次配置
* 功能描述:
* 输入：
=========================================================*/
void MainWindow::on_action_6_triggered()
{
    if(ui->label_9->text()=="正在检测")
    {
        QMessageBox box(QMessageBox::Warning,"提示","正在检测，批次不可修改！");
        box.setWindowIcon(QIcon(":/resource/tishi.png"));
        box.setIcon(QMessageBox::Warning);
        box.show();
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText((QMessageBox::Ok),QString("确定"));
        box.exec();
    }
    else if(ui->label_9->text()=="停止检测"&&production_lot_information_win_show==false)
    {
        production_lot_information_win* d=new production_lot_information_win();
        connect(d,SIGNAL(infoSend(QString,QString)) ,this,SLOT(receiveData(QString,QString)));
        d->setWindowModality(Qt::ApplicationModal);
        d->show();
        production_lot_information_win_show=true;
    }
}

/*=========================================================
* 函 数 名: 菜单栏相机配置
* 功能描述:
* 输入：
=========================================================*/
void MainWindow::on_action_triggered()
{
    if(ui->label_9->text()=="正在检测")
    {
        QMessageBox box(QMessageBox::Warning,"提示","正在检测，相机参数不可修改！");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.setIcon(QMessageBox::Warning);
        box.show();
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
    }
    else if(ui->label_9->text()=="停止检测"&&Form_Camera_Show==false)
    {
        Form_Camera* d=new Form_Camera();
        d->setWindowModality(Qt::ApplicationModal);
        d->show();
        Form_Camera_Show=true;
    }
}

/*=========================================================
* 函 数 名: 工具栏相机配置
* 功能描述:
* 输入：
=========================================================*/
void MainWindow::on_action_8_triggered()
{
    if(ui->label_9->text()=="正在检测")
    {
        QMessageBox box(QMessageBox::Warning,"提示","正在检测，相机参数不可修改！");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.setIcon(QMessageBox::Warning);
        box.show();
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
    }
    else if(ui->label_9->text()=="停止检测"&&Form_Camera_Show==false)
    {
        Form_Camera* d=new Form_Camera();
        d->setWindowModality(Qt::ApplicationModal);
        d->show();
        Form_Camera_Show=true;
    }
}

/*=========================================================
* 函 数 名: on_action_9_triggered
* 功能描述: 缺陷信息统计
* 输入：
=========================================================*/
void MainWindow::on_action_9_triggered()
{
    if(datastatistics_win_show==false)
    {
        datastatistics_win *d=new datastatistics_win();
        d->show();
        datastatistics_win_show=true;
    }
}

/*=========================================================
* 函 数 名: on_action_15_triggered
* 功能描述: 缺陷信息统计
* 输入：
=========================================================*/
void MainWindow::on_action_15_triggered()
{
    if(datastatistics_win_show==false)
    {
        datastatistics_win *d=new datastatistics_win();
        d->show();
        datastatistics_win_show=true;
    }

}
/*=========================================================
* 函 数 名: on_action_17_triggered
* 功能描述: 保存设置
* 输入：
=========================================================*/
void MainWindow::on_action_17_triggered()
{
    if(ui->label_9->text()=="正在检测")
    {
        QMessageBox box(QMessageBox::Warning,"提示","正在检测，保存设置不可修改！");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.setIcon(QMessageBox::Warning);
        box.show();
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
    }
    else if(ui->label_9->text()=="停止检测"&&save_images_show==false)
    {
        save_images *d=new save_images();
        d->setWindowModality(Qt::ApplicationModal);
        d->show();
        save_images_show=true;
    }
}

void MainWindow::on_action_18_triggered()
{
    Detector_Choose* dc=new Detector_Choose();
    dc->setModal(true);
    dc->show();
}

void MainWindow::ini_state()
{

    QSqlQuery query(db);
    QString sql="SELECT 是否检测 FROM defect_type";
    bool signal=query.exec(sql);                                //执行
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "获取缺陷检测状态错误!");
    }
    else
    {
        QList<QString> list; //创建取结果的列表
        while(query.next())
        {
            QString parameter = query.value("是否检测").toString();
            list.append(parameter);
        }
        if(list.at(0)=="0")  //背光异物
            ForeignBody_Checked=false;
        else
            ForeignBody_Checked=true;
        if(list.at(1)=="0")  //少料
            LessMaterial_Checked=false;
        else
            LessMaterial_Checked=true;
        if(list.at(2)=="0")  //十字
            CrossLine_Checked=false;
        else
             CrossLine_Checked=true;
        if(list.at(3)=="0")  //爆灯
            boom_light_Checked=false;
        else
            boom_light_Checked=true;
        if(list.at(4)=="0")  //边框
            Frame_Checked=false;
        else
            Frame_Checked=true;
        if(list.at(5)=="0")  //白印-膜拱-白团
            mura_Checked=false;
        else
            mura_Checked=true;
        if(list.at(6)=="0")  //灯眼
            LampEye_Checked=false;
        else
            LampEye_Checked=true;
        if(list.at(7)=="0")  //亮边
            Brightedge_Checked=false;
        else
            Brightedge_Checked=true;
        if(list.at(8)=="0")  //白点
            WhiteDot_Checked=false;
        else
            WhiteDot_Checked=true;
        if(list.at(9)=="0")  //划伤
            Scratch_Checked=false;
        else
            Scratch_Checked=true;
        if(list.at(10)=="0")  //显异-暗区
            Abshow_White_Detect_Checked=false;
        else
            Abshow_White_Detect_Checked=true;
        if(list.at(11)=="0")  //暗角
            DarkCorner_Checked=false;
        else
            DarkCorner_Checked=true;
        if(list.at(12)=="0")  //色差
            SunSpots_Checked=false;
        else
            SunSpots_Checked=true;
        if(list.at(13)=="0")  //漏光
            Light_leakage_Checked=false;
        else
            Light_leakage_Checked=true;
    }

}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox->currentText()=="离线")
    {
        Flag_Running_State="Offline";
        QString sq1=QStringLiteral("UPDATE Running_State SET run_state ='%1'").arg("离线");
        QSqlQuery query1(db);
        query1.exec(sq1);
    }
    else if(ui->comboBox->currentText()=="在线")
    {
        Flag_Running_State="Online";
        QString sq1=QStringLiteral("UPDATE Running_State SET run_state ='%1'").arg("在线");
        QSqlQuery query1(db);
        query1.exec(sq1);
    }
}


//检测数据查询功能
void MainWindow::on_action_14_triggered()
{
    history_data *hd=new history_data();
    hd->show();
}

void MainWindow::on_action_clear_triggered()
{
    if(ui->label_9->text()=="正在检测")
    {
        QMessageBox box(QMessageBox::Warning,"提示","正在检测，无法清除数据！");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.setIcon(QMessageBox::Warning);
        box.show();
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
    }
    else if(ui->label_9->text()=="停止检测")
    {
        QMessageBox box(QMessageBox::Warning,"提示","是否确定清除该批次测试数据？");
        box.setWindowIcon(QIcon(":/resourse/xitong.png"));
        box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.setButtonText (QMessageBox::Cancel,QString("取 消"));
        int ok=box.exec();
        if(ok == QMessageBox::Ok)//确认修改参数
        {
        QSqlQuery query(db);
        QString sq1=QStringLiteral("update history_batch set 测试总数='0' where 生产批次编码='%1'").arg(product_Lot);
        query.exec(sq1);

        sq1=QStringLiteral("update history_batch set 良品总数='0' where 生产批次编码='%1'").arg(product_Lot);
        query.exec(sq1);

        sq1=QStringLiteral("update history_batch set 缺陷总数='0' where 生产批次编码='%1'").arg(product_Lot);
        query.exec(sq1);

        sq1=QStringLiteral("update history_batch set 良品率='0.0' where 生产批次编码='%1'").arg(product_Lot);
        query.exec(sq1);

        sq1=QStringLiteral("delete from '%1'").arg(product_Type+product_Lot);
        query.exec(sq1);

        sq1=QStringLiteral("VACUUM");
        query.exec(sq1);

        test_num = 0;
        qualified_num = 0;
        pass_per = 0;
        yiwu_num = 0;
        white_num = 0;
        mura_num = 0;
        lackline_num = 0;
        scratch_num = 0;
        dengyan_num = 0;
        liangbian_num=0;
        biankuang_num=0;
        anjiao_num=0;
        shaoliao_num=0;
        baodeng_num=0;
        wuxian_num = 0;

        light_num = 0.0;//可以点亮的屏幕的数量
        Average_mainlight = 0.0;  //主相机平均亮度
        Average_rightlight = 0.0;//右相机平均亮度
        Average_leftlight = 0.0;//左相机平均亮度
        Max_mainlight = 0.0;
        Min_mainlight = 0.0;
        Max_rightlight = 0.0;
        Min_rightlight = 0.0;
        Max_leftlight = 0.0;
        Min_leftlight = 0.0;

        ui->label_48->setText("0");
        ui->label_50->setText("0");
        ui->label_52->setText("0");
        ui->label_34->setText("0");
        ui->label_36->setText("0");
        ui->label_38->setText("0");
        ui->label_40->setText("0");
        ui->label_42->setText("0");
        ui->label_44->setText("0");
//        ui->label_46->setText("0");
//        ui->label_54->setText("0");
//        ui->label_63->setText("0");
        ui->label_65->setText("0");
        ui->label_69->setText("0");
        ui->label_71->setText("0");
        ui->label_101->setText("0");
        ui->label_86->setText("0");
        ui->label_15->setText("0");
        ui->label_83->setText("0");
        ui->label_82->setText("0");
        ui->label_85->setText("0");
        ui->label_84->setText("0");
        ui->label_102->setText("0");
        ui->label_103->setText("0");
        ui->label_57->setText("");
        ui->label_10->setText("");
        ui->label_14->setText("");
        }
    }

}
