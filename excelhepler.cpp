#include "excelhepler.h"
#include<QDebug>
#include <QSqlQuery>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include <QAxObject>
#include<QTableWidget>
#include <QDesktopServices>
ExcelHepler::ExcelHepler()
{

}
bool ExcelHepler::saveFromTable(QString filePath, QTableView *tableView, QString comment)  //未使用，需配置ODBC数据源，且和当前软件开发位数相关
{
    QAbstractItemModel *model=tableView->model();
    const int column=model->columnCount();
    const int row=model->rowCount();

    //header
    QStringList headers;
    for(int i=0;i<column;i++)
    {
        //隐藏列
        if(tableView->isColumnHidden(i))
            continue;
        headers<<model->headerData(i,Qt::Horizontal).toString();
    }

    //data
    QStringList list;
    QList<QStringList> data;
    for(int i=0;i<row;i++)
    {
        if(model->index(i,0).data().isNull())
            continue;
        list.clear();
        for(int j=0;j<column;j++){
            //隐藏列
            if(tableView->isColumnHidden(j))
                continue;
            list<<model->index(i,j).data().toString();
        }
        data<<list;
    }
    return ExcelHepler::save(filePath,headers,data,comment);
}
bool ExcelHepler::save(QString filePath, QStringList headers, QList<QStringList> data,QString comment)
{
    QString sheetName = "Sheet1";

    // 创建一个数据库实例， 设置连接字符串
    QSqlDatabase dbexcel = QSqlDatabase::addDatabase("QODBC","excelexport");
    if(!dbexcel.isValid())
    {
        qDebug()<<"数据库驱动异常";
        return false;   //! type error
    }

    //QString dsn = QString("DRIVER={Microsoft Excel Driver (*.xls, *.xlsx, *.xlsm, *.xlsb)};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%1\";DBQ=%2").
    //            arg(filePath).arg(filePath);
    QString dsn = QString("DRIVER={Microsoft Excel Driver (*.xls)};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%1\";DBQ=%2").
                  arg(filePath).arg(filePath);
    qDebug()<<dsn;
    dbexcel.setDatabaseName(dsn);

    // open connection
    if(!dbexcel.open())
    {
         qDebug()<<"无法打开数据库";
        return false;  //! db error
    }

    QSqlQuery query(dbexcel);
    QString sql;

    // drop the table if it's already exists
    sql = QString("DROP TABLE [%1]").arg(sheetName);
    query.exec( sql);
    //create the table (sheet in Excel file)
    sql = QString("CREATE TABLE [%1] (").arg(sheetName);
    foreach (QString name, headers) {
        sql +=QString("[%1] varchar(200)").arg(name);
        if(name!=headers.last())
            sql +=",";
    }
    sql += ")";
    query.prepare(sql);
    if( !query.exec()) {
        //UIDemo01::printError( query.lastError());
        dbexcel.close();
        return false;
    }
    foreach (QStringList slist, data) {
        insert(query,sheetName,slist);
    }
    if(!comment.isEmpty())
    {
        QStringList slist;
        slist<<comment;
        for(int i=0,n=headers.size()-1;i<n;i++)
        {
            slist<<"";
        }
        insert(query,sheetName,slist);
    }

    dbexcel.close();
    return true;
}
bool ExcelHepler::insert(QSqlQuery &query, QString sheetName, QStringList slist)
{
    QString sSql = QString("INSERT INTO [%1] VALUES(").arg(sheetName);
    for(int i=0,n=slist.size();i<n;i++)
    {
        sSql+=QString(":%1").arg(i);
        if(i!=n-1)
            sSql+=",";
        else
            sSql+=")";
    }
    query.prepare(sSql);
    for(int i=0,n=slist.size();i<n;i++)
    {
        query.bindValue(QString(":%1").arg(i),slist.at(i));
    }
    if( !query.exec()) {

        return false;
    }
    return true;
}

//第一个参数是页面上的表格，第二个是导出文件的表头数据
 void ExcelHepler::Table2ExcelByHtml(QTableView *table,QString title)
 {
     QMessageBox *msgBox;
     msgBox = new QMessageBox("提示","正在保存...", QMessageBox::NoIcon,0,0,0);
     msgBox->show();
     QString fileName = QFileDialog::getSaveFileName(table, "保存",QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),"Excel 文件(*.xls)");
     if (fileName!="")
     {
         QAxObject *excel = new QAxObject;
         if (excel->setControl("Excel.Application")) //连接Excel控件
         {
             excel->dynamicCall("SetVisible (bool Visible)","false");//不显示窗体
             excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
             QAxObject *workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
             workbooks->dynamicCall("Add");//新建一个工作簿
             QAxObject *workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
             QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);

             int i,j;
             //QTablewidget 获取数据的列数
             //int colcount=table->columnCount();
              //QTablewidget 获取数据的行数
             //int rowscount=table->rowCount()
             //QTableView 获取列数
             QAbstractItemModel *model=table->model();

             const int colount=model->columnCount();
             //QTableView 获取行数
             const int rowcount=model->rowCount();

             QAxObject *cell,*col;
             //标题行
             cell=worksheet->querySubObject("Cells(int,int)", 1, 1);
             cell->dynamicCall("SetValue(const QString&)", title);
             cell->querySubObject("Font")->setProperty("Size", 18);
             //调整行高
             worksheet->querySubObject("Range(const QString&)", "1:1")->setProperty("RowHeight", 30);
             //合并标题行
             QString cellTitle;
             cellTitle.append("A1:");
             cellTitle.append(QChar(colount - 1 + 'A'));
             cellTitle.append(QString::number(1));
             QAxObject *range = worksheet->querySubObject("Range(const QString&)", cellTitle);
             range->setProperty("WrapText", true);
             range->setProperty("MergeCells", true);
             range->setProperty("HorizontalAlignment", -4108);//xlCenter
             range->setProperty("VerticalAlignment", -4108);//xlCenter
             //列标题
             for(i=0;i<colount;i++)
             {
                 QString columnName;
                 columnName.append(QChar(i  + 'A'));
                 columnName.append(":");
                 columnName.append(QChar(i + 'A'));
                 col = worksheet->querySubObject("Columns(const QString&)", columnName);
                 col->setProperty("ColumnWidth", table->columnWidth(i)/6);
                 cell=worksheet->querySubObject("Cells(int,int)", 2, i+1);
                 //QTableWidget 获取表格头部文字信息
                 //columnName=table->horizontalHeaderItem(i)->text();
                 //QTableView 获取表格头部文字信息
                  columnName=model->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString();
                 cell->dynamicCall("SetValue(const QString&)", columnName);
                 cell->querySubObject("Font")->setProperty("Bold", true);
                 cell->querySubObject("Interior")->setProperty("Color",QColor(191, 191, 191));
                 cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                 cell->setProperty("VerticalAlignment", -4108);//xlCenter
             }

            //数据区

            //QTableWidget 获取表格数据部分
            /* for(i=0;i<rowcount;i++){
                 for (j=0;j<colcount;j++)
                 {
                     worksheet->querySubObject("Cells(int,int)", i+3, j+1)->dynamicCall("SetValue(const QString&)", table->item(i,j)?table->item(i,j)->text():"");
                 }
             }*/


            //QTableView 获取表格数据部分
             for(i=0;i<rowcount;i++) //行数
               {
                     for (j=0;j<colount;j++)   //列数
                     {
                         QModelIndex index = model->index(i, j);
                         QString strdata=model->data(index).toString();
                         worksheet->querySubObject("Cells(int,int)", i+3, j+1)->dynamicCall("SetValue(const QString&)", strdata);
                     }
               }


             //画框线
             QString lrange;
             lrange.append("A2:");
             lrange.append(colount - 1 + 'A');
             lrange.append(QString::number(rowcount + 2));
             range = worksheet->querySubObject("Range(const QString&)", lrange);
             range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
             range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));
             //调整数据区行高
             QString rowsName;
             rowsName.append("2:");
             rowsName.append(QString::number(rowcount + 2));
             range = worksheet->querySubObject("Range(const QString&)", rowsName);
             range->setProperty("RowHeight", 20);
             workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName));//保存至fileName
             workbook->dynamicCall("Close()");//关闭工作簿
             excel->dynamicCall("Quit()");//关闭excel
             delete excel;
             excel=NULL;
             msgBox->close();
             QMessageBox::information(NULL,"提示","保存成功");

//             if (QMessageBox::question(NULL,"完成","文件已经导出，是否现在打开？",QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
//             {
//                 QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
//             }
         }
         else
         {
             msgBox->close();
             QMessageBox::warning(NULL,"错误","未能创建 Excel 对象，请安装 Microsoft Excel。",QMessageBox::Apply);
         }
     }
     else
     {
         msgBox->close();
     }
 }
