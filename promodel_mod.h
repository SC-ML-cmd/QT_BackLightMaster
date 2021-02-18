#ifndef PROMODEL_MOD_H
#define PROMODEL_MOD_H

#include <QMainWindow>

namespace Ui {
class promodel_mod;
}

class promodel_mod : public QMainWindow
{
    Q_OBJECT

public:
    explicit promodel_mod(QWidget *parent = nullptr);
    ~promodel_mod();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

signals:
    void infoSend3(QString);

private:
    Ui::promodel_mod *ui;
};

#endif // PROMODEL_MOD_H
