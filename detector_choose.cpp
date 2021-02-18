#include "detector_choose.h"
#include "ui_detector_choose.h"
#include "dbhelper.h"
#include <QtSql/QSqlQuery>

bool ForeignBody_Checked; //异物检测标志位
bool LessMaterial_Checked;//少料检测标志位
bool CrossLine_Checked; //十字检测标志位
bool boom_light_Checked;//爆灯检测标志位
bool Frame_Checked; //边框检测标志位
bool mura_Checked;  //白印、膜拱、白团检测标志位
bool LampEye_Checked; //灯眼检测标志位
bool Brightedge_Checked;  //亮边检测标志位
bool WhiteDot_Checked; //白点检测标志位
bool Scratch_Checked;  //划伤检测标志位
bool Abshow_White_Detect_Checked; //显异、暗区检测标志位
bool Shitfing_Checked;//移位标志位
bool DarkCorner_Checked;  //暗角检测标志位
bool SunSpots_Checked;   //色差检测标志位
bool Light_leakage_Checked;  //漏光检测标志位


Detector_Choose::Detector_Choose(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Detector_Choose)
{
    ui->setupUi(this);
    ini_state();
//    ui->Cbx_CrossLine->setVisible(false);  //十字屏蔽
//    ui->Cbx_SunSpots->setVisible(false);   //色差屏蔽
}



Detector_Choose::~Detector_Choose()
{
    delete ui;
}


void Detector_Choose::ini_state()
{

    QSqlQuery query(db);
    QString sql="SELECT 是否检测 FROM defect_type";
    bool signal=query.exec(sql);                                //执行
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "获取缺陷状态错误!");
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
            ui->Cbx_ForeignBody->setCheckState(Qt::Unchecked);
        else
             ui->Cbx_ForeignBody->setCheckState(Qt::Checked);
//        if(list.at(1)=="0")  //少料
//            ui->Cbx_LessMaterial->setCheckState(Qt::Unchecked);
//        else
//             ui->Cbx_LessMaterial->setCheckState(Qt::Checked);
//        if(list.at(2)=="0")  //十字
//            ui->Cbx_CrossLine->setCheckState(Qt::Unchecked);
//        else
//             ui->Cbx_CrossLine->setCheckState(Qt::Checked);
        if(list.at(3)=="0")  //爆灯
            ui->Cbx_boom_light->setCheckState(Qt::Unchecked);
        else
             ui->Cbx_boom_light->setCheckState(Qt::Checked);
//        if(list.at(4)=="0")  //边框
//            ui->Cbx_Frame->setCheckState(Qt::Unchecked);
//        else
//             ui->Cbx_Frame->setCheckState(Qt::Checked);
        if(list.at(5)=="0")  //白印-膜拱-白团
            ui->Cbx_mura->setCheckState(Qt::Unchecked);
        else
             ui->Cbx_mura->setCheckState(Qt::Checked);
        if(list.at(6)=="0")  //灯眼
            ui->Cbx_LampEye->setCheckState(Qt::Unchecked);
        else
             ui->Cbx_LampEye->setCheckState(Qt::Checked);
        if(list.at(7)=="0")  //亮边
            ui->Cbx_Brightedge->setCheckState(Qt::Unchecked);
        else
             ui->Cbx_Brightedge->setCheckState(Qt::Checked);
        if(list.at(8)=="0")  //白点
            ui->Cbx_WhiteDot->setCheckState(Qt::Unchecked);
        else
             ui->Cbx_WhiteDot->setCheckState(Qt::Checked);
        if(list.at(9)=="0")  //划伤
            ui->Cbx_Scratch->setCheckState(Qt::Unchecked);
        else
             ui->Cbx_Scratch->setCheckState(Qt::Checked);
        if(list.at(10)=="0")  //显异-暗区
            ui->Cbx_Abshow_White_Detect->setCheckState(Qt::Unchecked);
        else
             ui->Cbx_Abshow_White_Detect->setCheckState(Qt::Checked);
//        if(list.at(11)=="0")  //暗角
//            ui->Cbx_DarkCorner->setCheckState(Qt::Unchecked);
//        else
//             ui->Cbx_DarkCorner->setCheckState(Qt::Checked);
//        if(list.at(12)=="0")  //色差
//            ui->Cbx_SunSpots->setCheckState(Qt::Unchecked);
//        else
//             ui->Cbx_SunSpots->setCheckState(Qt::Checked);
//        if(list.at(13)=="0")  //漏光
//            ui->Cbx_Light_leakage->setCheckState(Qt::Unchecked);
//        else
//             ui->Cbx_Light_leakage->setCheckState(Qt::Checked);


    }

}

void Detector_Choose::on_Cbx_ForeignBody_stateChanged(int arg1)
{
    ForeignBody_Checked=ui->Cbx_ForeignBody->isChecked();
}


void Detector_Choose::on_btn_sure_clicked()
{
     ForeignBody_Checked=ui->Cbx_ForeignBody->isChecked();
//     LessMaterial_Checked=ui->Cbx_LessMaterial->isChecked();
     LessMaterial_Checked=false;  //少料屏蔽
//     CrossLine_Checked=ui->Cbx_CrossLine->isChecked();
     CrossLine_Checked=false;  //十字屏蔽
     boom_light_Checked=ui->Cbx_boom_light->isChecked();
//     Frame_Checked=ui->Cbx_Frame->isChecked();
     Frame_Checked=false;  //变形屏蔽
     mura_Checked=ui->Cbx_mura->isChecked();
     LampEye_Checked=ui->Cbx_LampEye->isChecked();
     Brightedge_Checked=ui->Cbx_Brightedge->isChecked();
     WhiteDot_Checked=ui->Cbx_WhiteDot->isChecked();
     Scratch_Checked=ui->Cbx_Scratch->isChecked();
     Abshow_White_Detect_Checked=ui->Cbx_Abshow_White_Detect->isChecked();
//     DarkCorner_Checked=ui->Cbx_DarkCorner->isChecked();
     DarkCorner_Checked=false;  //暗角屏蔽
//     SunSpots_Checked=ui->Cbx_SunSpots->isChecked();
     SunSpots_Checked=false;  //色差屏蔽
//     Light_leakage_Checked=ui->Cbx_Light_leakage->isChecked();

     db.transaction();
     QSqlQuery query(db);
     QString sql1=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '背光异物'").arg(ForeignBody_Checked);
     query.exec(sql1);

     QString sql2=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '少料'").arg(LessMaterial_Checked);
     query.exec(sql2);

     QString sql3=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '十字'").arg(CrossLine_Checked);
     query.exec(sql3);

     QString sql4=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '爆灯'").arg(boom_light_Checked);
     query.exec(sql4);

     QString sql5=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '边框'").arg(Frame_Checked);
     query.exec(sql5);

     QString sql6=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '白印-膜拱-白团'").arg(mura_Checked);
     query.exec(sql6);

     QString sql7=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '灯眼'").arg(LampEye_Checked);
     query.exec(sql7);

     QString sql8=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '亮边'").arg(Brightedge_Checked);
     query.exec(sql8);

     QString sql9=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '白点'").arg(WhiteDot_Checked);
     query.exec(sql9);

     QString sql10=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '划伤'").arg(Scratch_Checked);
     query.exec(sql10);

     QString sql11=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '显异-暗区'").arg(Abshow_White_Detect_Checked);
     query.exec(sql11);

     QString sql12=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '暗角'").arg(DarkCorner_Checked);
     query.exec(sql12);

     QString sql13=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '色差'").arg(SunSpots_Checked);
     query.exec(sql13);

     QString sql14=QStringLiteral("update defect_type set 是否检测='%1' where 缺陷名称= '漏光'").arg(Light_leakage_Checked);
     query.exec(sql14);

     bool conmmitsignal=db.commit();//提交事务执行SQL语句
     qDebug()<<conmmitsignal<<endl;
     if(!conmmitsignal)
     {
         QMessageBox box(QMessageBox::Warning,"提示","设置状态失败!");
         box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
         box.setButtonText (QMessageBox::Ok,QString("确 定"));
         box.setButtonText (QMessageBox::Cancel,QString("取 消"));
         box.exec ();
         qDebug()<<query.lastError()<<endl;
     }
     else
     {
         this->accept();
         this->destroy(true,true);

     }
}



void Detector_Choose::on_btn_Cancel_clicked()
{
    this->accept();
    this->destroy(true,true);
}





void Detector_Choose::on_combx_SelectAllOrNo_activated(const QString &arg1)
{
    if(ui->combx_SelectAllOrNo->currentText()=="全选")
    {
        ui->Cbx_ForeignBody->setCheckState(Qt::Checked);
//        ui->Cbx_LessMaterial->setCheckState(Qt::Checked);
//        ui->Cbx_CrossLine->setCheckState(Qt::Checked);
        ui->Cbx_boom_light->setCheckState(Qt::Checked);
//        ui->Cbx_Frame->setCheckState(Qt::Checked);
        ui->Cbx_mura->setCheckState(Qt::Checked);
        ui->Cbx_LampEye->setCheckState(Qt::Checked);
        ui->Cbx_Brightedge->setCheckState(Qt::Checked);
        ui->Cbx_WhiteDot->setCheckState(Qt::Checked);
        ui->Cbx_Scratch->setCheckState(Qt::Checked);
        ui->Cbx_Abshow_White_Detect->setCheckState(Qt::Checked);
//        ui->Cbx_DarkCorner->setCheckState(Qt::Checked);
//        ui->Cbx_SunSpots->setCheckState(Qt::Checked);
//        ui->Cbx_Light_leakage->setCheckState(Qt::Checked);
    }
    else if(ui->combx_SelectAllOrNo->currentText()=="全不选")
    {
        ui->Cbx_ForeignBody->setCheckState(Qt::Unchecked);
//        ui->Cbx_LessMaterial->setCheckState(Qt::Unchecked);
//        ui->Cbx_CrossLine->setCheckState(Qt::Unchecked);
        ui->Cbx_boom_light->setCheckState(Qt::Unchecked);
//        ui->Cbx_Frame->setCheckState(Qt::Unchecked);
        ui->Cbx_mura->setCheckState(Qt::Unchecked);
        ui->Cbx_LampEye->setCheckState(Qt::Unchecked);
        ui->Cbx_Brightedge->setCheckState(Qt::Unchecked);
        ui->Cbx_WhiteDot->setCheckState(Qt::Unchecked);
        ui->Cbx_Scratch->setCheckState(Qt::Unchecked);
        ui->Cbx_Abshow_White_Detect->setCheckState(Qt::Unchecked);
//        ui->Cbx_DarkCorner->setCheckState(Qt::Unchecked);
//        ui->Cbx_SunSpots->setCheckState(Qt::Unchecked);
//        ui->Cbx_Light_leakage->setCheckState(Qt::Unchecked);
    }

}
