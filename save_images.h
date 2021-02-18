#ifndef SAVE_IMAGES_H
#define SAVE_IMAGES_H

#include <QMainWindow>
#include<QCloseEvent>
#include<global_variable.h>

//extern QString imageSavePath;//图片保存路径
//extern bool autoSave;//是否自动保存标志位

namespace Ui {
class save_images;
}

class save_images : public QMainWindow
{
    Q_OBJECT


public:
    explicit save_images(QWidget *parent = nullptr);
    ~save_images();

private slots:

    void on_toolButton_clicked();


    void on_pushButton_confirm_clicked();

    void on_pushButton_cancel_clicked();

    void closeEvent(QCloseEvent *event);

    void on_comboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::save_images *ui;

};

#endif // SAVE_IMAGES_H
