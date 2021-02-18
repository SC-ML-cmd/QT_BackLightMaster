#include "camera.h"
#include "ui_form_camera.h"
#include "mainwindow.h"
#include "QString"
#include<QDebug>
#include<QtConcurrent>
#include "mainwindow.h"
#include"global_variable.h"
#include"math.h"
#include"camera_set_gray.h"
#include"parameter_control.h"
HWND appwnd1;
HWND appwnd2;
HWND appwnd3;
HWND appwnd4;
HWND appwnd5;
HWND appwnd6;

bool Flag_ROI_Finished=false;;
bool Flag_adap;
bool Flag_Manual;
int Set_exposure_value=0;
int Set_exposure_value_white=160;
int Set_exposure_value_black=18;
int Set_exposure_value_gray=45;
int Set_exposure_value_sidelight=55;

QString Product_type_Selected;
QString Product_mode_Selected;
bool Flag_white_background=false;
int index;
void* camera_name;      //当前调整相机的名字
double exposure_value;  //算法输出曝光值
double exposure_value_last;  //上一周期算法输出曝光值
double Mean_gray_value;  //ROI平均灰度
double Mean_gray_value_last;
double Range_Error=10;   //灰度的允许误差范围
int Adaptive_Num;     //调整次数

Mat M_L;  //左相机透视变换矩阵
Mat M_R;  //右相机透视变换矩阵
Mat M_white_black;  //主相机黑白透视变换矩阵
Mat M_front;  //前相机透视变换矩阵
Mat M_back;     //后相机透视变换矩阵
Mat M_color;     //主相机彩色透视变换矩阵
Mat Mask_L;
Mat Mask_R;
Mat Mask_F;
Mat Mask_B;
Mat Mask_MW;
Mat Mask_MC;
Mat M_L_E;
Mat M_R_E;

bool Mask_L_Get;
bool Mask_R_Get;
bool Mask_F_Get;
bool Mask_B_Get;
bool Mask_MW_Get;
bool Mask_MC_Get;

double E;
double Ec;
double Esk;
double LastE;
double Kp=5;
double Ki=30;
double Kd=0;
double P_Weight=10;
double I_Weight=10;
double D_Weight=10;
double Low_Limit=-2000000;
double High_Limit=2000000;


int Num_background;
double Set_exposure_value_all[6][4];

double Left_exposure;
double Right_exposure;
double delay_time;    //相机曝光起作用的延时时间

bool refresh_delay_time;
HikvisionSDK *HikvisionSDK::m_pInstance = NULL;
Form_Camera::Form_Camera(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Camera)
{
    ui->setupUi(this);
    ui->pushButton_Add_View_1->setEnabled(0);
    ui->pushButton_Add_View_2->setEnabled(0);
    ui->pushButton_Add_View_3->setEnabled(0);
    ui->pushButton_Add_View_4->setEnabled(0);
    ui->pushButton_Add_View_5->setEnabled(0);
    ui->pushButton_Add_View_6->setEnabled(0);
    ui->spinBox_1->setMinimum(100);
    ui->spinBox_1->setMaximum(1000000);
    ui->horizontalSlider->setMinimum(100);
    ui->horizontalSlider->setMaximum(1000000);
    ui->spinBox_1->setValue(0);
    ui->horizontalSlider->setValue(0);
    ui->spinBox_2->setMinimum(100);
    ui->spinBox_2->setMaximum(1000000);
    ui->horizontalSlider_2->setMinimum(100);
    ui->horizontalSlider_2->setMaximum(1000000);
    ui->spinBox_2->setValue(0);
    ui->horizontalSlider_2->setValue(0);
    ui->spinBox_3->setMinimum(100);
    ui->spinBox_3->setMaximum(1000000);
    ui->horizontalSlider_3->setMinimum(100);
    ui->horizontalSlider_3->setMaximum(1000000);
    ui->spinBox_3->setValue(0);
    ui->horizontalSlider_3->setValue(0);
    ui->spinBox_4->setMinimum(100);
    ui->spinBox_4->setMaximum(1000000);
    ui->horizontalSlider_4->setMinimum(100);
    ui->horizontalSlider_4->setMaximum(1000000);
    ui->spinBox_4->setValue(0);
    ui->horizontalSlider_4->setValue(0);
    ui->spinBox_5->setMinimum(100);
    ui->spinBox_5->setMaximum(1000000);
    ui->horizontalSlider_5->setMinimum(100);
    ui->horizontalSlider_5->setMaximum(1000000);
    ui->spinBox_5->setValue(0);
    ui->horizontalSlider_5->setValue(0);
    ui->spinBox_6->setMinimum(100);
    ui->spinBox_6->setMaximum(1000000);
    ui->horizontalSlider_6->setMinimum(100);
    ui->horizontalSlider_6->setMaximum(1000000);
    ui->spinBox_6->setValue(0);
    ui->horizontalSlider_6->setValue(0);

    ui->comboBox_Adaptive_exposure_list->addItem("白底曝光");
    ui->comboBox_Adaptive_exposure_list->addItem("黑底曝光");
    ui->comboBox_Adaptive_exposure_list->addItem("灰底曝光");
    ui->comboBox_Adaptive_exposure_list->addItem("侧光灯曝光");
    ui->comboBox_product_type->addItem("矩形屏");
    ui->comboBox_product_type->addItem("R角水滴屏");

    Flag_adap=false;
    Flag_Manual=false;

    QSqlQuery query=QSqlQuery(db);
    QString sql="SELECT 白底曝光灰度设定,黑底曝光灰度设定,灰底曝光灰度设定,侧光灯曝光灰度设定 FROM camera_grayvalue_set";
    query.exec(sql);
    for(int i=0;i<6;i++)
    {
        query.next();
        Set_exposure_value_all[i][0]=query.value(0).toDouble();
        Set_exposure_value_all[i][1]=query.value(1).toDouble();
        Set_exposure_value_all[i][2]=query.value(2).toDouble();
        Set_exposure_value_all[i][3]=query.value(3).toDouble();
    }

    QString sql1="SELECT 产品型号 FROM product_model";
    query.exec(sql1);
    while (query.next())
    {
        ui->comboBox_Pro_Mode->addItem(query.value(0).toString());
    }

    ui->comboBox_Pro_Mode->setCurrentIndex(-1);
    ui->comboBox_product_type->setCurrentIndex(-1);
    ui->comboBox_Adaptive_exposure_list->setCurrentIndex(-1);

    ui->pushButton_find_product->setEnabled(false);
    ui->pushButton_find_product->setVisible(false);
    connect(this,SIGNAL(send()),this,SLOT(on_pushButton_3_clicked()));
}

Form_Camera::~Form_Camera()
{
    delete ui;
}

void Form_Camera::on_pushButton_clicked()//查找设备
{
    camera->Find_Device();
    for(unsigned int index=0;index<camera->m_stDevList.nDeviceNum;index++)
    {

        sprintf_s(strUserName, "%s",camera->m_stDevList.pDeviceInfo[index]->SpecialInfo.stGigEInfo.chSerialNumber);
        QString qStr(strUserName);
        QString num = QString("%1").arg(index);
        ui->comboBox_Device_List->addItem("第"+num+"个相机："+qStr);
    }
//    ui->comboBox_Adaptive_exposure_list->addItem("白底曝光");
//    ui->comboBox_Adaptive_exposure_list->addItem("黑底曝光");
//    ui->comboBox_Adaptive_exposure_list->addItem("灰底曝光");
//    ui->comboBox_Adaptive_exposure_list->addItem("侧光灯曝光");
    ui->pushButton_Add_View_1->setEnabled(1);
    ui->pushButton_Add_View_2->setEnabled(1);
    ui->pushButton_Add_View_3->setEnabled(1);
    ui->pushButton_Add_View_4->setEnabled(1);
    ui->pushButton_Add_View_5->setEnabled(1);
    ui->pushButton_Add_View_6->setEnabled(1);
}

void Form_Camera::on_pushButton_Add_View_1_clicked()//添加相机1
{
    camera->connectCamera(ui->comboBox_Device_List->currentIndex(),&camera->m_hDevHandle_1);
    char SerialNumber[32]={0};
    sprintf_s(SerialNumber, "%s",camera->m_stDevList.pDeviceInfo[ui->comboBox_Device_List->currentIndex()]->SpecialInfo.stGigEInfo.chSerialNumber );
    QString sq=QString("update camera_device set 主彩色相机序列号='%1' where 设备编号='%2'").arg(SerialNumber).arg(0);
    QSqlQuery query(db);
    query.exec(sq);
    camera->startCamera( camera->m_hDevHandle_1);
    MV_CC_SetEnumValue(camera->m_hDevHandle_1, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
    MV_CC_SetEnumValue(camera->m_hDevHandle_1,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_1,"ExposureTime", 10000);
    MV_CC_Display(camera->m_hDevHandle_1,appwnd_1);
    ui->pushButton_Add_View_1->setEnabled(0);
    //ui->comboBox_Device_List->setItemData(0, 0, Qt::UserRole - 1);

}

void Form_Camera::on_pushButton_Remove_View_1_clicked()//关闭相机1
{   //camera->m_hDevHandle=camera->m_hDevHandle_1;
    camera->closeCamera( camera->m_hDevHandle_1);
    ui->label_PreView_1->clear();
    ui->pushButton_Add_View_1->setEnabled(1);
    ui->comboBox_Device_List->setItemData(0, 1, Qt::UserRole - 1);
}

void Form_Camera::on_pushButton_Add_View_2_clicked()//添加相机2
{
    camera->connectCamera(ui->comboBox_Device_List->currentIndex(),&camera->m_hDevHandle_2);
    char SerialNumber[32]={0};
    sprintf_s(SerialNumber, "%s",camera->m_stDevList.pDeviceInfo[ui->comboBox_Device_List->currentIndex()]->SpecialInfo.stGigEInfo.chSerialNumber );
    QString sq=QString("update camera_device set 左彩色相机序列号='%1' where 设备编号='%2'").arg(SerialNumber).arg(0);
    QSqlQuery query(db);
    query.exec(sq);
    MV_CC_SetEnumValue(camera->m_hDevHandle_1, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
    MV_CC_SetEnumValue(camera->m_hDevHandle_1,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_1,"ExposureTime", 10000);
    camera->startCamera(camera->m_hDevHandle_2);
    MV_CC_Display(camera->m_hDevHandle_2,appwnd_2);
    ui->pushButton_Add_View_2->setEnabled(0);

}

void Form_Camera::on_pushButton_Remove_View_2_clicked()//关闭相机2
{
    //camera->m_hDevHandle=camera->m_hDevHandle_2;
    camera->closeCamera(camera->m_hDevHandle_2);
    ui->label_PreView_2->clear();
    ui->pushButton_Add_View_2->setEnabled(1);
}

void Form_Camera::on_pushButton_Add_View_3_clicked()//添加相机3
{   camera->connectCamera(ui->comboBox_Device_List->currentIndex(),&camera->m_hDevHandle_3);
    char SerialNumber[32]={0};
    sprintf_s(SerialNumber, "%s",camera->m_stDevList.pDeviceInfo[ui->comboBox_Device_List->currentIndex()]->SpecialInfo.stGigEInfo.chSerialNumber );
    QString sq=QString("update camera_device set 右彩色相机序列号='%1' where 设备编号='%2'").arg(SerialNumber).arg(0);
    QSqlQuery query(db);
    query.exec(sq);
    camera->connectCamera(ui->comboBox_Device_List->currentIndex(), camera->m_hDevHandle_3);
    camera->startCamera( camera->m_hDevHandle_3);
    MV_CC_Display(camera->m_hDevHandle_3,appwnd_3);
    ui->pushButton_Add_View_3->setEnabled(1);
}

void Form_Camera::on_pushButton_Remove_View_3_clicked()//关闭相机3
{
    camera->closeCamera( camera->m_hDevHandle_3);
    ui->label_PreView_3->clear();
    ui->pushButton_Add_View_3->setEnabled(1);
}

void Form_Camera::on_pushButton_Add_View_4_clicked()
{
//    camera->connectCamera(ui->comboBox_Device_List->currentIndex(),&camera->m_hDevHandle_4);
//    char SerialNumber[32]={0};
//    sprintf_s(SerialNumber, "%s",camera->m_stDevList.pDeviceInfo[ui->comboBox_Device_List->currentIndex()]->SpecialInfo.stGigEInfo.chSerialNumber );
//    QString sq=QString("update camera_device set 二号左侧相机序列号='%1' where 设备编号='%2'").arg(SerialNumber).arg(0);
//    QSqlQuery query(db);
//    query.exec(sq);
//    camera->connectCamera(ui->comboBox_Device_List->currentIndex(), camera->m_hDevHandle_4);
//    camera->startCamera( camera->m_hDevHandle_4);
//    MV_CC_Display(camera->m_hDevHandle_4,appwnd_4);
//    ui->pushButton_Add_View_4->setEnabled(1);
}

void Form_Camera::on_pushButton_Remove_View_4_clicked()
{
//    // camera->m_hDevHandle=camera->m_hDevHandle_4;
//    camera->closeCamera( camera->m_hDevHandle_4);
//    ui->label_PreView_4->clear();
}

void Form_Camera::on_pushButton_Add_View_5_clicked()
{
//    camera->connectCamera(ui->comboBox_Device_List->currentIndex(),&camera->m_hDevHandle_5);
//    char SerialNumber[32]={0};
//    sprintf_s(SerialNumber, "%s",camera->m_stDevList.pDeviceInfo[ui->comboBox_Device_List->currentIndex()]->SpecialInfo.stGigEInfo.chSerialNumber );
//    QString sq=QString("update camera_device set 二号中间相机序列号='%1' where 设备编号='%2'").arg(SerialNumber).arg(0);
//    QSqlQuery query(db);
//    query.exec(sq);
//    camera->connectCamera(ui->comboBox_Device_List->currentIndex(),camera->m_hDevHandle_5);
//    camera->startCamera(camera->m_hDevHandle_5);
//    MV_CC_Display(camera->m_hDevHandle_5,appwnd_5);
//    ui->pushButton_Add_View_5->setEnabled(1);
}

void Form_Camera::on_pushButton_Remove_View_5_clicked()
{
//    //camera->m_hDevHandle=camera->m_hDevHandle_5;
//    camera->closeCamera(camera->m_hDevHandle_5);
//    ui->label_PreView_5->clear();
}

void Form_Camera::on_pushButton_Add_View_6_clicked()
{
//    camera->connectCamera(ui->comboBox_Device_List->currentIndex(),&camera->m_hDevHandle_6);
//    char SerialNumber[32]={0};
//    sprintf_s(SerialNumber, "%s",camera->m_stDevList.pDeviceInfo[ui->comboBox_Device_List->currentIndex()]->SpecialInfo.stGigEInfo.chSerialNumber );
//    QString sq=QString("update camera_device set 二号右侧相机序列号='%1' where 设备编号='%2'").arg(SerialNumber).arg(0);
//    QSqlQuery query(db);
//    query.exec(sq);
//    camera->connectCamera(ui->comboBox_Device_List->currentIndex(), camera->m_hDevHandle_6);
//    camera->startCamera( camera->m_hDevHandle_6);
//    MV_CC_Display(camera->m_hDevHandle_6,appwnd_6);
//    ui->pushButton_Add_View_6->setEnabled(1);
}

void Form_Camera::on_pushButton_Remove_View_6_clicked()
{
//    //camera->m_hDevHandle=camera->m_hDevHandle_6;
//    camera->closeCamera( camera->m_hDevHandle_6);
//    ui->label_PreView_6->clear();
}
void Form_Camera::on_pushButton_Full_Screen_1_clicked()//全屏显示1

{

    if( ui->pushButton_Full_Screen_1->text()=="全屏")
    {

        ui->widget_13->hide();
        ui->widget_15->hide();
        ui->widget_17->hide();
        ui->widget_19->hide();
        ui->widget_21->hide();
        ui->pushButton_Full_Screen_1->setText("还原");
    }
    else
    {
        ui->widget_13->show();
        ui->widget_15->show();
        ui->widget_17->show();
        ui->widget_19->show();
        ui->widget_21->show();
        ui->pushButton_Full_Screen_1->setText("全屏");
    }
}

void Form_Camera::on_pushButton_Full_Screen_2_clicked()//全屏显示2

{


}


void Form_Camera::on_pushButton_Full_Screen_3_clicked()
{


}

void Form_Camera::on_pushButton_Full_Screen_4_clicked()
{

}

void Form_Camera::on_pushButton_Full_Screen_5_clicked()
{

}

void Form_Camera::on_pushButton_Full_Screen_6_clicked()
{

}

void Form_Camera::on_horizontalSlider_sliderMoved(int position)//滑动
{
    ui->spinBox_1->setValue(position);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_1, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_1,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_1, "ExposureTime", position);
    if(Flag_Manual&&!Flag_adap)
    {
        Mean_gray_value=GetMeanGrayValue(1);
        ui->label_c1->setText(QString("%1").arg(Mean_gray_value));
    }
}

void Form_Camera::on_spinBox_1_valueChanged(int arg1)
{
    ui->horizontalSlider->setValue(arg1);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_1, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_1,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_1, "ExposureTime", arg1);
    //如果手动曝光功能开启
    if(Flag_Manual&&!Flag_adap)
    {
        Mean_gray_value=GetMeanGrayValue(1);
        ui->label_c1->setText(QString("%1").arg(Mean_gray_value));
    }
}

void Form_Camera::on_spinBox_2_valueChanged(int arg1)
{
    ui->horizontalSlider_2->setValue(arg1);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_2, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_2,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_2, "ExposureTime", arg1);
    //如果手动曝光功能开启
    if(Flag_Manual&&!Flag_adap)
    {
        Mean_gray_value=GetMeanGrayValue(2);
        ui->label_c2->setText(QString("%1").arg(Mean_gray_value));
    }
}

void Form_Camera::on_horizontalSlider_2_sliderMoved(int position)
{
    ui->spinBox_2->setValue(position);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_2, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_2,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_2, "ExposureTime", position);
    //如果手动曝光功能开启
    if(Flag_Manual&&!Flag_adap)
    {
        Mean_gray_value=GetMeanGrayValue(2);
        ui->label_c2->setText(QString("%1").arg(Mean_gray_value));
    }
}

void Form_Camera::on_horizontalSlider_3_sliderMoved(int position)
{
    ui->spinBox_3->setValue(position);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_3, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_3,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_3, "ExposureTime", position);
    //如果手动曝光功能开启
    if(Flag_Manual&&!Flag_adap)
    {
        Mean_gray_value=GetMeanGrayValue(3);
        ui->label_c3->setText(QString("%1").arg(Mean_gray_value));
    }
}

void Form_Camera::on_spinBox_3_valueChanged(int arg1)
{
    ui->horizontalSlider_3->setValue(arg1);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_3, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_3,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_3, "ExposureTime", arg1);
    //如果手动曝光功能开启
    if(Flag_Manual&&!Flag_adap)
    {
        Mean_gray_value=GetMeanGrayValue(3);
        ui->label_c3->setText(QString("%1").arg(Mean_gray_value));
    }
}

void Form_Camera::on_spinBox_4_valueChanged(int arg1)
{
    ui->horizontalSlider_4->setValue(arg1);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_4, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_4,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_4, "ExposureTime", arg1);
    //如果手动曝光功能开启
    if(Flag_Manual&&!Flag_adap)
    {
       Mean_gray_value=GetMeanGrayValue(4);
        ui->label_c4->setText(QString("%1").arg(Mean_gray_value));
    }
}

void Form_Camera::on_horizontalSlider_4_sliderMoved(int position)
{
    ui->spinBox_4->setValue(position);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_4, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_4,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_4, "ExposureTime", position);
    if(Flag_Manual&&!Flag_adap)
    {
        Mean_gray_value=GetMeanGrayValue(4);
        ui->label_c4->setText(QString("%1").arg(Mean_gray_value));
    }
}
void Form_Camera::on_spinBox_5_valueChanged(int arg1)
{
    ui->horizontalSlider_5->setValue(arg1);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_5, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_5,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_5, "ExposureTime", arg1);
    //如果手动曝光功能开启
    if(Flag_Manual&&!Flag_adap)
    {
        Mean_gray_value=GetMeanGrayValue(5);
        ui->label_c4->setText(QString("%1").arg(Mean_gray_value));
    }
}

void Form_Camera::on_horizontalSlider_5_sliderMoved(int position)
{
    ui->spinBox_5->setValue(position);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_5, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_5,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_5, "ExposureTime", position);
    //如果手动曝光功能开启
    if(Flag_Manual&&!Flag_adap)
    {
        Mean_gray_value=GetMeanGrayValue(5);
        ui->label_c5->setText(QString("%1").arg(Mean_gray_value));
    }
}

void Form_Camera::on_spinBox_6_valueChanged(int arg1)
{
    ui->horizontalSlider_6->setValue(arg1);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_6, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_6,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_6, "ExposureTime", arg1);
    //如果手动曝光功能开启
    if(Flag_Manual&&!Flag_adap)
    {
        Mean_gray_value=GetMeanGrayValue(6);
        ui->label_c6->setText(QString("%1").arg(Mean_gray_value));
    }
}

void Form_Camera::on_horizontalSlider_6_sliderMoved(int position)
{
    ui->spinBox_6->setValue(position);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_6, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_6,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(camera->m_hDevHandle_6, "ExposureTime", position);
    //如果手动曝光功能开启
    if(Flag_Manual&&!Flag_adap)
    {
        Mean_gray_value=GetMeanGrayValue(6);
        ui->label_c6->setText(QString("%1").arg(Mean_gray_value));
    }
}




void Form_Camera::closeEvent(QCloseEvent *event)//关闭事件

{
    QFuture<void> f5 =QtConcurrent::run(this,&Form_Camera::display);
    Form_Camera_Show=false;
    MV_CC_SetFloatValue(camera->m_hDevHandle_1,"ExposureTime", 30000); //左黑白主相机拍摄
    MV_CC_SetFloatValue(camera->m_hDevHandle_2,"ExposureTime", 30000); //主黑白主相机拍摄
    MV_CC_SetFloatValue(camera->m_hDevHandle_3,"ExposureTime", 30000);//右黑白主相机拍摄
    MV_CC_SetFloatValue(camera->m_hDevHandle_4,"ExposureTime", 30000);//前黑白主相机拍摄
    MV_CC_SetFloatValue(camera->m_hDevHandle_5,"ExposureTime", 30000); //后黑白主相机拍摄

}
void Form_Camera::display()
{
    //    MV_CC_Display(camera->m_hDevHandle_1,appwnd1);
    //     MV_CC_Display(camera->m_hDevHandle_2,appwnd2);
    //      MV_CC_Display(camera->m_hDevHandle_3,appwnd3);
    //       MV_CC_Display(camera->m_hDevHandle_4,appwnd4);
    //        MV_CC_Display(camera->m_hDevHandle_5,appwnd5);
    //         MV_CC_Display(camera->m_hDevHandle_6,appwnd6);

}

void delay7(int MSecs)
{
    QTime dieTime= QTime::currentTime().addMSecs(MSecs);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Form_Camera::on_pushButton_8_clicked() //连接相机设备按钮
{
    camera->Find_Device();//查找相机设备列表
    for( int index=0;index<camera->m_stDevList.nDeviceNum;index++)
    {
        camera->connectCamera(index);//根据相机序列号连接相机
    }
    delay7(1000);

    //相机1设置主彩色相机
    MV_CC_SetEnumValue(camera->m_hDevHandle_1, "PixelFormat", PixelType_Gvsp_BayerGR8);
    MV_CC_SetFloatValue(camera->m_hDevHandle_1, "AcquisitionFrameRate", 20);//相机节点名称
    //ch:设置触发模式为off || en:set trigger mode as off
    //连续采集模式：对静态检测可选择，产品连续运动不能给触发信号的可选择
    //软件触发模式：对动态检测可选择，产品连续运动能给触发信号的可选择
    //硬件触发模式：对高速动态检测可选择，产品连续高速运动能给触发信号的可选择
    MV_CC_SetEnumValue(camera->m_hDevHandle_1, "TriggerMode", MV_TRIGGER_MODE_OFF);
    camera->startCamera(camera->m_hDevHandle_1);//开始采集图像
    MV_CC_SetEnumValue(camera->m_hDevHandle_1, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
    MV_CC_SetEnumValue(camera->m_hDevHandle_1,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    //MV_CC_SetFloatValue(camera->m_hDevHandle_1,"ExposureTime", Num_1_ceLL_Exposure_white);
    ui->spinBox_1->setValue(20000);
    HWND appwnd_1;
    appwnd_1 = (HWND)ui->label_PreView_1->winId();
    MV_CC_Display(camera->m_hDevHandle_1,appwnd_1);//显示采集到的图像

    //相机2设置左彩色相机
    MV_CC_SetEnumValue(camera->m_hDevHandle_2, "PixelFormat", PixelType_Gvsp_BayerGR8);
    MV_CC_SetFloatValue(camera->m_hDevHandle_2, "AcquisitionFrameRate", 20);
    MV_CC_SetEnumValue(camera->m_hDevHandle_2, "TriggerMode", MV_TRIGGER_MODE_OFF);
    camera->startCamera( camera->m_hDevHandle_2);
    MV_CC_SetEnumValue(camera->m_hDevHandle_2, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
    MV_CC_SetEnumValue(camera->m_hDevHandle_2,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    //MV_CC_SetFloatValue(camera->m_hDevHandle_2,"ExposureTime", Num_1_L_Exposure_white);
    ui->spinBox_2->setValue(20000);

    //主相机多余的参数设置情况
    MV_CC_SetBoolValue(camera->m_hDevHandle_2, "GammaEnable", 1);
    // MV_CC_SetEnumValue(camera->m_hDevHandle_2, "GammaSelector", MV_GAMMA_SELECTOR_SRGB);
    MV_CC_SetGamma(camera->m_hDevHandle_2, 0.7);

    //设置相机自动白平衡模式
    //MV_BALANCEWHITE_AUTO_OFF         = 0,  //关闭
    // MV_BALANCEWHITE_AUTO_CONTINUOUS  = 1,  //连续
    //MV_BALANCEWHITE_AUTO_ONCE        = 2   //一次
    MV_CC_SetBalanceWhiteAuto(camera->m_hDevHandle_2, MV_BALANCEWHITE_AUTO_OFF);
    //设置相机图像白平衡绿色比例
    MV_CC_SetBalanceRatioGreen(camera->m_hDevHandle_2, 1000);
    //设置相机图像白平衡绿色比例
    MV_CC_SetBalanceRatioRed(camera->m_hDevHandle_2, 1782);
    //设置相机图像白平衡绿色比例
    MV_CC_SetBalanceRatioBlue(camera->m_hDevHandle_2, 1400);
    HWND appwnd_2;
    appwnd_2 = (HWND)ui->label_PreView_2->winId();
    MV_CC_Display(camera->m_hDevHandle_2,appwnd_2);

    //相机3设置右彩色相机
    MV_CC_SetEnumValue(camera->m_hDevHandle_3, "PixelFormat", PixelType_Gvsp_BayerGR8);//彩色相机像素格式：PixelType_Gvsp_BayerGR8
    MV_CC_SetFloatValue(camera->m_hDevHandle_3, "AcquisitionFrameRate", 20);
    MV_CC_SetEnumValue(camera->m_hDevHandle_3, "TriggerMode", MV_TRIGGER_MODE_OFF);
    camera->startCamera( camera->m_hDevHandle_3);
    MV_CC_SetEnumValue(camera->m_hDevHandle_3, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
    MV_CC_SetEnumValue(camera->m_hDevHandle_3,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    //MV_CC_SetFloatValue(camera->m_hDevHandle_3,"ExposureTime", Num_1_ceLR_Exposure_white);
    ui->spinBox_3->setValue(20000);
    HWND appwnd_3;
    appwnd_3 = (HWND)ui->label_PreView_3->winId();
    MV_CC_Display(camera->m_hDevHandle_3,appwnd_3);

//    //相机4设置
//    MV_CC_SetEnumValue(camera->m_hDevHandle_4, "PixelFormat", PixelType_Gvsp_BayerGR8);
//    MV_CC_SetFloatValue(camera->m_hDevHandle_4, "AcquisitionFrameRate", 10);//相机节点名称
//    //ch:设置触发模式为off || en:set trigger mode as off
//    //连续采集模式：对静态检测可选择，产品连续运动不能给触发信号的可选择
//    //软件触发模式：对动态检测可选择，产品连续运动能给触发信号的可选择
//    //硬件触发模式：对高速动态检测可选择，产品连续高速运动能给触发信号的可选择
//    MV_CC_SetEnumValue(camera->m_hDevHandle_4, "TriggerMode", MV_TRIGGER_MODE_OFF);
//    camera->startCamera( camera->m_hDevHandle_4);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_4, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_4,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
//    //MV_CC_SetFloatValue(camera->m_hDevHandle_4,"ExposureTime", Num_1_ceLL_Exposure_white);
//    ui->spinBox_4->setValue(200000);
//    HWND appwnd_4;
//    appwnd_4 = (HWND)ui->label_PreView_4->winId();
//    MV_CC_Display(camera->m_hDevHandle_4,appwnd_4);

//    //相机5设置
//    MV_CC_SetEnumValue(camera->m_hDevHandle_5, "PixelFormat", PixelType_Gvsp_BayerGR8);
//    MV_CC_SetFloatValue(camera->m_hDevHandle_5, "AcquisitionFrameRate", 10);//相机节点名称
//    //ch:设置触发模式为off || en:set trigger mode as off
//    //连续采集模式：对静态检测可选择，产品连续运动不能给触发信号的可选择
//    //软件触发模式：对动态检测可选择，产品连续运动能给触发信号的可选择
//    //硬件触发模式：对高速动态检测可选择，产品连续高速运动能给触发信号的可选择
//    MV_CC_SetEnumValue(camera->m_hDevHandle_5, "TriggerMode", MV_TRIGGER_MODE_OFF);
//    camera->startCamera( camera->m_hDevHandle_5);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_5, "ExposureMode", MV_EXPOSURE_MODE_TIMED);//555
//    MV_CC_SetEnumValue(camera->m_hDevHandle_5,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);//555
//    //MV_CC_SetFloatValue(camera->m_hDevHandle_5,"ExposureTime", Num_1_ceLL_Exposure_white);//3万曝光
//    ui->spinBox_5->setValue(200000);
//    HWND appwnd_5;
//    appwnd_5 = (HWND)ui->label_PreView_5->winId();
//    MV_CC_Display(camera->m_hDevHandle_5,appwnd_5);

//    //相机6设置
//    MV_CC_SetEnumValue(camera->m_hDevHandle_6, "PixelFormat", PixelType_Gvsp_BayerGR8);
//    MV_CC_SetFloatValue(camera->m_hDevHandle_6, "AcquisitionFrameRate", 10);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_6, "TriggerMode", MV_TRIGGER_MODE_OFF);
//    camera->startCamera( camera->m_hDevHandle_6);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_6, "ExposureMode", MV_EXPOSURE_MODE_TIMED);
//    MV_CC_SetEnumValue(camera->m_hDevHandle_6,"ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
//    //MV_CC_SetFloatValue(camera->m_hDevHandle_6,"ExposureTime", Num_1_L_Exposure_white);
//    ui->spinBox_6->setValue(20000);
//    HWND appwnd_6;
//    appwnd_6 = (HWND)ui->label_PreView_6->winId();
//    MV_CC_Display(camera->m_hDevHandle_6,appwnd_6);
}




/*====================================================================
* 功能：分底色自适应曝光调整，目前仅为手动
* 输入：
* 输出：
* 其他：
======================================================================*/
void Form_Camera::on_pushButton_Adaptive_exposure_clicked()
{
    QFuture<void> d1;
    if(!Flag_adap)
    {
        if(ui->comboBox_Pro_Mode->currentIndex()==-1||ui->comboBox_product_type->currentIndex()==-1||ui->comboBox_Adaptive_exposure_list->currentIndex()==-1)
        {
            QString inf;
            if(ui->comboBox_Pro_Mode->currentIndex()==-1)
                inf="请先选择产品型号！";
            else if(ui->comboBox_product_type->currentIndex()==-1)
                inf="请先选择屏幕类型！";
            else if(ui->comboBox_Adaptive_exposure_list->currentIndex()==-1)
                inf="请先选择曝光类型！";
            QMessageBox box(QMessageBox::Warning,"提示",inf);
            box.setStandardButtons (QMessageBox::Ok);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.exec ();
            return;
        }
        if(ui->comboBox_Adaptive_exposure_list->currentText()=="白底曝光")
        {
            Range_Error=10;
            Flag_white_background=true;
            Set_exposure_value=Set_exposure_value_white;
            Num_background=0;
            Mask_L_Get=Mask_B_Get=Mask_F_Get=Mask_R_Get=Mask_MC_Get=Mask_MW_Get=true;
        }
        else if(ui->comboBox_Adaptive_exposure_list->currentText()=="黑底曝光")
        {
            if(!Flag_ROI_Finished)
            {
                QMessageBox box(QMessageBox::Warning,"提示","请先在白底下调整曝光值显示出屏幕轮廓，并先进行白底下自校正！",QMessageBox::Ok);
                box.exec();
                return;
            }
            Range_Error=3;
            Set_exposure_value=Set_exposure_value_black;
            Flag_white_background=false;
            Num_background=1;
        }
        else if(ui->comboBox_Adaptive_exposure_list->currentText()=="灰底曝光")
        {
            if(!Flag_ROI_Finished)
            {
                QMessageBox box(QMessageBox::Warning,"提示","请先在白底下调整曝光值显示出屏幕轮廓，并先进行白底下自校正！",QMessageBox::Ok);
                box.exec();
                return;
            }
            Range_Error=5;
            Set_exposure_value=Set_exposure_value_gray;
            Flag_white_background=false;
            Num_background=2;
        }
        else if(ui->comboBox_Adaptive_exposure_list->currentText()=="侧光灯曝光")
        {
            ui->spinBox_1->setValue(5000);
            ui->spinBox_2->setValue(5000);
            ui->spinBox_3->setValue(5000);
            if(!Flag_ROI_Finished)
            {
                QMessageBox box(QMessageBox::Warning,"提示","请先在白底下调整曝光值显示出屏幕轮廓，并先进行白底下自校正！",QMessageBox::Ok);
                box.exec();
                return;
            }
            Range_Error=5;
            Set_exposure_value=Set_exposure_value_sidelight;
            Flag_white_background=false;
            Num_background=3;
        }

        Product_type_Selected=ui->comboBox_product_type->currentText();
        Flag_adap=true;
        ui->comboBox_Pro_Mode->setEnabled(false);
        ui->comboBox_product_type->setEnabled(false);
        ui->comboBox_Adaptive_exposure_list->setEnabled(false);
        ui->pushButton_Adaptive_exposure->setText("结束自动模式");
        d1=QtConcurrent::run(this,&Form_Camera::Adaptive_exposure);  //开处理线程


    }
    else
    {
        Flag_adap=false;
        d1.cancel();
        Flag_white_background=false;
        ui->pushButton_Adaptive_exposure->setText("自动校正模式");
        if(!Flag_adap&&!Flag_Manual)
        {
            ui->comboBox_Pro_Mode->setEnabled(true);
            ui->comboBox_product_type->setEnabled(true);
            ui->comboBox_Adaptive_exposure_list->setEnabled(true);
        }
    }

}

/*====================================================================
* 功能：六个相机依次自适应曝光调整
* 输入：
* 输出：
* 其他：
======================================================================*/
void Form_Camera::Adaptive_exposure()
{
    pixel_num=3000;
    for(index=0;index<camera->m_stDevList.nDeviceNum;index++)
    {
            Set_exposure_value=(int)Set_exposure_value_all[index][Num_background];
            camera->connectCamera(index);//根据相机序列号连接相机
            if(index==0)
            {
                camera_name=camera->m_hDevHandle_1;
                exposure_value=ui->spinBox_1->value();
                Esk=ui->spinBox_1->value()/Ki/I_Weight;
            }
            if(index==1)
            {
                camera_name=camera->m_hDevHandle_2;
                exposure_value=ui->spinBox_2->value();
                Esk=ui->spinBox_2->value()/Ki/I_Weight;
            }
            if(index==2)
            {
                camera_name=camera->m_hDevHandle_3;
                exposure_value=ui->spinBox_3->value();
                Esk=ui->spinBox_3->value()/Ki/I_Weight;
            }
            Adaptive_exposure_main_ROI(index+1);


            Mean_gray_value=-100;

            Left_exposure=100;
            Right_exposure=1000000;
            exposure_value_last=exposure_value;
            int Num_Wait=0;
            // 控制函数运行
            for(Adaptive_Num=0;qAbs(Mean_gray_value-Set_exposure_value)>Range_Error&&Flag_adap==true;Adaptive_Num++)
            {
                Mean_gray_value=GetMeanGrayValue(index+1);
                if(qAbs(Mean_gray_value-Set_exposure_value)>Range_Error) //精确到灰度小数点后两位不同才认为曝光值改变
                {
                    if(int(Mean_gray_value_last*100)!=int(Mean_gray_value*100)||Num_Wait>=2)  //曝光设定等待时间一般不会超过4s，防止过曝或过暗，背光源曝光值要低
                    {
                        exposure_value=Half_Control(Mean_gray_value,Set_exposure_value,exposure_value);
                        Num_Wait=0;
                    }
                    else
                    {
                        Num_Wait++;
                    }
                }
                delay_time=qAbs(exposure_value)*0.003;
                if(delay_time<=2000) //延时最少2s
                    delay_time=2000;
                ui->pushButton_8->setText(QString("%1").arg(delay_time));
                if(index==0)
                {
                    ui->label_c1->setText(QString("%1").arg(Mean_gray_value));
                    ui->spinBox_1->setValue(int(exposure_value));
                }
                if(index==1)
                {
                    ui->label_c2->setText(QString("%1").arg(Mean_gray_value));
                    ui->spinBox_2->setValue(int(exposure_value));

                }
                if(index==2)
                {
                    ui->label_c3->setText(QString("%1").arg(Mean_gray_value));
                    ui->spinBox_3->setValue(int(exposure_value));
                }
//                if(index==3)
//                {
//                    ui->spinBox_4->setValue(int(exposure_value));
//                    ui->label_c4->setText(QString("%1").arg(Mean_gray_value));
//                }
//                if(index==4)
//                {
//                    ui->spinBox_5->setValue(int(exposure_value));
//                    ui->label_c5->setText(QString("%1").arg(Mean_gray_value));
//                }
//                if(index==5)
//                {
//                    ui->spinBox_6->setValue(int(exposure_value));
//                    ui->label_c6->setText(QString("%1").arg(Mean_gray_value));
//                }
                delay7(int(delay_time));
                exposure_value_last=exposure_value;
                Mean_gray_value_last=Mean_gray_value;
            }
            qDebug()<<"调整次数:"<<Adaptive_Num<<endl;
            qDebug()<<"自适应曝光值:"<<exposure_value<<endl;
    }
    if(camera->m_stDevList.nDeviceNum!=0)
    {
        emit send();
    }
}



/*====================================================================
* 功能：提取透视变换矩阵
* 输入：
* 输出：
* 其他：
======================================================================*/
bool Form_Camera::Adaptive_exposure_main_ROI(int C_index)
{
   Mat M_black_1;
   Mat M_louguang_1;
   Mat M_abshow_white,M_white_abshow;
   Mat toushi_ju;
   QFuture<cv::Mat> c1 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, C_index);
   c1.waitForFinished();  //等待线程关闭
   Mat src_L1=c1.result().clone();

   if(Flag_white_background==true)  //白底下提取ROI，必须先进行白底下自适应曝光
   {
       if(C_index==1)
       {
           //左侧相机黑白ROI提取
           if(Product_type_Selected=="矩形屏")
               roi_ceshi(src_L1,-10,&M_L);
           else if(Product_type_Selected=="R角水滴屏")
               //RoiSide_Arcangle(src_L1, -10, &M_L,&M_L_E);
           f_LeftRightCam_PersTransMatCal(src_L1, &M_L, &M_L_1_E, Product_type_Selected,15);
           //cv::imwrite("D:\\ML.bmp",M_L);
           //ui->spinBox_1->setValue(int(1000));
       }
       else if(C_index==2)
       {
           //主相机黑白ROI提取
           if(Product_type_Selected=="矩形屏")
               roi_white(src_L1,-2,0,70,&M_white_black,&M_black_1,&M_louguang_1,&M_abshow_white,1);
           else if(Product_type_Selected=="R角水滴屏")
               //RoiWhite_Arcangle(src_L1,-2,0,70,&M_white_black,&M_black_1,&M_louguang_1,1);
           f_MainCam_PersTransMatCal(src_L1, 0, 10, &M_white_black, &M_biankuang, &M_white_abshow, 1, Product_type_Selected);
           //ui->spinBox_2->setValue(int(1000));

       }
       else if(C_index==3)
       {
           //右侧相机黑白ROI提取
           if(Product_type_Selected=="矩形屏")
               roi_ceshi(src_L1,-10,&M_R);
           else if(Product_type_Selected=="R角水滴屏")
               //RoiSide_Arcangle(src_L1, -10, &M_R,&M_R_E);
           f_LeftRightCam_PersTransMatCal(src_L1, &M_R, &M_R_E, Product_type_Selected,15);
           Flag_ROI_Finished=true;  //背光源三个相机
           //ui->spinBox_3->setValue(int(1000));
       }
//       else if(C_index==4)
//       {
//           //前侧黑白相机ROI提取
//           if(Product_type_Selected=="矩形屏")
//               roi_front_back(src_L1,3,&M_front);
//           else if(Product_type_Selected=="R角水滴屏")
//               roi_front_back(src_L1,3,&M_front);
//          // ui->spinBox_4->setValue(int(1000));

//       }
//       else if(C_index==5)
//       {
//           //后侧黑白相机ROI提取
//           if(Product_type_Selected=="矩形屏")
//               roi_front_back(src_L1,3,&M_back);
//           else if(Product_type_Selected=="R角水滴屏")
//               roi_front_back(src_L1,3,&M_back);
//           //ui->spinBox_5->setValue(int(1000));

//       }
//       else if(C_index==6)
//       {
//           //彩色主相机ROI提取
//           if(Product_type_Selected=="矩形屏")
//               roi_white_color(src_L1,-1,0,70,&M_color,&M_black_2,&M_louguang_2,1);
//           else if(Product_type_Selected=="R角水滴屏")
//               RoiColor_Arcangle(src_L1,-1,0,70,&M_color,&M_black_2,&M_louguang_2,1);
//          // ui->spinBox_6->setValue(int(1000));
//           Flag_ROI_Finished=true;
//       }

   }
   else
   {
       //其他自适应场景ROI都为白底下提取ROI
   }


}





/*====================================================================
* 功能：积分分离PID
* 输入：
* 输出：
* 其他：
======================================================================*/
double Form_Camera::Adaptive_exposure_PID(double meangray,int Set_gray,double I_limit)
{
    E = Set_gray-meangray;
    if (qAbs(E) < I_limit)
        Esk += E;
    else
        Esk += 0;
        Ec = E - LastE;
        LastE = E;
        double Pout = Kp * E;
        double Iout = Ki * Esk;
        double Dout = Kd * Ec;
        double PID_Out_1 = Pout * P_Weight + Iout * I_Weight + Dout * D_Weight;
        double PID_Out = Constrain_Float(PID_Out_1, Low_Limit, High_Limit);
        return PID_Out;

}
/*====================================================================
* 功能：限幅函数，对PID输出限幅，视情况使用
* 输入：
* 输出：
* 其他：
======================================================================*/
double Form_Camera::Constrain_Float(double amt, double low, double high)
{
    return ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)));
}

/*====================================================================
* 功能：透视变换，得到ROI区域
* 输入：
* 输出：
* 其他：
======================================================================*/
Mat Form_Camera::PerspectiveTransformation(Mat image,int C_index)
{
    Mat result;
    //左黑白相机
    if(C_index==1)
    {
        if(Product_type_Selected=="矩形屏")
        {
            result=toushi_white(image,M_L,-5,3000,1775);
        }
        else if(Product_type_Selected=="R角水滴屏")
        {
            result=toushi_white(image,M_L,-1,3000,1500);
        }
        //cv::imwrite("D:\\result.bmp",result);
    }
    //主黑白相机
    else if(C_index==2)
    {
        if(Product_type_Selected=="矩形屏")
        {
            result=toushi_white(image,M_white_black,-1,3000,1775);
        }
        else if(Product_type_Selected=="R角水滴屏")
        {
            result=toushi_white(image,M_white_black,-1,3000,1500);
        }
    }
    //右黑白相机
    else if(C_index==3)
    {
        if(Product_type_Selected=="矩形屏")
        {
            result=toushi_white(image,M_R,-5,3000,1775);
        }
        else if(Product_type_Selected=="R角水滴屏")
        {
            result=toushi_white(image,M_R,-1,3000,1500);
        }
    }
    //前黑白相机
//    else if(C_index==4)
//    {
//        if(Product_type_Selected=="矩形屏")
//        {
//            result=toushi_white(image,M_front,-5,3000,1775);
//        }
//        else if(Product_type_Selected=="R角水滴屏")
//        {
//            result=toushi_white(image,M_front,-1,3000,1500);
//        }
//    }
//    //后黑白相机
//    else if(C_index==5)
//    {
//        if(Product_type_Selected=="矩形屏")
//        {
//            result=toushi_white(image,M_back,-5,3000,1775);
//        }
//        else if(Product_type_Selected=="R角水滴屏")
//        {
//            result=toushi_white(image,M_back,-1,3000,1500);
//        }
//    }
//    //主彩色相机
//    else if(C_index==6)
//    {
//        if(Product_type_Selected=="矩形屏")
//        {
//            result=toushi_white(image,M_color,-1,3000,1775);
//        }
//        else if(Product_type_Selected=="R角水滴屏")
//        {
//            result=toushi_white(image,M_color,-1,3000,1500);
//        }
//    }
    return result;
}

void Form_Camera::on_pushButton_10_clicked()
{
    Camera_set_gray *csg=new Camera_set_gray();
    csg->setModal(false);
    csg->show();
}
/*====================================================================
* 功能：开启手动曝光功能
* 输入：按钮
* 输出：无
* 其他：
======================================================================*/
void Form_Camera::on_pushButton_manualExposure_clicked()
{
    if(!Flag_Manual)
    {
        if(ui->comboBox_Pro_Mode->currentIndex()==-1||ui->comboBox_product_type->currentIndex()==-1||ui->comboBox_Adaptive_exposure_list->currentIndex()==-1)
        {
            QString inf;
            if(ui->comboBox_Pro_Mode->currentIndex()==-1)
                inf="请先选择产品型号！";
            else if(ui->comboBox_product_type->currentIndex()==-1)
                inf="请先选择屏幕类型！";
            else if(ui->comboBox_Adaptive_exposure_list->currentIndex()==-1)
                inf="请先选择曝光类型！";
            QMessageBox box(QMessageBox::Warning,"提示",inf);
            box.setStandardButtons (QMessageBox::Ok);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.exec ();
            return;
        }
        if(ui->comboBox_Adaptive_exposure_list->currentText()=="白底曝光")
        {
            Flag_white_background=true;
            if(!Flag_adap)
                Mask_L_Get=Mask_B_Get=Mask_F_Get=Mask_R_Get=Mask_MC_Get=Mask_MW_Get=true;
        }
        else
        {
            Flag_white_background=false;
        }
        Product_type_Selected=ui->comboBox_product_type->currentText();
        //获得各个相机的透视变换矩阵
        if(!Flag_adap)
        {
            for(int i=0;i<int(camera->m_stDevList.nDeviceNum);i++)
            {
                camera->connectCamera(i);//根据相机序列号连接相机
                Adaptive_exposure_main_ROI(i+1);
            }

        }
        ui->pushButton_manualExposure->setText("结束手动校正");
        Flag_Manual=true;
        ui->comboBox_Pro_Mode->setEnabled(false);
        ui->comboBox_product_type->setEnabled(false);
        ui->comboBox_Adaptive_exposure_list->setEnabled(false);

    }
    else
    {
        Flag_Manual=false;
        ui->pushButton_manualExposure->setText("手动校正模式");
        if(!Flag_adap&&!Flag_Manual)
        {
            ui->comboBox_Pro_Mode->setEnabled(true);
            ui->comboBox_product_type->setEnabled(true);
            ui->comboBox_Adaptive_exposure_list->setEnabled(true);
        }
    }
}

/*====================================================================
* 功能：手动曝光功能，实时计算提取出图像的灰度均值
* 输入：相机的编号
* 输出：手机屏幕的灰度均值
* 其他：
======================================================================*/
double Form_Camera::GetMeanGrayValue(int cameraNum)
{
    double meanGrayValue=0;
    //开启线程读取相机图片
    QFuture<cv::Mat> c1 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, int(cameraNum));
    c1.waitForFinished();  //等待线程关闭
    Mat image=c1.result().clone();
    //通过透视变换得到ROI区域
    cvtColor(image, image, CV_BGR2GRAY);
    Mat image_ROI =PerspectiveTransformation(image,cameraNum);
    if(Product_type_Selected=="矩形屏")
    {
        //主彩色相机
        if(cameraNum==6)
        {
            Mat result_gray;
            cvtColor(image_ROI, result_gray, CV_BGR2GRAY);
            meanGrayValue=mean(result_gray)[0];
        }
        else
        {//其他黑白相机
            meanGrayValue=mean(image_ROI)[0];
        }
    }
    else if(Product_type_Selected=="R角水滴屏")
    {
        Mat th1;
        th1=image_ROI.clone();
        //R角水滴屏掩膜，lv前缀的意思是局部变量
        Mat lvMask;
        //前后左右黑白相机
        if(cameraNum==1)
        {
            //掩膜只取一次
            if(Mask_L_Get)
            {
                threshold(th1, Mask_L, 20, 255, CV_THRESH_BINARY);
                Mask_L_Get=false;
            }
            meanGrayValue=mean(image_ROI,Mask_L)[0];
        }
        else if(cameraNum==2)
        {
            if(Mask_MW_Get)
            {
                threshold(th1, Mask_MW, 30, 255, CV_THRESH_BINARY);
                Mask_MW_Get=false;
            }
            meanGrayValue=mean(image_ROI,Mask_MW)[0];
        }
        else if(cameraNum==3)
        {
            if(Mask_R_Get)
            {
                threshold(th1, Mask_R, 20, 255, CV_THRESH_BINARY);
                Mask_R_Get=false;
            }
            meanGrayValue=mean(image_ROI,Mask_R)[0];
        }
        else if(cameraNum==4)
        {
            if(Mask_F_Get)
            {
                threshold(th1, Mask_F, 20, 255, CV_THRESH_BINARY);
                Mask_F_Get=false;
            }
            meanGrayValue=mean(image_ROI,Mask_F)[0];
        }
        else if(cameraNum==5)
        {
            if(Mask_B_Get)
            {
                threshold(th1, Mask_B, 20, 255, CV_THRESH_BINARY);
                Mask_B_Get=false;
            }
            meanGrayValue=mean(image_ROI,Mask_B)[0];
        }
        else if(cameraNum==6)
        {
            Mat colorgray;
            cvtColor(th1, colorgray, CV_BGR2GRAY);
            if(Mask_MC_Get)
            {
                threshold(colorgray, Mask_MC, 20, 255, CV_THRESH_BINARY);
                Mask_MC_Get=false;
            }
            meanGrayValue=mean(colorgray,Mask_MC)[0];
        }

//        if(cameraNum==1||cameraNum==3||cameraNum==4||cameraNum==5)
//        {
//            threshold(th1, lvMask, 20, 255, CV_THRESH_BINARY);
//            meanGrayValue=mean(image_ROI,lvMask)[0];
//        }
//        //主黑白相机
//        else if(cameraNum==2)
//        {
//           threshold(th1, lvMask, 30, 255, CV_THRESH_BINARY);
//           meanGrayValue=mean(image_ROI,lvMask)[0];
//        }
//        //主彩色相机
//        else if(cameraNum==6)
//        {
//            Mat colorgray;
//            cvtColor(th1, colorgray, CV_BGR2GRAY);
//            threshold(colorgray, lvMask, 20, 255, CV_THRESH_BINARY);
//            meanGrayValue=mean(colorgray,lvMask)[0];
//        }
    }
    return meanGrayValue;
}
/*====================================================================
* 功能：对半法控制
* 输入：
* 输出：
* 其他：
======================================================================*/
double Form_Camera::Half_Control(double meangray,int Set_gray,double Current)
{
    double result_half;
    if(meangray>=Set_gray)
    {
        Left_exposure=Left_exposure;
        Right_exposure=Current;
    }
    else
    {
        Left_exposure=Current;
        Right_exposure=Right_exposure;
    }
     result_half=(Right_exposure+Left_exposure)/2;
    return result_half;
}

void Form_Camera::on_comboBox_Pro_Mode_currentTextChanged(const QString &arg1)
{
    QSqlQuery query=QSqlQuery(db);
    QString sql=QStringLiteral("SELECT 屏幕类型 FROM product_model WHERE 产品型号='%1'").arg(arg1);
    query.exec(sql);
    query.next();
    ui->comboBox_product_type->setCurrentText(query.value(0).toString());
}

void Form_Camera::on_pushButton_3_clicked()
{
    QSqlQuery query=QSqlQuery(db);
    QString inf=QStringLiteral("是否保存%1在%2下的曝光值？").arg(ui->comboBox_Pro_Mode->currentText()).arg(ui->comboBox_Adaptive_exposure_list->currentText());
    QMessageBox box(QMessageBox::Warning,"提示",inf);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec();
    if(ok == QMessageBox::Ok)//确认修改参数
    {
        QString sq=QStringLiteral("SELECT 产品型号编码 FROM product_model WHERE 产品型号='%1'").arg(ui->comboBox_Pro_Mode->currentText());
        query.exec(sq);
        query.next();
        QString ModeCode=query.value(0).toString();
        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        if(ui->comboBox_Adaptive_exposure_list->currentText()=="白底曝光")
        {
            //相机侧光参数

            //相机白底参数
            QString sq6=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_2->value()).arg("Exposure_White_Black_Middle").arg(ModeCode);
            query.exec(sq6);
            QString sq7=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_6->value()).arg("Exposure_White_Color_Middle").arg(ModeCode);
            query.exec(sq7);
            QString sq8=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_1->value()).arg("Exposure_White_Black_Left").arg(ModeCode);
            query.exec(sq8);
            QString sq9=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_3->value()).arg("Exposure_White_Color_Right").arg(ModeCode);
            query.exec(sq9);
            QString sq10=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_4->value()).arg("Exposure_White_Black_Front").arg(ModeCode);
            query.exec(sq10);
            QString sq11=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_5->value()).arg("Exposure_White_Black_Back").arg(ModeCode);
            query.exec(sq11);

        }
        else if(ui->comboBox_Adaptive_exposure_list->currentText()=="黑底曝光")
            {
            //相机黑底参数
                QString sq12=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_2->value()).arg("Exposure_Black_Middle").arg(ModeCode);
                query.exec(sq12);
                QString sq13=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_6->value()).arg("Exposure_Black_Color").arg(ModeCode);
                query.exec(sq13);

            }
        else if(ui->comboBox_Adaptive_exposure_list->currentText()=="灰底曝光")
        {
            QString sq15=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_2->value()).arg("Exposure_Gray_Main").arg(ModeCode);
            query.exec(sq15);
            QString sq16=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_1->value()).arg("Exposure_Gray_Left").arg(ModeCode);
            query.exec(sq16);
            QString sq17=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_3->value()).arg("Exposure_Gray_Right").arg(ModeCode);
            query.exec(sq17);
        }
        else if(ui->comboBox_Adaptive_exposure_list->currentText()=="侧光灯曝光")
        {
            QString sq1=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_2->value()).arg("Exposure_Ceguang_Main").arg(ModeCode);
            query.exec(sq1);
            QString sq2=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_1->value()).arg("Exposure_Ceguang_Left").arg(ModeCode);
            query.exec(sq2);
            QString sq3=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_3->value()).arg("Exposure_Ceguang_Right").arg(ModeCode);
            query.exec(sq3);
            QString sq4=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_4->value()).arg("Exposure_Ceguang_Front").arg(ModeCode);
            query.exec(sq4);
            QString sq5=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(ui->spinBox_5->value()).arg("Exposure_Ceguang_Back").arg(ModeCode);
            query.exec(sq5);
        }

            bool conmmitsignal=db.commit();//提交事务执行SQL语句
            if(conmmitsignal)
            {
                QMessageBox box(QMessageBox::Warning,"提示","保存成功");
                box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
                box.setButtonText (QMessageBox::Ok,QString("确 定"));
                box.setButtonText (QMessageBox::Cancel,QString("取 消"));
                box.exec ();
            }
            else
            {
                QMessageBox box(QMessageBox::Warning,"提示","保存相机曝光值失败!");
                box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
                box.setButtonText (QMessageBox::Ok,QString("确 定"));
                box.setButtonText (QMessageBox::Cancel,QString("取 消"));
                box.exec ();
                qDebug()<<query.lastError()<<endl;

            }
            qDebug()<<conmmitsignal<<endl;


    }


}





//void Form_Camera::on_pushButton_2_clicked()
//{
//    auto start = std::chrono::steady_clock::now();
//    QFuture<cv::Mat> c1 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 1);
//    QFuture<cv::Mat> c2 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 2);
//    QFuture<cv::Mat> c3 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 3);
//    QFuture<cv::Mat> c4 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 4);
//    QFuture<cv::Mat> c5 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 5);
//    QFuture<cv::Mat> c6 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 6);
//    c1.waitForFinished();          //等待线程关闭
//    c2.waitForFinished();          //等待线程关闭
//    c3.waitForFinished();          //等待线程关闭
//    c4.waitForFinished();          //等待线程关闭
//    c5.waitForFinished();          //等待线程关闭
//    c6.waitForFinished();          //等待线程关闭
//    auto end = std::chrono::steady_clock::now();
//    std::chrono::duration<double, std::micro> elapsed = end - start; // std::micro 表示以微秒为时间单位
//    double timetotal = elapsed.count();


//    auto start1 = std::chrono::steady_clock::now();
//    QFuture<cv::Mat> d1 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 1);
//    d1.waitForFinished();          //等待线程关闭
//    auto end1 = std::chrono::steady_clock::now();
//    std::chrono::duration<double, std::micro> elapsed1 = end1 - start1; // std::micro 表示以微秒为时间单位
//    double timetotal1 = elapsed1.count();

//    auto start2 = std::chrono::steady_clock::now();
//    QFuture<cv::Mat> d2 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 2);
//    d2.waitForFinished();          //等待线程关闭
//    auto end2 = std::chrono::steady_clock::now();
//    std::chrono::duration<double, std::micro> elapsed2 = end2 - start2; // std::micro 表示以微秒为时间单位
//    double timetotal2 = elapsed2.count();

//    auto start3 = std::chrono::steady_clock::now();
//    QFuture<cv::Mat> d3 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 3);
//    d3.waitForFinished();          //等待线程关闭
//    auto end3 = std::chrono::steady_clock::now();
//    std::chrono::duration<double, std::micro> elapsed3= end3 - start3; // std::micro 表示以微秒为时间单位
//    double timetotal3 = elapsed3.count();

//    auto start4 = std::chrono::steady_clock::now();
//    QFuture<cv::Mat> d4 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 4);
//    d4.waitForFinished();          //等待线程关闭
//    auto end4 = std::chrono::steady_clock::now();
//    std::chrono::duration<double, std::micro> elapsed4= end4 - start4; // std::micro 表示以微秒为时间单位
//    double timetotal4 = elapsed4.count();


//    auto start5 = std::chrono::steady_clock::now();
//    QFuture<cv::Mat> d5 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 5);
//    d5.waitForFinished();          //等待线程关闭
//    auto end5 = std::chrono::steady_clock::now();
//    std::chrono::duration<double, std::micro> elapsed5= end5 - start5; // std::micro 表示以微秒为时间单位
//    double timetotal5 = elapsed5.count();


//    auto start6 = std::chrono::steady_clock::now();
//    QFuture<cv::Mat> d6 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 6);
//    d6.waitForFinished();          //等待线程关闭
//    auto end6 = std::chrono::steady_clock::now();
//    std::chrono::duration<double, std::micro> elapsed6= end6 - start6; // std::micro 表示以微秒为时间单位
//    double timetotal6 = elapsed6.count();

//    ui->label_10->setText(QString("总%1").arg(timetotal));
//    ui->label_11->setText(QString("左黑白%1").arg(timetotal1));
//    ui->label_12->setText(QString("主黑白%1").arg(timetotal2));
//    ui->label_13->setText(QString("右黑白%1").arg(timetotal3));
//    ui->label_14->setText(QString("前黑白%1").arg(timetotal4));
//    ui->label_15->setText(QString("后黑白%1").arg(timetotal5));
//    ui->label_16->setText(QString("主彩色%1").arg(timetotal6));

//    Mat src1=c1.result().clone();
//    Mat src2=c2.result().clone();
//    Mat src3=c3.result().clone();
//    Mat src4=c4.result().clone();
//    Mat src5=c5.result().clone();
//    Mat src6=c6.result().clone();

//    imwrite("D:\\result\\src1.bmp",src1);
//    imwrite("D:\\result\\src2.bmp",src2);
//    imwrite("D:\\result\\src3.bmp",src3);
//    imwrite("D:\\result\\src4.bmp",src4);
//    imwrite("D:\\result\\src5.bmp",src5);
//    imwrite("D:\\result\\src6.bmp",src6);

//    Mat separete1=d1.result().clone();
//    Mat separete2=d2.result().clone();
//    Mat separete3=d3.result().clone();
//    Mat separete4=d4.result().clone();
//    Mat separete5=d5.result().clone();
//    Mat separete6=d6.result().clone();

//    imwrite("D:\\result\\separete1.bmp",separete1);
//    imwrite("D:\\result\\separete2.bmp",separete2);
//    imwrite("D:\\result\\separete3.bmp",separete3);
//    imwrite("D:\\result\\separete4.bmp",separete4);
//    imwrite("D:\\result\\separete5.bmp",separete5);
//    imwrite("D:\\result\\separete6.bmp",separete6);
//}


void Form_Camera::on_comboBox_Adaptive_exposure_list_currentTextChanged(const QString &arg1)
{
    QSqlQuery query=QSqlQuery(db);
    QString sq=QStringLiteral("SELECT 产品型号编码 FROM product_model WHERE 产品型号='%1'").arg(ui->comboBox_Pro_Mode->currentText());
    query.exec(sq);
    query.next();
    QString ModeCode=query.value(0).toString();

    if(ui->comboBox_Adaptive_exposure_list->currentText()=="白底曝光")
    {
        QString sq6=QStringLiteral("SELECT 相机参数值 FROM camera_exposure where 相机参数名='%1'and 产品型号编码='%2'").arg("Exposure_White_Black_Left").arg(ModeCode);
        query.exec(sq6);
        query.next();
        ui->spinBox_1->setValue(query.value(0).toInt());
        QString sq7=QStringLiteral("SELECT 相机参数值 FROM camera_exposure where 相机参数名='%1'and 产品型号编码='%2'").arg("Exposure_White_Black_Middle").arg(ModeCode);
        query.exec(sq7);
        query.next();
        ui->spinBox_2->setValue(query.value(0).toInt());
        QString sq8=QStringLiteral("SELECT 相机参数值 FROM camera_exposure where 相机参数名='%1'and 产品型号编码='%2'").arg("Exposure_White_Color_Right").arg(ModeCode);
        query.exec(sq8);
        query.next();
        ui->spinBox_3->setValue(query.value(0).toInt());
    }
    else if(ui->comboBox_Adaptive_exposure_list->currentText()=="侧光灯曝光")
    {
        QString sq1=QStringLiteral("SELECT 相机参数值 FROM camera_exposure where 相机参数名='%1'and 产品型号编码='%2'").arg("Exposure_Ceguang_Main").arg(ModeCode);
        query.exec(sq1);
        query.next();
        ui->spinBox_2->setValue(query.value(0).toInt());
        QString sq2=QStringLiteral("SELECT 相机参数值 FROM camera_exposure where 相机参数名='%1'and 产品型号编码='%2'").arg("Exposure_Ceguang_Left").arg(ModeCode);
        query.exec(sq2);
        query.next();
        ui->spinBox_1->setValue(query.value(0).toInt());
        QString sq3=QStringLiteral("SELECT 相机参数值 FROM camera_exposure where 相机参数名='%1'and 产品型号编码='%2'").arg("Exposure_Ceguang_Right").arg(ModeCode);
        query.exec(sq3);
        query.next();
        ui->spinBox_3->setValue(query.value(0).toInt());
    }
}

void Form_Camera::on_pushButton_2_clicked()
{
    QFuture<cv::Mat> d1 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 1);
    d1.waitForFinished();          //等待线程关闭
    QFuture<cv::Mat> d2 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 2);
    d2.waitForFinished();          //等待线程关闭
    QFuture<cv::Mat> d3 =QtConcurrent::run(camera,&HikvisionSDK::saveImage7, 3);
    d3.waitForFinished();          //等待线程关闭
    Mat separete1=d1.result().clone();
    Mat separete2=d2.result().clone();
    Mat separete3=d3.result().clone();
    if(ui->comboBox_Adaptive_exposure_list->currentText()=="白底曝光")
    {
        imwrite("D:\\leftwhite.bmp",separete1);
        imwrite("D:\\mainwhite.bmp",separete2);
        imwrite("D:\\rightwhite.bmp",separete3);
    }
    if(ui->comboBox_Adaptive_exposure_list->currentText()=="侧光灯曝光")
    {
        imwrite("D:\\leftceguang.bmp",separete1);
        imwrite("D:\\mainceguang.bmp",separete2);
        imwrite("D:\\rightceguang.bmp",separete3);
    }
}
