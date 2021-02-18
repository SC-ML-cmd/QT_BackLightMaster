#include "history_data.h"
#include "ui_history_data.h"
#include<QDebug>
#include <QMessageBox>
#include <QSqlRecord>
#include "qfiledialog.h"
#include<QStandardPaths>
#include"excelhepler.h"
#include<QDate>
#include"global_variable.h"

#include<history_batchfind.h>
history_data::history_data(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::history_data)
{
    ui->setupUi(this);
    model = new QSqlTableModel(this);
    model->setTable("history_batch");
    model->select();//选取整个表的所有行
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置保存策略为手动提交
    ui->TV_ShowData->setModel(model); //绑定模型与视图
    ui->TV_ShowData->horizontalHeader()->setSectionsClickable(true);
    ui->TV_ShowData->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->TV_ShowData->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->TV_ShowData->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
    //ui->tableView->setColumnWidth(5,150);//设置列宽
    ui->action_return->setEnabled(false);

}

int Select_tablerows_index=0;
bool Flag_detail=0;
bool Flag_find=0;
QString SQl_Find;


history_data::~history_data()
{
    delete ui;
}
//查询按钮
void history_data::on_pushButton_clicked()
{
//History_batch_find* hbf=new  History_batch_find();
}

//删除一个批次记录按钮，同时删除该批次下保存的图片样本
//双击批次号得到该批次下的每张样本的详细信息
void history_data::on_tableView_doubleClicked(const QModelIndex &index)
{

}



//导出数据到Excel表中
void history_data::on_btn_OutputData_clicked()
{
    if(Flag_detail==0)
ExcelHepler::Table2ExcelByHtml(ui->TV_ShowData,"检测数据汇总");
    else if(Flag_detail==1)
    ExcelHepler::Table2ExcelByHtml(ui->TV_ShowData,"生产批次明细");

}

void history_data::on_btn_FindData_clicked()
{

}

void history_data::on_btn_DeleteData_clicked()
{
    int curRow = ui->TV_ShowData->currentIndex().row();
    //if(ui->TV_ShowData->currentIndex().sibling())
    //获取选中的行
    if(curRow<0)
        QMessageBox::information(NULL,"提示","请选择一行！");
    else
    {
    model->removeRow(curRow);
    QModelIndex index = model->index(curRow,0); //获得生产批次编码
    QModelIndex index1 = model->index(curRow,2);  //获得产品型号
    QString tablename=model->data(index1).toString()+model->data(index).toString();
    //删除该行
    int ok = QMessageBox::warning(this,tr("警告!"),tr("确定""删除当前行吗"),QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
    {
        model->revertAll();//如果不删除，则撤销
    }
    else
    {
        model->submitAll(); //否则提交，在数据库中删除该行
        //在批次表中删除该生产批次
        QString sql=QStringLiteral("DELETE FROM productionLot WHERE productionLotCode ='%1'").arg(model->data(index).toString());
        QSqlQuery query=QSqlQuery(db);
        db.exec(sql);
        QString sql1=QStringLiteral("DROP TABLE %0").arg(tablename);
        db.exec(sql1);
        QString sql2=QStringLiteral("VACUUM");  //删除后压缩数据库
        db.exec(sql2);
    }
    }
}

void history_data::on_TV_ShowData_doubleClicked(const QModelIndex &index)
{
    if(Flag_detail!=1)
    {
    QModelIndex i1=  index.sibling(index.row(),0);//选中一行选中生产批次编码
    QModelIndex i2=index.sibling(index.row(),2);
    Select_tablerows_index=index.row();
    QString batch= i1.data().toString();
    QString pro_type=i2.data().toString();
    qDebug()<<batch<<endl;
    model = new QSqlTableModel(this);
    model->setTable(pro_type+batch);
    qDebug()<<pro_type+batch<<endl;
    model->select();//选取整个表的所有行
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置保存策略为手动提交
    ui->TV_ShowData->setModel(model); //绑定模型与视图
    model->removeColumn(0);
    Flag_detail=1;
    ui->action_return->setEnabled(true);
    ui->action_detail->setEnabled(false);
    //    QSqlQueryModel *model1 = new QSqlQueryModel(ui->TV_ShowData);
    //    QString sq=QStringLiteral("select * from history_detail where 批次号='%1'").arg(batch);
    //    model1->setQuery(QString(sq));
    //    ui->TV_ShowData->setModel(model1); //绑定模型与视图
    }

}

void history_data::on_action_delete_triggered()
{
    history_data::on_btn_DeleteData_clicked();
}

void history_data::on_action_triggered()
{
    history_data::on_btn_OutputData_clicked();
}

void history_data::on_action_detail_triggered()
{
    if(ui->TV_ShowData->currentIndex().row()>=0)
     history_data::on_TV_ShowData_doubleClicked(ui->TV_ShowData->currentIndex());
    else {
        QMessageBox::information(NULL,"提示","请选择一行！");
    }
//    connect(this, SIGNAL(sig()), SLOT(on_TV_ShowData_doubleClicked(const QModelIndex &index)), Qt::AutoConnection);
//    emit sig();

}

void history_data::on_action_return_triggered()
{
    if(Flag_detail==1&&Flag_find==1)
    {

        QSqlQueryModel *model1 = new QSqlQueryModel(ui->TV_ShowData);
        model1->setQuery(QString(SQl_Find));
        ui->TV_ShowData->setModel(model1); //绑定模型与视图
        ui->TV_ShowData->selectRow(Select_tablerows_index);
        ui->action_detail->setEnabled(true);
        Flag_detail=0;
    }
    else
    {
        model = new QSqlTableModel(this);
        model->setTable("history_batch");
        model->select();//选取整个表的所有行
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置保存策略为手动提交
        ui->TV_ShowData->setModel(model); //绑定模型与视图
        ui->TV_ShowData->selectRow(Select_tablerows_index);
        Flag_detail=0;
        ui->action_return->setEnabled(false);
        ui->action_detail->setEnabled(true);
        if(Flag_find==1)
            Flag_find=0;
    }


}

void history_data::receiveData(QString e)
{
    SQl_Find=e;

}
void history_data::on_action_find_triggered()
{
    history_batchfind* hbf=new  history_batchfind();
    hbf->setModal(true);
    hbf->show();
    int res=hbf->exec();
    if(res==QDialog::Accepted)
    {
        connect(hbf, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
        QSqlQueryModel *model1 = new QSqlQueryModel(ui->TV_ShowData);
        model1->setQuery(QString(hbf->SQ_Find));
        SQl_Find=hbf->SQ_Find;
        ui->TV_ShowData->setModel(model1); //绑定模型与视图
        Flag_find=1;
        ui->action_return->setEnabled(true);
        ui->action_detail->setEnabled(true);
    }
}
