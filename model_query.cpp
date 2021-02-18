#include "model_query.h"
#include "ui_model_query.h"

#include<QDebug>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSplitter>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QDateTime>
#include <QCloseEvent>

Model_query::Model_query(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Model_query)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/resourse/chaxunq.png"));
    this->setWindowFlags(Qt::WindowCloseButtonHint);

    setFixedSize(this->width(),this->height());

    QSqlQuery *sqlQuery = new QSqlQuery;

    ui->comboBox_2->addItem("·请选择·");
    sqlQuery->exec("select Screen_type from Screen_type"); //屏幕类型表
    while(sqlQuery->next())
    {
        QString text = sqlQuery->value(0).toString();
        ui->comboBox_2->addItem(text);
    }

    ui->comboBox->addItem("·请选择·");
    sqlQuery->exec("select res_power from res_power"); //屏幕分辨率表
    while(sqlQuery->next())
    {
        QString text2 = sqlQuery->value(0).toString();
        ui->comboBox->addItem(text2);
    }

    ui->comboBox_5->addItem("·请选择·");
    sqlQuery->exec("select Screen_size from Screen_size"); //屏幕尺寸表
    while(sqlQuery->next())
    {
        QString text3 = sqlQuery->value(0).toString();
        ui->comboBox_5->addItem(text3);
    }

//    ui->comboBox_4->addItem("·请选择·");
//    sqlQuery->exec("select bond_size from bond_size"); //外形尺寸表
//    while(sqlQuery->next())
//    {
//        QString text4 = sqlQuery->value(0).toString();
//        ui->comboBox_4->addItem(text4);
//    }

    ui->comboBox_10->addItem("·请选择·");
    sqlQuery->exec("select flood_screen from flood_screen");          //刷屏方式表
    while(sqlQuery->next())
    {
        QString text5 = sqlQuery->value(0).toString();
        ui->comboBox_10->addItem(text5);
    }

    ui->comboBox_9->addItem("·请选择·");
    sqlQuery->exec("select config from config");          //方案配置表
    while(sqlQuery->next())
    {
        QString text6 = sqlQuery->value(0).toString();
        ui->comboBox_9->addItem(text6);
    }

    ui->comboBox_7->addItem("·请选择·");
    sqlQuery->exec("select shield_area from shield_area");          //屏蔽区域表
    while(sqlQuery->next())
    {
        QString text7 = sqlQuery->value(0).toString();
        ui->comboBox_7->addItem(text7);
    }

}

Model_query::~Model_query()
{
    delete ui;
}

void Model_query::on_pushButton_2_clicked()
{
    QString batch=ui->comboBox_2->currentText();  //屏幕类型
    QString size=ui->comboBox_5->currentText();   //屏幕尺寸
    QString date=ui->comboBox->currentText();     //分辨率
    QString type=ui->comboBox_10->currentText();  //刷屏方式
    //QString creator=ui->comboBox_4->currentText();//外形尺寸
    QString creator=ui->lineEdit_2->text()+"*"+ui->lineEdit_3->text();          //产品型号名称
    QString config=ui->comboBox_9->currentText(); //方案配置
    QString area=ui->comboBox_7->currentText();   //屏蔽区域

    emit infoSend4(batch,size,date,type,creator,config,area); //infoSend函数不需要有函数体
    //this->destroy(true,true);
    this->close();
    delete ui;

}

void Model_query::on_pushButton_3_clicked()
{
    //this->destroy(true,true);
    this->close();
    delete ui;
}
