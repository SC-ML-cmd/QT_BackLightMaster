#include "modify_window.h"
#include "ui_modify_window.h"
#include"dbhelper.h"
modify_window::modify_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modify_window)
{
    ui->setupUi(this);

}

modify_window::~modify_window()
{
    delete ui;
}
void modify_window::setTextEditData(QString productID,QString productLotCode,QString prodectLot,QString testTime,QString productType)
{
    updateProductID=productID;
    ui->textEdit->setText(productLotCode);
    ui->textEdit_2->setText(prodectLot);
    ui->textEdit_3->setText(productType);
    ui->label_5->setText(testTime);
}
void modify_window::on_pushButton_clicked()
{
    bool resultBool;
    QString productLotCode=ui->textEdit->toPlainText();
    QString productLot=ui->textEdit_2->toPlainText();
    QString productType=ui->textEdit_3->toPlainText();
    QString updateSQL=QString("UPDATE productionLot SET productionLotCode='%1',productionLot='%2',produtionType='%3' WHERE ID=%4")
            .arg(productLotCode).arg(productLot).arg(productType).arg(updateProductID);
    qDebug()<<updateSQL<<endl;
    QSqlQuery query(db);
    resultBool=query.exec(updateSQL);
    if(resultBool)
    {
        qDebug()<<"修改成功"<<endl;
    }
    else
    {
        query.lastError();
    }
    accept();
}

void modify_window::on_pushButton_2_clicked()
{
    this->close();
}
