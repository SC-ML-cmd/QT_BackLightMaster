#include "camera_set_gray.h"
#include "ui_camera_set_gray.h"
#include"QSqlTableModel"
#include"dbhelper.h"


Camera_set_gray::Camera_set_gray(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Camera_set_gray)
{
    ui->setupUi(this);
    model = new QSqlTableModel(this);
    model->setTable("camera_grayvalue_set");
    model->select();//选取整个表的所有行
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置保存策略为手动提交

    ui->Tb_Set_gray_show->setModel(model); //绑定模型与视图
    ui->Tb_Set_gray_show->horizontalHeader()->setSectionsClickable(true);
    ui->Tb_Set_gray_show->horizontalHeader()->setHighlightSections(true);//选中列字体变粗
    //ui->Tb_Set_gray_show->setEditTriggers(QAbstractItemView::NoEditTriggers);//将表格变为禁止编辑
    ui->Tb_Set_gray_show->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(189,215,238)};");
    //ui->tableView->setColumnWidth(5,150);//设置列宽
    ui->Tb_Set_gray_show->setEnabled(true);
    ui->Tb_Set_gray_show->hideColumn(0);
    ui->Tb_Set_gray_show->verticalHeader()->hide();
    ui->Tb_Set_gray_show->setColumnWidth(2,130);
    ui->Tb_Set_gray_show->setColumnWidth(3,130);
    ui->Tb_Set_gray_show->setColumnWidth(4,130);
    ui->Tb_Set_gray_show->setColumnWidth(5,130);
    setFixedSize(this->width(),this->height());
}

Camera_set_gray::~Camera_set_gray()
{
    delete ui;
}

void Camera_set_gray::on_pushButton_clicked()
{
    QSqlQuery query=QSqlQuery(db);
    QString inf=QStringLiteral("是否保存设定灰度值？");
    QMessageBox box(QMessageBox::Warning,"提示",inf);
    box.setWindowIcon(QIcon(":/resourse/xitong.png"));
    box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText (QMessageBox::Ok,QString("确 定"));
    box.setButtonText (QMessageBox::Cancel,QString("取 消"));
    int ok=box.exec();
    if(ok == QMessageBox::Ok)//确认修改参数
    {
        for(int i=0;i<6;i++)
        {
             double gray_1=ui->Tb_Set_gray_show->model()->index(i,2).data().toDouble();
             double gray_2= ui->Tb_Set_gray_show->model()->index(i,3).data().toDouble();
             double gray_3= ui->Tb_Set_gray_show->model()->index(i,4).data().toDouble();
             double gray_4= ui->Tb_Set_gray_show->model()->index(i,5).data().toDouble();

             QSqlQuery query=QSqlQuery(db);
             QString sql=QStringLiteral("update camera_grayvalue_set set 白底曝光灰度设定='%0', 黑底曝光灰度设定='%1',灰底曝光灰度设定='%2',侧光灯曝光灰度设定='%3' where 相机编号='%4'").arg(gray_1).arg(gray_2).arg(gray_3).arg(gray_4).arg(i);
             query.exec(sql);

        }
        box.close();
        this->close();
    }
    else
       box.close();
}
