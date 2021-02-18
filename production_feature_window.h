#ifndef PRODUCTION_FEATURE_WINDOW_H
#define PRODUCTION_FEATURE_WINDOW_H

#include <QDialog>

namespace Ui {
class production_feature_window;
}

class production_feature_window : public QDialog
{
    Q_OBJECT

public:
    explicit production_feature_window(QWidget *parent = 0);
    void sendProductType(QString productType);
    QString productionType;
    ~production_feature_window();

private slots:
    void on_pushButton_clicked();

private:
    Ui::production_feature_window *ui;
};

#endif // PRODUCTION_FEATURE_WINDOW_H
