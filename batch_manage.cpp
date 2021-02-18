#include "batch_manage.h"
#include "ui_batch_manage.h"
#include <QSplitter>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include<QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QCloseEvent>

batch_manage::batch_manage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::batch_manage)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("批次管理界面"));
    QDateTime time= QDateTime::currentDateTime();
    QString str = time.toString("yyyy/MM/dd-hh:mm");
    ui->lineEdit_2->setText(str);//日期显示

    QString sq=QStringLiteral("select * from product_manger");
    QSqlQuery query(db);
    query.exec(sq);
    while(query.next())
       {
               QString ss= query.value("产品名称").toString();
               ui->comboBox->addItem(ss);
       }
}

batch_manage::~batch_manage()
{
    delete ui;
}

void batch_manage::on_pushButton_clicked()
{
    QIntValidator *validator = new QIntValidator(0, 1000, this);
    ui->lineEdit->setValidator(validator);   //限制没起作用?
    QString batch=ui->lineEdit->text();          //批次
    QString date=ui->lineEdit_2->text();        //日期
    QString size=ui->comboBox->currentText();   //产品尺寸
    QString creator=ui->lineEdit_3->text();        //创建人
    QString type=ui->comboBox_2->currentText(); //产品类型 直角屏/水滴屏
    QString model=ui->lineEdit_4->text();

    QSqlQuery query(db);
    QString sq=QStringLiteral("select count(*) from batch_manger where 批次号='%1'").arg(batch);
    query.exec(sq);
    //查找是否存在重复的批次号
    int num_of_batch=0;
    while(query.next())
    {
        num_of_batch= query.value(0).toInt();
    }
    if(num_of_batch>0)
    {
        QMessageBox box(QMessageBox::Warning,"提示","该批次号已存在，请输入新的批次号");
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
        emit sendData(type,batch);//批次和产品类型
        QString sq=QStringLiteral("INSERT INTO batch_manger VALUES('%1', '%2','%3','%4','%5')").arg(batch).arg(size).arg(date).arg(creator).arg(type);
        QSqlQuery query(db);
        query.exec(sq);
        QString sq1=QStringLiteral("INSERT INTO history_data VALUES('%1', '%2','%3','%4','%5','%6','%7','%8','%9','%10')").arg(batch).arg(size).arg(model).arg(type).arg(date).arg(0).arg(0).arg(0).arg(0).arg(0);
        bool insertsignal=query.exec(sq1);
        qDebug()<<insertsignal<<endl;
        this->destroy(true,true);
    }
}
void batch_manage::closeEvent(QCloseEvent *event)
{
    int button;
    button = QMessageBox::question(this, tr("退出"),QString(tr("确认已输入批次信息?")),QMessageBox::Yes | QMessageBox::No);
    if (button == QMessageBox::No)
    {
          event->ignore();//忽略退出信号，程序继续运行
    }
    else if (button == QMessageBox::Yes)
    {
          event->accept();//接受退出信号，程序退出
    }
}

