#include "parameter_control.h"
#include "ui_parameter_control.h"
#include"global_variable.h"
#include<QDebug>
#include <QMessageBox>
#include <QtSql/QSqlQuery>
#include<QElapsedTimer>
#include <QTime>
#include <QSqlError>

int Yiwu_Detect_Grade;                  //异物检测等级
int Yiwu_Detect_Grade_After;            //异物检测等级

int Whitepoint_Detect_Grade;                  //白点检测等级
int Whitepoint_Detect_Grade_After;            //白点检测等级

int Whiteprint_Detect_Grade;                  //白印检测等级
int Whiteprint_Detect_Grade_After;            //白印检测等级

parameter_control::parameter_control(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::parameter_control)   //parent
{
    ui->setupUi(this);
    ui->tabWidget->setTabPosition(QTabWidget::West);
    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    setStyleSheet("border-color:rgb(0,0,0),border:2px");
    ui->tabWidget->setStyleSheet("QTabWidget::pane{ \border-left: 1px solid #eeeeee;\}");
    ui->spinBox_23->setFocusPolicy(Qt::NoFocus);
    ui->spinBox_25->setFocusPolicy(Qt::NoFocus);
    ui->spinBox_26->setFocusPolicy(Qt::NoFocus);
    //ui->spinBox_23->valueChanged;

    //参数限制
    parameter_control::initial_para1();
    //==========================海康相机曝光参数==============================
    //相机侧光参数
    ui->spinBox->setValue(Exposure_Ceguang_Main);
    ui->spinBox_2->setValue(Exposure_Ceguang_Left);
    ui->spinBox_3->setValue(Exposure_Ceguang_Right);
    ui->spinBox_4->setValue(Exposure_Ceguang_Front);
    ui->spinBox_5->setValue(Exposure_Ceguang_Back);
    //相机白底参数
    ui->spinBox_6->setValue(Exposure_White_Black_Middle);
    ui->spinBox_7->setValue(Exposure_White_Color_Middle);
    ui->spinBox_9->setValue(Exposure_White_Black_Left);
    ui->spinBox_10->setValue(Exposure_White_Color_Right);
    ui->spinBox_11->setValue(Exposure_White_Black_Front);
    ui->spinBox_12->setValue(Exposure_White_Black_Back);
    //相机黑底参数
    ui->spinBox_15->setValue(Exposure_Black_Middle);
    ui->spinBox_16->setValue(Exposure_Black_Color);
    ui->spinBox_17->setValue(Num_1_L_Exposure_white);
    //相机灰底参数
    ui->spinBox_17->setValue(Exposure_Gray_Main);
    ui->spinBox_21->setValue(Exposure_Gray_Left);
    ui->spinBox_22->setValue(Exposure_Gray_Right);

    //==========================PLC设备切换延时参数===========================
    ui->spinBox_8->setValue(SideLightOpenTime);         //侧光灯打开延时
    ui->spinBox_13->setValue(BoxOpenTime);              //测试盒打开点亮白底延时
    ui->spinBox_14->setValue(BlackSwitchTime);          //屏幕白色切换到黑色延时
    ui->spinBox_18->setValue(GraySwitchTime);           //屏幕黑色切换到灰色延时
    ui->spinBox_19->setValue(modbus_time);              //PLC读写指令延时

    //==========================少线缺陷算法参数===========================
    ui->spinBox_20->setValue(lackline_bolckSize);       //少线分割邻域块大小
    ui->doubleSpinBox->setValue(lackline_delta);        //少线分割邻域块阈值
    ui->doubleSpinBox_2->setValue(longShortlowerth);    //少线长宽比下限
    ui->spinBox_797->setValue(longLowerth);             //少线长度下限
    ui->spinBox_798->setValue(shortHigherth);           //少线宽度上限
    ui->doubleSpinBox_5->setValue(lackscratchth);       //少线划痕阈值排除表面贴膜划痕干扰

    //==========================异物缺陷算法参数===========================
    ui->spinBox_799->setValue(yiwu_pre_size);           //异物分割邻域块大小
    ui->doubleSpinBox_11->setValue(yiwu_pre_th);        //异物分割邻域块阈值
    ui->spinBox_800->setValue(yiwu_area_lower);         //异物一次面积下限
    ui->spinBox_801->setValue(yiwu_area_upper);         //异物一次面积上限
    ui->spinBox_802->setValue(yiwu_k_w_h);              //异物长宽比，点状和线状
    ui->spinBox_66->setValue(yiwu_sec_size);            //异物2次分割邻域块大小
    ui->spinBox_803->setValue(yiwu_sec_th);             //异物2次分割邻域块阈值
    ui->spinBox_804->setValue(yiwu_sec_area_lower);     //异物二次面积下限
    ui->spinBox_805->setValue(yiwu_sec_area_upper);     //异物二次面积上限
    ui->doubleSpinBox_19->setValue(yiwu_lighth);        //异物分割邻域块大小

//    Yiwu_Detect_Grade=8.1-yiwu_lighth;
//    ui->spinBox_23->setValue(Yiwu_Detect_Grade);
    ui->spinBox_25->setValue(Whiteprint_Detect_Grade);
    //========================== 白印缺陷算法参数===========================
    ui->doubleSpinBox_326->setValue(black_mura_gray_th); //亮度阈值主调节参数
    ui->spinBox_65->setValue(black_mura_area_lower);     //面积阈值下限
    ui->spinBox_806->setValue(black_mura_area_upper);    //面积阈值上限
    ui->spinBox_807->setValue(black_mura_boder);         //外围宽度范围
    ui->spinBox_808->setValue(black_mura_corner);        //边角宽度范围
    ui->doubleSpinBox_324->setValue(mura_bubbleth);      //白印缺陷排除气泡干扰阈值
    ui->doubleSpinBox_327->setValue(mura_centerlighth);  //白印缺陷中心亮度阈值
    ui->doubleSpinBox_33->setValue(mura_lighth);         //白印缺陷亮度阈值
    ui->doubleSpinBox_35->setValue(black_mura_area_1);   //一级灰度面积
    ui->doubleSpinBox_37->setValue(black_mura_th_1);     //一级灰度差值
    ui->doubleSpinBox_39->setValue(black_mura_area_2);   //二级灰度面积
    ui->doubleSpinBox_325->setValue(black_mura_th_2);    //二级灰度差值

    //==========================白点缺陷算法参数===========================
    ui->doubleSpinBox_653->setValue(whitePoint_step);    //白点自适应分割领域块大小
    ui->doubleSpinBox_40->setValue(whitePoint_threshold);//白点自适应分割领阈值
    ui->spinBox_91->setValue(whitePoint_lowerArea);      //面积下限
    ui->spinBox_92->setValue(whitePoint_higherArea);     //面积上限
    ui->doubleSpinBox_46->setValue(scratchth);           //白点排除划痕阈值大于则认为贴膜划痕
    ui->doubleSpinBox_655->setValue(bubbleth);           //白点排除气泡阈值大于则认为贴膜气泡
    ui->doubleSpinBox_654->setValue(dotdomainnum);       //白点连通域个数大于则认为噪点
    ui->doubleSpinBox_41->setValue(defectouterth);       //白点缺陷处灰度均值与缺陷外围灰度差
    ui->doubleSpinBox_43->setValue(corewholeth);         //白点缺陷中心点灰度值与整块疑似缺陷灰度均值差
    ui->doubleSpinBox_45->setValue(spotpeak);            //白点缺陷最亮点与最暗点灰度差
    ui->doubleSpinBox_47->setValue(siecevariance);       //侧光图中排除划痕参数
    ui->doubleSpinBox_656->setValue(whitePoint_w_h);     //白点缺陷长宽比阈值

//    Whitepoint_Detect_Grade=9.5-spotpeak;
//    ui->spinBox_26->setValue(Whitepoint_Detect_Grade);

    //=========================划伤缺陷算法参数============================
    ui->spinBox_485->setValue(scratch__bolckSize);       //白点自适应分割领域块大小
    ui->doubleSpinBox_54->setValue(scratch__delta);      //白点自适应分割领阈值
    ui->spinBox_93->setValue(scratch_higherArea);        //划伤面积上限
    ui->spinBox_94->setValue(scratch_lowerArea);         //划伤面积下限
    ui->doubleSpinBox_52->setValue(scratch_lowerLongShort);//划伤长宽比下限
    ui->doubleSpinBox_724->setValue(scratch_higherLongShort);//划伤长宽比上限
    ui->spinBox_487->setValue(scratch_lowerWidth);       //划伤宽度下限
    ui->spinBox_488->setValue(scratch_higherWidth);      //划伤宽度下限
    ui->spinBox_489->setValue(scratch_lowerLength);      //划伤长度下限
    ui->spinBox_490->setValue(scratch_higherLength);     //划伤长度上限
    ui->doubleSpinBox_49->setValue(filmscratch);         //贴膜划痕系数
    ui->doubleSpinBox_50->setValue(scratchbubbleth);     //贴膜排除气泡系数1
    ui->doubleSpinBox_51->setValue(scratchbubbleth1);    //贴膜排除气泡系数2
    ui->doubleSpinBox_53->setValue(scratchbrighth);      //划伤亮度阈值
    ui->doubleSpinBox_725->setValue(minscratchbrighth);  //最小划伤亮度阈值

    //=========================亮点缺陷算法参数============================
    ui->doubleSpinBox_903->setValue(black_light_gram);  //亮点亮度增强系数
    ui->spinBox_529->setValue(black_lightPoint_step);   //滑窗间隔步长
    ui->spinBox_532->setValue(black_lightPoint_size);   //滑窗窗口大小
    ui->spinBox_530->setValue(black_lightPoint_gray_th);//滑窗二值化亮度阈值
    ui->spinBox_531->setValue(black_lightPoint_area_upper);//面积阈值下限
    ui->doubleSpinBox_899->setValue(black_lightPoint_area_lower);//面积阈值下限
    ui->spinBox_533->setValue(black_lightPoint_diff);   //亮点亮度阈值
    ui->doubleSpinBox_898->setValue(black_lightPoint_w_h); //亮点缺陷长宽比

    //=========================红蓝斑缺陷算法参数===========================
    ui->doubleSpinBox_960->setValue(black_light_red);     //红蓝斑缺陷长宽比
    ui->spinBox_574->setValue(redBlue_area_lower);        //红蓝斑缺陷长宽比
    ui->spinBox_575->setValue(redBlue_area_upper);        //红蓝斑缺陷长宽比
    ui->spinBox_576->setValue(redBlue_numupper);          //红蓝斑缺陷长宽比

    //=========================漏光算法参数================================
    ui->spinBox_660->setValue(leakage_bolckSize);         //黑底漏光适应分割领域块大小
    ui->doubleSpinBox_1065->setValue(leakage_delta);      //黑底漏光适应分割阈值大小
    ui->spinBox_662->setValue(leakage_area_lower);        //黑底漏光面积下限
    ui->spinBox_663->setValue(leakage_area_upper);        //黑底漏光面积上限
    ui->doubleSpinBox_1066->setValue(leakageth);          //黑底漏光亮度阈值
    ui->doubleSpinBox_1067->setValue(leakage_w_h);        //黑底漏光长宽比阈值

    //=========================显示异常算法参数============================
    ui->doubleSpinBox_704->setValue(white_gray_lower);          //白色灰度下限，白底亮度过低认为显示异常
    ui->spinBox_705->setValue(black_gray_upper);          //黑色灰度上限  黑底亮度过高认为显示异常

    //=========================外部厂家产品型号显示============================
    qDebug()<<"Pro_Model_Mod"<<endl;
    ui->label_2->setText(Pro_Model_Mod);
    ui->label_28->setText(Pro_Model_Mod);
    ui->label_85->setText(Pro_Model_Mod);
    ui->label_509->setText(Pro_Model_Mod);
    ui->label_515->setText(Pro_Model_Mod);
    ui->label_524->setText(Pro_Model_Mod);
    ui->label_685->setText(Pro_Model_Mod);
    ui->label_934->setText(Pro_Model_Mod);
    ui->label_1929->setText(Pro_Model_Mod);
    ui->label_1935->setText(Pro_Model_Mod);
    ui->label_1938->setText(Pro_Model_Mod);
}

parameter_control::~parameter_control()
{
    delete ui;
}

/*=========================================================
* 函 数 名: on_pushButton_clicked
* 功能描述: 相机曝光参数值修改
=========================================================*/
void parameter_control::on_pushButton_clicked()
{
    QMessageBox box(QMessageBox::Warning,"警告","确定修改相机参数？");
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec ();

    if(ok == QMessageBox::Ok)//确认修改参数
    {
        qDebug()<<"innerparameter"<<Model_Code<<endl;
        qDebug()<<"outerparameter"<<Pro_Model_Mod<<endl;
        //相机侧光参数
        Exposure_Ceguang_Main=ui->spinBox->text().toInt();
        Exposure_Ceguang_Left=ui->spinBox_2->text().toInt();
        Exposure_Ceguang_Right=ui->spinBox_3->text().toInt();
        Exposure_Ceguang_Front=ui->spinBox_4->text().toInt();
        Exposure_Ceguang_Back=ui->spinBox_5->text().toInt();

        //相机白底参数
        Exposure_White_Black_Middle=ui->spinBox_6->text().toInt();
        Exposure_White_Color_Middle=ui->spinBox_7->text().toInt();
        Exposure_White_Black_Left=ui->spinBox_9->text().toInt();
        Exposure_White_Color_Right=ui->spinBox_10->text().toInt();
        Exposure_White_Black_Front=ui->spinBox_11->text().toInt();
        Exposure_White_Black_Back=ui->spinBox_12->text().toInt();

        //相机黑底参数
        Exposure_Black_Middle=ui->spinBox_15->text().toInt();
        Exposure_Black_Color=ui->spinBox_16->text().toInt();
        Num_1_L_Exposure_white=ui->spinBox_17->text().toInt();

        //相机灰底底参数
        Exposure_Gray_Main=ui->spinBox_17->text().toInt();         //主黑白相机灰色曝光值
        Exposure_Gray_Left=ui->spinBox_21->text().toInt();         //左黑白相机灰色曝光值
        Exposure_Gray_Right=ui->spinBox_22->text().toInt();        //右黑白相机灰色曝光值

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        //相机侧光参数
        QString sq1=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_Ceguang_Main).arg("Exposure_Ceguang_Main").arg(Model_Code);
        query.exec(sq1);
        QString sq2=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_Ceguang_Left).arg("Exposure_Ceguang_Left").arg(Model_Code);
        query.exec(sq2);
        QString sq3=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_Ceguang_Right).arg("Exposure_Ceguang_Right").arg(Model_Code);
        query.exec(sq3);
        QString sq4=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_Ceguang_Front).arg("Exposure_Ceguang_Front").arg(Model_Code);
        query.exec(sq4);
        QString sq5=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_Ceguang_Back).arg("Exposure_Ceguang_Back").arg(Model_Code);
        query.exec(sq5);
        //相机白底参数
        QString sq6=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_White_Black_Middle).arg("Exposure_White_Black_Middle").arg(Model_Code);
        query.exec(sq6);
        QString sq7=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_White_Color_Middle).arg("Exposure_White_Color_Middle").arg(Model_Code);
        query.exec(sq7);
        QString sq8=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_White_Black_Left).arg("Exposure_White_Black_Left").arg(Model_Code);
        query.exec(sq8);
        QString sq9=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_White_Color_Right).arg("Exposure_White_Color_Right").arg(Model_Code);
        query.exec(sq9);
        QString sq10=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_White_Black_Front).arg("Exposure_White_Black_Front").arg(Model_Code);
        query.exec(sq10);
        QString sq11=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_White_Black_Back).arg("Exposure_White_Black_Back").arg(Model_Code);
        query.exec(sq11);
        //相机黑底参数
        QString sq12=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_Black_Middle).arg("Exposure_Black_Middle").arg(Model_Code);
        query.exec(sq12);
        QString sq13=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_Black_Color).arg("Exposure_Black_Color").arg(Model_Code);
        query.exec(sq13);
        //相机灰底参数
        QString sq15=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_Gray_Main).arg("Exposure_Gray_Main").arg(Model_Code);
        query.exec(sq15);
        QString sq16=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_Gray_Left).arg("Exposure_Gray_Left").arg(Model_Code);
        query.exec(sq16);
        QString sq17=QStringLiteral("update camera_exposure set 相机参数值='%0' where 相机参数名='%1'and 产品型号编码='%2'").arg(Exposure_Gray_Right).arg("Exposure_Gray_Right").arg(Model_Code);
        query.exec(sq17);

        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        if(conmmitsignal)
        {
            parameter_alter();//执行全局变量参数更新
            ui->label_24->setText("更新参数成功");
            delay1(500);
            this->destroy(true,true);
            parameter_control_show=false;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"提示","修改相机参数失败!");
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
            qDebug()<<query.lastError()<<endl;
            ui->label_24->setText("更新参数失败");
            delay1(500);
        }
        qDebug()<<conmmitsignal<<endl;
    }
    else//取消修改参数
    {
        this->destroy(true,true);
        parameter_control_show=false;
    }
}

void parameter_control::on_pushButton_3_clicked()
{
    this->destroy(true,true);
    parameter_control_show=false;
}

/*=========================================================
* 函 数 名: on_pushButton_5_clicked
* 功能描述: PLC设备延时参数值修改
=========================================================*/
void parameter_control::on_pushButton_5_clicked()
{
    QMessageBox box(QMessageBox::Warning,"警告","确定修改延时参数？");
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec ();
    if(ok == QMessageBox::Ok)//确认修改参数
    {
        //==========================PLC设备切换延时参数===========================
        SideLightOpenTime=ui->spinBox_8->text().toInt();  //侧光灯打开延时
        BoxOpenTime=ui->spinBox_13->text().toInt();       //测试盒打开点亮白底延时
        BlackSwitchTime=ui->spinBox_14->text().toInt();   //屏幕白色切换到黑色延时
        GraySwitchTime=ui->spinBox_18->text().toInt();    //屏幕白色切换到黑色延时
        modbus_time=ui->spinBox_19->text().toInt();       //PLC读写指令延时

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        //相机侧光参数
        QString sq15=QStringLiteral("update plc_delaytime set 延时参数值='%0' where 延时参数名='%1'and 产品型号编码='%2'").arg(SideLightOpenTime).arg("SideLightOpenTime").arg(Model_Code);
        query.exec(sq15);
        QString sq16=QStringLiteral("update plc_delaytime set 延时参数值='%0' where 延时参数名='%1'and 产品型号编码='%2'").arg(BoxOpenTime).arg("BoxOpenTime").arg(Model_Code);
        query.exec(sq16);
        QString sq17=QStringLiteral("update plc_delaytime set 延时参数值='%0' where 延时参数名='%1'and 产品型号编码='%2'").arg(BlackSwitchTime).arg("BlackSwitchTime").arg(Model_Code);
        query.exec(sq17);
        QString sq18=QStringLiteral("update plc_delaytime set 延时参数值='%0' where 延时参数名='%1'and 产品型号编码='%2'").arg(GraySwitchTime).arg("GraySwitchTime").arg(Model_Code);
        query.exec(sq18);
        QString sq19=QStringLiteral("update plc_delaytime set 延时参数值='%0' where 延时参数名='%1'and 产品型号编码='%2'").arg(modbus_time).arg("modbus_time").arg(Model_Code);
        query.exec(sq19);
        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        qDebug()<<conmmitsignal<<endl;

        if(conmmitsignal)
        {
            parameter_alter();//执行全局变量参数更新
            ui->label_26->setText("更新参数成功");
            delay1(500);
            this->destroy(true,true);
            parameter_control_show=false;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"提示","修改延时参数失败!");
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
            qDebug()<<query.lastError()<<endl;
            ui->label_26->setText("更新参数失败");
            delay1(500);
        }
    }
    else
    {
        this->destroy(true,true);
        parameter_control_show=false;
    }
}

/*=========================================================
* 函 数 名: on_pushButton_15_clicked
* 功能描述: 少线参数修改
=========================================================*/
void parameter_control::on_pushButton_6_clicked()
{
    QMessageBox box(QMessageBox::Warning,"警告","确定修改少线参数？");
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec ();
    if(ok == QMessageBox::Ok)//确认修改参数
    {
        //==========================少线参数===========================
        lackline_bolckSize=ui->spinBox_20->text().toDouble();      //少线分割邻域块大小
        lackline_delta=ui->doubleSpinBox->text().toDouble();       //少线分割邻域块阈值
        longShortlowerth=ui->doubleSpinBox_2->text().toDouble();   //少线长宽比下限
        shortHigherth=ui->spinBox_797->text().toDouble();          //少线宽度上限
        longLowerth=ui->spinBox_798->text().toDouble();            //少线长度下限
        lackscratchth=ui->doubleSpinBox_5->text().toDouble();      //少线排除贴膜划痕阈值                                      //少线排除贴膜划痕阈值

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        //相机侧光参数
        QString sq20=QStringLiteral("update lackline_para set 少线参数值='%0' where 少线参数名='%1'and 产品型号编码='%2'").arg(lackline_bolckSize).arg("lackline_bolckSize").arg(Model_Code);
        query.exec(sq20);
        QString sq21=QStringLiteral("update lackline_para set 少线参数值='%0' where 少线参数名='%1'and 产品型号编码='%2'").arg(lackline_delta).arg("lackline_delta").arg(Model_Code);
        query.exec(sq21);
        QString sq22=QStringLiteral("update lackline_para set 少线参数值='%0' where 少线参数名='%1'and 产品型号编码='%2'").arg(longShortlowerth).arg("longShortlowerth").arg(Model_Code);
        query.exec(sq22);
        QString sq23=QStringLiteral("update lackline_para set 少线参数值='%0' where 少线参数名='%1'and 产品型号编码='%2'").arg(shortHigherth).arg("shortHigherth").arg(Model_Code);
        query.exec(sq23);
        QString sq24=QStringLiteral("update lackline_para set 少线参数值='%0' where 少线参数名='%1'and 产品型号编码='%2'").arg(longLowerth).arg("longLowerth").arg(Model_Code);
        query.exec(sq24);
        QString sq25=QStringLiteral("update lackline_para set 少线参数值='%0' where 少线参数名='%1'and 产品型号编码='%2'").arg(lackscratchth).arg("lackscratchth").arg(Model_Code);
        query.exec(sq25);

        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        qDebug()<<conmmitsignal<<endl;

        if(conmmitsignal)
        {
            parameter_alter();//执行全局变量参数更新
            ui->label_87->setText("更新参数成功");
            delay1(500);
            this->destroy(true,true);
            parameter_control_show=false;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"提示","修改少线参数失败!");
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
            qDebug()<<query.lastError()<<endl;
            ui->label_87->setText("更新参数失败");
            delay1(500);
        }
    }
    else
    {
        this->destroy(true,true);
        parameter_control_show=false;
    }
}

/*=========================================================
* 函 数 名: on_pushButton_8_clicked
* 功能描述: 异物参数修改
=========================================================*/
void parameter_control::on_pushButton_8_clicked()
{
    QMessageBox box(QMessageBox::Warning,"警告","确定修改异物参数？");
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec ();
    if(ok == QMessageBox::Ok)//确认修改参数
    {
        //==========================异物缺陷算法参数===========================
        yiwu_pre_size=ui->spinBox_799->text().toDouble();        //异物分割邻域块大小
        yiwu_pre_th=ui->doubleSpinBox_11->text().toDouble();     //异物分割邻域块阈值
        yiwu_area_lower=ui->spinBox_800->text().toDouble();      //异物一次面积下限
        yiwu_area_upper=ui->spinBox_801->text().toDouble();      //异物一次面积上限
        yiwu_k_w_h=ui->spinBox_802->text().toDouble();           //异物长宽比，点状和线状
        yiwu_sec_size=ui->spinBox_66->text().toDouble();         //异物2次分割邻域块大小
        yiwu_sec_th=ui->spinBox_803->text().toDouble();          //异物2次分割邻域块阈值
        yiwu_sec_area_lower=ui->spinBox_804->text().toDouble();  //异物二次面积下限
        yiwu_sec_area_upper=ui->spinBox_805->text().toDouble();  //异物二次面积上限
        yiwu_lighth=ui->doubleSpinBox_19->text().toDouble();     //异物分割邻域块大小

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        //相机侧光参数
        QString sq1=QStringLiteral("update yiwu_para set 异物参数值='%0' where 异物参数名='%1'and 产品型号编码='%2'").arg(yiwu_pre_size).arg("yiwu_pre_size").arg(Model_Code);
        query.exec(sq1);
        QString sq2=QStringLiteral("update yiwu_para set 异物参数值='%0' where 异物参数名='%1'and 产品型号编码='%2'").arg(yiwu_pre_th).arg("yiwu_pre_th").arg(Model_Code);
        query.exec(sq2);
        QString sq3=QStringLiteral("update yiwu_para set 异物参数值='%0' where 异物参数名='%1'and 产品型号编码='%2'").arg(yiwu_area_lower).arg("yiwu_area_lower").arg(Model_Code);
        query.exec(sq3);
        QString sq4=QStringLiteral("update yiwu_para set 异物参数值='%0' where 异物参数名='%1'and 产品型号编码='%2'").arg(yiwu_area_upper).arg("yiwu_area_upper").arg(Model_Code);
        query.exec(sq4);
        QString sq5=QStringLiteral("update yiwu_para set 异物参数值='%0' where 异物参数名='%1'and 产品型号编码='%2'").arg(yiwu_k_w_h).arg("yiwu_k_w_h").arg(Model_Code);
        query.exec(sq5);
        QString sq6=QStringLiteral("update yiwu_para set 异物参数值='%0' where 异物参数名='%1'and 产品型号编码='%2'").arg(yiwu_sec_size).arg("yiwu_sec_size").arg(Model_Code);
        query.exec(sq6);
        QString sq7=QStringLiteral("update yiwu_para set 异物参数值='%0' where 异物参数名='%1'and 产品型号编码='%2'").arg(yiwu_sec_th).arg("yiwu_sec_th").arg(Model_Code);
        query.exec(sq7);
        QString sq8=QStringLiteral("update yiwu_para set 异物参数值='%0' where 异物参数名='%1'and 产品型号编码='%2'").arg(yiwu_sec_area_lower).arg("yiwu_sec_area_lower").arg(Model_Code);
        query.exec(sq8);
        QString sq9=QStringLiteral("update yiwu_para set 异物参数值='%0' where 异物参数名='%1'and 产品型号编码='%2'").arg(yiwu_sec_area_upper).arg("yiwu_sec_area_upper").arg(Model_Code);
        query.exec(sq9);
        QString sq10=QStringLiteral("update yiwu_para set 异物参数值='%0' where 异物参数名='%1'and 产品型号编码='%2'").arg(yiwu_lighth).arg("yiwu_lighth").arg(Model_Code);
        query.exec(sq10);

        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        qDebug()<<conmmitsignal<<endl;

        if(conmmitsignal)
        {
            parameter_alter();//执行全局变量参数更新
            ui->label_632->setText("更新参数成功");
            delay1(500);
            this->destroy(true,true);
            parameter_control_show=false;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"提示","修改异物参数失败!");
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
            qDebug()<<query.lastError()<<endl;
            ui->label_632->setText("更新参数失败");
            delay1(500);
        }
    }
    else
    {
//        this->close();
//        delete ui;
        this->destroy(true,true);
        parameter_control_show=false;
    }
}

/*=========================================================
* 函 数 名: on_pushButton_10_clicked
* 功能描述: 白印参数修改
=========================================================*/
void parameter_control::on_pushButton_10_clicked()
{
    QMessageBox box(QMessageBox::Warning,"警告","确定修改白印参数？");
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec ();
    if(ok == QMessageBox::Ok)//确认修改参数
    {
        //==========================白印缺陷算法参数===========================
        black_mura_gray_th=ui->doubleSpinBox_326->text().toDouble();        //亮度阈值主调节参数
        black_mura_area_lower=ui->spinBox_65->text().toDouble();            //面积阈值下限
        black_mura_area_upper=ui->spinBox_806->text().toDouble();           //面积阈值上限
        black_mura_boder=ui->spinBox_807->text().toDouble();                //外围宽度范围
        black_mura_corner=ui->spinBox_808->text().toDouble();               //边角宽度范围
        mura_bubbleth=ui->doubleSpinBox_324->text().toDouble();             //边角宽度范围
        mura_centerlighth=ui->doubleSpinBox_327->text().toDouble();         //白印缺陷排除气泡干扰阈值
        mura_lighth=ui->doubleSpinBox_33->text().toDouble();                //白印缺陷中心亮度阈值
        black_mura_area_1=ui->doubleSpinBox_35->text().toDouble();          //一级灰度面积
        black_mura_th_1=ui->doubleSpinBox_37->text().toDouble();            //一级灰度差值
        black_mura_area_2=ui->doubleSpinBox_39->text().toDouble();          //二级灰度面积
        black_mura_th_2=ui->doubleSpinBox_325->text().toDouble();           //二级灰度差值

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        //相机侧光参数
        QString sq1=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(black_mura_gray_th).arg("black_mura_gray_th").arg(Model_Code);
        query.exec(sq1);
        QString sq2=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(black_mura_area_lower).arg("black_mura_area_lower").arg(Model_Code);
        query.exec(sq2);
        QString sq3=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(black_mura_area_upper).arg("black_mura_area_upper").arg(Model_Code);
        query.exec(sq3);
        QString sq4=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(black_mura_boder).arg("black_mura_boder").arg(Model_Code);
        query.exec(sq4);
        QString sq5=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(black_mura_corner).arg("black_mura_corner").arg(Model_Code);
        query.exec(sq5);
        QString sq6=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(mura_bubbleth).arg("mura_bubbleth").arg(Model_Code);
        query.exec(sq6);
        QString sq7=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(mura_centerlighth).arg("mura_centerlighth").arg(Model_Code);
        query.exec(sq7);
        QString sq8=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(mura_lighth).arg("mura_lighth").arg(Model_Code);
        query.exec(sq8);
        QString sq9=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(black_mura_area_1).arg("black_mura_area_1").arg(Model_Code);
        query.exec(sq9);
        QString sq10=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(black_mura_th_1).arg("black_mura_th_1").arg(Model_Code);
        query.exec(sq10);
        QString sq11=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(black_mura_area_2).arg("black_mura_area_2").arg(Model_Code);
        query.exec(sq11);
        QString sq12=QStringLiteral("update mura_para set 白印参数值='%0' where 白印参数变量='%1'and 产品型号编码='%2'").arg(black_mura_th_2).arg("black_mura_th_2").arg(Model_Code);
        query.exec(sq12);

        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        qDebug()<<conmmitsignal<<endl;

        if(conmmitsignal)
        {
            parameter_alter();//执行全局变量参数更新
            ui->label_632->setText("更新参数成功");
            delay1(500);
            this->destroy(true,true);
            parameter_control_show=false;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"提示","修改白印参数失败!");
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
            qDebug()<<query.lastError()<<endl;
            ui->label_632->setText("更新参数失败");
            delay1(500);
        }
    }
    else
    {
        this->destroy(true,true);
        parameter_control_show=false;
    }
}

/*=========================================================
* 函 数 名: on_pushButton_10_clicked
* 功能描述: 白点参数修改
=========================================================*/
void parameter_control::on_pushButton_62_clicked()
{
    QMessageBox box(QMessageBox::Warning,"警告","确定修改白点参数？");
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec ();
    if(ok == QMessageBox::Ok)//确认修改参数
    {
        //==========================白点缺陷算法参数===========================
        whitePoint_step=ui->doubleSpinBox_653->text().toDouble();      //白点自适应分割领域块大小
        whitePoint_threshold=ui->doubleSpinBox_40->text().toDouble();  //白点自适应分割领阈值
        whitePoint_lowerArea=ui->spinBox_91->text().toDouble();        //面积下限
        whitePoint_higherArea=ui->spinBox_92->text().toDouble();       //面积上限
        scratchth=ui->doubleSpinBox_46->text().toDouble();             //白点排除划痕阈值大于则认为贴膜划痕
        bubbleth=ui->doubleSpinBox_655->text().toDouble();             //白点排除气泡阈值大于则认为贴膜气泡
        dotdomainnum = ui->doubleSpinBox_654->text().toDouble();       //白点连通域个数大于则认为噪点
        defectouterth= ui->doubleSpinBox_41->text().toDouble();   	   //白点缺陷处灰度均值与缺陷外围灰度差
        corewholeth = ui->doubleSpinBox_43->text().toDouble();         //白点缺陷中心点灰度值与整块疑似缺陷灰度均值差
        spotpeak = ui->doubleSpinBox_45->text().toDouble();            //白点缺陷最亮点与最暗点灰度差
        siecevariance= ui->doubleSpinBox_47->text().toDouble();        //侧光图中排除划痕参数
        whitePoint_w_h= ui->doubleSpinBox_656->text().toDouble();      //白点缺陷长宽比阈值

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        //相机侧光参数
        QString sq1=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(whitePoint_step).arg("whitePoint_step").arg(Model_Code);
        query.exec(sq1);
        QString sq2=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(whitePoint_threshold).arg("whitePoint_threshold").arg(Model_Code);
        query.exec(sq2);
        QString sq3=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(whitePoint_lowerArea).arg("whitePoint_lowerArea").arg(Model_Code);
        query.exec(sq3);
        QString sq4=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(whitePoint_higherArea).arg("whitePoint_higherArea").arg(Model_Code);
        query.exec(sq4);
        QString sq5=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(scratchth).arg("scratchth").arg(Model_Code);
        query.exec(sq5);
        QString sq6=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(bubbleth).arg("bubbleth").arg(Model_Code);
        query.exec(sq6);
        QString sq7=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(dotdomainnum).arg("dotdomainnum").arg(Model_Code);
        query.exec(sq7);
        QString sq8=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(defectouterth).arg("defectouterth").arg(Model_Code);
        query.exec(sq8);
        QString sq9=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(corewholeth).arg("corewholeth").arg(Model_Code);
        query.exec(sq9);
        QString sq10=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(spotpeak).arg("spotpeak").arg(Model_Code);
        query.exec(sq10);
        QString sq11=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(siecevariance).arg("siecevariance").arg(Model_Code);
        query.exec(sq11);
        QString sq12=QStringLiteral("update whitedot_para set 白点参数值='%0' where 白点参数变量='%1'and 产品型号编码='%2'").arg(whitePoint_w_h).arg("whitePoint_w_h").arg(Model_Code);
        query.exec(sq12);

        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        qDebug()<<conmmitsignal<<endl;

        if(conmmitsignal)
        {
            parameter_alter();//执行全局变量参数更新
            ui->label_648->setText("更新参数成功");
            delay1(500);
            this->destroy(true,true);
            parameter_control_show=false;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"提示","修改白点参数失败!");
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
            qDebug()<<query.lastError()<<endl;
            ui->label_648->setText("更新参数失败");
            delay1(500);
        }
    }
    else
    {
        this->destroy(true,true);
        parameter_control_show=false;
    }
}

/*=========================================================
* 函 数 名: on_pushButton_10_clicked
* 功能描述: 划伤参数修改
=========================================================*/

void parameter_control::on_pushButton_75_clicked()
{
    QMessageBox box(QMessageBox::Warning,"警告","确定修改划伤参数？");
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec ();
    if(ok == QMessageBox::Ok)//确认修改参数
    {
        //==========================划伤缺陷算法参数===========================
        scratch__bolckSize=ui->spinBox_485->text().toDouble(); //划伤自适应分割领域块大小
        scratch__delta= ui->doubleSpinBox_54->text().toDouble();//划伤自适应分割领阈值
        scratch_higherArea=ui->spinBox_93->text().toDouble();   //划伤面积上限
        scratch_lowerArea=ui->spinBox_94->text().toDouble();    //划伤面积下限
        scratch_lowerLongShort=ui->doubleSpinBox_52->text().toDouble();//划伤长宽比下限
        scratch_higherLongShort=ui->doubleSpinBox_724->text().toDouble();//划伤长宽比上限
        scratch_lowerWidth=ui->spinBox_487->text().toDouble();  //划伤宽度下限
        scratch_higherWidth= ui->spinBox_488->text().toDouble();//划伤宽度上限
        scratch_lowerLength=ui->spinBox_489->text().toDouble();//划伤长度下限
        scratch_higherLength=ui->spinBox_490->text().toDouble();//划伤长度上限限
        filmscratch=ui->doubleSpinBox_49->text().toDouble();    //贴膜划痕系数
        scratchbubbleth=ui->doubleSpinBox_50->text().toDouble(); //贴膜排除气泡系数1
        scratchbubbleth1=ui->doubleSpinBox_51->text().toDouble();//贴膜排除气泡系数2
        scratchbrighth=ui->doubleSpinBox_53->text().toDouble();//划伤亮度阈值
        minscratchbrighth=ui->doubleSpinBox_725->text().toDouble();//最小划伤亮度阈值

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        //划伤缺陷算法参数
        QString sq1=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratch__bolckSize).arg("scratch__bolckSize").arg(Model_Code);
        query.exec(sq1);
        QString sq2=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratch__delta).arg("scratch__delta").arg(Model_Code);
        query.exec(sq2);
        QString sq3=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratch_higherArea).arg("scratch_higherArea").arg(Model_Code);
        query.exec(sq3);
        QString sq4=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratch_lowerArea).arg("scratch_lowerArea").arg(Model_Code);
        query.exec(sq4);
        QString sq5=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratch_lowerLongShort).arg("scratch_lowerLongShort").arg(Model_Code);
        query.exec(sq5);
        QString sq6=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratch_higherLongShort).arg("scratch_higherLongShort").arg(Model_Code);
        query.exec(sq6);
        QString sq7=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratch_lowerWidth).arg("scratch_lowerWidth").arg(Model_Code);
        query.exec(sq7);
        QString sq8=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratch_higherWidth).arg("scratch_higherWidth").arg(Model_Code);
        query.exec(sq8);
        QString sq9=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratch_lowerLength).arg("scratch_lowerLength").arg(Model_Code);
        query.exec(sq9);
        QString sq10=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratch_higherLength).arg("scratch_higherLength").arg(Model_Code);
        query.exec(sq10);
        QString sq11=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(filmscratch).arg("filmscratch").arg(Model_Code);
        query.exec(sq11);
        QString sq12=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratchbubbleth).arg("scratchbubbleth").arg(Model_Code);
        query.exec(sq12);
        QString sq13=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratchbubbleth1).arg("scratchbubbleth1").arg(Model_Code);
        query.exec(sq13);
        QString sq14=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(scratchbrighth).arg("scratchbrighth").arg(Model_Code);
        query.exec(sq14);
        QString sq15=QStringLiteral("update scractch_para set 划伤参数值='%0' where 划伤参数变量='%1'and 产品型号编码='%2'").arg(minscratchbrighth).arg("minscratchbrighth").arg(Model_Code);
        query.exec(sq15);

        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        qDebug()<<conmmitsignal<<endl;

        if(conmmitsignal)
        {
            parameter_alter();//执行全局变量参数更新
            ui->label_648->setText("更新参数成功");
            delay1(500);
            this->destroy(true,true);
            parameter_control_show=false;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"提示","修改划伤参数失败!");
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
            qDebug()<<query.lastError()<<endl;
            ui->label_648->setText("更新参数失败");
            delay1(500);
        }
    }
    else
    {
        this->destroy(true,true);
        parameter_control_show=false;
    }
}

/*=========================================================
* 函 数 名: on_pushButton_10_clicked
* 功能描述: 亮点缺陷参数修改
=========================================================*/
void parameter_control::on_pushButton_119_clicked()
{
    QMessageBox box(QMessageBox::Warning,"警告","确定修改亮点参数？");
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec ();
    if(ok == QMessageBox::Ok)//确认修改参数
    {
        //=========================亮点缺陷算法参数============================
        black_light_gram=ui->doubleSpinBox_903->text().toDouble();//亮点亮度增强系数
        black_lightPoint_step= ui->spinBox_529->text().toDouble();//滑窗间隔步长
        black_lightPoint_size=ui->spinBox_532->text().toDouble(); //滑窗窗口大小
        black_lightPoint_gray_th=ui->spinBox_530->text().toDouble();//滑窗二值化亮度阈值
        black_lightPoint_area_upper=ui->spinBox_531->text().toDouble();//面积阈值上限
        black_lightPoint_area_lower= ui->doubleSpinBox_899->text().toDouble();//面积阈值下限
        black_lightPoint_diff=ui->spinBox_533->text().toDouble(); //亮点亮度阈值
        black_lightPoint_w_h=ui->doubleSpinBox_898->text().toDouble();//亮点缺陷长宽比

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        //亮点缺陷算法参数
        QString sq1=QStringLiteral("update lightpoint_para set 亮点参数值='%0' where 亮点参数变量='%1'and 产品型号编码='%2'").arg(black_light_gram).arg("black_light_gram").arg(Model_Code);
        query.exec(sq1);
        QString sq2=QStringLiteral("update lightpoint_para set 亮点参数值='%0' where 亮点参数变量='%1'and 产品型号编码='%2'").arg(black_lightPoint_step).arg("black_lightPoint_step").arg(Model_Code);
        query.exec(sq2);
        QString sq3=QStringLiteral("update lightpoint_para set 亮点参数值='%0' where 亮点参数变量='%1'and 产品型号编码='%2'").arg(black_lightPoint_size).arg("black_lightPoint_size").arg(Model_Code);
        query.exec(sq3);
        QString sq4=QStringLiteral("update lightpoint_para set 亮点参数值='%0' where 亮点参数变量='%1'and 产品型号编码='%2'").arg(black_lightPoint_gray_th).arg("black_lightPoint_gray_th").arg(Model_Code);
        query.exec(sq4);
        QString sq5=QStringLiteral("update lightpoint_para set 亮点参数值='%0' where 亮点参数变量='%1'and 产品型号编码='%2'").arg(black_lightPoint_area_upper).arg("black_lightPoint_area_upper").arg(Model_Code);
        query.exec(sq5);
        QString sq6=QStringLiteral("update lightpoint_para set 亮点参数值='%0' where 亮点参数变量='%1'and 产品型号编码='%2'").arg(black_lightPoint_area_lower).arg("black_lightPoint_area_lower").arg(Model_Code);
        query.exec(sq6);
        QString sq7=QStringLiteral("update lightpoint_para set 亮点参数值='%0' where 亮点参数变量='%1'and 产品型号编码='%2'").arg(black_lightPoint_diff).arg("black_lightPoint_diff").arg(Model_Code);
        query.exec(sq7);
        QString sq8=QStringLiteral("update lightpoint_para set 亮点参数值='%0' where 亮点参数变量='%1'and 产品型号编码='%2'").arg(black_lightPoint_w_h).arg("black_lightPoint_w_h").arg(Model_Code);
        query.exec(sq8);

        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        qDebug()<<conmmitsignal<<endl;

        if(conmmitsignal)
        {
            parameter_alter();//执行全局变量参数更新
            ui->label_648->setText("更新参数成功");
            delay1(500);
            this->destroy(true,true);
            parameter_control_show=false;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"提示","修改亮点参数失败!");
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
            qDebug()<<query.lastError()<<endl;
            ui->label_648->setText("更新参数失败");
            delay1(500);
        }
    }
    else
    {
        this->destroy(true,true);
        parameter_control_show=false;
    }
}

/*=========================================================
* 函 数 名: on_pushButton_10_clicked
* 功能描述: 红蓝斑缺陷参数修改
=========================================================*/
void parameter_control::on_pushButton_121_clicked()
{
    QMessageBox box(QMessageBox::Warning,"警告","确定修改红蓝斑参数？");
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec ();
    if(ok == QMessageBox::Ok)//确认修改参数
    {
        //=========================红蓝斑缺陷算法参数============================
        black_light_red=ui->doubleSpinBox_960->text().toDouble();//红蓝斑亮度增强系数
        redBlue_area_lower=ui->spinBox_574->text().toDouble();   //红蓝斑面积下限
        redBlue_area_upper=ui->spinBox_575->text().toDouble();   //红蓝斑面积上限
        redBlue_numupper= ui->spinBox_576->text().toDouble();    //红蓝斑判定阈值数目

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        //红蓝斑缺陷算法参数
        QString sq1=QStringLiteral("update redblue_para set 红蓝斑参数值='%0' where 红蓝斑参数变量='%1'and 产品型号编码='%2'").arg(black_light_red).arg("black_light_red").arg(Model_Code);
        query.exec(sq1);
        QString sq2=QStringLiteral("update redblue_para set 红蓝斑参数值='%0' where 红蓝斑参数变量='%1'and 产品型号编码='%2'").arg(redBlue_area_lower).arg("redBlue_area_lower").arg(Model_Code);
        query.exec(sq2);
        QString sq3=QStringLiteral("update redblue_para set 红蓝斑参数值='%0' where 红蓝斑参数变量='%1'and 产品型号编码='%2'").arg(redBlue_area_upper).arg("redBlue_area_upper").arg(Model_Code);
        query.exec(sq3);
        QString sq4=QStringLiteral("update redblue_para set 红蓝斑参数值='%0' where 红蓝斑参数变量='%1'and 产品型号编码='%2'").arg(redBlue_numupper).arg("redBlue_numupper").arg(Model_Code);
        query.exec(sq4);


        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        qDebug()<<conmmitsignal<<endl;

        if(conmmitsignal)
        {
            parameter_alter();//执行全局变量参数更新
            ui->label_648->setText("更新参数成功");
            delay1(500);
            this->destroy(true,true);
            parameter_control_show=false;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"提示","修改红蓝斑参数失败!");
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
            qDebug()<<query.lastError()<<endl;
            ui->label_648->setText("更新参数失败");
            delay1(500);
        }
    }
    else
    {
        this->destroy(true,true);
        parameter_control_show=false;
    }
}

/*=========================================================
* 函 数 名: on_pushButton_10_clicked
* 功能描述: 漏光缺陷参数修改
=========================================================*/
void parameter_control::on_pushButton_141_clicked()
{
    QMessageBox box(QMessageBox::Warning,"警告","确定修改漏光参数？");
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec ();
    if(ok == QMessageBox::Ok)//确认修改参数
    {
        //=========================漏光缺陷算法参数============================
        leakage_bolckSize=ui->spinBox_660->text().toDouble();   //黑底漏光适应分割领域块大小
        leakage_delta=ui->doubleSpinBox_1065->text().toDouble();//黑底漏光适应分割阈值大小
        leakage_area_lower=ui->spinBox_662->text().toDouble();  //黑底漏光面积下限
        leakage_area_upper=ui->spinBox_663->text().toDouble();  //黑底漏光面积上限
        leakageth=ui->doubleSpinBox_1066->text().toDouble();    //黑底漏光亮度阈值
        leakage_w_h=ui->doubleSpinBox_1067->text().toDouble();  //黑底漏光长宽比阈值

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        //漏光缺陷算法参数
        QString sq1=QStringLiteral("update leakage_para set 漏光参数值='%0' where 漏光参数变量='%1'and 产品型号编码='%2'").arg(leakage_bolckSize).arg("leakage_bolckSize").arg(Model_Code);
        query.exec(sq1);
        QString sq2=QStringLiteral("update leakage_para set 漏光参数值='%0' where 漏光参数变量='%1'and 产品型号编码='%2'").arg(leakage_delta).arg("leakage_delta").arg(Model_Code);
        query.exec(sq2);
        QString sq3=QStringLiteral("update leakage_para set 漏光参数值='%0' where 漏光参数变量='%1'and 产品型号编码='%2'").arg(leakage_area_lower).arg("leakage_area_lower").arg(Model_Code);
        query.exec(sq3);
        QString sq4=QStringLiteral("update leakage_para set 漏光参数值='%0' where 漏光参数变量='%1'and 产品型号编码='%2'").arg(leakage_area_upper).arg("leakage_area_upper").arg(Model_Code);
        query.exec(sq4);
        QString sq5=QStringLiteral("update leakage_para set 漏光参数值='%0' where 漏光参数变量='%1'and 产品型号编码='%2'").arg(leakageth).arg("leakageth").arg(Model_Code);
        query.exec(sq5);
        QString sq6=QStringLiteral("update leakage_para set 漏光参数值='%0' where 漏光参数变量='%1'and 产品型号编码='%2'").arg(leakage_w_h).arg("leakage_w_h").arg(Model_Code);
        query.exec(sq6);


        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        qDebug()<<conmmitsignal<<endl;

        if(conmmitsignal)
        {
            parameter_alter();//执行全局变量参数更新
            ui->label_1933->setText("更新参数成功");
            delay1(500);
            this->destroy(true,true);
            parameter_control_show=false;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"提示","修改漏光参数失败!");
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
            qDebug()<<query.lastError()<<endl;
            ui->label_1933->setText("更新参数失败");
            delay1(500);
        }
    }
    else
    {
        this->destroy(true,true);
        parameter_control_show=false;
    }
}

/*=========================================================
* 函 数 名: on_pushButton_10_clicked
* 功能描述: 显示异常缺陷参数修改
=========================================================*/
void parameter_control::on_pushButton_258_clicked()
{

    QMessageBox box(QMessageBox::Warning,"警告","确定修改显示异常参数？");
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec ();
    if(ok == QMessageBox::Ok)
    {
        //=========================显示异常缺陷算法参数============================
        white_gray_lower=ui->doubleSpinBox_704->text().toDouble();  //白色灰度下限，白底亮度过低认为显示异常
        black_gray_upper=ui->spinBox_705->text().toDouble();  //黑色灰度上限  黑底亮度过高认为显示异常

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);
        //漏光缺陷算法参数
        QString sq1=QStringLiteral("update displayerror_para set 显示异常参数值='%0' where 显示异常参数变量='%1'and 产品型号编码='%2'").arg(white_gray_lower).arg("white_gray_lower").arg(Model_Code);
        query.exec(sq1);
        QString sq2=QStringLiteral("update displayerror_para set 显示异常参数值='%0' where 显示异常参数变量='%1'and 产品型号编码='%2'").arg(black_gray_upper).arg("black_gray_upper").arg(Model_Code);
        query.exec(sq2);

        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        qDebug()<<conmmitsignal<<endl;

        if(conmmitsignal)
        {
            parameter_alter();//执行全局变量参数更新
            ui->label_648->setText("更新参数成功");
            delay1(500);
            this->destroy(true,true);
            parameter_control_show=false;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"提示","修改显示异常参数失败!");
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();
            qDebug()<<query.lastError()<<endl;
            ui->label_648->setText("更新参数失败");
            delay1(500);
        }
    }
    else
    {
        this->destroy(true,true);
        parameter_control_show=false;
    }
}

/*=========================================================
* 函 数 名: delay1
* 功能描述: 延时函数
=========================================================*/
void parameter_control::delay1(int MSecs)
{
    QTime dieTime= QTime::currentTime().addMSecs(MSecs);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

/*=========================================================
* 函 数 名: parameter_alter
* 功能描述: 参数修改函数，参数更新后调用此函数实现系统参数修改
=========================================================*/
void parameter_control::parameter_alter()
{
    //==========================海康相机曝光参数===========================
    QString sq=QStringLiteral("select *from camera_exposure where 产品型号编码='%0'").arg(Model_Code); //读取相机曝光参数
    QSqlQuery query(db);
    query.exec(sq);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("相机参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================海康相机曝光参数==============================
            Exposure_Ceguang_Front=list.at(0);            //前黑白相机侧光曝光值
            Exposure_Ceguang_Back=list.at(1);             //后黑白相机侧光曝光值
            Exposure_Ceguang_Left=list.at(2);             //左黑白相机侧光曝光值
            Exposure_Ceguang_Right=list.at(3);            //右黑白相机侧光曝光值
            Exposure_Ceguang_Main=list.at(4);             //主黑白相机侧光曝光值

            Exposure_White_Black_Left=list.at(5);;        //左黑白白底相机曝光值     5.5寸屏幕30万   6.0寸50万
            Exposure_White_Black_Middle=list.at(6);;      //主黑白白底相机曝光值    5.5寸屏幕2万    6.0寸3.5万
            Exposure_White_Color_Right=list.at(7);;       //右侧黑白相机白底曝光值   5.5寸屏幕30万   6.0寸50万
            Exposure_White_Black_Back=list.at(8);;        //后侧黑白相机白底曝光值
            Exposure_White_Black_Front=list.at(9);;       //前侧黑白相机白底曝光值
            Exposure_White_Color_Middle=list.at(10);;     //主彩色相机白底曝光值     5.5寸屏幕1800    6.0寸25000

            Exposure_Black_Middle=list.at(11);;           //主黑白相机黑色底色曝光值
            Exposure_Black_Color=list.at(12);;            //主彩色相机黑色底色曝光值    5.5寸78万

            Exposure_Gray_Main=list.at(13);              //主黑白相机灰色曝光值
            Exposure_Gray_Left=list.at(14);              //左黑白相机灰色曝光值
            Exposure_Gray_Right=list.at(14);             //右黑白相机灰色曝光值
        }
    }

    //==========================PLC设备延时参数===========================
    QString sq1=QStringLiteral("select *from plc_delaytime where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq1);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("延时参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================延时参数值==============================
            SideLightOpenTime=list.at(0);    //侧光灯打开延时
            BoxOpenTime=list.at(1);          //测试盒打开点亮白底延时
            BlackSwitchTime=list.at(2);      //屏幕白色切换到黑色延时
            GraySwitchTime=list.at(3);       //屏幕白色切换到黑色延时
            modbus_time=list.at(4);          //PLC读写指令延时
        }
    }

    //==========================少线算法参数===========================
    QString sq2=QStringLiteral("select *from lackline_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq2);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("少线参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================少线参数值==============================
            lackline_bolckSize=list.at(0);     //少线分割邻域块大小
            lackline_delta=list.at(1);         //少线分割邻域块阈值
            longShortlowerth=list.at(2);       //少线长宽比下限
            shortHigherth=list.at(3);          //少线宽度上限
            longLowerth=list.at(4);            //少线长度下限
            lackscratchth=list.at(5);          //少线排除贴膜划痕阈值
        }
    }

    //==========================异物算法参数===========================
    QString sq3=QStringLiteral("select *from yiwu_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq3);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("异物参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================异物参数值==============================
            yiwu_pre_size=list.at(0);          //异物分割邻域块大小
            yiwu_pre_th=list.at(1);            //异物分割邻域块阈值
            yiwu_area_lower=list.at(2);        //异物一次面积下限
            yiwu_area_upper=list.at(3);        //异物一次面积上限
            yiwu_k_w_h=list.at(4);             //异物长宽比，点状和线状
            yiwu_sec_size=list.at(5);          //异物2次分割邻域块大小
            yiwu_sec_th=list.at(6);            //异物2次分割邻域块阈值
            yiwu_sec_area_lower=list.at(7);    //异物二次面积下限
            yiwu_sec_area_upper=list.at(8);    //异物二次面积上限
            yiwu_lighth=list.at(9);            //异物亮度阈值
        }
    }

    //==========================白印算法参数===========================
    QString sq4=QStringLiteral("select *from mura_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq4);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("白印参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================白印参数值==============================
            black_mura_gray_th=list.at(0);           //亮度阈值主调节参数
            black_mura_area_lower=list.at(1);        //面积阈值下限
            black_mura_area_upper=list.at(2);        //面积阈值上限
            black_mura_boder=list.at(3);             //外围宽度范围
            black_mura_corner=list.at(4);            //边角宽度范围
            mura_bubbleth=list.at(5);                //边角宽度范围
            mura_centerlighth=list.at(6);            //白印缺陷排除气泡干扰阈值
            mura_lighth=list.at(7);                  //白印缺陷中心亮度阈值
            black_mura_area_1=list.at(8);            //一级灰度面积
            black_mura_th_1=list.at(9);              //一级灰度差值
            black_mura_area_2=list.at(10);           //二级灰度面积
            black_mura_th_2=list.at(11);             //二级灰度差值
        }
    }

    //==========================白点算法参数===========================
    QString sq5=QStringLiteral("select *from whitedot_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq5);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("白点参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================白点参数值==============================
            whitePoint_step=list.at(0);       //白点自适应分割领域块大小
            whitePoint_threshold=list.at(1);  //白点自适应分割领阈值
            whitePoint_lowerArea=list.at(2);  //面积下限
            whitePoint_higherArea=list.at(3); //面积上限
            scratchth=list.at(4);             //白点排除划痕阈值大于则认为贴膜划痕
            bubbleth=list.at(5);              //白点排除气泡阈值大于则认为贴膜气泡
            dotdomainnum = list.at(6);        //白点连通域个数大于则认为噪点
            defectouterth= list.at(7);   	  //白点缺陷处灰度均值与缺陷外围灰度差
            corewholeth =list.at(8);          //白点缺陷中心点灰度值与整块疑似缺陷灰度均值差
            spotpeak = list.at(9);            //白点缺陷最亮点与最暗点灰度差
            siecevariance= list.at(10);       //侧光图中排除划痕参数
            whitePoint_w_h= list.at(11);      //白点缺陷长宽比阈值
        }
    }

    //==========================划伤算法参数===========================
    QString sq6=QStringLiteral("select *from scractch_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq6);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("划伤参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================划伤参数值==============================
            scratch__bolckSize=list.at(0);      //划伤自适应分割领域块大小
            scratch__delta= list.at(1);         //划伤自适应分割领阈值
            scratch_higherArea=list.at(2);      //划伤面积上限
            scratch_lowerArea=list.at(3);       //划伤面积下限
            scratch_lowerLongShort=list.at(4);  //划伤长宽比下限
            scratch_higherLongShort=list.at(5); //划伤长宽比上限
            scratch_lowerWidth=list.at(6);      //划伤宽度下限
            scratch_higherWidth= list.at(7);    //划伤宽度上限
            scratch_lowerLength=list.at(8);     //划伤长度下限
            scratch_higherLength=list.at(9);    //划伤长度上限限
            filmscratch=list.at(10);            //贴膜划痕系数
            scratchbubbleth=list.at(11);        //贴膜排除气泡系数1
            scratchbubbleth1=list.at(12);       //贴膜排除气泡系数2
            scratchbrighth=list.at(13);         //划伤亮度阈值
            minscratchbrighth=list.at(14);      //最小划伤亮度阈值
        }
    }

    //==========================亮点算法参数===========================
    QString sq7=QStringLiteral("select *from lightpoint_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq7);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("亮点参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================亮点参数值==============================
            black_light_gram=list.at(0);             //亮点亮度增强系数
            black_lightPoint_step= list.at(1);       //滑窗间隔步长
            black_lightPoint_size=list.at(2);        //滑窗窗口大小
            black_lightPoint_gray_th=list.at(3);     //滑窗二值化亮度阈值
            black_lightPoint_area_lower=list.at(4);  //面积阈值下限
            black_lightPoint_area_upper= list.at(5); //面积阈值上限
            black_lightPoint_diff=list.at(6);        //亮点亮度阈值
            black_lightPoint_w_h=list.at(7);         //亮点缺陷长宽比
        }
    }

    //==========================红蓝斑算法参数===========================
    QString sq8=QStringLiteral("select *from redblue_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq8);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("红蓝斑参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================红蓝斑参数值==============================
            black_light_red=list.at(0);      //红蓝斑亮度增强系数
            redBlue_area_lower=list.at(1);   //红蓝斑面积下限
            redBlue_area_upper=list.at(2);   //红蓝斑面积上限
            redBlue_numupper= list.at(3);    //红蓝斑判定阈值数目
        }
    }

    //==========================漏光算法参数===========================
    QString sq9=QStringLiteral("select *from leakage_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq9);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("漏光参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================漏光参数值==============================
            leakage_bolckSize=list.at(0);        //黑底漏光适应分割领域块大小
            leakage_delta=list.at(1);            //黑底漏光适应分割阈值大小
            leakage_area_lower=list.at(2);       //黑底漏光面积下限
            leakage_area_upper=list.at(3);       //黑底漏光面积上限
            leakageth=list.at(4);                //黑底漏光亮度阈值
            leakage_w_h=list.at(5);              //黑底漏光长宽比阈值
        }
    }

    //==========================显示异常算法参数===========================
    QString sq10=QStringLiteral("select *from displayerror_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq10);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("显示异常参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================显示异常参数值==============================
            white_gray_lower=list.at(0);    //白色灰度下限，白底亮度过低认为显示异常
            black_gray_upper=list.at(1);    //黑色灰度上限  黑底亮度过高认为显示异常
        }
    }
}

/*====================================================================
* 类型：initial_para1
* 功能：算法参数初始化从数据库读取参数
======================================================================*/
void parameter_control::initial_para1()
{
    //==========================海康相机曝光参数===========================
    QString sq=QStringLiteral("select *from camera_exposure where 产品型号编码='%0'").arg(Model_Code);
    QSqlQuery query(db);                                       //执行操作类对象查询数据
    bool signal=query.exec(sq);                                //执行
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("相机参数值").toDouble();
            list.append(parameter);
        }
        if(!query.next())//未查询到结果的话赋予0值
        {
            //==========================海康相机曝光参数==============================
            Exposure_Ceguang_Front=0;            //前黑白相机侧光曝光值
            Exposure_Ceguang_Back=0;             //后黑白相机侧光曝光值
            Exposure_Ceguang_Left=0;             //左黑白相机侧光曝光值
            Exposure_Ceguang_Right=0;            //右黑白相机侧光曝光值
            Exposure_Ceguang_Main=0;             //主黑白相机侧光曝光值

            Exposure_White_Black_Left=0;        //左黑白白底相机曝光值
            Exposure_White_Black_Middle=0;      //主黑白白底相机曝光值
            Exposure_White_Color_Right=0;       //右侧黑白相机白底曝光值
            Exposure_White_Black_Back=0;        //后侧黑白相机白底曝光值
            Exposure_White_Black_Front=0;       //前侧黑白相机白底曝光值
            Exposure_White_Color_Middle=0;      //主彩色相机白底曝光值

            Exposure_Black_Middle=0;           //主黑白相机黑色底色曝光值
            Exposure_Black_Color=0;            //主彩色相机黑色底色曝光值
            Num_1_L_Exposure_white=0;          //主黑白相机灰色及其余颜色曝光值
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================海康相机曝光参数==============================
            Exposure_Ceguang_Front=list.at(0);            //前黑白相机侧光曝光值
            Exposure_Ceguang_Back=list.at(1);             //后黑白相机侧光曝光值
            Exposure_Ceguang_Left=list.at(2);             //左黑白相机侧光曝光值
            Exposure_Ceguang_Right=list.at(3);            //右黑白相机侧光曝光值
            Exposure_Ceguang_Main=list.at(4);             //主黑白相机侧光曝光值

            Exposure_White_Black_Left=list.at(5);        //左黑白白底相机曝光值     5.5寸屏幕30万   6.0寸50万
            Exposure_White_Black_Middle=list.at(6);      //主黑白白底相机曝光值    5.5寸屏幕2万    6.0寸3.5万
            Exposure_White_Color_Right=list.at(7);       //右侧黑白相机白底曝光值   5.5寸屏幕30万   6.0寸50万
            Exposure_White_Black_Back=list.at(8);        //后侧黑白相机白底曝光值
            Exposure_White_Black_Front=list.at(9);       //前侧黑白相机白底曝光值
            Exposure_White_Color_Middle=list.at(10);     //主彩色相机白底曝光值     5.5寸屏幕1800    6.0寸25000

            Exposure_Black_Middle=list.at(11);           //主黑白相机黑色底色曝光值
            Exposure_Black_Color=list.at(12);            //主彩色相机黑色底色曝光值    5.5寸78万

            Exposure_Gray_Main=list.at(13);              //主黑白相机灰色曝光值
            Exposure_Gray_Left=list.at(14);              //左黑白相机灰色曝光值
            Exposure_Gray_Right=list.at(15);             //右黑白相机灰色曝光值
        }
    }

    //==========================PLC设备延时参数===========================
    QString sq1=QStringLiteral("select *from plc_delaytime where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq1);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("延时参数值").toDouble();
            list.append(parameter);
        }
        if(!query.next())//未查询到结果的话赋予0值
        {
            //==========================延时参数值==============================
            SideLightOpenTime=0;    //侧光灯打开延时
            BoxOpenTime=0;          //测试盒打开点亮白底延时
            BlackSwitchTime=0;      //屏幕白色切换到黑色延时
            GraySwitchTime=0;       //屏幕白色切换到黑色延时
            modbus_time=0;          //PLC读写指令延时
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================延时参数值==============================
            SideLightOpenTime=list.at(0);    //侧光灯打开延时
            BoxOpenTime=list.at(1);          //测试盒打开点亮白底延时
            BlackSwitchTime=list.at(2);      //屏幕白色切换到黑色延时
            GraySwitchTime=list.at(3);       //屏幕白色切换到黑色延时
            modbus_time=list.at(4);          //PLC读写指令延时
        }
    }

    //==========================少线算法参数===========================
    QString sq2=QStringLiteral("select *from lackline_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq2);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("少线参数值").toDouble();
            list.append(parameter);
        }
        if(!query.next())//未查询到结果的话赋予0值
        {
            //==========================少线参数值==============================
            lackline_bolckSize=0;     //少线分割邻域块大小
            lackline_delta=0;         //少线分割邻域块阈值
            longShortlowerth=0;       //少线长宽比下限
            shortHigherth=0;          //少线宽度上限
            longLowerth=0;            //少线长度下限
            lackscratchth=0;          //少线排除贴膜划痕阈值
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================少线参数值==============================
            lackline_bolckSize=list.at(0);     //少线分割邻域块大小
            lackline_delta=list.at(1);         //少线分割邻域块阈值
            longShortlowerth=list.at(2);       //少线长宽比下限
            shortHigherth=list.at(3);          //少线宽度上限
            longLowerth=list.at(4);            //少线长度下限
            lackscratchth=list.at(5);          //少线排除贴膜划痕阈值
        }
    }

    //==========================异物算法参数===========================
    QString sq3=QStringLiteral("select *from yiwu_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq3);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("异物参数值").toDouble();
            list.append(parameter);
        }
        if(!query.next())//未查询到结果的话赋予0值
        {
            //==========================异物参数值==============================
            yiwu_pre_size=0;          //异物分割邻域块大小
            yiwu_pre_th=0;            //异物分割邻域块阈值
            yiwu_area_lower=0;        //异物一次面积下限
            yiwu_area_upper=0;        //异物一次面积上限
            yiwu_k_w_h=0;             //异物长宽比，点状和线状
            yiwu_sec_size=0;          //异物2次分割邻域块大小
            yiwu_sec_th=0;            //异物2次分割邻域块阈值
            yiwu_sec_area_lower=0;    //异物二次面积下限
            yiwu_sec_area_upper=0;    //异物二次面积上限
            yiwu_lighth=0;            //异物亮度阈值
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================异物参数值==============================
            yiwu_pre_size=list.at(0);          //异物分割邻域块大小
            yiwu_pre_th=list.at(1);            //异物分割邻域块阈值
            yiwu_area_lower=list.at(2);        //异物一次面积下限
            yiwu_area_upper=list.at(3);        //异物一次面积上限
            yiwu_k_w_h=list.at(4);             //异物长宽比，点状和线状
            yiwu_sec_size=list.at(5);          //异物2次分割邻域块大小
            yiwu_sec_th=list.at(6);            //异物2次分割邻域块阈值
            yiwu_sec_area_lower=list.at(7);    //异物二次面积下限
            yiwu_sec_area_upper=list.at(8);    //异物二次面积上限
            yiwu_lighth=list.at(9);            //异物亮度阈值
        }
    }

    //==========================白印算法参数===========================
    QString sq4=QStringLiteral("select *from mura_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq4);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("白印参数值").toDouble();
            list.append(parameter);
        }
        if(!query.next())//未查询到结果的话赋予0值
        {
            //==========================白印参数值==============================
            black_mura_gray_th=0;           //亮度阈值主调节参数
            black_mura_area_lower=0;        //面积阈值下限
            black_mura_area_upper=0;        //面积阈值上限
            black_mura_boder=0;             //外围宽度范围
            black_mura_corner=0;            //边角宽度范围
            mura_bubbleth=0;                //边角宽度范围
            mura_centerlighth=0;            //白印缺陷排除气泡干扰阈值
            mura_lighth=0;                  //白印缺陷中心亮度阈值
            black_mura_area_1=0;            //一级灰度面积
            black_mura_th_1=0;              //一级灰度差值
            black_mura_area_2=0;           //二级灰度面积
            black_mura_th_2=0;             //二级灰度差值
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================白印参数值==============================
            black_mura_gray_th=list.at(0);           //亮度阈值主调节参数
            black_mura_area_lower=list.at(1);        //面积阈值下限
            black_mura_area_upper=list.at(2);        //面积阈值上限
            black_mura_boder=list.at(3);             //外围宽度范围
            black_mura_corner=list.at(4);            //边角宽度范围
            mura_bubbleth=list.at(5);                //边角宽度范围
            mura_centerlighth=list.at(6);            //白印缺陷排除气泡干扰阈值
            mura_lighth=list.at(7);                  //白印缺陷中心亮度阈值
            black_mura_area_1=list.at(8);            //一级灰度面积
            black_mura_th_1=list.at(9);              //一级灰度差值
            black_mura_area_2=list.at(10);           //二级灰度面积
            black_mura_th_2=list.at(11);             //二级灰度差值
        }
    }

    //==========================白点算法参数===========================
    QString sq5=QStringLiteral("select *from whitedot_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq5);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("白点参数值").toDouble();
            list.append(parameter);
        }
        if(!query.next())//未查询到结果的话赋予0值
        {
            //==========================白点参数值==============================
            whitePoint_step=0;       //白点自适应分割领域块大小
            whitePoint_threshold=0;  //白点自适应分割领阈值
            whitePoint_lowerArea=0;  //面积下限
            whitePoint_higherArea=0; //面积上限
            scratchth=0;             //白点排除划痕阈值大于则认为贴膜划痕
            bubbleth=0;              //白点排除气泡阈值大于则认为贴膜气泡
            dotdomainnum = 0;        //白点连通域个数大于则认为噪点
            defectouterth= 0;   	 //白点缺陷处灰度均值与缺陷外围灰度差
            corewholeth =0;          //白点缺陷中心点灰度值与整块疑似缺陷灰度均值差
            spotpeak = 0;            //白点缺陷最亮点与最暗点灰度差
            siecevariance= 0;        //侧光图中排除划痕参数
            whitePoint_w_h= 0;       //白点缺陷长宽比阈值
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================白点参数值==============================
            whitePoint_step=list.at(0);       //白点自适应分割领域块大小
            whitePoint_threshold=list.at(1);  //白点自适应分割领阈值
            whitePoint_lowerArea=list.at(2);  //面积下限
            whitePoint_higherArea=list.at(3); //面积上限
            scratchth=list.at(4);             //白点排除划痕阈值大于则认为贴膜划痕
            bubbleth=list.at(5);              //白点排除气泡阈值大于则认为贴膜气泡
            dotdomainnum = list.at(6);        //白点连通域个数大于则认为噪点
            defectouterth= list.at(7);   	  //白点缺陷处灰度均值与缺陷外围灰度差
            corewholeth =list.at(8);          //白点缺陷中心点灰度值与整块疑似缺陷灰度均值差
            spotpeak = list.at(9);            //白点缺陷最亮点与最暗点灰度差
            siecevariance= list.at(10);       //侧光图中排除划痕参数
            whitePoint_w_h= list.at(11);      //白点缺陷长宽比阈值
        }
    }

    //==========================划伤算法参数===========================
    QString sq6=QStringLiteral("select *from scractch_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq6);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("划伤参数值").toDouble();
            list.append(parameter);
        }
        if(!query.next())//未查询到结果的话赋予0值
        {
            //==========================划伤参数值==============================
            scratch__bolckSize=0;      //划伤自适应分割领域块大小
            scratch__delta= 0;         //划伤自适应分割领阈值
            scratch_higherArea=0;      //划伤面积上限
            scratch_lowerArea=0;       //划伤面积下限
            scratch_lowerLongShort=0;  //划伤长宽比下限
            scratch_higherLongShort=0; //划伤长宽比上限
            scratch_lowerWidth=0;      //划伤宽度下限
            scratch_higherWidth= 0;    //划伤宽度上限
            scratch_lowerLength=0;     //划伤长度下限
            scratch_higherLength=0;    //划伤长度上限限
            filmscratch=0;            //贴膜划痕系数
            scratchbubbleth=0;        //贴膜排除气泡系数1
            scratchbubbleth1=0;       //贴膜排除气泡系数2
            scratchbrighth=0;         //划伤亮度阈值
            minscratchbrighth=0;      //最小划伤亮度阈值
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================划伤参数值==============================
            scratch__bolckSize=list.at(0);      //划伤自适应分割领域块大小
            scratch__delta= list.at(1);         //划伤自适应分割领阈值
            scratch_higherArea=list.at(2);      //划伤面积上限
            scratch_lowerArea=list.at(3);       //划伤面积下限
            scratch_lowerLongShort=list.at(4);  //划伤长宽比下限
            scratch_higherLongShort=list.at(5); //划伤长宽比上限
            scratch_lowerWidth=list.at(6);      //划伤宽度下限
            scratch_higherWidth= list.at(7);    //划伤宽度上限
            scratch_lowerLength=list.at(8);     //划伤长度下限
            scratch_higherLength=list.at(9);    //划伤长度上限限
            filmscratch=list.at(10);            //贴膜划痕系数
            scratchbubbleth=list.at(11);        //贴膜排除气泡系数1
            scratchbubbleth1=list.at(12);       //贴膜排除气泡系数2
            scratchbrighth=list.at(13);         //划伤亮度阈值
            minscratchbrighth=list.at(14);      //最小划伤亮度阈值
        }
    }

    //==========================亮点算法参数===========================
    QString sq7=QStringLiteral("select *from lightpoint_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq7);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("亮点参数值").toDouble();
            list.append(parameter);
        }
        if(!query.next())//未查询到结果的话赋予0值
        {
            //==========================亮点参数值==============================
            black_light_gram=0;             //亮点亮度增强系数
            black_lightPoint_step=0;        //滑窗间隔步长
            black_lightPoint_size=0;        //滑窗窗口大小
            black_lightPoint_gray_th=0;     //滑窗二值化亮度阈值
            black_lightPoint_area_upper=0;  //面积阈值上限
            black_lightPoint_area_lower= 0; //面积阈值下限
            black_lightPoint_diff=0;        //亮点亮度阈值
            black_lightPoint_w_h=0;         //亮点缺陷长宽比
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================亮点参数值==============================
            black_light_gram=list.at(0);             //亮点亮度增强系数
            black_lightPoint_step= list.at(1);       //滑窗间隔步长
            black_lightPoint_size=list.at(2);        //滑窗窗口大小
            black_lightPoint_gray_th=list.at(3);     //滑窗二值化亮度阈值
            black_lightPoint_area_lower=list.at(4);  //面积阈值下限
            black_lightPoint_area_upper= list.at(5); //面积阈值上限
            black_lightPoint_diff=list.at(6);        //亮点亮度阈值
            black_lightPoint_w_h=list.at(7);         //亮点缺陷长宽比
        }
    }

    //==========================红蓝斑算法参数===========================
    QString sq8=QStringLiteral("select *from redblue_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq8);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("红蓝斑参数值").toDouble();
            list.append(parameter);
        }
        if(!query.next())//未查询到结果的话赋予0值
        {
            //==========================红蓝斑参数值==============================
            black_light_red=0;      //红蓝斑亮度增强系数
            redBlue_area_lower=0;   //红蓝斑面积下限
            redBlue_area_upper=0;   //红蓝斑面积上限
            redBlue_numupper= 0;    //红蓝斑判定阈值数目
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================红蓝斑参数值==============================
            black_light_red=list.at(0);      //红蓝斑亮度增强系数
            redBlue_area_lower=list.at(1);   //红蓝斑面积下限
            redBlue_area_upper=list.at(2);   //红蓝斑面积上限
            redBlue_numupper= list.at(3);    //红蓝斑判定阈值数目
        }
    }

    //==========================漏光算法参数===========================
    QString sq9=QStringLiteral("select *from leakage_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq9);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("漏光参数值").toDouble();
            list.append(parameter);
        }
        if(!query.next())//未查询到结果的话赋予0值
        {
            //==========================漏光参数值==============================
            leakage_bolckSize=0;        //黑底漏光适应分割领域块大小
            leakage_delta=0;            //黑底漏光适应分割阈值大小
            leakage_area_lower=0;       //黑底漏光面积下限
            leakage_area_upper=0;       //黑底漏光面积上限
            leakageth=0;                //黑底漏光亮度阈值
            leakage_w_h=0;              //黑底漏光长宽比阈值
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================漏光参数值==============================
            leakage_bolckSize=list.at(0);        //黑底漏光适应分割领域块大小
            leakage_delta=list.at(1);            //黑底漏光适应分割阈值大小
            leakage_area_lower=list.at(2);       //黑底漏光面积下限
            leakage_area_upper=list.at(3);       //黑底漏光面积上限
            leakageth=list.at(4);                //黑底漏光亮度阈值
            leakage_w_h=list.at(5);              //黑底漏光长宽比阈值
        }
    }

    //==========================显示异常算法参数===========================
    QString sq10=QStringLiteral("select *from displayerror_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq10);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("显示异常参数值").toDouble();
            list.append(parameter);
        }
        if(!query.next())//未查询到结果的话赋予0值
        {
            white_gray_lower=0;    //白色灰度下限，白底亮度过低认为显示异常
            black_gray_upper=0;    //黑色灰度上限  黑底亮度过高认为显示异常
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================显示异常参数值==============================
            white_gray_lower=list.at(0);    //白色灰度下限，白底亮度过低认为显示异常
            black_gray_upper=list.at(1);    //黑色灰度上限  黑底亮度过高认为显示异常
        }
    }

    black_colorline=0.9;          //彩线亮度增强系数
    //==========================测试数据显示信息栏============================
//    test_num=0;        //测试总数目
//    qualified_num=0;   //合格总数目
//    yiwu_num=0;        //异物缺陷数目
//    liangdian_num=0;   //亮点
//    white_num=0;       //白点数目
//    mura_num=0;        //白印数目
//    lackline_num=0;    //少线数目
//    leakage_num=0;     //漏光数目
//    redBlue_num=0;     //红蓝斑数目
//    scratch_num=0;     //划伤数目
//    dispalyError_num=0;//显示异常数目
//    pass_per=0;        //合格率
}


void parameter_control::on_spinBox_23_valueChanged(const QString &arg1)
{
//    Yiwu_Detect_Grade_After=ui->spinBox_23->text().toDouble();

//    //异物亮度阈值随等级变化
//    yiwu_lighth=yiwu_lighth-(Yiwu_Detect_Grade_After-Yiwu_Detect_Grade);
//    ui->doubleSpinBox_19->setValue(yiwu_lighth);

//    //异物面积下限随等级变化
//    if(Yiwu_Detect_Grade_After==5)
//    {
//        ui->spinBox_800->setValue(0);           //异物一次面积下限
//        ui->spinBox_804->setValue(0);           //异物二次面积下限
//    }
//    if(Yiwu_Detect_Grade_After==4)
//    {
//        ui->spinBox_800->setValue(2);           //异物一次面积下限
//        ui->spinBox_804->setValue(2);           //异物二次面积下限
//    }
//    if(Yiwu_Detect_Grade_After==3)
//    {
//        ui->spinBox_800->setValue(3);           //异物一次面积下限
//        ui->spinBox_804->setValue(9);           //异物二次面积下限
//    }
//    if(Yiwu_Detect_Grade_After==2)
//    {
//        ui->spinBox_800->setValue(3);           //异物一次面积下限
//        ui->spinBox_804->setValue(9);           //异物二次面积下限
//    }
//    if(Yiwu_Detect_Grade_After==1)
//    {
//        ui->spinBox_800->setValue(3);           //异物一次面积下限
//        ui->spinBox_804->setValue(9);           //异物二次面积下限
//    }

//    Yiwu_Detect_Grade=Yiwu_Detect_Grade_After;
}

void parameter_control::on_pushButton_9_clicked()
{
    this->destroy(true,true);
}

void parameter_control::on_spinBox_26_valueChanged(const QString &arg1)
{
//    Whitepoint_Detect_Grade_After=ui->spinBox_26->text().toDouble();

//    //白点参数随等级变化
//    spotpeak=spotpeak-(Whitepoint_Detect_Grade_After-Whitepoint_Detect_Grade);//白点缺陷最亮点与最暗点灰度差
//    corewholeth=corewholeth-(Whitepoint_Detect_Grade_After-Whitepoint_Detect_Grade)*2;//白点缺陷中心点灰度值与整块疑似缺陷灰度均值差
//    siecevariance=siecevariance+(Whitepoint_Detect_Grade_After-Whitepoint_Detect_Grade)*1.5;//侧光图中排除划痕参数
//    defectouterth=defectouterth-(Whitepoint_Detect_Grade_After-Whitepoint_Detect_Grade)*1.5;//白点缺陷处灰度均值与缺陷外围灰度差
//    bubbleth=bubbleth+(Whitepoint_Detect_Grade_After-Whitepoint_Detect_Grade)*2;//白点排除气泡阈值大于则认为贴膜气泡

//    //白点排除划痕阈值大于则认为贴膜划痕
//    if(Whitepoint_Detect_Grade_After==5||Whitepoint_Detect_Grade_After==4)
//    {
//        ui->doubleSpinBox_46->setValue(2);           //白点排除划痕阈值大于则认为贴膜划痕
//    }
//    if(Whitepoint_Detect_Grade_After==3||Whitepoint_Detect_Grade_After==2||Whitepoint_Detect_Grade_After==1)
//    {
//        ui->doubleSpinBox_46->setValue(1);           //白点排除划痕阈值大于则认为贴膜划痕
//    }

//    Whitepoint_Detect_Grade=Whitepoint_Detect_Grade_After;

////    ui->doubleSpinBox_653->setValue(whitePoint_step);    //白点自适应分割领域块大小
////    ui->doubleSpinBox_40->setValue(whitePoint_threshold);//白点自适应分割领阈值
////    ui->spinBox_91->setValue(whitePoint_lowerArea);      //面积下限
////    ui->spinBox_92->setValue(whitePoint_higherArea);     //面积上限
//    //ui->doubleSpinBox_46->setValue(scratchth);           //白点排除划痕阈值大于则认为贴膜划痕
//    ui->doubleSpinBox_655->setValue(bubbleth);           //白点排除气泡阈值大于则认为贴膜气泡
////    ui->doubleSpinBox_654->setValue(dotdomainnum);       //白点连通域个数大于则认为噪点
//    ui->doubleSpinBox_41->setValue(defectouterth);       //白点缺陷处灰度均值与缺陷外围灰度差
//    ui->doubleSpinBox_43->setValue(corewholeth);         //白点缺陷中心点灰度值与整块疑似缺陷灰度均值差
//    ui->doubleSpinBox_45->setValue(spotpeak);            //白点缺陷最亮点与最暗点灰度差
//    ui->doubleSpinBox_47->setValue(siecevariance);       //侧光图中排除划痕参数
//    ui->doubleSpinBox_656->setValue(whitePoint_w_h);     //白点缺陷长宽比阈值
}
void parameter_control::on_spinBox_25_valueChanged(const QString &arg1) //白印缺陷等级调整
{
    Whiteprint_Detect_Grade_After=ui->spinBox_25->text().toDouble();
    Whiteprint_Detect_Flag = Whiteprint_Detect_Grade_After;//白印缺陷检测等级，全局变量
    if(Whiteprint_Detect_Grade_After == 5)
    {
        ui->spinBox_65->setValue(380);//白印缺陷面积下限
    }
    else {
        ui->spinBox_65->setValue(1200);//白印缺陷面积下限
    }
    Whiteprint_Detect_Grade=Whiteprint_Detect_Grade_After;
}


void parameter_control::on_pushButton_4_clicked()
{
    this->destroy(true,true);
    parameter_control_show=false;
}

void parameter_control::on_pushButton_7_clicked()
{
    this->destroy(true,true);
    parameter_control_show=false;
}

void parameter_control::on_pushButton_11_clicked()
{
    this->destroy(true,true);
    parameter_control_show=false;
}

void parameter_control::on_pushButton_61_clicked()
{
    this->destroy(true,true);
    parameter_control_show=false;
}

void parameter_control::on_pushButton_76_clicked()
{
    this->destroy(true,true);
    parameter_control_show=false;
}

void parameter_control::on_pushButton_120_clicked()
{
    this->destroy(true,true);
    parameter_control_show=false;
}

void parameter_control::on_pushButton_122_clicked()
{
    this->destroy(true,true);
    parameter_control_show=false;
}

void parameter_control::on_pushButton_142_clicked()
{
    this->destroy(true,true);
    parameter_control_show=false;
}

void parameter_control::on_pushButton_259_clicked()
{
    this->destroy(true,true);
    parameter_control_show=false;
}

void parameter_control::closeEvent(QCloseEvent *event)
{
    this->destroy(true,true);
    parameter_control_show=false;
}
