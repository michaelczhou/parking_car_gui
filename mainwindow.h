#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <qdebug.h>
#include <qlabel.h>
#include <QString>
#include <QFileDialog>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>

using namespace cv;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_selectcarport_clicked();

    void on_textBrowser_destroyed();

private:
    Ui::MainWindow *ui;
    Mat image;
    QImage img;                  //将用到的定义全部声明，应该没有必要，我习惯
    QString img_name;
};

#endif // MAINWINDOW_H