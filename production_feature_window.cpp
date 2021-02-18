#include "production_feature_window.h"
#include "ui_production_feature_window.h"
#include <QTextStream>
#include <QSqlQueryModel>
#include<QStandardItemModel>
#include"dbhelper.h"
#include <QMessageBox>
production_feature_window::production_feature_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::production_feature_window)
{
    ui->setupUi(this);
    ui->lineEdit_2->setEnabled(false);
    ui->lineEdit_3->setEnabled(false);
    ui->lineEdit_4->setEnabled(false);
    ui->lineEdit_5->setEnabled(false);
    ui->lineEdit_6->setEnabled(false);
    ui->lineEdit_7->setEnabled(false);
    ui->lineEdit_8->setEnabled(false);
    ui->lineEdit_9->setEnabled(false);
}

production_feature_window::~production_feature_window()
{
    delete ui;
}
void production_feature_window::sendProductType(QString productType)
{
    productionType=productType;
    qDebug()<<productionType;
    QSqlQuery query(db);
    QString displayProductFeatrue=QString("SELECT * FROM product_model WHERE 产品型号='%1'").arg(productionType);
    qDebug()<<displayProductFeatrue;
    bool ok= query.exec(displayProductFeatrue);
    query.next();
    if(ok)
    {
        qDebug()<<"查询。";
    }
    ui->lineEdit_9->setText(productionType);
    ui->lineEdit_2->setText(query.value(3).toString());
    ui->lineEdit_3->setText(query.value(7).toString());
    ui->lineEdit_4->setText(query.value(5).toString());
    ui->lineEdit_5->setText(query.value(6).toString());
    ui->lineEdit_6->setText(query.value(4).toString());
    ui->lineEdit_7->setText(query.value(8).toString());
    ui->lineEdit_8->setText(query.value(9).toString());
}
void production_feature_window::on_pushButton_clicked()
{
    this->close();
}

