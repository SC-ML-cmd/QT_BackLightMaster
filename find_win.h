#ifndef FIND_WIN_H
#define FIND_WIN_H

#include <QDialog>

namespace Ui {
class find_win;
}

class find_win : public QDialog
{
    Q_OBJECT

public:
    explicit find_win(QWidget *parent = 0);
    ~find_win();

private slots:
    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_comboBox_2_currentTextChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::find_win *ui;
};

#endif // FIND_WIN_H

