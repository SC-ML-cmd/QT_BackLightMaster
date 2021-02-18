#ifndef PRODUCTION_LOT_INFORMATION_WIN_H
#define PRODUCTION_LOT_INFORMATION_WIN_H

#include <QMainWindow>
#include"newproductionlot_window.h"
#include"modify_window.h"
#include"production_feature_window.h"
#include"find_win.h"
#include <QHeaderView>
#include <QList>
#include <functional>
#include <PagingWidget.h>
#include<global_variable.h>
#include <vector>
#include<QVector>
#include <QTableView>
namespace Ui {
class production_lot_information_win;
}
struct UserData
{
    QString productID;
    QString productLotCode;
    QString productLot;
    QString testTime;
    QString productType;
};
class production_lot_information_win : public QMainWindow
{
    Q_OBJECT

public:
    explicit production_lot_information_win(QWidget *parent = 0);
    ~production_lot_information_win();
   // void  isfloating(QDockWidget *dock);
    void updateData(int rows);
    QTableView *dateCountTable;
    void SetPages(int rows, int totalRows);

    void ClearData();

    void ClearTable();

    int sqliteCols;

    void InitTable(int rows, int totalRows);

     void UpdateTable(int startRow, int endRow);

     void CurrentPageChanged(int page);

     void initial_para();

     void update_alreadyNum();


     //QDockWidget *dock;
private slots:
    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_3_triggered();

    void onTableClicked(const QModelIndex &index);

    void on_action_4_triggered();
    //void setmaxDock(QDockWidget *dock);
    void SlotCurrentPageChanged(int page);
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void closeEvent(QCloseEvent* event);

signals:
    void infoSend(QString,QString);

private:
    Ui::production_lot_information_win *ui;
    newProductionlot_window *new_product_win;
    modify_window *modyfy_win;
    production_feature_window *product_feature_win;
    find_win *find_window;
    int TotalRows_ = 0;
    int Pages_ = 0; //最大页数
    int EachRows_ = 0; //每页行数
    PagingWidget *PagingWidget_ = nullptr;
    QList<UserData*> DataList_;
    QVector<QString> dataList;


};

#endif // PRODUCTION_LOT_INFORMATION_WIN_H
