#ifndef CAMERA_SET_GRAY_H
#define CAMERA_SET_GRAY_H
#include"QSqlTableModel"
#include <QDialog>

namespace Ui {
class Camera_set_gray;
}

class Camera_set_gray : public QDialog
{
    Q_OBJECT

public:
    explicit Camera_set_gray(QWidget *parent = nullptr);
    ~Camera_set_gray();
    QSqlTableModel *model;

private slots:
    void on_pushButton_clicked();

private:
    Ui::Camera_set_gray *ui;
};

#endif // CAMERA_SET_GRAY_H
