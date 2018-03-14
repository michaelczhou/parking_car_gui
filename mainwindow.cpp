#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    img_name = QFileDialog::getOpenFileName( this, tr("Open Image"), ".",tr("Image Files(*.png *.jpg *.jpeg *.bmp)"));
    if(img_name.length()<=0)return;
    //imread的第二个参数：读取4通道的png图像。其中第四个通道的数据类型和其他通道的一样，都是uchar型，完全透明为0，否则为255
    image=imread(img_name.toUtf8().data());
    //image = cv::imread(img_name.toLatin1().data());
    cv::cvtColor(image,image,CV_BGR2RGB);
    QImage img = QImage((const unsigned char*)(image.data),image.cols,image.rows,QImage::Format_RGB888);
    ui->label->setPixmap(QPixmap::fromImage(img));
    ui->label->resize(ui->label->pixmap()->size());
    ui->graphicsView->resize(img.width(), img.height());
    QGraphicsScene *scene = new QGraphicsScene;
    //scene->setSceneRect(0,0,img.width(),img.height());
    scene->addPixmap(QPixmap::fromImage(img));
    ui->graphicsView->setScene(scene);
    ui->graphicsView->adjustSize();
    ui->label->hide();
    ui->graphicsView->show();
//    ui->label->show();
//    ui->graphicsView->hide();


}

void MainWindow::on_pushButton_2_clicked()
{
    close();
}


void MainWindow::on_selectcarport_clicked()
{

}

void MainWindow::on_textBrowser_destroyed()
{

}
