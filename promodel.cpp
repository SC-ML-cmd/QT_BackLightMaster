#include "promodel.h"
#include "ui_promodel.h"
#include<QDebug>
#include <QMessageBox>
#include <QSqlRecord>
#include "add_promodel.h"       //新增产品型号界面头文件
#include "para_copy.h"          //参数复制界面头文件
#include "promodel_mod.h"       //修改产品型号界面头文件
#include"global_variable.h"     //全局变量头文件
#include "model_query.h"        //产品型号查询头文件
#include "parameter_control.h"  //产品型号参数修改头文件

QString Model_Code;             //内部产品型号编码
QString Model_Mod_id;
QString Pro_Model_Mod;          //外部显示产品型号
QString Model_Mod_type;
QString Model_Mod_size;
QString Model_Mod_fenbian;
QString Model_Mod_shuaping;
QString Model_Mod_waixing;
QString Model_Mod_fangan;
QString Model_Mod_pingbi;

QString Modcopy_bef;

promodel::promodel(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::promodel)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/resourse/peizhi.png"));
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    //this->setWindowModality(Qt::ApplicationModal);

    model = new QSqlTableModel(this);
    model->setTable("product_model"); //产品型号表
    model->select();//选取整个表的所有行
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置保存策略为手动提交
    ui->tableView->setModel(model); //绑定模型与视图
    ui->tableView->hideColumn(0);
    ui->tableView->horizontalHeader()->setSectionsClickable(true);
    ui->tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");

    //this->setWindowModality(Qt::NonModal);
//    ui->tableView->clientListTableView->contentInset = UIEdgeInsetsMake(0, 0, 0, 0);
//    self.clientListTableView.scrollIndicatorInsets = UIEdgeInsetsMake(0, 0, 0, 0);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setFixedSize(this->width(),this->height());

}

promodel::~promodel()
{
    delete ui;
}


/*=========================================================
* 函 数 名: on_action_2_triggered
* 功能描述: 新增产品型号菜单
=========================================================*/
void promodel::on_action_triggered()        //新增
{
    int rows=ui->tableView->model()->rowCount();
    QAbstractItemModel *modessl = ui->tableView->model();
    QModelIndex indextemp0 = modessl->index(rows-1,2);//遍历第一行的所有列 i 是你要取值的列的下标
    QVariant datatemp0 = modessl->data(indextemp0);
    QString code=datatemp0.toString();
    Model_Code=code;//产品型号编码
    add_promodel* d=new add_promodel();
    connect(d,SIGNAL(infoSend(QString)) ,this,SLOT(infoRecv(QString)));
    d->setWindowTitle("新增产品型号");
    //d->setWindowFlag(Qt::Dialog);//Qt::Dialog是必须的
    d->setWindowModality(Qt::ApplicationModal);
    d->show();
}

/*=========================================================
* 函 数 名: infoRecv
* 功能描述: 新增产品型号槽函数
=========================================================*/
void promodel::infoRecv(QString sInfo)
{
    model = new QSqlTableModel(this);
    model->setTable("product_model");
    model->select();//选取整个表的所有行
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置保存策略为手动提交
    ui->tableView->setModel(model); //绑定模型与视图
    ui->tableView->hideColumn(0);
    ui->tableView->horizontalHeader()->setSectionsClickable(true);
    ui->tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setFixedSize(this->width(),this->height());
    emit (infoSend7(sInfo));
}

/*=========================================================
* 函 数 名: on_action_2_triggered
* 功能描述: 删除产品型号菜单
=========================================================*/
void promodel::on_action_2_triggered()
{ 
    int curRow=ui->tableView->currentIndex().row();//选中行
    QAbstractItemModel *modessl = ui->tableView->model();

    QModelIndex indextemp9 = modessl->index(curRow,10);//遍历第一行的所有列 i 是你要取值的列的下标
    QVariant datatemp9 = modessl->data(indextemp9);
    QString delete_if=datatemp9.toString();

    if(delete_if=="N")
    {
        int chooseIndex=QMessageBox::question(this,tr("删除"),QString(tr("确认删除选中项目信息？")),"确认","取消");//删除信息
        if(chooseIndex==0)//更新数据表
        {
            //==========================删除产品型号总表的参数数据===========================
            QModelIndex index_row = ui->tableView->selectionModel()->currentIndex();//当前行号
            int iSel = index_row.row();
            QModelIndex index=model->index(iSel,1,QModelIndex());
            QString manufacturer_Model=index.data().toString(); //外部厂家产品型号

            QModelIndex index1=model->index(iSel,2,QModelIndex());
            QString modelCode=index1.data().toString(); //内部产品型号编码

            db.transaction(); //开启Sqlite启动事务
            QSqlQuery query(db);
            //==========================删除产品型号总表的参数数===========================
            QString sq1=QStringLiteral("delete from product_model where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq1);
            //==========================删除分表下该产品型号的参数===========================
            QString sq2=QStringLiteral("delete from camera_exposure where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq2);
            QString sq3=QStringLiteral("delete from plc_delaytime where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq3);
            QString sq4=QStringLiteral("delete from displayerror_para where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq4);
            QString sq5=QStringLiteral("delete from lackline_para where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq5);
            QString sq6=QStringLiteral("delete from leakage_para where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq6);
            QString sq7=QStringLiteral("delete from lightpoint_para where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq7);
            QString sq8=QStringLiteral("delete from mura_para where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq8);
            QString sq9=QStringLiteral("delete from redblue_para where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq9);
            QString sq10=QStringLiteral("delete from scractch_para where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq10);
            QString sq11=QStringLiteral("delete from whitedot_para where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq11);
            QString sq12=QStringLiteral("delete from yiwu_para where 产品型号编码='%1'").arg(modelCode);
            query.exec(sq12);
            bool conmmitsignal=db.commit();//提交事务执行SQL语句
            //==========================界面数据更新===========================
            model = new QSqlTableModel(this);
            model->setTable("product_model");
            model->select();//选取整个表的所有行
            model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置保存策略为手动提交
            ui->tableView->setModel(model); //绑定模型与视图
            ui->tableView->hideColumn(0);
            ui->tableView->horizontalHeader()->setSectionsClickable(true);
            ui->tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
            ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
            ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");

            ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            setFixedSize(this->width(),this->height());
        }
    }
    else
    {
        QMessageBox box(QMessageBox::Warning,"删除失败","内置产品型号模板不可删除");
        box.setWindowIcon(QIcon(":/resourse/shibai.png"));
        box.setIcon(QMessageBox::Critical);
        box.show();

        box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.setButtonText (QMessageBox::Cancel,QString("取 消"));
        box.exec ();
    }

}

/*=========================================================
* 函 数 名: on_action_5_triggered
* 功能描述: 产品型号参数管理
=========================================================*/
void promodel::on_action_5_triggered()
{
    int curRow=ui->tableView->currentIndex().row();
    QAbstractItemModel *modessl = ui->tableView->model();

    QModelIndex indextemp = modessl->index(curRow,1);
    QVariant datatemp = modessl->data(indextemp);
    Pro_Model_Mod=datatemp.toString();//外部产品型号

    QModelIndex indextemp1 = modessl->index(curRow,2);
    QVariant datatemp1 = modessl->data(indextemp1);
    Model_Code=datatemp1.toString();//内部产品型号编码

    parameter_control* d=new parameter_control();
    //d->setWindowModality(Qt::NonModal);
    //d->setWindowModality(Qt::ApplicationModal);
    //this->setWindowModality(Qt::NonModal);
    //d->setWindowFlags(Qt::WindowStaysOnTopHint);
    d->show();
}

/*=========================================================
* 函 数 名: on_action_6_triggered
* 功能描述: 产品型号参数复制
=========================================================*/
void promodel::on_action_6_triggered()      //参数复制
{
    Para_Copy* d=new Para_Copy();
    connect(d,SIGNAL(infoSend5(QString)) ,this,SLOT(infoRecv5(QString)));
    d->setWindowTitle("参数复制到");

    int curRow=ui->tableView->currentIndex().row();
    //复制前内部厂家型号
    QAbstractItemModel *modessl = ui->tableView->model();
    QModelIndex indextemp = modessl->index(curRow,1);
    QVariant datatemp = modessl->data(indextemp);
    Modcopy_bef=datatemp.toString();

    //复制前内部产品型号编码
    QModelIndex indextemp1 = modessl->index(curRow,2);
    QVariant datatemp1 = modessl->data(indextemp1);
    Mode_Code_bef=datatemp1.toString();

    d->setWindowModality(Qt::ApplicationModal);
    d->show();
}

void promodel::infoRecv5(QString sInfo)      //tableview显示更新
{
    model = new QSqlTableModel(this);
    model->setTable("product_model");
    model->select();//选取整个表的所有行
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置保存策略为手动提交
    ui->tableView->setModel(model); //绑定模型与视图
    ui->tableView->hideColumn(0);
    ui->tableView->horizontalHeader()->setSectionsClickable(true);
    ui->tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setFixedSize(this->width(),this->height());
}

/*=========================================================
* 函 数 名: on_action_4_triggered
* 功能描述: 修改产品型号
=========================================================*/
void promodel::on_action_4_triggered()      //修改产品型号
{
    int curRow=ui->tableView->currentIndex().row();//选中行
    QAbstractItemModel *modessl = ui->tableView->model();

    QModelIndex indextemp0 = modessl->index(curRow,0);//遍历第一行的所有列 i 是你要取值的列的下标
    QVariant datatemp0 = modessl->data(indextemp0);
    Model_Mod_id=datatemp0.toString();//ID

    QModelIndex indextemp = modessl->index(curRow,1);//遍历第一行的所有列 i 是你要取值的列的下标
    QVariant datatemp = modessl->data(indextemp);
    Pro_Model_Mod=datatemp.toString();//产品型号

    QModelIndex indextemp1 = modessl->index(curRow,3);//遍历第一行的所有列 i 是你要取值的列的下标
    QVariant datatemp1 = modessl->data(indextemp1);
    Model_Mod_type=datatemp1.toString();//屏幕类型

    QModelIndex indextemp2 = modessl->index(curRow,4);//遍历第一行的所有列 i 是你要取值的列的下标
    QVariant datatemp2 = modessl->data(indextemp2);
    Model_Mod_size=datatemp2.toString();//屏幕尺寸

    QModelIndex indextemp3 = modessl->index(curRow,5);//遍历第一行的所有列 i 是你要取值的列的下标
    QVariant datatemp3 = modessl->data(indextemp3);
    Model_Mod_fenbian=datatemp3.toString();//屏幕分辨率

    QModelIndex indextemp4 = modessl->index(curRow,6);//遍历第一行的所有列 i 是你要取值的列的下标
    QVariant datatemp4 = modessl->data(indextemp4);
    Model_Mod_shuaping=datatemp4.toString();//刷屏方式

    QModelIndex indextemp5 = modessl->index(curRow,7);//遍历第一行的所有列 i 是你要取值的列的下标
    QVariant datatemp5 = modessl->data(indextemp5);
    Model_Mod_waixing=datatemp5.toString();//外形尺寸

    QModelIndex indextemp6 = modessl->index(curRow,8);//遍历第一行的所有列 i 是你要取值的列的下标
    QVariant datatemp6 = modessl->data(indextemp6);
    Model_Mod_fangan=datatemp6.toString();//方案配置

    QModelIndex indextemp7 = modessl->index(curRow,9);//遍历第一行的所有列 i 是你要取值的列的下标
    QVariant datatemp7 = modessl->data(indextemp7);
    Model_Mod_pingbi=datatemp7.toString();//屏蔽区域

    promodel_mod* d=new promodel_mod();
    connect(d,SIGNAL(infoSend3(QString)) ,this,SLOT(infoRecv3(QString)));
    d->setWindowTitle("修改产品型号");
    d->setWindowModality(Qt::ApplicationModal);
    d->show();
}
/*=========================================================
* 函 数 名: infoRecv3
* 功能描述: 修改产品型号槽函数
=========================================================*/
void promodel::infoRecv3(QString sInfo3)
{
    model = new QSqlTableModel(this);
    model->setTable("product_model");
    model->select();//选取整个表的所有行
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置保存策略为手动提交
    ui->tableView->setModel(model); //绑定模型与视图
    ui->tableView->hideColumn(0);
    ui->tableView->horizontalHeader()->setSectionsClickable(true);
    ui->tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setFixedSize(this->width(),this->height());
}
/*=========================================================
* 函 数 名: on_action_3_triggered
* 功能描述: 查询功能
=========================================================*/
void promodel::on_action_3_triggered()
{
    Model_query* d=new Model_query();
    connect(d,SIGNAL(infoSend4(QString,QString,QString,QString,QString,QString,QString)) ,this,SLOT(infoRecv4(QString,QString,QString,QString,QString,QString,QString)));
    d->setWindowTitle("产品型号查询");
    d->setWindowModality(Qt::ApplicationModal);
    d->show();
}
/*=========================================================
* 函 数 名: on_action_3_triggered
* 功能描述: 查询功能槽函数
=========================================================*/
void promodel::infoRecv4(QString batch,QString size,QString date,QString type,QString creator,QString config,QString area)
{
    int rows=ui->tableView->model()->rowCount();
    QAbstractItemModel *modessl = ui->tableView->model();

    if(batch!="·请选择·")
    {
        for(int i=0;i<rows;i++)
        {

            QModelIndex indextemp0 = modessl->index(i,3);//遍历第一行的所有列 i 是你要取值的列的下标
            QVariant datatemp0 = modessl->data(indextemp0);
            QString code=datatemp0.toString();
            if(code!=batch)
            {
                //ui->tableView->hideColumn(0);
                ui->tableView->hideRow(i);
            }
        }
    }
    if(size!="·请选择·")
    {
        for(int i=0;i<rows;i++)
        {

            QModelIndex indextemp0 = modessl->index(i,4);//遍历第一行的所有列 i 是你要取值的列的下标
            QVariant datatemp0 = modessl->data(indextemp0);
            QString code=datatemp0.toString();
            if(code!=size)
            {
                //ui->tableView->hideColumn(0);
                ui->tableView->hideRow(i);
            }
        }
    }
    if(date!="·请选择·")
    {
        for(int i=0;i<rows;i++)
        {

            QModelIndex indextemp0 = modessl->index(i,5);//遍历第一行的所有列 i 是你要取值的列的下标
            QVariant datatemp0 = modessl->data(indextemp0);
            QString code=datatemp0.toString();
            if(code!=date)
            {
                //ui->tableView->hideColumn(0);
                ui->tableView->hideRow(i);
            }
        }
    }
    if(type!="·请选择·")
    {
        for(int i=0;i<rows;i++)
        {

            QModelIndex indextemp0 = modessl->index(i,6);//遍历第一行的所有列 i 是你要取值的列的下标
            QVariant datatemp0 = modessl->data(indextemp0);
            QString code=datatemp0.toString();
            if(code!=type)
            {
                //ui->tableView->hideColumn(0);
                ui->tableView->hideRow(i);
            }
        }
    }
    if(creator!="*")
    {
        for(int i=0;i<rows;i++)
        {

            QModelIndex indextemp0 = modessl->index(i,7);//遍历第一行的所有列 i 是你要取值的列的下标
            QVariant datatemp0 = modessl->data(indextemp0);
            QString code=datatemp0.toString();
            if(code!=creator)
            {
                //ui->tableView->hideColumn(0);
                ui->tableView->hideRow(i);
            }
        }
    }
    if(config!="·请选择·")
    {
        for(int i=0;i<rows;i++)
        {

            QModelIndex indextemp0 = modessl->index(i,8);//遍历第一行的所有列 i 是你要取值的列的下标
            QVariant datatemp0 = modessl->data(indextemp0);
            QString code=datatemp0.toString();
            if(code!=config)
            {
                //ui->tableView->hideColumn(0);
                ui->tableView->hideRow(i);
            }
        }
    }
    if(area!="·请选择·")
    {
        for(int i=0;i<rows;i++)
        {

            QModelIndex indextemp0 = modessl->index(i,9);//遍历第一行的所有列 i 是你要取值的列的下标
            QVariant datatemp0 = modessl->data(indextemp0);
            QString code=datatemp0.toString();
            if(code!=area)
            {
                //ui->tableView->showRow();
                ui->tableView->hideRow(i);
            }
        }
    }

    if(batch=="·请选择·" && size=="·请选择·" && date=="·请选择·" && type=="·请选择·" && creator=="·请选择·" && config=="·请选择·" && area=="·请选择·")
    {
        for(int i=0;i<rows;i++)
        {
            ui->tableView->showRow(i);
        }
    }

}
/*=========================================================
* 函 数 名: on_action_7_triggered
* 功能描述: 刷新功能
=========================================================*/
void promodel::on_action_7_triggered()
{
    int rows=ui->tableView->model()->rowCount();
    for(int i=0;i<rows;i++)
    {
        ui->tableView->showRow(i);
    }
}
void promodel::closeEvent(QCloseEvent *event)
{
    this->close();
    promodel_show=false;
}
