#ifndef HISTORY_DATA_H
#define HISTORY_DATA_H
#include <QWidget>
#include "dbhelper.h"
#include <QSqlTableModel>

namespace Ui {
class history_data;
}

class history_data : public QWidget
{
    Q_OBJECT

public:
    explicit history_data(QWidget *parent = nullptr);
    ~history_data();

private slots:
    void on_pushButton_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);

    void receiveData(QString e);

    void on_btn_OutputData_clicked();

    void on_btn_FindData_clicked();

    void on_btn_DeleteData_clicked();

    void on_TV_ShowData_doubleClicked(const QModelIndex &index);

    void on_action_delete_triggered();

    void on_action_triggered();

    void on_action_detail_triggered();

    void on_action_return_triggered();

    void on_action_find_triggered();

signals:
     void sendBatch(QString a);//信号
     void sig();
private:
    Ui::history_data *ui;
    QSqlTableModel *model;
    void addItemContent(int row, int column, QString content);
    void on_TableView_doubleClicked(const QModelIndex &index);

};

#endif // HISTORY_DATA_H
