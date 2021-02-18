#ifndef PROMODEL_H
#define PROMODEL_H

#include <QMainWindow>
#include <QWidget>
#include "dbhelper.h"
#include <QSqlTableModel>
#include <QDialog>
#include<QCloseEvent>
#include<global_variable.h>
namespace Ui {
class promodel;
}

class promodel : public QMainWindow
{
    Q_OBJECT

public:
    explicit promodel(QWidget *parent = nullptr);
    ~promodel();

private slots:
    void on_action_triggered();
    void on_action_2_triggered();

    void on_action_5_triggered();

    void on_action_6_triggered();

    void on_action_4_triggered();

    void on_action_3_triggered();

    void on_action_7_triggered();

    void closeEvent(QCloseEvent *event);

signals:
    void infoSend2(QString);
    void infoSend7(QString);

public slots:
    void infoRecv(QString);
    void infoRecv3(QString);
    void infoRecv4(QString,QString,QString,QString,QString,QString,QString);
    void infoRecv5(QString);
private:
    Ui::promodel *ui;
    QSqlTableModel *model;
    QSqlTableModel *model1;
};

#endif // PROMODEL_H
