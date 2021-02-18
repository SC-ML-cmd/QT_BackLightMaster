#ifndef MODIFY_WINDOW_H
#define MODIFY_WINDOW_H

#include <QDialog>

namespace Ui {
class modify_window;
}

class modify_window : public QDialog
{
    Q_OBJECT

public:
    explicit modify_window(QWidget *parent = 0);
    void setTextEditData(QString productID,QString productLotCode,QString prodectLot,QString testTime,QString productType);
    QString updateProductID;
    ~modify_window();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::modify_window *ui;
};

#endif // MODIFY_WINDOW_H
