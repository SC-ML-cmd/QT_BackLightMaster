#ifndef PARA_COPY_H
#define PARA_COPY_H

#include <QMainWindow>
#include <QWidget>
#include "dbhelper.h"
#include <QSqlTableModel>
#include <QDialog>


namespace Ui {
class Para_Copy;
}

class Para_Copy : public QMainWindow
{
    Q_OBJECT

public:
    explicit Para_Copy(QWidget *parent = nullptr);
    ~Para_Copy();
//    static QString Modcopy_bef;
//    static QString Modcopy_aft;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

public slots:
    void infoRecv2(QString);

signals:
    void infoSend5(QString);

private:
    Ui::Para_Copy *ui;
    QSqlTableModel *model;
};

#endif // PARA_COPY_H
