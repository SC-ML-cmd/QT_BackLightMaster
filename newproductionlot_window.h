#ifndef NEWPRODUCTIONLOT_WINDOW_H
#define NEWPRODUCTIONLOT_WINDOW_H

#include <QDialog>

namespace Ui {
class newProductionlot_window;
}

class newProductionlot_window : public QDialog
{
    Q_OBJECT

public:
    explicit newProductionlot_window(QWidget *parent = 0);
    void production_Lot_ComBox(QString currentDate);

    QString product_type;


    ~newProductionlot_window();

private slots:
    void on_pushButton_clicked();

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_comboBox_2_currentTextChanged(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void add_product_type(QString sInfo);

private:
    Ui::newProductionlot_window *ui;
};

#endif // NEWPRODUCTIONLOT_WINDOW_H

