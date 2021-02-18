#ifndef PARAMETER_CONTROL_H
#define PARAMETER_CONTROL_H
#include <QWidget>
#include <QApplication>
#include <QStyleOptionTab>
#include <QStylePainter>
#include <QTabBar>
#include <QTabWidget>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleOptionTab>
#include "dbhelper.h"
#include "mainwindow.h"
#include<QCloseEvent>
namespace Ui {
class parameter_control;
}

class parameter_control : public QWidget
{
    Q_OBJECT

public:
    explicit parameter_control(QWidget *parent = nullptr);
    void delay1(int MSecs);
    void parameter_alter();
    void initial_para1();
    ~parameter_control();


private slots:
    void on_pushButton_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_62_clicked();
    void on_pushButton_75_clicked();
    void on_pushButton_119_clicked();
    void on_pushButton_121_clicked();
    void on_pushButton_141_clicked();
    void on_pushButton_258_clicked();

    void on_spinBox_23_valueChanged(const QString &arg1);

    void on_pushButton_9_clicked();

    void on_spinBox_26_valueChanged(const QString &arg1);
    void on_spinBox_25_valueChanged(const QString &arg1);

    void on_pushButton_4_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_61_clicked();

    void on_pushButton_76_clicked();

    void on_pushButton_120_clicked();

    void on_pushButton_122_clicked();

    void on_pushButton_142_clicked();

    void on_pushButton_259_clicked();

    void closeEvent(QCloseEvent *event);

private:
    Ui::parameter_control *ui;
};


class CustomTabStyle : public QProxyStyle
{
public:
    /* sizeFromContents
     *  用于设置Tab标签大小
     * 1.获取原标签大小
     * 2.宽高切换
     * 3.强制宽高
     * 4.return
     * */
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,const QSize &size, const QWidget *widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab)
        {
            s.transpose();
            s.rwidth() = 120; // 设置每个tabBar中item的大小
            s.rheight() = 44;
        }
        return s;
    }
    /* drawControl
     *  画控件
     * 1.过滤CE_TabBarTabLabel
     * 2.选择状态下的背景绘制
     *      a.获取Rect
     *      b.保存旧的画笔
     *      c.设置新画笔
     *      d.画 Rect
     *      e.恢复旧的画笔
     * 3.设定Text对齐及选中/非选中状态下的Text颜色
     * 4.过滤CE_TabBarTab，对其进行绘制
     * */
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel)
        {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
            {
                QRect allRect = tab->rect;
                if (tab->state & QStyle::State_Selected)
                {
                    painter->save();
                    painter->setPen(0x89cfff);
                    painter->setBrush(QBrush(0x89cfff));
                    QRect nrct = allRect.adjusted(6, 6, -6, -6);
                    painter->drawRect(allRect.adjusted(6, 6, -6, -6));
                    painter->restore();
                }
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                if (tab->state & QStyle::State_Selected)
                {
                    painter->setPen(0xf8fcff);
                }
                else
                {
                    painter->setPen(0x5d5d5d);
                }
                painter->drawText(allRect, tab->text, option);
                return;
            }
        }
        if (element == CE_TabBarTab)
        {

            QProxyStyle::drawControl(element, option, painter, widget);

        }
    }
};

#endif // PARAMETER_CONTROL_H
