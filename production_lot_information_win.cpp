#include "production_lot_information_win.h"
#include "ui_production_lot_information_win.h"
//#include"new_production_lot.h"
#include <QSqlQueryModel>
#include<QStandardItemModel>
#include<QMessageBox>
#include <QAbstractScrollArea>
#include"modify_window.h"
#include"QCheckBox"
#include"QLabel"
#include <QTableView>
#include"production_feature_window.h"
#include"newproductionlot_window.h"
#include <QAbstractScrollArea>
#include <QDockWidget>
#include <QLayout>
#include<QFormLayout>
#include<QVector>
#include<QDebug>
#include<QSqlQuery>
#include<QSqlError>
#include<dbhelper.h>
#include"mainwindow.h"
QString production_lot_code;
bool addFlag=false;
QSqlQueryModel *model;
QString sqlite;
//QSqlDatabase db;
QStandardItemModel *dateCountModel;
QStandardItemModel *dataModel;
QString product_ID;
QString product_Lot;
QString product_Customize_Lot;
QString product_Time;
QString product_Type;
bool pushbotton_Flag=false;
production_lot_information_win::production_lot_information_win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::production_lot_information_win)
{
    ui->setupUi(this);
    sqliteCols=5;//productionlot数据库列数
    QVector<QString> productionTypeVector;//存放productiontype容器
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//设置可以多选列
    QSqlQuery query = QSqlQuery(db);//访问数据库
    query.exec("SELECT * FROM productionLot ");
    QSqlQueryModel *queryModel = new QSqlQueryModel();
    queryModel->setQuery(query);
    int rows = queryModel->rowCount();//获取数据库行数
    this->ui->tableView->installEventFilter(this);
    this->PagingWidget_ = new PagingWidget();//初始化pagingWidget界面
    this->InitTable(10,rows);//初始化tableview；
    connect(PagingWidget_, &PagingWidget::SignalCurrentPageChanged,
            this, &production_lot_information_win::SlotCurrentPageChanged);//信号当页数发生变化产生
    this->ui->scrollArea->setWidget(this->PagingWidget_);//将分页栏放入scrollArea；
    ui->scrollArea->setFrameShape(QFrame::NoFrame);//设置无边框
    ui->tableView->selectRow(0);//默认选择0行
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onTableClicked(const QModelIndex &)));//双击信号槽
}

production_lot_information_win::~production_lot_information_win()
{
    delete ui;
}
/*=========================================================
* 函 数 名: on_action_triggered
* 功能描述: 新建产品批次
=========================================================*/
void production_lot_information_win::on_action_triggered()//新建产品批次
{
    new_product_win=new newProductionlot_window;
    new_product_win->setModal(true);
    new_product_win->show();
    if(new_product_win->exec()==QDialog::Accepted)//发送数据
    {
        addFlag=true;
        QSqlQuery query = QSqlQuery(db);
        query.exec("SELECT * FROM productionLot ");
        QSqlQueryModel *queryModel = new QSqlQueryModel();
        queryModel->setQuery(query);
        int rows = queryModel->rowCount();
        this->ui->tableView->installEventFilter(this);
       this->PagingWidget_ = new PagingWidget();
       this->InitTable(10,rows);
        //updateData(rows);

    }
     ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}
/*=========================================================
* 函 数 名: on_action_2_triggered
* 功能描述: 修改产品批次
=========================================================*/
void production_lot_information_win::on_action_2_triggered()//修改产品批次
{
//    QSqlQuery query = QSqlQuery(db);
//    int row;
//    QItemSelectionModel *model_selection = ui->tableView->selectionModel();//取选择的行数
//    QModelIndexList IndexList= model_selection->selectedIndexes();
//    if(IndexList.count()==0)//为提示选择产品批次
//    {
//        QMessageBox msg;
//                msg.setWindowTitle(tr("警告"));
//                msg.setText("请选择产品批次");
//                msg.setStyleSheet("font: 9pt;background-color:rgb(230 ,230, 230)");
//                msg.setIcon(QMessageBox::Warning);
//                msg.addButton(tr("确定"),QMessageBox::ActionRole);
//                msg.setWindowIcon(QIcon(":/resourse/告警.png"));
//                msg.exec();
//    }
//    else
//    {
//        foreach (QModelIndex index, IndexList)
//        {
//            row=index.row();
//            break;
//        }
//        modyfy_win=new modify_window;
//        modyfy_win->setModal(true);
//        //int row=ui->tableView->currentIndex().row();
//        qDebug()<<row;
//        //获取产品批次信息
//        QString productID=dataModel->item(row,0)->text();
//        QString productLotCode=dataModel->item(row,1)->text();
//        QString prodectLot=dataModel->item(row,2)->text();
//        QString testTime=dataModel->item(row,3)->text();
//        QString productType=dataModel->item(row,4)->text();
//        qDebug()<<productID;
//        qDebug()<<prodectLot;
//        modyfy_win->setTextEditData(productID,productLotCode,prodectLot,testTime,productType);//传到modyfy界面
//        modyfy_win->setModal(true);
//        modyfy_win->show();
//        if(modyfy_win->exec()==QDialog::Accepted)
//        {
//            //更新数据表
//            QMessageBox msg;
//                    msg.setWindowTitle(tr("提示"));
//                    msg.setText("修改成功");
//                    msg.setStyleSheet("font: 9pt;background-color:rgb(230 ,230, 230)");
//                    msg.setIcon(QMessageBox::Information);
//                    msg.addButton(tr("确定"),QMessageBox::ActionRole);
//                    msg.setWindowIcon(QIcon(":/resourse/编辑.png"));
//                    msg.exec();
//            QSqlQuery query = QSqlQuery(db);
//            query.exec("SELECT * FROM productionLot ");
//            QSqlQueryModel *queryModel = new QSqlQueryModel();
//            queryModel->setQuery(query);
//            int rows = queryModel->rowCount();
//            this->ui->tableView->installEventFilter(this);
//            this->PagingWidget_ = new PagingWidget();
//            this->InitTable(10,rows);
//        }
//    }
//     ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
//     ui->tableView->selectRow(0);
}
/*=========================================================
* 函 数 名: on_action_3_triggered
* 功能描述: 删除产品批次
=========================================================*/
void production_lot_information_win::on_action_3_triggered()//删除产品批次
{
    int row;
    int chooseIndex=QMessageBox::question(this,tr("删除"),QString(tr("确认删除选中项目信息？")),"是","否");//删除信息
    bool deleteFlag;
    if(chooseIndex==0)//更新数据表
    {
        QItemSelectionModel *model_selection = ui->tableView->selectionModel();
        QModelIndexList IndexList= model_selection->selectedIndexes();
        QMap <int, int> rowMap;
        //多选删除
        foreach (QModelIndex index, IndexList)
        {
             row=index.row();
             QString productionID=dataModel->item(row,0)->text();
             QSqlQuery query1(db);
             deleteFlag=query1.exec(QString("DELETE FROM productionLot WHERE ID=%1").arg(productionID));
             //rowMap.insert(index.row(), 0); //QModelIndex 有更多数据可用
             if(deleteFlag)
             {
                 qDebug()<<"删除成功";
             }
             else
             {
                 qDebug()<<query1.lastError();
             }
        }
        qDebug() << "print : " <<rowMap;
        qDebug() << "acount : " <<rowMap.count() ;
        qDebug() <<"i = 0 : "<<rowMap.keys();
        QSqlQuery query = QSqlQuery(db);
        query.exec("SELECT * FROM productionLot ");
        QSqlQueryModel *queryModel = new QSqlQueryModel();
        queryModel->setQuery(query);
        int rows = queryModel->rowCount();
        this->ui->tableView->installEventFilter(this);
        this->PagingWidget_ = new PagingWidget();
        this->InitTable(10,rows);
        // ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->selectRow(0);
         //updateData(rows);
    }
}
/*=========================================================
* 函 数 名: onTableClicked
* 功能描述:双击显示产品型号信息
=========================================================*/
void production_lot_information_win::onTableClicked(const QModelIndex &index)
{
    qDebug()<<"双击成功";
    int row=ui->tableView->currentIndex().row();
    QString productType=dataModel->item(row,4)->text();
    product_feature_win=new production_feature_window;
    product_feature_win->sendProductType(productType);
    product_feature_win->setModal(true);
    product_feature_win->show();
    product_feature_win->setModal(true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->selectRow(0);
}

/*=========================================================
* 函 数 名: on_action_4_triggered
* 功能描述:查询界面
=========================================================*/
void production_lot_information_win::on_action_4_triggered()
{
    find_window=new find_win;
    find_window->setModal(true);
    find_window->show();
}

/*=========================================================
* 函 数 名: InitTable
* 功能描述:初始化tableview
=========================================================*/
void production_lot_information_win::InitTable(int rows, int totalRows)
{
    DataList_.clear();
    if(rows < 0 || totalRows < 0)
        return;
    QSqlQuery query = QSqlQuery(db);
    query.exec("SELECT * FROM productionLot order by ID desc");
    while(query.next())
    {
        UserData *data = new UserData();
        data->productID=query.value(0).toString();
        data->productLotCode=query.value(1).toString();
        data->productLot=query.value(2).toString();
        data->testTime=query.value(3).toString();
        data->productType=query.value(4).toString();
        this->DataList_.push_back(data);
    }
    //设置分页栏页面数据
    this->SetPages(rows, totalRows);
    this->SlotCurrentPageChanged(1);
}

/*=========================================================
* 函 数 名: SetPages
* 功能描述:设置分页栏
=========================================================*/
void production_lot_information_win::SetPages(int rows, int totalRows)
{
    if(rows < 0 || totalRows < 0)
        return;
    int offset = (totalRows % rows) > 0 ? 1 : 0;
    //计算总页数
    this->Pages_ = totalRows / rows + offset;
    //总行数
    this->TotalRows_ = totalRows;
    //每页行数
    this->EachRows_ = rows;
    //设置分页栏最大页面数
    this->PagingWidget_->SetMaxPage(this->Pages_);
}

/*=========================================================
* 函 数 名: UpdateTable
* 功能描述:更新数据表
=========================================================*/
void production_lot_information_win::UpdateTable(int startRow, int endRow)
{
    this->ClearTable();
    int row = 0;
    int selectRow=0;
    QString add_production_lot_code="";
    QStandardItem* itemRow = 0;
    QString ItemQString;
    selectRow=0;
    itemRow = new QStandardItem;
    dataModel = new QStandardItemModel();
    dataModel->setColumnCount(sqliteCols);
    dataModel->setHeaderData(0, Qt::Horizontal, tr("序号"));
    dataModel->setHeaderData(1, Qt::Horizontal, tr("生产批次编码"));
    dataModel->setHeaderData(2, Qt::Horizontal, tr("生产批次"));
    dataModel->setHeaderData(3, Qt::Horizontal, tr("测试时间"));
    dataModel->setHeaderData(4, Qt::Horizontal, tr("产品型号"));
    itemRow->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if(addFlag)
    {
        addFlag=false;
        add_production_lot_code=production_lot_code;
    }
    for(int i = startRow; i < this->DataList_.size(); ++i)
    {
        if(i > endRow)
            break;
        dataList.clear();
        UserData *data = this->DataList_.at(i);
        dataList.append(data->productID);
        dataList.append(data->productLotCode);
        dataList.append(data->productLot);
        dataList.append(data->testTime);
        dataList.append(data->productType);
        if(data)
        {
            if(!data || row < 0)
                return;
            for(int j=0;j<sqliteCols;j++)
            {
                itemRow = new QStandardItem(QString("%1").arg(dataList[j]));
                ItemQString=QString("%1").arg(dataList[j]);
                dataModel->setItem(row,j,itemRow);
                if(ItemQString==add_production_lot_code)
                {
                    selectRow=row;
                }
            }
            row++;
        }
    }

    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");

    ui->tableView->setModel(dataModel);
    ui->tableView->selectRow(selectRow);
    ui->tableView->hideColumn(0);
}

void production_lot_information_win::ClearData()
{
    for(UserData *data : this->DataList_)
        if(data)
            delete data;
    this->DataList_.clear();
}

void production_lot_information_win::ClearTable()
{
    //清空表格数据
    this->ui->tableView->clearMask();
}
/*=========================================================
* 函 数 名: CurrentPageChanged
* 功能描述:页面更改
=========================================================*/
void production_lot_information_win::CurrentPageChanged(int page)
{
    //根据当前页决定要显示数据范围
    int rowstart = (page - 1) * this->EachRows_;
    int rowend = page * this->EachRows_ - 1;
    //根据起点和终点行更新表格页面
    this->UpdateTable(rowstart, rowend);
}
//当前页回调
void production_lot_information_win::SlotCurrentPageChanged(int page)
{
    //根据当前页决定要显示数据范围
    int rowstart = (page - 1) * this->EachRows_;
    int rowend = page * this->EachRows_ - 1;
    //根据起点和终点行更新表格页面
    this->UpdateTable(rowstart, rowend);
}
/*=========================================================
* 函 数 名: on_pushButton_clicked()
* 功能描述:点击确定按钮
=========================================================*/
void production_lot_information_win::on_pushButton_clicked()
{
    QSqlQuery query = QSqlQuery(db);
    int row;
    QItemSelectionModel *model_selection = ui->tableView->selectionModel();
    QModelIndexList IndexList= model_selection->selectedIndexes();
    if(IndexList.count()==0)
    {
        QMessageBox msg;
                msg.setWindowTitle(tr("警告"));
                msg.setText("请选择产品批次");
                msg.setStyleSheet("font: 9pt;background-color:rgb(230 ,230, 230)");
                msg.setIcon(QMessageBox::Warning);
                msg.addButton(tr("确定"),QMessageBox::ActionRole);
                msg.setWindowIcon(QIcon(":/resourse/告警.png"));
                msg.exec();
    }
    else
    {
        foreach (QModelIndex index, IndexList)
        {
            row=index.row();
            break;
        }
        qDebug()<<row;
        //全局变量，需要使用区域
        product_ID=dataModel->item(row,0)->text();
        product_Lot=dataModel->item(row,1)->text();
        product_Customize_Lot=dataModel->item(row,2)->text();
        product_Time=dataModel->item(row,3)->text();
        product_Type=dataModel->item(row,4)->text();
        //内部产品型号编码
        QSqlQuery query(db);
        QString sq=QString("select * from product_model where 产品型号='%0'").arg(product_Type);
        query.exec(sq);
        query.next();
        Pro_Model_Mod=query.value("产品型号").toString();     //全局变量厂家产品型号
        Model_Code=query.value("产品型号编码").toString();    //全局变量内部产品型号编码
        Model_Mod_type=query.value("屏幕类型").toString();    //全局变量屏幕类型
        Model_Mod_pingbi=query.value("屏蔽区域").toString();  //全局变量屏蔽位置
        paraConfigOk=query.value("参数配置完成").toString();  //全局参数配置完成标志位

        qDebug()<<"Model_Code"<<Model_Code<<endl;
        qDebug()<<"Model_Mod_type"<<Model_Mod_type<<endl;
        qDebug()<<"Model_Mod_pingbi"<<Model_Mod_pingbi<<endl;


        //选择型号后进行参数初始化
        initial_para();
        QString productionName=product_Type+product_Lot;
        bool ok=query.exec(QString("CREATE TABLE '%1' ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "样本编号 VARCHAR(40) NOT NULL, "
                           " 生产批次 VARCHAR(40) NOT NULL, "
                           "生产批次编码 VARCHAR(40) NOT NULL,"
                           "产品型号 VARCHAR(40) NOT NULL,"
                           "产品型号编码 VARCHAR(40) NOT NULL,"
                           "是否缺陷 VARCHAR(40) NOT NULL,"
                           "缺陷类型编码 VARCHAR(40) NOT NULL,"
                           "缺陷类型名称 VARCHAR(40) NOT NULL,"
                           "是否保存样本 VARCHAR(40) NOT NULL)").arg(productionName));

        update_alreadyNum();  // 刷新缺陷数值
        emit infoSend(product_Type,product_Lot);
        QMessageBox msg;
                msg.setWindowTitle(tr("提示"));
                msg.setText("生产批次及系统参数配置成功！");
                msg.setStyleSheet("font: 9pt;background-color:rgb(230 ,230, 230)");
                msg.setIcon(QMessageBox::Information);
                msg.addButton(tr("确定"),QMessageBox::ActionRole);
                msg.setWindowIcon(QIcon(":/resourse/peizhi.png"));
                msg.exec();
        if(ok)
        {
            qDebug()<<"创建成功";
        }
        else
        {
            qDebug()<<query.lastError();
        }
        pushbotton_Flag=true;
        this->close();
    }
}

void production_lot_information_win::on_pushButton_2_clicked()
{
    QMessageBox *messageBox = new QMessageBox(this);
        messageBox->setIcon(QMessageBox::Warning);
        messageBox->setWindowTitle("提示");
        messageBox->setText("确定取消配置？");
        messageBox->addButton("取消", QMessageBox::RejectRole); //自定义取消按钮
        messageBox->addButton("确定", QMessageBox::AcceptRole); //自定义确定按钮
        if(messageBox->exec() == QDialog::Accepted)
        { //如果按下确定按钮，则执行清空所有配置
            pushbotton_Flag=true;
            this->close();

        }




}
void production_lot_information_win::closeEvent(QCloseEvent* event)
{
    if(pushbotton_Flag)
    {
        pushbotton_Flag=false;
        event->accept();
        production_lot_information_win_show=false;
    }
    else
    {
        QMessageBox *messageBox = new QMessageBox(this);
            messageBox->setIcon(QMessageBox::Warning);
            messageBox->setWindowTitle("提示");
            messageBox->setText("确定取消配置？");
            messageBox->addButton("取消", QMessageBox::RejectRole); //自定义取消按钮
            messageBox->addButton("确定", QMessageBox::AcceptRole); //自定义确定按钮
            if(messageBox->exec() == QDialog::Accepted)
            { //如果按下确定按钮，则执行清空所有配置
                event->accept();
                production_lot_information_win_show=false;
            }
            else
            {
                event->ignore();
            }
    }

}

/*====================================================================
* 类型：initial_para
* 功能：算法参数初始化从数据库读取参数
======================================================================*/
void production_lot_information_win::initial_para()
{
    //==========================海康相机曝光参数===========================
    QString sq=QStringLiteral("select *from camera_exposure where 产品型号编码='%0'").arg(Model_Code);
    QSqlQuery query(db);                                       //执行操作类对象查询数据
    bool signal=query.exec(sq);                                //执行
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("相机参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================海康相机曝光参数==============================
            Exposure_Ceguang_Front=list.at(0);            //前黑白相机侧光曝光值
            Exposure_Ceguang_Back=list.at(1);             //后黑白相机侧光曝光值
            Exposure_Ceguang_Left=list.at(2);             //左黑白相机侧光曝光值
            Exposure_Ceguang_Right=list.at(3);            //右黑白相机侧光曝光值
            Exposure_Ceguang_Main=list.at(4);             //主黑白相机侧光曝光值

            Exposure_White_Black_Left=list.at(5);        //左黑白白底相机曝光值     5.5寸屏幕30万   6.0寸50万
            Exposure_White_Black_Middle=list.at(6);      //主黑白白底相机曝光值    5.5寸屏幕2万    6.0寸3.5万
            Exposure_White_Color_Right=list.at(7);       //右侧黑白相机白底曝光值   5.5寸屏幕30万   6.0寸50万
            Exposure_White_Black_Back=list.at(8);        //后侧黑白相机白底曝光值
            Exposure_White_Black_Front=list.at(9);       //前侧黑白相机白底曝光值
            Exposure_White_Color_Middle=list.at(10);     //主彩色相机白底曝光值     5.5寸屏幕1800    6.0寸25000

            Exposure_Black_Middle=list.at(11);           //主黑白相机黑色底色曝光值
            Exposure_Black_Color=list.at(12);            //主彩色相机黑色底色曝光值    5.5寸78万

            Exposure_Gray_Main=list.at(13);              //主黑白相机灰色曝光值
            Exposure_Gray_Left=list.at(14);              //左黑白相机灰色曝光值
            Exposure_Gray_Right=list.at(15);             //右黑白相机灰色曝光值
        }
    }

    //==========================PLC设备延时参数===========================
    QString sq1=QStringLiteral("select *from plc_delaytime where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq1);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("延时参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================延时参数值==============================
            SideLightOpenTime=list.at(0);    //侧光灯打开延时
            BoxOpenTime=list.at(1);          //测试盒打开点亮白底延时
            BlackSwitchTime=list.at(2);      //屏幕白色切换到黑色延时
            GraySwitchTime=list.at(3);       //屏幕白色切换到黑色延时
            modbus_time=list.at(4);          //PLC读写指令延时
        }
    }

    //==========================少线算法参数===========================
    QString sq2=QStringLiteral("select *from lackline_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq2);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("少线参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================少线参数值==============================
            lackline_bolckSize=list.at(0);     //少线分割邻域块大小
            lackline_delta=list.at(1);         //少线分割邻域块阈值
            longShortlowerth=list.at(2);       //少线长宽比下限
            shortHigherth=list.at(3);          //少线宽度上限
            longLowerth=list.at(4);            //少线长度下限
            lackscratchth=list.at(5);          //少线排除贴膜划痕阈值
        }
    }

    //==========================异物算法参数===========================
    QString sq3=QStringLiteral("select *from yiwu_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq3);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("异物参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================异物参数值==============================
            yiwu_pre_size=list.at(0);          //异物分割邻域块大小
            yiwu_pre_th=list.at(1);            //异物分割邻域块阈值
            yiwu_area_lower=list.at(2);        //异物一次面积下限
            yiwu_area_upper=list.at(3);        //异物一次面积上限
            yiwu_k_w_h=list.at(4);             //异物长宽比，点状和线状
            yiwu_sec_size=list.at(5);          //异物2次分割邻域块大小
            yiwu_sec_th=list.at(6);            //异物2次分割邻域块阈值
            yiwu_sec_area_lower=list.at(7);    //异物二次面积下限
            yiwu_sec_area_upper=list.at(8);    //异物二次面积上限
            yiwu_lighth=list.at(9);            //异物亮度阈值
        }
    }

    //==========================白印算法参数===========================
    QString sq4=QStringLiteral("select *from mura_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq4);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("白印参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================白印参数值==============================
            black_mura_gray_th=list.at(0);           //亮度阈值主调节参数
            black_mura_area_lower=list.at(1);        //面积阈值下限
            black_mura_area_upper=list.at(2);        //面积阈值上限
            black_mura_boder=list.at(3);             //外围宽度范围
            black_mura_corner=list.at(4);            //边角宽度范围
            mura_bubbleth=list.at(5);                //边角宽度范围
            mura_centerlighth=list.at(6);            //白印缺陷排除气泡干扰阈值
            mura_lighth=list.at(7);                  //白印缺陷中心亮度阈值
            black_mura_area_1=list.at(8);            //一级灰度面积
            black_mura_th_1=list.at(9);              //一级灰度差值
            black_mura_area_2=list.at(10);           //二级灰度面积
            black_mura_th_2=list.at(11);             //二级灰度差值
        }
    }

    //==========================白点算法参数===========================
    QString sq5=QStringLiteral("select *from whitedot_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq5);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("白点参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================白点参数值==============================
            whitePoint_step=list.at(0);       //白点自适应分割领域块大小
            whitePoint_threshold=list.at(1);  //白点自适应分割领阈值
            whitePoint_lowerArea=list.at(2);  //面积下限
            whitePoint_higherArea=list.at(3); //面积上限
            scratchth=list.at(4);             //白点排除划痕阈值大于则认为贴膜划痕
            bubbleth=list.at(5);              //白点排除气泡阈值大于则认为贴膜气泡
            dotdomainnum = list.at(6);        //白点连通域个数大于则认为噪点
            defectouterth= list.at(7);   	  //白点缺陷处灰度均值与缺陷外围灰度差
            corewholeth =list.at(8);          //白点缺陷中心点灰度值与整块疑似缺陷灰度均值差
            spotpeak = list.at(9);            //白点缺陷最亮点与最暗点灰度差
            siecevariance= list.at(10);       //侧光图中排除划痕参数
            whitePoint_w_h= list.at(11);      //白点缺陷长宽比阈值
        }
    }

    //==========================划伤算法参数===========================
    QString sq6=QStringLiteral("select *from scractch_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq6);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("划伤参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================划伤参数值==============================
            scratch__bolckSize=list.at(0);      //划伤自适应分割领域块大小
            scratch__delta= list.at(1);         //划伤自适应分割领阈值
            scratch_higherArea=list.at(2);      //划伤面积上限
            scratch_lowerArea=list.at(3);       //划伤面积下限
            scratch_lowerLongShort=list.at(4);  //划伤长宽比下限
            scratch_higherLongShort=list.at(5); //划伤长宽比上限
            scratch_lowerWidth=list.at(6);      //划伤宽度下限
            scratch_higherWidth= list.at(7);    //划伤宽度上限
            scratch_lowerLength=list.at(8);     //划伤长度下限
            scratch_higherLength=list.at(9);    //划伤长度上限限
            filmscratch=list.at(10);            //贴膜划痕系数
            scratchbubbleth=list.at(11);        //贴膜排除气泡系数1
            scratchbubbleth1=list.at(12);       //贴膜排除气泡系数2
            scratchbrighth=list.at(13);         //划伤亮度阈值
            minscratchbrighth=list.at(14);      //最小划伤亮度阈值
        }
    }

    //==========================亮点算法参数===========================
    QString sq7=QStringLiteral("select *from lightpoint_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq7);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("亮点参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================亮点参数值==============================
            black_light_gram=list.at(0);             //亮点亮度增强系数
            black_lightPoint_step= list.at(1);       //滑窗间隔步长
            black_lightPoint_size=list.at(2);        //滑窗窗口大小
            black_lightPoint_gray_th=list.at(3);     //滑窗二值化亮度阈值
            black_lightPoint_area_upper=list.at(4);  //面积阈值上限
            black_lightPoint_area_lower= list.at(5); //面积阈值下限
            black_lightPoint_diff=list.at(6);        //亮点亮度阈值
            black_lightPoint_w_h=list.at(7);         //亮点缺陷长宽比
        }
    }

    //==========================红蓝斑算法参数===========================
    QString sq8=QStringLiteral("select *from redblue_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq8);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("红蓝斑参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================红蓝斑参数值==============================
            black_light_red=list.at(0);      //红蓝斑亮度增强系数
            redBlue_area_lower=list.at(1);   //红蓝斑面积下限
            redBlue_area_upper=list.at(2);   //红蓝斑面积上限
            redBlue_numupper= list.at(3);    //红蓝斑判定阈值数目
        }
    }

    //==========================漏光算法参数===========================
    QString sq9=QStringLiteral("select *from leakage_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq9);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("漏光参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================漏光参数值==============================
            leakage_bolckSize=list.at(0);        //黑底漏光适应分割领域块大小
            leakage_delta=list.at(1);            //黑底漏光适应分割阈值大小
            leakage_area_lower=list.at(2);       //黑底漏光面积下限
            leakage_area_upper=list.at(3);       //黑底漏光面积上限
            leakageth=list.at(4);                //黑底漏光亮度阈值
            leakage_w_h=list.at(5);              //黑底漏光长宽比阈值
        }
    }

    //==========================显示异常算法参数===========================
    QString sq10=QStringLiteral("select *from displayerror_para where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq10);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<double> list; //创建取结果的列表
        while(query.next())
        {
            double parameter = query.value("显示异常参数值").toDouble();
            list.append(parameter);
        }
        if(!list.empty() && list.size()>0)
        {
            //==========================显示异常参数值==============================
            white_gray_lower=list.at(0);    //白色灰度下限，白底亮度过低认为显示异常
            black_gray_upper=list.at(1);    //黑色灰度上限  黑底亮度过高认为显示异常
        }
    }

    //==========================ROI提取算法参数===========================
    QString sq11=QStringLiteral("select *from product_model where 产品型号编码='%0'").arg(Model_Code);
    query.exec(sq11);
    if(!query.exec())
    {
        QMessageBox::about(this, "严重报错", "参数初始化出错!");
    }
    else
    {
        QList<QString> list; //创建取结果的列表
        QString parameter;
        while(query.next())
        {
            parameter = query.value("外形尺寸").toString();
            list.append(parameter);
        }

        if(parameter=="l*w*h"||parameter=="0*0")
        {
            pixel_num=3000;
        }
        else
        {
            if(!list.empty() && list.size()>0)
            {
                //获取长宽尺寸
                QString num_long,num_width;
                for(int i=0;i<parameter.size();i++)
                {
                    if(parameter[i]!="*")
                    num_long+=parameter[i];
                    if(parameter[i]=="*")
                    break;
                }
                for(int i=0;i<parameter.size();i++)
                {
                    if(parameter[i]=="*")
                    {
                        i++;
                        while(i<parameter.size())
                        {
                            num_width+=parameter[i];
                            i++;
                        }
                        break;
                    }
                }

                size_long=num_long.toDouble();
                size_width=num_width.toDouble();

                roi_proportion=size_long/size_width;
                pixel_num=1500*roi_proportion;
                if(pixel_num%2!=0)
                    pixel_num++;
        }


        }
    }

    black_colorline=0.9;          //彩线亮度增强系数

    //==========================太阳斑缺陷参数值===========================
    white_seban_step = 30;            //步长
    white_seban_size = 200;           //窗口大小
    white_seban_th_h_low = 15;        //H阈值
    white_seban_th_s_low = 15;        //S阈值
    white_seban_th_v_low = 50;        //V阈值
    white_seban_th_h_up = 15;         //H阈值
    white_seban_th_s_up = 15;         //S阈值
    white_seban_th_v_up = 50;         //Y阈值
    white_seban_size2 = 50;           //步长
    white_seban_area_lower = 200;     //面积阈值下限700
    white_seban_area_upper = 2000;   //面积阈值上限

    //==========================测试数据显示信息栏============================
    test_num=0;        //测试总数目
    qualified_num=0;   //合格总数目
    yiwu_num=0;        //异物缺陷数目
    liangdian_num=0;   //亮点
    white_num=0;       //白点数目
    mura_num=0;        //白印数目
    lackline_num=0;    //少线数目
    leakage_num=0;     //漏光数目
    redBlue_num=0;     //红蓝斑数目
    scratch_num=0;     //划伤数目
    dispalyError_num=0;//显示异常数目
    pass_per=0;        //合格率

}


/*=========================================================
* 函 数 名: 更新当前批次的已有测试信息
* 功能描述:
* 输入：
=========================================================*/
void production_lot_information_win::update_alreadyNum()
{
    QSqlQuery query(db);


    QString sq1=QStringLiteral("SELECT 测试总数,良品总数,缺陷总数,良品率 FROM history_batch where 生产批次编码='%1'").arg(product_Lot);
    query.exec(sq1);
    query.next();
    test_num=query.value("测试总数").toInt();
    qualified_num=query.value("良品总数").toInt();
    pass_per=query.value("良品率").toDouble();
    qDebug()<<sq1<<endl;
    qDebug()<<test_num<<qualified_num<<pass_per<<endl;


    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='背光异物'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    yiwu_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='少料'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    shaoliao_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='白点'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    white_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='十字'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    shizi_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='爆灯'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    baodeng_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='边框'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    biankuang_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='白印-膜拱-白团'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    mura_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='灯眼'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    dengyan_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='亮边'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    liangbian_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='划伤'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    scratch_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='显异-暗区'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    lackline_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='暗角'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    anjiao_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='色差'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    secha_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='漏光'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    louguang_num=query.value(0).toInt();

    sq1=QStringLiteral("SELECT COUNT(*) FROM '%1' where 缺陷类型名称='打折'").arg(product_Type+product_Lot);
    query.exec(sq1);
    query.next();
    dazhe_num=query.value(0).toInt();


}
