#ifndef MODEL_QUERY_H
#define MODEL_QUERY_H

#include <QMainWindow>

namespace Ui {
class Model_query;
}

class Model_query : public QMainWindow
{
    Q_OBJECT

public:
    explicit Model_query(QWidget *parent = nullptr);
    ~Model_query();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

signals:
    void infoSend4(QString,QString,QString,QString,QString,QString,QString);

private:
    Ui::Model_query *ui;
};

#endif // MODEL_QUERY_H
