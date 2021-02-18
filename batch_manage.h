#ifndef BATCH_MANAGE_H
#define BATCH_MANAGE_H

#include <QWidget>
#include "dbhelper.h"
namespace Ui {
class batch_manage;
}

class batch_manage : public QWidget
{
    Q_OBJECT

public:
    explicit batch_manage(QWidget *parent = nullptr);
    ~batch_manage();

private slots:
    void on_pushButton_clicked();
    void closeEvent(QCloseEvent *event);//头文件中声明

signals:
    void sendData(QString e,QString a);//信号

private:
    Ui::batch_manage *ui;
};

#endif // BATCH_MANAGE_H
