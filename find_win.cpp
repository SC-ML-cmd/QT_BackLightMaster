#include "find_win.h"
#include "ui_find_win.h"
#include"dbhelper.h"
#include <QStandardItemModel>
#include"QMessageBox"
QStringList DateList;
QStringList productType_1;
#include <QDateTime>
#include<qsqlquerymodel.h>
QSqlQueryModel *model_1;
QDateTime findDayTime;
QDateTime dateTime(QDateTime::currentDateTime());
QString findDayTimeQS;
QString currentType;
QString sqlProductType;
QString sqlDate;
QString defaultTime;
QString sqlproductLot;
bool productlot_Flag=false;
QString sq1;
QString dateList_1;
QString productLot_line;
bool  combox_1Flag=false;
bool  combox_2Flag=false;
QString todayTimeQS=dateTime.toString("yyyy/MM/dd HH-mm-ss");
find_win::find_win(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::find_win)
{
    ui->setupUi(this);
    ui->comboBox->setStyleSheet("border:1px solid rgb(0,0,0);background:rgb(255,255,255)");
    ui->comboBox_2->setStyleSheet("border:1px solid rgb(0,0,0);background:rgb(255,255,255)");
    ui->lineEdit->setStyleSheet("border:1px solid rgb(0,0,0);background:rgb(255,255,255)");
    DateList<<"全部"<<"今天"<<"近三天"<<"近一周"<<"近一个月";
    ui->comboBox->addItems(DateList);
    ui->comboBox_2->addItem("全部");
    QStandardItemModel *model_2 = new QStandardItemModel();
    model_2->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("生产批次编码")));
    model_2->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("生产批次")));
    model_2->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("测试时间")));
    model_2->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("产品型号")));

    QStandardItem* item=new QStandardItem;
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableView->setModel(model_2);
//    ui->tableView->setColumnWidth(0,250);
//    ui->tableView->setColumnWidth(1,250);
//    ui->tableView->setColumnWidth(2,250);
//    ui->tableView->setColumnWidth(3,250);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->selectRow(0);
    QSqlQuery query(db);
    query.exec("SELECT * FROM product_model");//查询数据表获得型号类型
    while(query.next())
    {
        productType_1.append(query.value(1).toString());
    }
    ui->comboBox->setCurrentIndex(-1);
    ui->comboBox_2->setCurrentIndex(-1);
    ui->comboBox_2->addItems(productType_1);
    ui->comboBox->setCurrentText(" ");


}

find_win::~find_win()
{
    delete ui;
}

void find_win::on_comboBox_currentTextChanged(const QString &arg1)
{
    combox_1Flag=true;
    defaultTime=QString("2020/00/00 00-00-00");
    dateList_1=ui->comboBox->currentText();//combox控件加入文字
    if(dateList_1!="全部"&&dateList_1!="")//判定有无选中
    {
        if(dateList_1=="今天")
        {
            findDayTime=dateTime.addDays(-1);
       }

        else if(dateList_1=="近三天")
        {
            findDayTime=dateTime.addDays(-3);
        }
        else if(dateList_1=="近一周")
        {
            findDayTime=dateTime.addDays(-7);

        }
        else if(dateList_1=="近一月")
        {
            findDayTime=dateTime.addMonths(-1);
        }
        else if(dateList_1=="近三月")
        {
            findDayTime=dateTime.addMonths(-3);
        }
        findDayTimeQS=findDayTime.toString("yyyy/MM/dd HH-mm-ss");
        sqlDate=QString("SELECT * FROM productionLot WHERE testTime>='%1' and testTime <'%2'" ).arg(findDayTimeQS)//选择日期
                .arg(todayTimeQS);
    }

    else if(dateList_1=="全部")
    {
        sqlDate=QString("SELECT * FROM productionLot WHERE testTime>='%3'").arg(defaultTime);
    }
}

void find_win::on_comboBox_2_currentTextChanged(const QString &arg1)//选择产品型号
{
    combox_2Flag=true;
    if(ui->comboBox_2->currentText()!="全部")
    {
         sqlProductType.clear();
        currentType=ui->comboBox_2->currentText();
        sqlProductType=QString("and produtionType='%3'").arg(currentType);
    }
    else if(ui->comboBox_2->currentText()=="全部")
    {
        sqlProductType="";
    }
    else
    {
        QMessageBox msg;
                msg.setWindowTitle(tr("警告"));
                msg.setText("请输入产品型号");
                msg.setStyleSheet("font: 9pt;background-color:rgb(230 ,230, 230)");
                msg.setIcon(QMessageBox::Warning);
                msg.addButton(tr("确定"),QMessageBox::ActionRole);
                msg.setWindowIcon(QIcon(":/resourse/告警.png"));
                msg.exec();
    }
}

void find_win::on_pushButton_clicked()
{
    if(productlot_Flag)
    {
        int count=productLot_line.count();
        if((combox_1Flag&&dateList_1!="")&&(combox_2Flag&&ui->comboBox_2->currentText()!=""))
        {
            sq1=sqlDate+sqlProductType;
            sqlproductLot=QString("and productionLotCode IN(SELECT productionLotCode FROM productionLot where substr(productionLotCode,1,'%1')='%2')").arg(count).arg(productLot_line);
            sq1+=sqlproductLot;

        }
        else if((combox_1Flag&&dateList_1!="")&&(!combox_2Flag||ui->comboBox_2->currentText()==""))
        {
            sqlproductLot=QString("and productionLotCode IN(SELECT productionLotCode FROM productionLot where substr(productionLotCode,1,'%1')='%2')").arg(count).arg(productLot_line);
            sq1=sqlDate+sqlproductLot;
        }
        else if((!combox_1Flag||dateList_1=="")&&(combox_2Flag&&ui->comboBox_2->currentText()!=""))
        {
            sqlproductLot=QString("and productionLotCode IN(SELECT productionLotCode FROM productionLot where substr(productionLotCode,1,'%1')='%2')").arg(count).arg(productLot_line);
            sqlProductType=QString("SELECT * FROM productionLot WHERE produtionType='%1'").arg(currentType);
            sq1=sqlProductType+sqlproductLot;
        }
        else
        {
            sq1=QString("SELECT * FROM productionLot");
            sq1+=sqlproductLot;
        }

    }
    else
    {
        if(combox_1Flag==0||dateList_1=="")//查询功能
        {
            QMessageBox msg;
                    msg.setWindowTitle(tr("警告"));
                    msg.setText("请输入查询时间");
                    msg.setStyleSheet("font: 9pt;background-color:rgb(230 ,230, 230)");
                    msg.setIcon(QMessageBox::Warning);
                    msg.addButton(tr("确定"),QMessageBox::ActionRole);
                    msg.setWindowIcon(QIcon(":/resourse/告警.png"));
                    msg.exec();
        }
        else if(combox_2Flag==0||ui->comboBox_2->currentText()=="")
        {
            QMessageBox msg;
                    msg.setWindowTitle(tr("警告"));
                    msg.setText("请选择产品型号");
                    msg.setStyleSheet("font: 9pt;background-color:rgb(230 ,230, 230)");
                    msg.setIcon(QMessageBox::Warning);
                    msg.addButton(tr("确定"),QMessageBox::ActionRole);
                    msg.setWindowIcon(QIcon(":/resourse/告警.png"));
                    msg.exec();
        }
        else
        {
            sq1=sqlDate+sqlProductType;

        }
    }
    qDebug()<<sq1;
    model_1=new QSqlQueryModel(ui->tableView);
    model_1->setQuery(sq1);
    model_1->setHeaderData(0, Qt::Horizontal, tr("序号"));
    model_1->setHeaderData(1, Qt::Horizontal, tr("生产批次编码"));
    model_1->setHeaderData(2, Qt::Horizontal, tr("生产批次"));
    model_1->setHeaderData(3, Qt::Horizontal, tr("测试时间"));
    model_1->setHeaderData(4, Qt::Horizontal, tr("产品型号"));
    QStandardItem* item=new QStandardItem;
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableView->setModel(model_1);
    ui->tableView->hideColumn(0);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->selectRow(0);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
    qDebug()<<combox_1Flag;
}

void find_win::on_lineEdit_textChanged(const QString &arg1)
{
    productLot_line=ui->lineEdit->text();

    if(productLot_line=="")
    {
        productlot_Flag=false;
    }
    else
    {
        productlot_Flag=true;
    }
    qDebug()<<productlot_Flag;
}


