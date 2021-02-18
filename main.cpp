#include "mainwindow.h"
#include <QApplication>
//#include <QChartView>
//#include <QPieSeries>
//#include <QPieSlice>
#include <QLoggingCategory>
#include "dbhelper.h"
#include<QSharedMemory>
using namespace std;
QSqlDatabase  db;
QString  mysql;
int main(int argc, char *argv[])
{
    static QSharedMemory *shareMem = new QSharedMemory( "SingleApp"); //创建“SingleApp”的共享内存块
    if ( !shareMem->create( 1 ) )//创建大小1b的内存
    {
        qApp->quit(); //创建失败，说明已经有一个程序运行，退出当前程序
        return -1;
    }
    QApplication::addLibraryPath("./plugins");
    QApplication a(argc, argv);
    QApplication::addLibraryPath("./plugins");
    connect_mysql();
    MainWindow w;
    w.show();
    return a.exec();
}
