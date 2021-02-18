#include "add_promodel.h"
#include "ui_add_promodel.h"
#include<QDebug>
#include <QMessageBox>
#include <QSqlRecord>
//#include "product_model.h"          //产品型号界面头文件
#include <QSplitter>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QDateTime>
#include <QCloseEvent>
#include"global_variable.h"     //全局变量头文件

add_promodel::add_promodel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add_promodel)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/resourse/xinzengpeizhi.png"));
    this->setWindowFlags(Qt::WindowCloseButtonHint);

    setFixedSize(this->width(),this->height());

    QSqlQuery *sqlQuery = new QSqlQuery;

    ui->comboBox_2->addItem("·请选择·");
    sqlQuery->exec("select Screen_type from Screen_type");//屏幕类型表
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
    sqlQuery->exec("select Screen_size from Screen_size");//屏幕尺寸表
    while(sqlQuery->next())
    {
     QString text3 = sqlQuery->value(0).toString();
     ui->comboBox_5->addItem(text3);
     }

//    ui->comboBox_4->addItem("·请选择·");
//    sqlQuery->exec("select bond_size from bond_size");          //外形尺寸表
//    while(sqlQuery->next())
//    {
//     QString text4 = sqlQuery->value(0).toString();
//     ui->comboBox_4->addItem(text4);
//     }

    ui->comboBox_10->addItem("·请选择·");
    sqlQuery->exec("select flood_screen from flood_screen");          //刷屏方式表
    while(sqlQuery->next())
    {
     QString text5 = sqlQuery->value(0).toString();
     ui->comboBox_10->addItem(text5);
     }

    ui->comboBox_9->addItem("·请选择·");
    sqlQuery->exec("select config from config"); //方案配置表
    while(sqlQuery->next())
    {
     QString text6 = sqlQuery->value(0).toString();
     ui->comboBox_9->addItem(text6);
     }

    ui->comboBox_7->addItem("·请选择·");
    sqlQuery->exec("select shield_area from shield_area"); //屏蔽区域表
    while(sqlQuery->next())
    {
     QString text7 = sqlQuery->value(0).toString();
     ui->comboBox_7->addItem(text7);
     }

}

add_promodel::~add_promodel()
{
    delete ui;
}

void add_promodel::on_pushButton_2_clicked()
{
    QString model_name=ui->lineEdit->text();          //产品型号名称

    QString batch=ui->comboBox_2->currentText();  //屏幕类型
    QString size=ui->comboBox_5->currentText();   //屏幕尺寸
    QString date=ui->comboBox->currentText();     //分辨率
    QString type=ui->comboBox_10->currentText();  //刷屏方式

    QString creator=ui->lineEdit_2->text()+"*"+ui->lineEdit_3->text();          //产品型号名称
    //QString creator=ui->comboBox_4->currentText();//外形尺寸

    QString config=ui->comboBox_9->currentText(); //方案配置
    QString area=ui->comboBox_7->currentText();   //屏蔽区域

    QString width=ui->lineEdit_3->text();          //输入尺寸宽
    QString act_pix=QString("%1").arg(width.toDouble()/1500.0*1000.0);

    if(model_name==""||batch=="·请选择·"||size=="·请选择·"||date=="·请选择·"||type=="·请选择·"||creator=="*"||config=="·请选择·"||area=="·请选择·")
    {
        QMessageBox box(QMessageBox::Warning,"提示","产品型号信息输入有误，请重新输入");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.show();
        this->hide();
        box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.setButtonText (QMessageBox::Cancel,QString("取 消"));
        box.exec ();
        this->show();
    }
    else
    {
    int tmp = Model_Code.toInt();
    tmp=tmp+1;
    QString code_mod;
    if(tmp<10)
    {
        code_mod="0000"+QString::number(tmp);
    }
    else if(tmp>=10&&tmp<100)
    {
        code_mod="000"+QString::number(tmp);
    }
    else if(tmp>=100&&tmp<1000)
    {
        code_mod="00"+QString::number(tmp);
    }
    else if(tmp>=1000&&tmp<10000)
    {
        code_mod="0"+QString::number(tmp);
    }
    else if(tmp>=10000&&tmp<100000)
    {
        code_mod=QString::number(tmp);
    }

    //查找是否存在重复的批次号
    QSqlQuery query(db);
    QString sq=QStringLiteral("select count(*) from product_model where 产品型号='%1'").arg(model_name);
    query.exec(sq);
    int num_of_batch=0;
    while(query.next())
    {
        num_of_batch= query.value(0).toInt();
    }
    if(num_of_batch>0)
    {
        QMessageBox box(QMessageBox::Warning,"提示","该产品型号已存在，请输入新的产品型号");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.show();
        this->hide();
        box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.setButtonText (QMessageBox::Cancel,QString("取 消"));
        box.exec ();
        this->show();
    }
    else
    {
        QString sq1=QStringLiteral("INSERT INTO product_model VALUES(NULL,'%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13')").arg(model_name).arg(code_mod).arg(batch).arg(size).arg(date).arg(type).arg(creator).arg(config).arg(area).arg("N").arg("N").arg(act_pix);
        QSqlQuery query1(db);
        query1.exec(sq1);
        QString sInfo="ok";
        emit infoSend(sInfo); //infoSend函数不需要有函数体
        //this->destroy(true,true);
        this->close();
        delete ui;
    }
    }
}


void add_promodel::on_buttonBox_accepted()
{

}

void add_promodel::on_pushButton_3_clicked()
{
    //this->destroy(true,true);
    this->close();
    delete ui;
}
