#include "datastatistics_win.h"
#include "ui_datastatistics_win.h"
#include <QSqlQuery>
#include <QComboBox>
#include <QListView>
#include <QDebug>
#include"QMouseEvent"
#include <QSqlError>
#include "QTimer"

bool mousePreess_Flag=false;
bool mousePreess_Flag_1=false;
bool isShowed=false;
bool datastatistics_win_show;//统计窗口是否已经存在的标志位
QT_CHARTS_USE_NAMESPACE
using namespace std;
extern QSqlDatabase  db;
QSqlQueryModel *Data_Model;
ListView *show_listView;
QChartView *chartView;

datastatistics_win::datastatistics_win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::datastatistics_win)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);//最大化窗体
    //默认全选
    ui->SelectAll_radioButton1->setCheckable(true);
    ui->SelectAll_radioButton2->setCheckable(true);
    ui->SelectAll_radioButton3->setCheckable(true);
    ui->SelectAll_radioButton4->setCheckable(true);
    ui->SelectAll_radioButton5->setCheckable(true);
    //控制统计图和统计数据的显示比例
    ui->splitter->setStretchFactor(0,4);
    ui->splitter->setStretchFactor(1,1);
    //测试日期选择表
    Data_Model = new QSqlQueryModel;
    Data_Model->setQuery(tr("select  distinct 测试日期 from history_batch"));
    Data_Model->setHeaderData(0, Qt::Horizontal, tr("测试日期"));
    ui->SelectDate_tableView->setModel(Data_Model); //绑定模型与视图
    ui->SelectDate_tableView->horizontalHeader()->setSectionsClickable(true);//可点击
    ui->SelectDate_tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->SelectDate_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->SelectDate_tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
    ui->SelectDate_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->SelectDate_tableView->selectAll();

    //生产批次编码选择表
    QSqlQueryModel *Batch__Model = new QSqlQueryModel;
    Batch__Model->setQuery(tr("select  distinct 生产批次编码 from history_batch"));
    Batch__Model->setHeaderData(0, Qt::Horizontal, tr("生产批次编码"));
    ui->SelectBatch_tableView->setModel(Batch__Model); //绑定模型与视图
    ui->SelectBatch_tableView->horizontalHeader()->setSectionsClickable(true);//可点击
    ui->SelectBatch_tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->SelectBatch_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->SelectBatch_tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
    ui->SelectBatch_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->SelectBatch_tableView->selectAll();
    //产品型号选择表
    QSqlQueryModel *Product_Model = new QSqlQueryModel;
    Product_Model->setQuery(tr("select  distinct 产品型号 from history_batch"));
    Product_Model->setHeaderData(0, Qt::Horizontal, tr("产品型号"));
    ui->SelectProductModel_tableView->setModel(Product_Model); //绑定模型与视图
    ui->SelectProductModel_tableView->horizontalHeader()->setSectionsClickable(true);//可点击
    ui->SelectProductModel_tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->SelectProductModel_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->SelectProductModel_tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
    ui->SelectProductModel_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->SelectProductModel_tableView->selectAll();
    //屏幕类型选择表
    QSqlQueryModel *ScreenType_Model = new QSqlQueryModel;
    ScreenType_Model->setQuery(tr("select distinct 屏幕类型 from history_batch"));
    ScreenType_Model->setHeaderData(0, Qt::Horizontal, tr("屏幕类型"));
    ui->SelectScreenType_tableView->setModel(ScreenType_Model); //绑定模型与视图
    ui->SelectScreenType_tableView->horizontalHeader()->setSectionsClickable(true);//可点击
    ui->SelectScreenType_tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->SelectScreenType_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->SelectScreenType_tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
    ui->SelectScreenType_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->SelectScreenType_tableView->selectAll();

    //缺陷类型选择表
    QStandardItemModel *DefectType_Model = new QStandardItemModel();

    QStandardItem* item = new QStandardItem(QString("合格"));
    //设置表中数据
    DefectType_Model->setItem(0,0,item);
    item=new QStandardItem(QString("背光异物"));
    DefectType_Model->setItem(1,0,item);
    item=new QStandardItem(QString("少线"));
    DefectType_Model->setItem(2,0,item);
    item=new QStandardItem(QString("横线"));
    DefectType_Model->setItem(3,0,item);
    item=new QStandardItem(QString("白印"));
    DefectType_Model->setItem(4,0,item);
    item=new QStandardItem(QString("白点"));
    DefectType_Model->setItem(5,0,item);
    item=new QStandardItem(QString("划伤"));
    DefectType_Model->setItem(6,0,item);
    item=new QStandardItem(QString("灯眼"));
    DefectType_Model->setItem(7,0,item);
    item=new QStandardItem(QString("显示异常"));
    DefectType_Model->setItem(8,0,item);
    item=new QStandardItem(QString("黑底边角漏光"));
    DefectType_Model->setItem(9,0,item);
    item=new QStandardItem(QString("红蓝斑"));
    DefectType_Model->setItem(10,0,item);
    item=new QStandardItem(QString("亮点"));
    DefectType_Model->setItem(11,0,item);
    item=new QStandardItem(QString("彩线"));
    DefectType_Model->setItem(12,0,item);
    ui->SelectDefectType_tableView->setModel(DefectType_Model); //绑定模型与视图
    DefectType_Model->setHeaderData(0, Qt::Horizontal, tr("缺陷类型"));
    ui->SelectDefectType_tableView->horizontalHeader()->setSectionsClickable(true);//可点击
    ui->SelectDefectType_tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->SelectDefectType_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->SelectDefectType_tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
    ui->SelectDefectType_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->SelectDefectType_tableView->selectAll();
    //横轴选项
    ui->VariableGrouping_comboBox->addItem("测试日期");
    ui->VariableGrouping_comboBox->addItem("生产批次编码");
    ui->VariableGrouping_comboBox->addItem("产品型号");
    ui->VariableGrouping_comboBox->addItem("屏幕类型");
    ui->VariableGrouping_comboBox->addItem("缺陷类型");
    //纵轴选项
    ui->Statistics_comboBox->addItem("测试总数");
    ui->Statistics_comboBox->addItem("缺陷总数");
    ui->Statistics_comboBox->addItem("良品总数");
    ui->Statistics_comboBox->addItem("良品率");
    ui->Statistics_comboBox->addItem("生产批次总数");
    //显示方式选项
    ui->Pattern_comboBox->addItem("柱状图");
    ui->Pattern_comboBox->addItem("饼状图");
    ui->Pattern_comboBox->addItem("折线图");
    //默认全选
    ui->SelectAll_radioButton1->setChecked(true);
    ui->SelectAll_radioButton2->setChecked(true);
    ui->SelectAll_radioButton3->setChecked(true);
    ui->SelectAll_radioButton4->setChecked(true);
    ui->SelectAll_radioButton5->setChecked(true);
    //下拉列表中默认没有内容
    ui->VariableGrouping_comboBox->setCurrentIndex(-1);
    ui->Statistics_comboBox->setCurrentIndex(-1);
    ui->Pattern_comboBox->setCurrentIndex(-1);
    //禁用纵轴所有选项
    for(int i=0;i<ui->Statistics_comboBox->count();i++)
    {
        QModelIndex index = ui->Statistics_comboBox->model()->index(i, 0);  //第2项
        QVariant v(0);
        ui->Statistics_comboBox->model()->setData(index,v,Qt::UserRole -1);
    }
    //禁用统计方式下拉框
    for(int i=0;i<ui->Pattern_comboBox->count();i++)
    {
        QModelIndex index = ui->Pattern_comboBox->model()->index(i, 0);  //第2项
        QVariant v(0);
        ui->Pattern_comboBox->model()->setData(index,v,Qt::UserRole -1);
    }
    //禁用toolbox
    for(int i=0;i<ui->StatisticSetting_toolBox->count();i++)
    {
        ui->StatisticSetting_toolBox->setItemEnabled(i,false);
    }
    ui->StatisticSetting_toolBox->setCurrentIndex(0);
    Data_Model->clear();
    ui->SelectDate_tableView->viewport()->installEventFilter(this);
    ui->SelectBatch_tableView->viewport()->installEventFilter(this);
    connect(this,SIGNAL(sig_focusChanged()),this,SLOT(SelectBatch_tableview_show()));
    connect(this,SIGNAL(sig_focusChanged_1()),this,SLOT(SelectModel_tableview_show()));


     show_listView=new ListView( ui->groupBox_2);//实例化一个自定义的ListView控件
     QVBoxLayout  *generlar = new QVBoxLayout ;//垂直布局
     generlar->addWidget(show_listView);        //将show_listView放在布局里
     generlar->setContentsMargins(0, 0, 0, 0);//设置边界间隙为0
     ui->groupBox_2->setLayout(generlar);//这个布局在groupBox_2中
     show_listView->show();


}

datastatistics_win::~datastatistics_win()
{
    delete ui;
}
/*====================================================================
* 函数：on_Show_action_triggered槽函数
* 功能：点击显示按钮，根据设置显示统计图
======================================================================*/
void datastatistics_win::on_Show_action_triggered()
{
    isShowed=true;//表示listview中有图像了。
    QMap<QString,double> ShowData;//哈希表用于存储横轴和对应的纵轴值
    //获取统计范围设置部分的各个tableview的选中项
    QItemSelectionModel *SelectDateModel=ui->SelectDate_tableView->selectionModel();
    QModelIndexList DateList = SelectDateModel->selectedIndexes();

    QItemSelectionModel *SelectBatchModel=ui->SelectBatch_tableView->selectionModel();
    QModelIndexList BatchList = SelectBatchModel->selectedIndexes();

    QItemSelectionModel *SelectProductModel=ui->SelectProductModel_tableView->selectionModel();
    QModelIndexList ProductList = SelectProductModel->selectedIndexes();

    QItemSelectionModel *SelectScreenTypeModel=ui->SelectScreenType_tableView->selectionModel();
    QModelIndexList ScreenList = SelectScreenTypeModel->selectedIndexes();

    QItemSelectionModel *SelectDefectTypeModel=ui->SelectDefectType_tableView->selectionModel();
    QModelIndexList DefectList = SelectDefectTypeModel->selectedIndexes();
    //获得变量分组（横轴）对应的tableview中选中的项
    QModelIndexList VariableGroupingItems;//本列表用于存储选中的变量分组
    int currentIndex=ui->VariableGrouping_comboBox->currentIndex();
    switch(currentIndex)
    {
       case 0:VariableGroupingItems=SelectDateModel->selectedIndexes();break;
       case 1:VariableGroupingItems=SelectBatchModel->selectedIndexes();break;
       case 2:VariableGroupingItems=SelectProductModel->selectedIndexes();break;
       case 3:VariableGroupingItems=SelectScreenTypeModel->selectedIndexes();break;
       case 4:VariableGroupingItems=SelectDefectTypeModel->selectedIndexes();break;
    }
    //根据统计范围设置生成条件语句。
     QString Conditions=" where ";
     //如果测试日期表是全选，或者横轴是测试日期就先不在where条件后做日期限定
     if(!ui->SelectAll_radioButton1->isChecked()&&ui->VariableGrouping_comboBox->currentText()!="测试日期")
     {
         foreach(QModelIndex index,DateList)
         {
             Conditions+=ui->VariableGrouping_comboBox->itemText(0)+ "= '" + index.data().toString()+ "',";
         }
     }
     //如果生产批次编码表是全选，或者横轴是生产批次编码就先不在where条件后做生产批次编码限定
     if(!ui->SelectAll_radioButton2->isChecked()&&ui->VariableGrouping_comboBox->currentText()!="生产批次编码")
     {
         foreach(QModelIndex index,BatchList)
         {
             Conditions+= ui->VariableGrouping_comboBox->itemText(1)+ "= '" + index.data().toString()+ "',";
         }
     }
     //如果产品型号表是全选，或者横轴是产品型号就先不在where条件后做产品型号限定
     if(!ui->SelectAll_radioButton3->isChecked()&&ui->VariableGrouping_comboBox->currentText()!="产品型号")
     {
         foreach(QModelIndex index,ProductList)
         {
             Conditions+=ui->VariableGrouping_comboBox->itemText(2)+ "= '" + index.data().toString()+ "',";
         }
     }
     //如果屏幕类型表是全选，或者横轴是屏幕类型就先不在where条件后做屏幕类型限定
     if(!ui->SelectAll_radioButton4->isChecked()&&ui->VariableGrouping_comboBox->currentText()!="屏幕类型")
     {
         foreach(QModelIndex index,ScreenList)
         {
             Conditions+= ui->VariableGrouping_comboBox->itemText(3)+ "= '" + index.data().toString()+ "',";
         }
     }
     //如果缺陷类型表是全选，或者横轴是缺陷类型就先不在where条件后做缺陷类型限定
     if(!ui->SelectAll_radioButton4->isChecked()&&ui->VariableGrouping_comboBox->currentText()!="缺陷类型")
     {
         foreach(QModelIndex index,ScreenList)
         {
             Conditions+= ui->VariableGrouping_comboBox->itemText(4)+ "= '" + index.data().toString()+ "',";
         }
     }
     //轮流获取变量分组的各个成员作为ShowData的key
     foreach(QModelIndex index,VariableGroupingItems)
     {
         ShowData.insert(index.data().toString(),0);
     }
      QString SelectSql;//总的查询语句
      QSqlQuery *sqlQuery = new QSqlQuery;
      //由于缺陷类型在子表中，查询的时候另当别论
     if(ui->VariableGrouping_comboBox->currentText()=="缺陷类型")
     {
          if(ui->Statistics_comboBox->currentText()=="测试总数")
          {
              if(Conditions==" where ")//如果之前没有生成查询条件，那就将“where”去掉
              {
                  Conditions="";
              }
              else//如果之前生成了查询条件，就删除最后的逗号
              {
                  Conditions.remove(Conditions.length()-2,1);
              }

              SelectSql ="select 产品型号,生产批次编码 from history_batch"+Conditions;
              sqlQuery->exec(SelectSql);//根据设置先在历史记录表查询到满足条件的行
              QString Sql;
              QSqlQuery *sql = new QSqlQuery;
              while(sqlQuery->next())//轮流遍历查询到的每一行
              {
                foreach(QModelIndex index,VariableGroupingItems)//在每行对应的明细表中去查询每种缺陷的数量
                {
                    Sql="select 缺陷类型编码 from "+sqlQuery->value(0).toString()+""+sqlQuery->value(1).toString()+" where 缺陷类型编码='" + index.data().toString() + "'";//得到查询语句;
                    sql->exec(Sql);
                    int Count=0;//缺陷类型为index.data().toString()的个数;
                    while(sql->next())
                    {
                      Count++;
                    }
                     ShowData[index.data().toString()]+=Count;//哈希表存入变量分组成员（横轴）和被统计量的值（纵轴）。
                     //去除个数为0的缺陷
                     foreach (QString Name,ShowData.keys())
                     {
                         if(ShowData.operator[](Name)==0)
                             ShowData.remove(Name);
                     }
                }
              }
          }
     }
     else//缺陷类型以外的都在总表里
     {
         //这三种是求数据的总和
         if(ui->Statistics_comboBox->currentText()=="测试总数"||ui->Statistics_comboBox->currentText()=="缺陷总数"||ui->Statistics_comboBox->currentText()=="良品总数")
         {
             foreach(QModelIndex index,VariableGroupingItems)//轮流获取变量分组的各个成员
             {
                 SelectSql="select "+ui->Statistics_comboBox->currentText()+" from history_batch "+Conditions+ui->VariableGrouping_comboBox->currentText()+"='" + index.data().toString() + "'" ;
                sqlQuery->exec(SelectSql);
                int Sum=0;
                while(sqlQuery->next())
                {
                  Sum+= sqlQuery->value(0).toInt();
                }
                ShowData[index.data().toString()]=Sum;//哈希表存入变量分组成员（横轴）和被统计量的值（纵轴）。
             }
             qDebug()<<ShowData;
         }
         //这一种是将数据求平均
         else if(ui->Statistics_comboBox->currentText()=="良品率")
         {
             foreach(QModelIndex index,VariableGroupingItems)//轮流获取变量分组的各个成员
             {
                SelectSql="select "+ui->Statistics_comboBox->currentText()+" from history_batch "+Conditions+ui->VariableGrouping_comboBox->currentText()+"='" + index.data().toString() + "'" ;
                sqlQuery->exec(SelectSql);
                int Sum=0;
                int Count=0;
                while(sqlQuery->next())
                {
                  Count++;
                  Sum+= sqlQuery->value(0).toInt();
                }
                 ShowData[index.data().toString()]=QString::number(Sum/Count,'f',2).toDouble();//哈希表存入变量分组成员（横轴）和被统计量的值（纵轴）。
             }
             qDebug()<<ShowData;
         }
         //这一种是个数累加-批次数量
         else
         {
             foreach(QModelIndex index,VariableGroupingItems)//轮流获取变量分组的各个成员
             {
               SelectSql="select 生产批次编码 from history_batch "+Conditions+ui->VariableGrouping_comboBox->currentText()+"='" + index.data().toString() + "'" ;
                sqlQuery->exec(SelectSql);
                int Count=0;
                while(sqlQuery->next())
                {
                  Count++;
                }
               ShowData[index.data().toString()]=Count;//哈希表存入变量分组成员（横轴）和被统计量的值（纵轴）。
             }
             qDebug()<<ShowData;
         }
     }

     if(ui->Pattern_comboBox->currentText()=="柱状图")//显示柱状图
     {
         BarChart_Show(ShowData,"统计结果柱状图",ui->Statistics_comboBox->currentText());
         //显示表格数据
         DataTable_Show( ShowData,ui->Pattern_comboBox->currentText());
     }
     if(ui->Pattern_comboBox->currentText()=="饼状图")//显示饼状图
     {
          PieChart_Show(ShowData,"统计结果饼状图");
          //显示表格数据
          DataTable_Show( ShowData,ui->Pattern_comboBox->currentText());
     }
     if(ui->Pattern_comboBox->currentText()=="折线图")//显示折线图
     {
         LineChart_Show( ShowData,"统计结果折线图",ui->Statistics_comboBox->currentText());
         //显示表格数据
         DataTable_Show( ShowData,ui->Pattern_comboBox->currentText());
     }

}
/*====================================================================
* 函数：DataTable_Show
* 功能：数据表显示
======================================================================*/
void datastatistics_win::DataTable_Show(QMap<QString,double> ShowData,QString ShowStyle)
{
    ui->Data_tableView->horizontalHeader()->setSectionsClickable(true);//可点击
    ui->Data_tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->Data_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->Data_tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
    ui->Data_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QVector <double> data;
    if(ShowStyle=="柱状图")
    {
        double Sum=0;
        foreach (double Variable, ShowData.values())//为这组柱状图的每个竖条赋值
        {
            data.append(Variable);
            Sum+=Variable;
        }
        auto max =std::max_element(std::begin(data), std::end(data));
        double Max=*max;
        auto min =std::min_element(std::begin(data), std::end(data));
        double Min=*min;
        QStandardItemModel *model = new QStandardItemModel(this);
         /*设置行字段名*/
         model->setColumnCount(4);
         model->setHeaderData(0,Qt::Horizontal, "总数");
         model->setHeaderData(1,Qt::Horizontal, "最大值");
         model->setHeaderData(2,Qt::Horizontal, "最小值");
         model->setHeaderData(3,Qt::Horizontal, "平均值");
         /*设置一条数据*/
        model->setRowCount(1);
        model->setHeaderData(0,Qt::Vertical, "值");
        /*设置数据内容*/
        model->setItem(0, 0, new QStandardItem(QString::number(Sum,'f', 2)));//显示总数，保留两位小数
        model->setItem(0, 1, new QStandardItem(QString::number(Max,'f', 2)));
        model->setItem(0, 2, new QStandardItem(QString::number(Min,'f', 2)));
        model->setItem(0, 3, new QStandardItem(QString::number(Sum/ShowData.values().count(),'f', 2)));
        ui->Data_tableView->setModel(model);
    }
    else if(ShowStyle=="饼状图")
    {
        double Sum=0;
        foreach (double Variable, ShowData.values())//为这组柱状图的每个竖条赋值
        {
            data.append(Variable);
            Sum+=Variable;
        }
        auto max =std::max_element(std::begin(data), std::end(data));
        double Max=*max;
        auto min =std::min_element(std::begin(data), std::end(data));
        double Min=*min;
        auto positionmax = std::distance(std::begin(data),max);
        auto positionmin = std::distance(std::begin(data),min);
        int posmax = positionmax;
        int posmin = positionmin;
         qDebug()<<posmax;
         qDebug()<<posmin;
        QStandardItemModel *model = new QStandardItemModel(this);
         /*设置行字段名*/
         model->setColumnCount(4);
         model->setHeaderData(0,Qt::Horizontal, "类型个数");
         model->setHeaderData(1,Qt::Horizontal, "最大值");
         model->setHeaderData(2,Qt::Horizontal, "最小值");
         model->setHeaderData(3,Qt::Horizontal, "平均值");
         /*设置一条数据*/
        model->setRowCount(1);
        model->setHeaderData(0,Qt::Vertical, "值");
        /*设置数据内容*/
        model->setItem(0, 0, new QStandardItem(QString::number(ShowData.values().count(),'f', 1)));
        model->setItem(0, 1, new QStandardItem(ShowData.keys()[posmax]+":"+QString::number(Max/Sum*100,'f', 2)+"%"));
        model->setItem(0, 2, new QStandardItem(ShowData.keys()[posmin]+":"+QString::number(Min/Sum*100,'f', 2)+"%"));
        model->setItem(0, 3, new QStandardItem(QString::number(1.0/ShowData.values().count()*100,'f', 2)+"%"));
        ui->Data_tableView->setModel(model);
    }
    else//折线图
    {
        double Sum=0;
        foreach (double Variable, ShowData.values())//为这组柱状图的每个竖条赋值
        {
            data.append(Variable);
            Sum+=Variable;
        }
        auto max =std::max_element(std::begin(data), std::end(data));
        double Max=*max;
        auto min =std::min_element(std::begin(data), std::end(data));
        double Min=*min;
        QStandardItemModel *model = new QStandardItemModel(this);
         /*设置行字段名*/
         model->setColumnCount(4);
         model->setHeaderData(0,Qt::Horizontal, "总数");
         model->setHeaderData(1,Qt::Horizontal, "最大值");
         model->setHeaderData(2,Qt::Horizontal, "最小值");
         model->setHeaderData(3,Qt::Horizontal, "平均值");
         model->setHeaderData(3,Qt::Horizontal, "斜率");
         /*设置一条数据*/
        model->setRowCount(1);
        model->setHeaderData(0,Qt::Vertical, "值");
        /*设置数据内容*/
        model->setItem(0, 0, new QStandardItem(QString::number(Sum,'f', 2)));
        model->setItem(0, 1, new QStandardItem(QString::number(Max,'f', 2)));
        model->setItem(0, 2, new QStandardItem(QString::number(Min,'f', 2)));
        model->setItem(0, 3, new QStandardItem(QString::number(Sum/ShowData.values().count(),'f', 2)));
        model->setItem(0, 4, new QStandardItem(QString::number((ShowData.values()[ShowData.values().count()-1]-ShowData.values()[0])/(ShowData.values().count()-1),'f', 2)));
        ui->Data_tableView->setModel(model);
    }
}
/*====================================================================
* 函数：resizeEvent
* 功能：主窗口大小变化使，统计图的大小也随之改变
======================================================================*/
//void datastatistics_win::resizeEvent(QResizeEvent* event)
//{
//     QMainWindow::resizeEvent(event);
//     if(chartView!=nullptr)
//     chartView->resize(show_listView->size());
//}
/*====================================================================
* BarChart_Show
* 功能：显示柱状图
======================================================================*/
void datastatistics_win::BarChart_Show(QMap<QString,double> ShowData,QString ChartTitle,QString AxisYLable)
{
    QVector <double> data;
    QBarSet *set0 = new QBarSet("统计结果");//新建一组柱状图
    set0->setColor(QColor (0,0,255));
    set0->setLabelColor(QColor (200,160,210));
    set0->setLabelFont(QFont("宋体", 10, QFont::Bold));
    foreach (double Variable, ShowData.values())//为这组柱状图的每个竖条赋值
    {
        set0->append(Variable);
        data.append(Variable);
    }
    QStringList categories;
    foreach (QString VariableName, ShowData.keys())//每个竖条分别代表什么含义
    {
        categories.append(VariableName);
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);

    series->setVisible(true);//竖条可见
    series->setBarWidth(0.15);
    series->setLabelsPosition(QAbstractBarSeries::LabelsInsideEnd);
    series->setLabelsVisible(true);//标识可见
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(ChartTitle);
    chart->setTitleFont(QFont("Microsoft YaHei", 16, QFont::Normal));
    chart->setAnimationOptions(QChart::SeriesAnimations);//开启数据保真

    QBarCategoryAxis *axisX = new QBarCategoryAxis();//横轴
    axisX->append(categories);
    axisX->setLabelsColor(QColor(7,28,96));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    auto max =std::max_element(std::begin(data), std::end(data));
    double Max=*max;
    QValueAxis *axisY = new QValueAxis();//纵轴
    axisY->setRange(0,Max*1.08);
    axisY->setTitleText(AxisYLable);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->hide();

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);//图像保真
    chartView->setParent(show_listView);
    chartView->resize(show_listView->size());
    chartView->show();
}
/*====================================================================
* PieChart_Show
* 功能：显示饼状图
======================================================================*/
void datastatistics_win::PieChart_Show(QMap<QString,double> ShowData,QString ChartTitle)
{
    QPieSeries *series = new QPieSeries();
    double Sum=0;
    foreach (double value, ShowData.values())
    {
        Sum+=value;
    }
    QList<double> VariableList;
    QList<QString> Percentage;
    foreach (double value, ShowData.values())
    {
        Percentage.append(QString::number(value/Sum*100, 'f', 2)+"%");
        VariableList.append(value);
    }
    int Num=0;
    foreach (QString Variable, ShowData.keys())
    {
         QString  Text=Variable+":"+Percentage[Num];
         series->append(Text, VariableList[Num]);
         Num++;
    }
    series->setLabelsVisible(true);
    series->setUseOpenGL(true);//是否OpenGL加速绘制序列

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(ChartTitle);
    chart->setTitleFont(QFont("Microsoft YaHei", 16, QFont::Normal));
    chart->legend()->hide();

    chartView  = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
     chartView->setParent(show_listView);
    chartView->resize(show_listView->size());
    chartView->show();
}
/*====================================================================
* LineChart_Show
* 功能：显示折线图
======================================================================*/
void datastatistics_win::LineChart_Show(QMap<QString,double> ShowData,QString ChartTitle,QString AxisYLable)
{
    QVector <double> data;
    QLineSeries *lineseries = new QLineSeries();
    lineseries->setColor(QColor (0,0,0));
    lineseries->setName("统计结果图");
    int Num=0;
    foreach (double Variable, ShowData.values())//为这组柱状图的每个竖条赋值
    {
        data.append(Variable);
        lineseries->append(QPointF(Num, Variable));
        Num++;
    }
    QStringList categories;
    foreach (QString VariableName, ShowData.keys())//每个竖条分别代表什么含义
    {
        categories.append(VariableName);
    }
    lineseries->setPointLabelsFormat("@yPoint");
    lineseries->setPointLabelsVisible(true);
    QChart *chart = new QChart();
    chart->addSeries(lineseries);
    chart->setTitle(ChartTitle);
    chart->setTitleFont(QFont("Microsoft YaHei", 16, QFont::Normal));
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    lineseries->attachAxis(axisX);
    //axisX->setRange(QString("Jan"), QString("Jun"));
    auto max =std::max_element(std::begin(data), std::end(data));
    double Max=*max;
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    lineseries->attachAxis(axisY);
    axisY->setTitleText(AxisYLable);
    axisY->setRange(0, Max*1.1);
    chart->legend()->hide();
    chartView = new QChartView(chart);
     chartView->setRenderHint(QPainter::Antialiasing);
     chartView->setParent(show_listView);
    chartView->resize(show_listView->size());
    chartView->show();
}

/*====================================================================
* on_VariableGrouping_comboBox_currentTextChanged
* 功能：当变量分组下拉框的选项发生改变后
======================================================================*/
void datastatistics_win::on_VariableGrouping_comboBox_currentTextChanged(const QString &arg1)
{
    ui->Statistics_comboBox->clear();
    combobox_show();
    //设置纵轴下拉框
   if(ui->VariableGrouping_comboBox->currentText()=="测试日期")//选择测试日期,删除合格率，不合格率
   {
       ui->Statistics_comboBox->removeItem(2);
       ui->Statistics_comboBox->removeItem(2);
   }
   else if(ui->VariableGrouping_comboBox->currentText()=="生产批次编码")//选择生产批次编码，删除生产批次编码数量
   {
       ui->Statistics_comboBox->removeItem(0);
   }
   else if(ui->VariableGrouping_comboBox->currentText()=="产品型号")//选择产品型号,删除合格率，不合格率
   {
       ui->Statistics_comboBox->removeItem(2);
       ui->Statistics_comboBox->removeItem(2);
   }
   else if(ui->VariableGrouping_comboBox->currentText()=="屏幕类型")//选择屏幕类型,删除合格率，不合格率
   {
       ui->Statistics_comboBox->removeItem(2);
       ui->Statistics_comboBox->removeItem(2);
   }
   else if(ui->VariableGrouping_comboBox->currentText()=="缺陷类型")//选择缺陷类型,删除合格率，不合格率，生产批次编码数量
   {
       ui->Statistics_comboBox->removeItem(0);
       ui->Statistics_comboBox->removeItem(1);
       ui->Statistics_comboBox->removeItem(1);
   }
}
/*====================================================================
* combobox_show
* 功能：为被统计量下拉框赋值
======================================================================*/
void datastatistics_win::combobox_show()
{
    //纵轴选项
    ui->Statistics_comboBox->addItem("生产批次总数");
    ui->Statistics_comboBox->addItem("测试总数");
    ui->Statistics_comboBox->addItem("缺陷总数");
    ui->Statistics_comboBox->addItem("良品总数");
    ui->Statistics_comboBox->addItem("良品率");
}
/*====================================================================
* on_Statistics_comboBox_currentTextChanged
* 功能：当被统计量下拉框的选项发生改变后
======================================================================*/
void datastatistics_win::on_Statistics_comboBox_currentTextChanged(const QString &arg1)
{
    QString laterComboboxText=ui->VariableGrouping_comboBox->currentText();
    QString verticalComboboxText=ui->Statistics_comboBox->currentText();
    //禁用toolbox
    for(int i=0;i<ui->StatisticSetting_toolBox->count();i++)
    {
        ui->StatisticSetting_toolBox->setItemEnabled(i,false);
    }
    if((laterComboboxText=="生产批次编码"&&verticalComboboxText=="测试总数"))
    {
        ui->StatisticSetting_toolBox->setItemEnabled(0,true);//日期
        ui->StatisticSetting_toolBox->setItemEnabled(2,true);//产品型号
        ui->StatisticSetting_toolBox->setItemEnabled(3,true);//屏幕类型
        ui->StatisticSetting_toolBox->setItemEnabled(4,true);//缺陷类型
        ui->StatisticSetting_toolBox->setCurrentIndex(0);
    }
    else if((laterComboboxText=="测试日期"&&verticalComboboxText=="生产批次总数")||(laterComboboxText=="生产批次编码"&&verticalComboboxText=="良品率"))
    {
        ui->StatisticSetting_toolBox->setItemEnabled(0,true);//日期
        ui->StatisticSetting_toolBox->setItemEnabled(2,true);//产品型号
        ui->StatisticSetting_toolBox->setItemEnabled(3,true);//屏幕类型
        ui->StatisticSetting_toolBox->setCurrentIndex(0);
    }
    else if((laterComboboxText=="测试日期"&&verticalComboboxText=="测试总数"))
    {
        ui->StatisticSetting_toolBox->setItemEnabled(0,true);//日期
        ui->StatisticSetting_toolBox->setItemEnabled(1,true);//生产批次编码
        ui->StatisticSetting_toolBox->setItemEnabled(2,true);//产品型号
        ui->StatisticSetting_toolBox->setItemEnabled(3,true);//屏幕类型
        ui->StatisticSetting_toolBox->setItemEnabled(4,true);//缺陷类型
        ui->StatisticSetting_toolBox->setCurrentIndex(0);
    }
    else if((laterComboboxText=="产品型号"&&verticalComboboxText=="生产批次总数"))
    {
        ui->StatisticSetting_toolBox->setItemEnabled(0,true);//日期
        ui->StatisticSetting_toolBox->setItemEnabled(3,true);//屏幕类型
        ui->StatisticSetting_toolBox->setCurrentIndex(0);
    }
    else if((laterComboboxText=="产品型号"&&verticalComboboxText=="测试总数"))
    {
        ui->StatisticSetting_toolBox->setItemEnabled(0,true);//日期
        ui->StatisticSetting_toolBox->setItemEnabled(1,true);//生产批次编码
        ui->StatisticSetting_toolBox->setItemEnabled(3,true);//屏幕类型
        ui->StatisticSetting_toolBox->setItemEnabled(4,true);//缺陷类型
        ui->StatisticSetting_toolBox->setCurrentIndex(0);
    }
    else if((laterComboboxText=="屏幕类型"&&verticalComboboxText=="生产批次总数"))
    {
        ui->StatisticSetting_toolBox->setItemEnabled(2,true);//产品型号
        ui->StatisticSetting_toolBox->setItemEnabled(0,true);//日期
        ui->StatisticSetting_toolBox->setCurrentIndex(0);
    }
    else if((laterComboboxText=="屏幕类型"&&verticalComboboxText=="测试总数"))
    {
        ui->StatisticSetting_toolBox->setItemEnabled(0,true);//日期
        ui->StatisticSetting_toolBox->setItemEnabled(1,true);//生产批次编码
        ui->StatisticSetting_toolBox->setItemEnabled(2,true);//产品型号
        ui->StatisticSetting_toolBox->setItemEnabled(4,true);//缺陷类型
        ui->StatisticSetting_toolBox->setCurrentIndex(0);
    }
    else if((laterComboboxText=="缺陷类型"&&verticalComboboxText=="测试总数"))
    {
        ui->StatisticSetting_toolBox->setItemEnabled(0,true);//日期
        ui->StatisticSetting_toolBox->setItemEnabled(1,true);//生产批次编码
        ui->StatisticSetting_toolBox->setItemEnabled(2,true);//产品型号
        ui->StatisticSetting_toolBox->setItemEnabled(3,true);//屏幕类型
        ui->StatisticSetting_toolBox->setCurrentIndex(0);
    }
    //横坐标不等于空，解禁选择方式的下拉框
    if(laterComboboxText!="")
    {
        for(int i=0;i<ui->Pattern_comboBox->count();i++)
        {
            QModelIndex index = ui->Pattern_comboBox->model()->index(i, 0);  //第2项
            QVariant v(-1);
            ui->Pattern_comboBox->model()->setData(index,v,Qt::UserRole -1);
        }

       // QSqlQueryModel *Data_Model = new QSqlQueryModel;
        Data_Model->setQuery(tr("select  distinct 测试日期 from history_batch"));
        Data_Model->setHeaderData(0, Qt::Horizontal, tr("测试日期"));
        ui->SelectDate_tableView->setModel(Data_Model); //绑定模型与视图
        ui->SelectDate_tableView->selectAll();
    }
}
/*====================================================================
* on_Pattern_comboBox_currentTextChanged
* 功能：当统计方式下拉框发生改变时
======================================================================*/
void datastatistics_win::on_Pattern_comboBox_currentTextChanged(const QString &arg1)
{

}
/*====================================================================
* on_SelectAll_radioButton1_clicked
* 功能：日期全选按钮
======================================================================*/
void datastatistics_win::on_SelectAll_radioButton1_clicked()
{
    if(ui->SelectAll_radioButton1->isChecked())
    {
        ui->SelectDate_tableView->selectAll();
        QSqlQueryModel *Batch__Model = new QSqlQueryModel;
        Batch__Model->setQuery(tr("select  distinct 生产批次编码 from history_batch"));
        Batch__Model->setHeaderData(0, Qt::Horizontal, tr("生产批次编码"));
        ui->SelectBatch_tableView->setModel(Batch__Model); //绑定模型与视图
        ui->SelectBatch_tableView->horizontalHeader()->setSectionsClickable(true);//可点击
        ui->SelectBatch_tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
        ui->SelectBatch_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
        ui->SelectBatch_tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
        ui->SelectBatch_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->SelectBatch_tableView->selectAll();

        //产品型号选择表
        QSqlQueryModel *Product_Model = new QSqlQueryModel;
        Product_Model->setQuery(tr("select  distinct 产品型号 from history_batch"));
        Product_Model->setHeaderData(0, Qt::Horizontal, tr("产品型号"));
        ui->SelectProductModel_tableView->setModel(Product_Model); //绑定模型与视图
        ui->SelectProductModel_tableView->horizontalHeader()->setSectionsClickable(true);//可点击
        ui->SelectProductModel_tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
        ui->SelectProductModel_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
        ui->SelectProductModel_tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
        ui->SelectProductModel_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->SelectProductModel_tableView->selectAll();
    }
    else
    {
        ui->SelectDate_tableView->selectRow(0);
    }
}
/*====================================================================
* on_SelectAll_radioButton2_clicked
* 功能：批次全选按钮
======================================================================*/
void datastatistics_win::on_SelectAll_radioButton2_clicked()
{
    if(ui->SelectAll_radioButton2->isChecked())
    {
       ui->SelectBatch_tableView->selectAll();
       emit sig_focusChanged();
    }
    else
    {
        ui->SelectBatch_tableView->selectRow(0);
    }
}
/*====================================================================
* on_SelectAll_radioButton3_clicked
* 功能：产品型号全选按钮
======================================================================*/
void datastatistics_win::on_SelectAll_radioButton3_clicked()
{
    if(ui->SelectAll_radioButton3->isChecked())
    {
        ui->SelectProductModel_tableView->selectAll();
    }
    else
    {
        ui->SelectProductModel_tableView->selectRow(0);
    }
}
/*====================================================================
* on_SelectAll_radioButton4_clicked
* 功能：屏幕类型全选
======================================================================*/
void datastatistics_win::on_SelectAll_radioButton4_clicked()
{
    if(ui->SelectAll_radioButton4->isChecked())
    {
        ui->SelectScreenType_tableView->selectAll();
    }
    else
    {
        ui->SelectScreenType_tableView->selectRow(0);
    }
}
/*====================================================================
* on_SelectAll_radioButton5_clicked
* 功能：缺陷类型全选
======================================================================*/
void datastatistics_win::on_SelectAll_radioButton5_clicked()
{
    if(ui->SelectAll_radioButton5->isChecked())
    {
        ui->SelectDefectType_tableView->selectAll();
    }
    else
    {
        ui->SelectDefectType_tableView->selectRow(0);
    }
}
/*====================================================================
* on_SelectDate_tableView_pressed
* 功能：日期tableview点击时取消其全选
======================================================================*/
void datastatistics_win::on_SelectDate_tableView_pressed(const QModelIndex &index)
{
    ui->SelectAll_radioButton1->setChecked(false);
    mousePreess_Flag=true;
}
/*====================================================================
* on_SelectBatch_tableView_pressed
* 功能：批次tableview点击时取消其全选
======================================================================*/
void datastatistics_win::on_SelectBatch_tableView_pressed(const QModelIndex &index)
{
    ui->SelectAll_radioButton2->setChecked(false);
    mousePreess_Flag_1=true;
}
/*====================================================================
* on_SelectProductModel_tableView_pressed
* 功能：产品类型tableview点击时取消其全选
======================================================================*/
void datastatistics_win::on_SelectProductModel_tableView_pressed(const QModelIndex &index)
{
    ui->SelectAll_radioButton3->setChecked(false);
}
/*====================================================================
* on_SelectScreenType_tableView_pressed
* 功能：屏幕类型tableview点击时取消其全选
======================================================================*/
void datastatistics_win::on_SelectScreenType_tableView_pressed(const QModelIndex &index)
{
    ui->SelectAll_radioButton4->setChecked(false);
}
/*====================================================================
* on_SelectDefectType_tableView_pressed
* 功能：缺陷类型tableview点击时取消其全选
======================================================================*/
void datastatistics_win::on_SelectDefectType_tableView_pressed(const QModelIndex &index)
{
    ui->SelectAll_radioButton5->setChecked(false);
}

/*====================================================================
* SelectBatch_tableview_show
* 功能：显示生产批次编码
======================================================================*/
void datastatistics_win::SelectBatch_tableview_show()
{
    QItemSelectionModel *model_selection = ui->SelectDate_tableView->selectionModel();
    QModelIndexList IndexList= model_selection->selectedIndexes();
    QAbstractItemModel *modelItem = ui->SelectDate_tableView->model ();
    QString sqlQueryBatch="SELECT 生产批次编码 FROM history_batch where ";
    QString sqlQueryModel="SELECT 产品型号 FROM history_batch where ";
    //选取选择时间内的生产批次编码
    foreach (QModelIndex index, IndexList)
    {
         int row=index.row();
         QString data = modelItem->data(index).toString();
         QString dateQstring=QString("测试日期='%1'").arg(data);
         sqlQueryBatch=sqlQueryBatch+dateQstring;
         sqlQueryBatch+=" OR ";
         sqlQueryModel=sqlQueryModel+dateQstring;
         sqlQueryModel+=" OR ";
    }
    sqlQueryBatch.remove(sqlQueryBatch.size()-4,3);
    QSqlQuery query1 = QSqlQuery(db);//访问数据库
    bool ok=query1.exec(sqlQueryBatch);
    QSqlQueryModel *Batch__Model = new QSqlQueryModel;
    Batch__Model->setQuery(sqlQueryBatch);
    Batch__Model->setHeaderData(0, Qt::Horizontal, tr("生产批次编码"));

    //生产批次编码显示
    ui->SelectBatch_tableView->setModel(Batch__Model); //绑定模型与视图
    ui->SelectBatch_tableView->horizontalHeader()->setSectionsClickable(true);//可点击
    ui->SelectBatch_tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->SelectBatch_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->SelectBatch_tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
    ui->SelectBatch_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->SelectBatch_tableView->selectAll();

    int rows = Batch__Model->rowCount();
    IndexList.clear();
    qDebug()<<rows;
    qDebug()<<sqlQueryBatch;

    sqlQueryModel.remove(sqlQueryModel.size()-4,3);
    sqlQueryModel+="group by 产品型号";
    //产品型号显示
    QSqlQueryModel *Product_Model = new QSqlQueryModel;
    Product_Model->setQuery(sqlQueryModel);
    Product_Model->setHeaderData(0, Qt::Horizontal, tr("产品型号"));
    ui->SelectProductModel_tableView->setModel(Product_Model); //绑定模型与视图
    ui->SelectProductModel_tableView->horizontalHeader()->setSectionsClickable(true);//可点击
    ui->SelectProductModel_tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    ui->SelectProductModel_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->SelectProductModel_tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
    ui->SelectProductModel_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->SelectProductModel_tableView->selectAll();
    qDebug()<<sqlQueryModel;
}
/*====================================================================
* SelectModel_tableview_show
* 功能：显示产品型号
======================================================================*/
void datastatistics_win::SelectModel_tableview_show()
{
        QItemSelectionModel *model_selection = ui->SelectBatch_tableView->selectionModel();
        QModelIndexList IndexList= model_selection->selectedIndexes();
        QAbstractItemModel *modelItem = ui->SelectBatch_tableView->model ();
        QString sqlQueryModel="SELECT 产品型号 FROM history_batch where ";
        //选取选择时间内的生产批次编码
        foreach (QModelIndex index, IndexList)
        {
             int row=index.row();
             QString data = modelItem->data(index).toString();
             QString dateQstring=QString("生产批次编码='%1'").arg(data);
             sqlQueryModel=sqlQueryModel+dateQstring;
             sqlQueryModel+=" OR ";
        }
        sqlQueryModel.remove(sqlQueryModel.size()-4,3);
        sqlQueryModel+="group by 产品型号";
        //产品型号显示
        QSqlQueryModel *Product_Model = new QSqlQueryModel;
        Product_Model->setQuery(sqlQueryModel);
        Product_Model->setHeaderData(0, Qt::Horizontal, tr("产品型号"));
        ui->SelectProductModel_tableView->setModel(Product_Model); //绑定模型与视图
        ui->SelectProductModel_tableView->horizontalHeader()->setSectionsClickable(true);//可点击
        ui->SelectProductModel_tableView->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
        ui->SelectProductModel_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
        ui->SelectProductModel_tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
        ui->SelectProductModel_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->SelectProductModel_tableView->selectAll();
        qDebug()<<sqlQueryModel;

}
/*====================================================================
* eventFilter
* 功能：重写鼠标触发事件
======================================================================*/
bool datastatistics_win::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->SelectDate_tableView->viewport())//允许tableview框中触发鼠标离开时间
    {
        if (e->type() == QMouseEvent::MouseButtonRelease)
        {
            if(mousePreess_Flag)
            {
                //延时，使得选中后的数据稳定
                QTimer::singleShot(10, this, SLOT(timeOutEvent()));
                mousePreess_Flag=false;

            }
        }
    }
    if (obj == ui->SelectBatch_tableView->viewport())
    {
        if (e->type() == QMouseEvent::MouseButtonRelease)
        {
            if(mousePreess_Flag_1)
            {
                //延时，使得选中后的数据稳定
                QTimer::singleShot(10, this, SLOT(timeOutEvent_1()));
                mousePreess_Flag_1=false;

            }
        }
    }
    return QWidget::eventFilter(obj,e);
}

void datastatistics_win::timeOutEvent()
{
    emit sig_focusChanged();
}
void datastatistics_win::timeOutEvent_1()
{
    emit sig_focusChanged_1();
}
/*====================================================================
* on_Save_action_triggered
* 功能：保存统计图
======================================================================*/
void datastatistics_win::on_Save_action_triggered()
{
   if(isShowed)
   {
       QString fileName = QFileDialog::getSaveFileName(this,
               tr("Open Config"),
               "",
               tr("Config Files (*.png)"));

           if (!fileName.isNull())
           {
               QScreen * screen = QGuiApplication::primaryScreen();
               QPixmap p = screen->grabWindow(show_listView->winId());
               QImage image = p.toImage();
               image.save(fileName);
           }
           else
           {
               //点的是取消
           }
   }
}
/*====================================================================
* on_Setting_action_triggered
* 功能：点击设置按钮，显示设置窗体
======================================================================*/
void datastatistics_win::on_Setting_action_triggered()
{
     ui->Settting_dockWidget->show();
}
/*====================================================================
* closeEvent
* 功能：关闭窗体
======================================================================*/
void datastatistics_win::closeEvent(QCloseEvent *event)
{
    this->close();
    datastatistics_win_show=false;
}
