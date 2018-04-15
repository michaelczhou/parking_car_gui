#include "mainwindow.h"
#include <QApplication>
#include "udpclient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    qDebug() << QString::fromLocal8Bit("输出貌似成功了！");
//    waitKey(0);
    qDebug("%s", __func__);

    return a.exec();
}
