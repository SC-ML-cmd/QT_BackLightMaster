#ifndef CAMERA_TEST_H
#define CAMERA_TEST_H

#include <QMainWindow>

namespace Ui {
class camera_test;
}

class camera_test : public QMainWindow
{
    Q_OBJECT

public:
    explicit camera_test(QWidget *parent = nullptr);
    ~camera_test();

private:
    Ui::camera_test *ui;
};

#endif // CAMERA_TEST_H
