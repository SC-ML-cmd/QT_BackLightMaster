#ifndef HISTORY_BATCHFIND_H
#define HISTORY_BATCHFIND_H

#include <QDialog>

namespace Ui {
class history_batchfind;
}


class history_batchfind : public QDialog
{
    Q_OBJECT

public:
    explicit history_batchfind(QWidget *parent = nullptr);
    ~history_batchfind();
    QString SQ_Find;

private slots:
    void on_Cbx_FindStyle_currentIndexChanged(const QString &arg1);

    void on_btn_Find_clicked();

    void on_history_batchfind_accepted();
signals:
    void sendData(QString e);
private:
    Ui::history_batchfind *ui;
};

#endif // HISTORY_BATCHFIND_H
