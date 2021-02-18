#ifndef DETECTOR_CHOOSE_H
#define DETECTOR_CHOOSE_H

#include <QDialog>
#include"global_variable.h"

namespace Ui {
class Detector_Choose;
}

class Detector_Choose : public QDialog
{
    Q_OBJECT

public:
    explicit Detector_Choose(QWidget *parent = nullptr);
    ~Detector_Choose();

private slots:
    void on_Cbx_ForeignBody_stateChanged(int arg1);

    void on_btn_sure_clicked();

    void on_btn_Cancel_clicked();

    void ini_state();


    void on_combx_SelectAllOrNo_activated(const QString &arg1);

private:
    Ui::Detector_Choose *ui;
};

#endif // DETECTOR_CHOOSE_H
