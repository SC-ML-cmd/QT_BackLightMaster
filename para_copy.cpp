#include "para_copy.h"
#include "ui_para_copy.h"
#include<QDebug>
#include <QMessageBox>
#include <QSqlRecord>
#include"global_variable.h"     //全局变量头文件

QString Modcopy_aft;    //复制后厂家产品型号
QString Mode_Code_bef;  //复制前内部产品型号编码
QString Mode_Code_aft;  //复制后内部产品型号编码
//QString paraConfigOk;      //参数复制后参数配置完成标志位

Para_Copy::Para_Copy(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Para_Copy)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/resourse/fuzhi.png"));
    this->setWindowFlags(Qt::WindowCloseButtonHint);

    QSqlQueryModel *model = new QSqlQueryModel(ui->tableView);
    model->setQuery(QString("select 产品型号,产品型号编码,屏幕类型,屏幕尺寸,屏幕分辨率,参数配置完成 from product_model"));

    ui->tableView->setModel(model);//数据放置进去

    ui->tableView->horizontalHeader()->setSectionsClickable(true);
    ui->tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setFixedSize(this->width(),this->height());
}

Para_Copy::~Para_Copy()
{
    delete ui;
}


void Para_Copy::infoRecv2(QString sInfo2)
{

}

/*=========================================================
* 函 数 名: on_pushButton_clicked
* 功能描述: 产品型号参数复制内部数据库操作
=========================================================*/
void Para_Copy::on_pushButton_clicked()
{
    int curRow=ui->tableView->currentIndex().row();//选中行
    QAbstractItemModel *modessl = ui->tableView->model();
    //外部厂家型号
    QModelIndex indextemp = modessl->index(curRow,0);
    QVariant datatemp = modessl->data(indextemp);
    QString aft=datatemp.toString();
    //内部型号编码
    QModelIndex indextemp1 = modessl->index(curRow,1);
    QVariant datatemp1 = modessl->data(indextemp1);
    QString aft1=datatemp1.toString();

    //复制到的产品是否存在参数标志位
    QModelIndex indextemp2 = modessl->index(curRow,5);
    QVariant datatemp2 = modessl->data(indextemp2);

    Modcopy_aft=aft;                   //后面复制到的厂家产品型号
    Mode_Code_aft=aft1;                //后面复制到的内部产品型号编码
    paraConfigOk=datatemp2.toString(); //是否存在参数标志位

    if(Mode_Code_aft==Mode_Code_bef||paraConfigOk=="Y")//不允许从同一型号参数复制
    {
        QMessageBox box(QMessageBox::Warning,"严重警告","不允许相同型号参数复制，且不允许型号已有参数进行复制");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.setIcon(QMessageBox::Information);
        box.show();
        this->hide();
        box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.setButtonText (QMessageBox::Cancel,QString("取 消"));
        box.exec ();
    }
    else
    {
        //==========================开始参数复制功能===========================
        qDebug()<<Modcopy_bef<<Modcopy_aft<<endl;
        qDebug()<< Mode_Code_bef<<Mode_Code_aft<<endl;

        db.transaction(); //开启Sqlite启动事务
        QSqlQuery query(db);

        //海康相机曝光参数
        QString sq1=QStringLiteral("insert into  camera_exposure(产品型号,产品型号编码,相机参数含义,相机参数名,相机参数值) "
                                   "select '%0','%1',相机参数含义,相机参数名,相机参数值 FROM camera_exposure where 产品型号编码='%2'").arg(Modcopy_aft).arg(Mode_Code_aft).arg(Mode_Code_bef);
        query.exec(sq1);

        //PLC设备切换延时参数
        QString sq2=QStringLiteral("insert into  plc_delaytime(产品型号,产品型号编码,延时参数含义,延时参数名,延时参数值) "
                                   "select '%0','%1',延时参数含义,延时参数名,延时参数值 FROM plc_delaytime where 产品型号编码='%2'").arg(Modcopy_aft).arg(Mode_Code_aft).arg(Mode_Code_bef);
        query.exec(sq2);

        //少线缺陷算法参数
        QString sq3=QStringLiteral("insert into  lackline_para(产品型号,产品型号编码,少线参数含义,少线参数名,少线参数值) "
                                   "select '%0','%1',少线参数含义,少线参数名,少线参数值 FROM lackline_para where 产品型号编码='%2'").arg(Modcopy_aft).arg(Mode_Code_aft).arg(Mode_Code_bef);
        query.exec(sq3);

        //异物缺陷算法参数
        QString sq4=QStringLiteral("insert into  yiwu_para(产品型号,产品型号编码,异物参数含义,异物参数名,异物参数值) "
                                   "select '%0','%1',异物参数含义,异物参数名,异物参数值 FROM yiwu_para where 产品型号编码='%2'").arg(Modcopy_aft).arg(Mode_Code_aft).arg(Mode_Code_bef);
        query.exec(sq4);

        //白印缺陷算法参数
        QString sq5=QStringLiteral("insert into  mura_para(产品型号,产品型号编码,白印参数名,白印参数变量,白印参数值) "
                                   "select '%0','%1',白印参数名,白印参数变量,白印参数值 FROM mura_para  where 产品型号编码='%2'").arg(Modcopy_aft).arg(Mode_Code_aft).arg(Mode_Code_bef);
        query.exec(sq5);
        //白点缺陷算法参数
        QString sq6=QStringLiteral("insert into  whitedot_para(产品型号,产品型号编码,白点参数含义,白点参数变量,白点参数值) "
                                   "select '%0','%1',白点参数含义,白点参数变量,白点参数值 FROM whitedot_para where 产品型号编码='%2'").arg(Modcopy_aft).arg(Mode_Code_aft).arg(Mode_Code_bef);
        query.exec(sq6);

        //划伤缺陷算法参数
        QString sq7=QStringLiteral("insert into  scractch_para(产品型号,产品型号编码,划伤参数含义,划伤参数变量,划伤参数值) "
                                   "select '%0','%1',划伤参数含义,划伤参数变量,划伤参数值 FROM scractch_para where 产品型号编码='%2'").arg(Modcopy_aft).arg(Mode_Code_aft).arg(Mode_Code_bef);
        query.exec(sq7);

        //亮点缺陷算法参数
        QString sq8=QStringLiteral("insert into  lightpoint_para(产品型号,产品型号编码,亮点参数含义,亮点参数变量,亮点参数值) "
                                   "select '%0','%1',亮点参数含义,亮点参数变量,亮点参数值 FROM lightpoint_para where 产品型号编码='%2'").arg(Modcopy_aft).arg(Mode_Code_aft).arg(Mode_Code_bef);
        query.exec(sq8);

        //红蓝斑缺陷算法参数
        QString sq9=QStringLiteral("insert into  redblue_para(产品型号,产品型号编码,红蓝斑参数含义,红蓝斑参数变量,红蓝斑参数值) "
                                   "select '%0','%1',红蓝斑参数含义,红蓝斑参数变量,红蓝斑参数值 FROM redblue_para where 产品型号编码='%2'").arg(Modcopy_aft).arg(Mode_Code_aft).arg(Mode_Code_bef);
        query.exec(sq9);

        //漏光算法参数
        QString sq10=QStringLiteral("insert into  leakage_para(产品型号,产品型号编码,漏光参数含义,漏光参数变量,漏光参数值) "
                                    "select '%0','%1',漏光参数含义,漏光参数变量,漏光参数值 FROM leakage_para where 产品型号编码='%2'").arg(Modcopy_aft).arg(Mode_Code_aft).arg(Mode_Code_bef);
        query.exec(sq10);

        //显示异常算法参数
        QString sq11=QStringLiteral("insert into  displayerror_para(产品型号,产品型号编码,显示异常参数含义,显示异常参数变量,显示异常参数值) "
                                    "select '%0','%1',显示异常参数含义,显示异常参数变量,显示异常参数值 FROM displayerror_para where 产品型号编码='%2'").arg(Modcopy_aft).arg(Mode_Code_aft).arg(Mode_Code_bef);;
        query.exec(sq11);

        bool conmmitsignal=db.commit();//提交事务执行SQL语句
        if(conmmitsignal)
        {
            //==========================参数复制功能完成标志位提示===========================
            QString sq12=QStringLiteral("UPDATE product_model SET 参数配置完成 ='%1' where 产品型号='%2'").arg("Y").arg(Modcopy_aft);
            QSqlQuery query1(db);
            query1.exec(sq12);
            QString sInfo="ok";
            emit infoSend5(sInfo); //infoSend函数不需要有函数体

            QSqlQuery query(db);
            QString sq=QString("select * from product_model where 产品型号='%0'").arg(Mode_Code_aft);//内部产品型号
            query.exec(sq);
            query.next();
            paraConfigOk=query.value("参数配置完成").toString(); //参数配置完成标志位

            QMessageBox box(QMessageBox::Warning,"参数复制成功","参数已成功从产品型号"+ Modcopy_bef +"复制到产品型号"+ Modcopy_aft);
            box.setWindowIcon(QIcon(":/resourse/tishi.png"));
            box.setIcon(QMessageBox::Information);
            box.show();
            this->hide();
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();

            //this->destroy(true,true);
            this->close();
            delete ui;
        }
        else
        {
            QMessageBox box(QMessageBox::Warning,"严重警告","参数复制失败");
            box.setWindowIcon(QIcon(":/resourse/tishi.png"));
            box.setIcon(QMessageBox::Information);
            box.show();
            this->hide();
            box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.setButtonText (QMessageBox::Cancel,QString("取 消"));
            box.exec ();

            //this->destroy(true,true);
            this->close();
            delete ui;
        }
    }
}

void Para_Copy::on_pushButton_2_clicked()
{
    //this->destroy(true,true);
    this->close();
    delete ui;
}
