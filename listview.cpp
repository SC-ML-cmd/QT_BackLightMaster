#include "listview.h"
#include<QDebug>
#include "datastatistics_win.h"
extern  QChartView *chartView;
ListView::ListView(QWidget *parent) : QListView(parent)
{

}
 void ListView::resizeEvent(QResizeEvent* event)
{
      QListView::resizeEvent(event);
   if(chartView!=nullptr)
       chartView->resize(event->size());
}
