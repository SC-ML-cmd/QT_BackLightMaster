#ifndef EXCELHEPLER_H
#define EXCELHEPLER_H
#include<QAxObject>
using namespace std;
#include <QSqlTableModel>
#include<QTableView>

class ExcelHepler
{
public:
    ExcelHepler();
    bool static save(QString filePath,QStringList headers,QList<QStringList> data,QString comment="");
   //将QTableView保存为excel
   bool static saveFromTable(QString filePath,QTableView *tableView,QString comment="");
   bool static insert(QSqlQuery& query, QString sheetName, QStringList slist);
   void static Table2ExcelByHtml(QTableView *table,QString title);
};

#endif // EXCELHEPLER_H
