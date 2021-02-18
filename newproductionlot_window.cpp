#include "newproductionlot_window.h"
#include "ui_newproductionlot_window.h"
#include "QDebug"
#include"dbhelper.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QList>
#include <QTextStream>
#include <QStandardItemModel>
#include <QMessageBox>
#include<qsqlquerymodel.h>
#include "mainwindow.h"
#include "promodel.h"
#include"global_variable.h"     //全局变量头文件

QDateTime current_data_time;
QString currentDate;
QString currentTime;
QString customize_production_lot_code;//自定义产品批次
//QString screen_type;//屏幕类型
QString boundary_dimension;//外部尺寸
QString resolution;//分辨率
QString screen_swiping;//刷屏方式
QString scheme_configuration;//方案配置
QString shielding_area;//屏蔽区域
//QString production_lot_code;
//产品类型
QString item;
QStringList productType;
int a=001;
QStringList strList;
QString selected_product_type;
bool modifyFlag;
newProductionlot_window::newProductionlot_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newProductionlot_window)
{
    ui->setupUi(this);
    ui->groupBox->setStyleSheet("QGroupBox{border:2px solid rgb(53, 153, 252)}");//设置边框颜色
    ui->groupBox_2->setStyleSheet("QGroupBox{border:2px solid rgb(53, 153, 252)}");
    ui->lineEdit->setStyleSheet("border:1px solid rgb(0,0,0);background:rgb(255,255,255)");
    ui->label_16->setStyleSheet("border:1px solid rgb(0,0,0);background:rgb(230,230,230)");
    ui->label_17->setStyleSheet("border:1px solid rgb(0,0,0);background:rgb(230,230,230)");
    ui->label_18->setStyleSheet("border:1px solid rgb(0,0,0);background:rgb(230,230,230)");
    ui->label_19->setStyleSheet("border:1px solid rgb(0,0,0);background:rgb(230,230,230)");
    ui->label_20->setStyleSheet("border:1px solid rgb(0,0,0);background:rgb(230,230,230)");
    ui->label_21->setStyleSheet("border:1px solid rgb(0,0,0);background:rgb(230,230,230)");
    ui->label_22->setStyleSheet("border:1px solid rgb(0,0,0);background:rgb(230,230,230)");

    modifyFlag=false;
    bool chaxun;
    current_data_time=QDateTime::currentDateTime();//获取日期时间
    currentDate=current_data_time.toString("yyyyMMdd");
    currentTime=current_data_time.toString("yyyy/MM/dd HH-mm-ss");
    ui->dateTimeEdit->setEnabled(false);
    production_Lot_ComBox(currentDate);
    customize_production_lot_code=ui->lineEdit->text();
    ui->dateTimeEdit->setDateTime(current_data_time);//设置时间
    ui->dateTimeEdit->setMinimumDate(QDate::currentDate().addDays(-365));
    ui->dateTimeEdit->setMaximumDate(QDate::currentDate().addDays(+365));
    QSqlQuery query2(db);
    query2.exec("SELECT * FROM productionLot");
    QSqlQuery query(db);
    query.exec("SELECT * FROM product_model");
    while(query.next())
    {
        productType.append(query.value(1).toString());//显示当前产品型号
    }
   ui->comboBox_2->addItems(productType);//厂家产品型号
   QStandardItemModel *pItemModel = qobject_cast<QStandardItemModel*>(ui->comboBox_2->model());
   for(int i=0;i<productType.size();i++)
   {
       pItemModel->item(i)->setForeground(QColor(0,0,0));
       pItemModel->item(i)->setBackground(QColor(255,255,255));
   }
   selected_product_type=ui->comboBox_2->currentText();//目前选中产品型号
   qDebug()<<selected_product_type;
   QSqlQuery query1(db);
   QString sq=QString("SELECT * FROM product_model WHERE 产品型号='%1'").arg(selected_product_type);
   chaxun=query1.exec(sq);
   query1.next();
   QString a=query1.value(1).toString();//当前选中产品型号
   qDebug()<<a;
   //产品型号参数信息右侧显示
   ui->label_16->setText(query1.value(3).toString());
   ui->label_17->setText(query1.value(4).toString());
   ui->label_18->setText(query1.value(7).toString());
   ui->label_19->setText(query1.value(6).toString());
   ui->label_20->setText(query1.value(9).toString());
   ui->label_21->setText(query1.value(8).toString());
   ui->label_22->setText(query1.value(5).toString());
}

newProductionlot_window::~newProductionlot_window()
{
    delete ui;
}
void newProductionlot_window::add_product_type(QString sInfo)
{
    ui->comboBox_2->clear();
    QSqlQuery query(db);
    query.exec("SELECT * FROM product_model");
    while(query.next())
    {
        productType.append(query.value(1).toString());
    }
   ui->comboBox_2->addItems(productType);
}
/*=========================================================
* 函 数 名: on_pushButton_clicked
* 功能描述: 新建产品批次号
=========================================================*/
void newProductionlot_window::on_pushButton_clicked()
{
    bool ok,ok1;
    //厂家外部产品型号
    product_type=ui->comboBox_2->currentText();
    //生产批次号
    production_lot_code=ui->comboBox->currentText();
    if(!modifyFlag)
    {

        customize_production_lot_code=ui->lineEdit->text();
        if(customize_production_lot_code.isEmpty())
        {
            customize_production_lot_code=production_lot_code;
        }
        product_Customize_Lot=customize_production_lot_code;   //传给全局变量

        QString sq=QStringLiteral("INSERT INTO productionLot VALUES(NULL,'%2','%3','%4','%5')").arg(production_lot_code).arg(customize_production_lot_code).arg(currentTime).arg(product_type);
        qDebug()<<sq<<endl;
        QSqlQuery query1(db);
        ok= query1.exec(sq);
        QString sq1=QStringLiteral("INSERT INTO history_batch VALUES('%1','%2','%3','%4','%5','%6','%7',0,0,0,0.0)").arg(production_lot_code).arg(customize_production_lot_code).arg(ui->comboBox_2->currentText()).arg(ui->label_16->text()).arg(currentTime).arg(ui->label_22->text()).arg(ui->label_17->text());
        qDebug()<<sq1<<endl;
        ok1=query1.exec(sq1);
        if(ok&&ok1)
        {
            qDebug()<<"插入成功";
        }
        else
        {
            qDebug()<<"插入失败";
            qDebug()<<query1.lastError();
        }
        accept();
        QMessageBox msg;
                msg.setWindowTitle(tr("提示"));
                msg.setText("新建生产批次成功");
                msg.setStyleSheet("font: 9pt;background-color:rgb(230 ,230, 230)");
                msg.setIcon(QMessageBox::Information);
                msg.addButton(tr("确定"),QMessageBox::ActionRole);
                msg.setWindowIcon(QIcon(":/resourse/保存.png"));
                msg.exec();

    }
    else
    {
        QString sq=QString("update productionLot set testTime='%1' where productionLotCode='%2'").arg(currentTime).arg(production_lot_code);
        QSqlQuery query1(db);
        ok= query1.exec(sq);
        QMessageBox msg;
                msg.setWindowTitle(tr("提示"));
                msg.setText("读取成功");
                msg.setStyleSheet("font: 9pt;background-color:rgb(230 ,230, 230)");
                msg.setIcon(QMessageBox::Information);
                msg.addButton(tr("确定"),QMessageBox::ActionRole);
                msg.setWindowIcon(QIcon(":/resourse/启用.png"));
                msg.exec();
        if(ok)
        {
            qDebug()<<"插入成功"<<sq;
        }
        else
        {
            qDebug()<<"插入失败";
            qDebug()<<query1.lastError();
        }
    }
    accept();
    this->close();
}

void newProductionlot_window::production_Lot_ComBox(QString currentDate)
{
    QString times;
    QString currentTime;
    int time;
    int selectRow;
    QStringList production_lot_code_list;
    QSqlQuery queryAll(db);
    QString sqlExec=QString("SELECT * FROM productionLot");
    queryAll.exec(sqlExec);
    QSqlQueryModel *queryModel = new QSqlQueryModel();
    queryModel->setQuery(queryAll);
    int rows=queryModel->rowCount();//批次记录表
    if(rows>=1)
    {
        //计算产品批次
        QSqlQuery queryLastTime(db);
        if(queryAll.last())//得到最后一行数据
        {
            times=queryAll.value(1).toString();
            times=times.right(3);//取后三位
            time=atoi(times.toStdString().c_str());
            qDebug()<<time;
            time+=1;//自增1
            currentTime=QString("%1").arg(time);
            if(time<10)//确定产品批次
            {
                production_lot_code=currentDate+"00"+currentTime;
                qDebug()<<production_lot_code;
            }
            else if(time>=10&&time<100)
            {
                production_lot_code=currentDate+"0"+currentTime;
            }
            else
            {
                production_lot_code=currentDate+currentTime;
            }
        }
        ui->comboBox->addItem(production_lot_code);
        if(rows<=5)
        {
            while(queryAll.next())
            {
                production_lot_code_list.append(queryAll.value(1).toString());
            }
            ui->comboBox->addItems(production_lot_code_list);
        }
        else
        {
            selectRow=rows-5;
            bool ok;
            QSqlQuery queryLastFiveTime(db);
            sqlExec=QString("select * from productionLot limit 5 offset %1").arg(selectRow);
            ok= queryLastFiveTime.exec(sqlExec);
            if(ok)
            {
                qDebug()<<selectRow<<queryLastFiveTime.size();
                qDebug()<<"c查询成功";
            }
            else
            {
                qDebug()<<queryLastFiveTime.lastError();
            }
            while(queryLastFiveTime.next())
            {
                production_lot_code_list.append(queryLastFiveTime.value(1).toString());
            }
            ui->comboBox->addItems(production_lot_code_list);
        }
    }
    else
    {
        production_lot_code=currentDate+"001";
        ui->comboBox->addItem(production_lot_code);
    }
}

void newProductionlot_window::on_comboBox_currentTextChanged(const QString &arg1)
{
    QString currentProductionLot=ui->comboBox->currentText();
    QString productionType;
    int productionTypeIndex;
    if(ui->comboBox->currentText()!=production_lot_code)//取近几次的产品批次
    {
        ui->comboBox_2->setEnabled(true);
        QSqlQuery queryProduction(db);

        queryProduction.exec(QString("SELECT * FROM productionLot WHERE productionLotCode=%1").arg(currentProductionLot));
        queryProduction.next();
        productionType=queryProduction.value(4).toString();
        ui->lineEdit->setText(queryProduction.value(2).toString());
        ui->lineEdit->setEnabled(false);
       for(int i=0;i<ui->comboBox_2->count();i++)
       {
           if(ui->comboBox_2->currentText()==productionType)
           {
               productionTypeIndex=i;
           }
       }
       ui->comboBox_2->setCurrentText(productionType);
       ui->comboBox_2->setEnabled(false);
       modifyFlag=true;
    }
    else
    {
        ui->lineEdit->setEnabled(true);
        ui->lineEdit->clear();
        ui->comboBox_2->setEnabled(true);
        modifyFlag=false;
    }
}

void newProductionlot_window::on_comboBox_2_currentTextChanged(const QString &arg1)
{
    selected_product_type=ui->comboBox_2->currentText();
    qDebug()<<selected_product_type;
    QSqlQuery query1(db);
    QString sq=QString("SELECT * FROM product_model WHERE 产品型号='%1'").arg(selected_product_type);
    query1.exec(sq);
    query1.next();
    QString a=query1.value(1).toString();
    qDebug()<<a;
    ui->label_16->setText(query1.value(3).toString());
    ui->label_17->setText(query1.value(4).toString());
    ui->label_18->setText(query1.value(7).toString());
    ui->label_19->setText(query1.value(6).toString());
    ui->label_20->setText(query1.value(9).toString());
    ui->label_21->setText(query1.value(8).toString());
    ui->label_22->setText(query1.value(5).toString());
}

void newProductionlot_window::on_pushButton_2_clicked()
{
    this->close();
}


/*====================================================================
* 类型：on_pushButton_3_clicked
* 功能：修改屏幕的型号参数
======================================================================*/
void newProductionlot_window::on_pushButton_3_clicked()
{
    promodel* d=new promodel();
    d->setWindowTitle("产品型号配置");
    d->show();
    //add_promodel* add_win=new add_promodel();

    //connect(add_win,SIGNAL(infoSend(QString)) ,this,SLOT(add_product_type(QString)));
    connect(d,SIGNAL(infoSend7(QString)) ,this,SLOT(add_product_type(QString)));

}

