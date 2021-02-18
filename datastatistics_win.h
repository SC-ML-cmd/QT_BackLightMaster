#ifndef DATASTATISTICS_WIN_H
#define DATASTATISTICS_WIN_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QComboBox>
#include <QMessageBox>
#include <QtCharts>
#include"QStandardItemModel"
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QFontDialog>
#include <QtCharts/QLineSeries>
#include <QStandardItemModel>
#include <QCoreApplication>
#include <QListView>
#include <QStringList>
#include <QDebug>
#include <stdlib.h>
#include <QFileDialog>
#include <QWidget>
#include "listview.h"
#include<global_variable.h>

namespace Ui {
class datastatistics_win;
}
class datastatistics_win : public QMainWindow
{
    Q_OBJECT

public:
    explicit datastatistics_win(QWidget *parent = nullptr);
    void combobox_show();
    ~datastatistics_win();


signals:
    void date_tableview_click();
    void sig_focusChanged();
    void sig_focusChanged_1();
private slots:
    void on_Show_action_triggered();

    void on_Pattern_comboBox_currentTextChanged(const QString &arg1);

    void on_SelectAll_radioButton1_clicked();

    void on_SelectAll_radioButton2_clicked();

    void on_SelectAll_radioButton3_clicked();

    void on_SelectAll_radioButton4_clicked();

    void on_SelectAll_radioButton5_clicked();

    void on_SelectDate_tableView_pressed(const QModelIndex &index);

    void on_SelectBatch_tableView_pressed(const QModelIndex &index);

    void on_SelectProductModel_tableView_pressed(const QModelIndex &index);

    void on_SelectScreenType_tableView_pressed(const QModelIndex &index);

    void on_SelectDefectType_tableView_pressed(const QModelIndex &index);

    void SelectBatch_tableview_show();

    void SelectModel_tableview_show();

    bool eventFilter(QObject *obj, QEvent *e);

    void timeOutEvent();

    void timeOutEvent_1();

    void on_VariableGrouping_comboBox_currentTextChanged(const QString &arg1);

    void on_Statistics_comboBox_currentTextChanged(const QString &arg1);

    void on_Save_action_triggered();

    void on_Setting_action_triggered();

    void closeEvent(QCloseEvent *event);

private:
    void BarChart_Show(QMap<QString,double> ShowData,QString ChartTitle,QString AxisYLable);
    void PieChart_Show(QMap<QString,double> ShowData,QString ChartTitle);
    void LineChart_Show(QMap<QString,double> ShowData,QString ChartTitle,QString AxisYLable);
    void DataTable_Show(QMap<QString,double> ShowData,QString ShowStyle);
   // void resizeEvent(QResizeEvent* event);

private:
       Ui::datastatistics_win *ui;
};

#endif // DATASTATISTICS_WIN_H
