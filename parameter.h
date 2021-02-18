#ifndef PARAMETER_H
#define PARAMETER_H

#include <QWidget>

namespace Ui {
class parameter;
}

class parameter : public QWidget
{
    Q_OBJECT

public:
    explicit parameter(QWidget *parent = nullptr);
    ~parameter();

private slots:
    void on_pushButton_clicked();

private:
    Ui::parameter *ui;
};

#endif // PARAMETER_H
