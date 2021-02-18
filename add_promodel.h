#ifndef ADD_PROMODEL_H
#define ADD_PROMODEL_H

#include <QDialog>
#include <QWidget>
#include "dbhelper.h"
#include <QSqlTableModel>

namespace Ui {
class add_promodel;
}

class add_promodel : public QDialog
{
    Q_OBJECT

public:
    explicit add_promodel(QWidget *parent = nullptr);
    ~add_promodel();

private slots:
    void on_buttonBox_accepted();

    //void on_buttonBox_clicked(QAbstractButton *button);

    void on_pushButton_2_clicked();

//signals:

//　　void infoSend(QString);
    void on_pushButton_3_clicked();

signals:
    void infoSend(QString);
private:
    Ui::add_promodel *ui;
};

#endif // ADD_PROMODEL_H
