#include "save_images.h"
#include "ui_save_images.h"
#include<QFileDialog>
#include<SwitchButton.h>

//bool autoSave;//是否自动保存的标志位
bool SaveSize;//保存的类型标志位，0-保存缺陷样本；1-保存全部样本
//QString imageSavePath;//图片的存储路径

save_images::save_images(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::save_images)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/resourse/baocun.png"));
    ui->lineEdit_path->setText("D:/Images");
    imageSavePath=ui->lineEdit_path->text();

    if(autoSave == false)
    {
        ui->comboBox->setCurrentText("不保存样本");
    }
    else if(autoSave == true&&SaveSize==false)
    {
        ui->comboBox->setCurrentText("保存缺陷样本");
    }
    else
    {
        ui->comboBox->setCurrentText("保存所有样本");
    }


   }

save_images::~save_images()
{
    delete ui;
}

//选择图片保存路径
void save_images::on_toolButton_clicked()
{
    //文件夹路径
    imageSavePath = QFileDialog::getExistingDirectory(
                   this, "choose save path",
                    "/");

    if (imageSavePath.isEmpty())
    {
        return;
    }
    else
    {
        ui->lineEdit_path->setText(imageSavePath);
    }
}

//确认按钮
void save_images::on_pushButton_confirm_clicked()
{
    //如果打开自动保存
    if(ui->comboBox->currentText() == "不保存样本")
    {
       autoSave = false;
    }
    else if(ui->comboBox->currentText() == "保存缺陷样本")
    {
       autoSave=true;
       SaveSize=false;
    }
    else {
        autoSave=true;
        SaveSize=true;
    }
  //如果打开自动保存
//  if(m_checked)
//  {
//      if(!imageSavePath.isEmpty())
//      {
//          imageSavePath.replace("/","\\\\");
//          autoSave=true;
//      }
//  }
//  else
//  {
//     autoSave=false;
//  }
  this->close();
  save_images_show=false;
}
//取消按钮
void save_images::on_pushButton_cancel_clicked()
{
    this->close();
    save_images_show=false;
}
void save_images::closeEvent(QCloseEvent *event)
{
    this->close();
    save_images_show=false;
}

void save_images::on_comboBox_currentIndexChanged(const QString &arg1)
{
    //如果打开自动保存
//    if(arg1 == "不保存样品")
//    {
//       autoSave = false;
//    }
//    else if(arg1 == "保存缺陷样本")
//    {
//       autoSave=true;
//       SaveSize=false;
//    }
//    else {
//        autoSave=true;
//        SaveSize=true;
//    }
    //this->close();
}
