#include "pagingwidget.h"
#include "ui_pagingwidget.h"

PagingWidget::PagingWidget(uint32_t maxPage,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PagingWidget)
{
    ui->setupUi(this);
    this->Init();
    this->SetCurrentPage(this->CurrentPage_);
    this->SetMaxPage(maxPage);
}

PagingWidget::~PagingWidget()
{
    delete ui;
}
void PagingWidget::Init()
{
    this->ui->Line_CurrentPage->setValidator(new QIntValidator(1, 10000000, this));
    this->ui->label_Previous->setProperty("page", "true");
    this->ui->label_Next->setProperty("page", "true");
    this->ui->label_Left->setProperty("page", "true");
    this->ui->label_Right->setProperty("page", "true");
    this->ui->label_Left->installEventFilter(this);
    this->ui->label_Right->installEventFilter(this);
    this->ui->label_Previous->installEventFilter(this);
    this->ui->label_Next->installEventFilter(this);
}

void PagingWidget::SetMaxPage(uint32_t maxPage)
{
    if(maxPage < 1)
        return;
    MaxPage_ = maxPage;
    //更新总页数label
    ui->label_MaxPage->setText("/" + QString::number(this->MaxPage_));

    //最大页数变动时检测当前页是否需要变化
    if(CurrentPage_ > maxPage)
        this->SetCurrentPage(maxPage);
}

bool PagingWidget::eventFilter(QObject *watched, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress)
    {
        if(watched == this->ui->label_Previous)
            this->ui->label_Previous->setStyleSheet("QLabel{background-color:rgb(51,133,255)}");
           // this->ui->label_Previous->
        else if(watched == this->ui->label_Next)
            this->ui->label_Next->setStyleSheet("QLabel{background-color:rgb(51,133,255);}");
            //page = this->CurrentPage_ + 1;
        else if(watched == this->ui->label_Left)
            this->ui->label_Left->setStyleSheet("QLabel{background-color:rgb(51,133,255);}");
        else if(watched == this->ui->label_Right)
            this->ui->label_Right->setStyleSheet("QLabel{background-color:rgb(51,133,255);}");
    }
    if(e->type() == QEvent::MouseButtonRelease)
    {
        int page = -1;
        if(watched == this->ui->label_Previous)
        {
            this->ui->label_Previous->setStyleSheet("QLabel{background-color:rgb(255,255,255)}");
            page = this->CurrentPage_ - 1;
        }
           // this->ui->label_Previous->
        else if(watched == this->ui->label_Next)
        {
          this->ui->label_Next->setStyleSheet("QLabel{background-color:rgb(255,255,255)}");
            page = this->CurrentPage_ + 1;
        }
        else if(watched == this->ui->label_Left)
        {
            this->ui->label_Left->setStyleSheet("QLabel{background-color:rgb(255,255,255)}");
            page = 1;
        }
        else if(watched == this->ui->label_Right)
        {
           this->ui->label_Right->setStyleSheet("QLabel{background-color:rgb(255,255,255)}");
            page = this->MaxPage_;
    }
        if(page >0 )
        {
            this->SetCurrentPage(page);
            return true;
        }
    }
    return QWidget::eventFilter(watched, e);
}

void PagingWidget::SetCurrentPage(uint32_t newPage)
{
    this->CurrentPage_ = newPage > this->MaxPage_ ? this->MaxPage_ : newPage;
    this->ui->Line_CurrentPage->setText(QString::number(this->CurrentPage_));
    //通知变化
    emit this->SignalCurrentPageChanged(this->CurrentPage_);
}

void PagingWidget::on_Line_CurrentPage_returnPressed()
{
    int page = this->ui->Line_CurrentPage->text().toInt();
    if(page > 0 && page != this->CurrentPage_)
        this->SetCurrentPage(page);
}
void PagingWidget::on_Line_CurrentPage_editingFinished()
{
}
