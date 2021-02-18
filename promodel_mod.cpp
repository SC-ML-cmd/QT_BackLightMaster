#include "promodel_mod.h"
#include "ui_promodel_mod.h"
#include"global_variable.h"     //全局变量头文件

promodel_mod::promodel_mod(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::promodel_mod)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/resourse/xiugai1.png"));
    this->setWindowFlags(Qt::WindowCloseButtonHint);

    setFixedSize(this->width(),this->height());

    ui->lineEdit->setText(Pro_Model_Mod);

    QSqlQuery *sqlQuery = new QSqlQuery;

    sqlQuery->exec("select Screen_type from Screen_type");      //屏幕类型表
    while(sqlQuery->next())
    {
     QString text = sqlQuery->value(0).toString();
     ui->comboBox_2->addItem(text);
     }
    ui->comboBox_2->setCurrentText(Model_Mod_type);

    sqlQuery->exec("select res_power from res_power");          //屏幕分辨率表
    while(sqlQuery->next())
    {
     QString text2 = sqlQuery->value(0).toString();
     ui->comboBox->addItem(text2);
     }
    ui->comboBox->setCurrentText(Model_Mod_fenbian);

    sqlQuery->exec("select Screen_size from Screen_size");          //屏幕尺寸表
    while(sqlQuery->next())
    {
     QString text3 = sqlQuery->value(0).toString();
     ui->comboBox_5->addItem(text3);
     }
    ui->comboBox_5->setCurrentText(Model_Mod_size);

//    sqlQuery->exec("select bond_size from bond_size");          //外形尺寸表
//    while(sqlQuery->next())
//    {
//     QString text4 = sqlQuery->value(0).toString();
//     ui->comboBox_4->addItem(text4);
//     }
    QString num_long,num_width;
    for(int i=0;i<Model_Mod_waixing.size();i++)
    {
        if(Model_Mod_waixing[i]!="*")
        num_long+=Model_Mod_waixing[i];
        if(Model_Mod_waixing[i]=="*")
        break;
    }
    for(int i=0;i<Model_Mod_waixing.size();i++)
    {
        if(Model_Mod_waixing[i]=="*")
        {
            i++;
            while(i<Model_Mod_waixing.size())
            {
                num_width+=Model_Mod_waixing[i];
                i++;
            }
            break;
        }
    }

    ui->lineEdit_2->setText(num_long);
    ui->lineEdit_3->setText(num_width);

    sqlQuery->exec("select flood_screen from flood_screen");          //刷屏方式表
    while(sqlQuery->next())
    {
     QString text5 = sqlQuery->value(0).toString();
     ui->comboBox_10->addItem(text5);
     }
    ui->comboBox_10->setCurrentText(Model_Mod_shuaping);

    sqlQuery->exec("select config from config");          //方案配置表
    while(sqlQuery->next())
    {
     QString text6 = sqlQuery->value(0).toString();
     ui->comboBox_9->addItem(text6);
     }
    ui->comboBox_9->setCurrentText(Model_Mod_fangan);

    sqlQuery->exec("select shield_area from shield_area");          //屏蔽区域表
    while(sqlQuery->next())
    {
     QString text7 = sqlQuery->value(0).toString();
     ui->comboBox_7->addItem(text7);
     }
    ui->comboBox_7->setCurrentText(Model_Mod_pingbi);
}

promodel_mod::~promodel_mod()
{
    delete ui;
}

void promodel_mod::on_pushButton_2_clicked()
{
    QString model_name=ui->lineEdit->text();          //产品型号名称

    QString batch=ui->comboBox_2->currentText();          //屏幕类型
    QString size=ui->comboBox_5->currentText();   //屏幕尺寸
    QString date=ui->comboBox->currentText();        //分辨率
    QString type=ui->comboBox_10->currentText(); //刷屏方式
    QString creator=ui->lineEdit_2->text()+"*"+ui->lineEdit_3->text();          //产品型号名称
    //QString creator=ui->comboBox_4->currentText();     //外形尺寸
    QString config=ui->comboBox_9->currentText(); //方案配置
    QString area=ui->comboBox_7->currentText(); //屏蔽区域

    QString width=ui->lineEdit_3->text();          //输入尺寸宽
    QString act_pix=QString("%1").arg(width.toDouble()/1500.0*1000.0);

    if(model_name==""||batch=="·请选择·"||size=="·请选择·"||date=="·请选择·"||type=="·请选择·"/*||creator=="·请选择·"*/||config=="·请选择·"||area=="·请选择·")
    {
        QMessageBox box(QMessageBox::Warning,"提示","产品型号信息输入有误，请重新输入");
        box.setWindowIcon(QIcon(":/resourse/tishi.png"));
        box.setIcon(QMessageBox::Warning);
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
            QString sq1=QStringLiteral("UPDATE product_model SET 产品型号 ='%1', 屏幕类型 ='%2',屏幕尺寸 = '%3',屏幕分辨率 = '%4',刷屏方式 = '%5',外形尺寸 = '%6',方案配置 = '%7',屏蔽区域 = '%8',一像素大小μm = '%9' where id='%10'").arg(model_name).arg(batch).arg(size).arg(date).arg(type).arg(creator).arg(config).arg(area).arg(act_pix).arg(Model_Mod_id);
            QSqlQuery query1(db);
            query1.exec(sq1);
            QString sInfo3="ok";
            emit infoSend3(sInfo3); //infoSend函数不需要有函数体
            //this->destroy(true,true);
            this->close();
            delete ui;
    }
}

void promodel_mod::on_pushButton_3_clicked()
{
    //this->destroy(true,true);
    this->close();
    delete ui;
}
