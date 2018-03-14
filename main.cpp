#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    qDebug() << QString::fromLocal8Bit("输出貌似成功了！");
    waitKey(0);
    return a.exec();
}
