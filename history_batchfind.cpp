#include "history_batchfind.h"
#include "ui_history_batchfind.h"
#include <dbhelper.h>
#include<QMessageBox>

history_batchfind::history_batchfind(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::history_batchfind)
{
    ui->setupUi(this);
    ui->Cbx_FindStyle->addItem("生产批次编码");
    ui->Cbx_FindStyle->addItem("生产批次");
    ui->Cbx_FindStyle->addItem("产品型号");
    ui->Cbx_FindStyle->addItem("屏幕类型");
    ui->Cbx_FindStyle->addItem("测试时间");
    ui->Cbx_FindStyle->addItem("屏幕分辨率");
    ui->Cbx_FindStyle->addItem("屏幕尺寸");

}

history_batchfind::~history_batchfind()
{
    delete ui;
}


void history_batchfind::on_Cbx_FindStyle_currentIndexChanged(const QString &arg1)
{

}

void history_batchfind::on_btn_Find_clicked()
{
    QString find_context=ui->Ledt_FindContext->text();
    if(ui->Cbx_FindStyle->currentText()=="生产批次" or ui->Cbx_FindStyle->currentText()=="生产批次编码")
      SQ_Find=QStringLiteral("select * from history_batch where %1 like '%2%'").arg(ui->Cbx_FindStyle->currentText()).arg(find_context);
    else if(ui->Cbx_FindStyle->currentText()=="屏幕尺寸")
      SQ_Find=QStringLiteral("select * from history_batch where %1 ='%2'").arg(ui->Cbx_FindStyle->currentText()).arg(find_context);
    else
      SQ_Find=QStringLiteral("select * from history_batch where %1 like '%%2%'").arg(ui->Cbx_FindStyle->currentText()).arg(find_context);
    QSqlQuery query(db);
    query.exec(SQ_Find);
    int num=0;
    while(query.next())
    {
        num= query.value(0).toInt();
    }
    if(num==0)
    {
        QMessageBox box(QMessageBox::Warning,"提示","未找到相关记录，请重新输入！");
        box.show();
        //this->hide();
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        //this->show();

    }
    else
    {
        emit sendData(SQ_Find);
        this->accept();
        this->close();
    }


}

void history_batchfind::on_history_batchfind_accepted()
{

}
