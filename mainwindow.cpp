#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <QtGui>

using namespace cv;
using namespace std;

IplImage* image = 0;
IplImage* dst = 0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lineEditkeyboard = new Keyboard();
    connect( ui->input ,SIGNAL(selectionChanged()),this ,SLOT(open_keyboard_lineEdit()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void mouseHandler(int event, int x, int y, int flags, void* param)
{
    IplImage *img0, *img1;
    img0 = (IplImage*)param;
    img1 = cvCreateImage(cvSize(img0->width, img0->height), img0->depth, img0->nChannels);
    cvCopy(img0, img1, NULL);

    CvFont font;
    uchar* ptr;
    char label[20];
    char label2[20];

    cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 1, 0, 1, 1);    //初始化字体

    if(event == CV_EVENT_LBUTTONDOWN)
    {
        //读取像素
        ptr = cvPtr2D(img0, y, x, NULL);

        sprintf(label, "Grey Level:%d", ptr[0]);
        sprintf(label2, "Pixel: (%d, %d)", x, y);
        //调整显示位置
        if(img0->width - x <= 180 || img0->height - y <= 20)
        {
    cvRectangle(img1, cvPoint(x-180, y-40), cvPoint(x-10, y-10), CV_RGB(255, 0, 0), CV_FILLED, 8, 0);
    cvPutText(img1, label, cvPoint(x-180, y-30), &font, CV_RGB(255, 255, 255));
    cvPutText(img1, label2, cvPoint(x-180, y-10), &font, CV_RGB(255, 255, 255));
        }
        else
        {
    cvRectangle(img1, cvPoint(x+10, y-12), cvPoint(x+180, y+20), CV_RGB(255, 0, 0), CV_FILLED, 8, 0);
    cvPutText(img1, label, cvPoint(x+10, y), &font, CV_RGB(255, 255, 255));
    cvPutText(img1, label2, cvPoint(x+10, y+20), &font, CV_RGB(255, 255, 255));
        }
        //以鼠标为中心画点
        CvPoint centerPoint;
        centerPoint.x = x;
        centerPoint.y = y;
        cvCircle(img1, centerPoint, 3, CV_RGB(0, 0, 0), 1, 8, 3);

        cvShowImage("img", img1);
    }
}
void on_mouse( int event, int x, int y, int flags, void* ustc)
{
    static CvPoint pre_pt = {-1,-1};
    static CvPoint cur_pt = {-1,-1};
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);
    char temp[16];

    if( event == CV_EVENT_LBUTTONDOWN )
    {
        cvCopy(dst,image);
        sprintf(temp,"(%d,%d)",x,y);
        pre_pt = cvPoint(x,y);
        cvPutText(image,temp, pre_pt, &font, cvScalar(0,0, 0, 255));
        cvCircle( image, pre_pt, 3,cvScalar(255,0,0,0) ,CV_FILLED, CV_AA, 0 );
        cvShowImage( "map", image );
        cvCopy(image,dst);
    }
    else if( event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))
    {
        cvCopy(dst,image);
        sprintf(temp,"(%d,%d)",x,y);
        cur_pt = cvPoint(x,y);
        cvPutText(image,temp, cur_pt, &font, cvScalar(0,0, 0, 255));
        cvShowImage( "map", image );
    }
    else if( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
    {
        cvCopy(dst,image);
        sprintf(temp,"(%d,%d)",x,y);
        cur_pt = cvPoint(x,y);
        cvPutText(image,temp, cur_pt, &font, cvScalar(0,0, 0, 255));
        cvRectangle(image, pre_pt, cur_pt, cvScalar(0,255,0,0), 1, 8, 0 );
        cvShowImage( "map", image );
    }
    else if( event == CV_EVENT_LBUTTONUP )
    {
        sprintf(temp,"(%d,%d)",x,y);
        cur_pt = cvPoint(x,y);
        cvPutText(image,temp, cur_pt, &font, cvScalar(0,0, 0, 255));
        cvCircle( image, cur_pt, 3,cvScalar(255,0,0,0) ,CV_FILLED, CV_AA, 0 );
        cvRectangle( image, pre_pt, cur_pt, cvScalar(0,255,0,0), 1, 8, 0 );
        cvShowImage( "map", image );
        cvCopy(image,dst);
    }
}
void MainWindow::on_pushButton_clicked()
{
    img_name = QFileDialog::getOpenFileName( this, tr("Open Image"), ".",tr("Image Files(*.png *.jpg *.jpeg *.bmp)"));
    if(img_name.length()<=0)return;
    //imread的第二个参数：读取4通道的png图像。其中第四个通道的数据类型和其他通道的一样，都是uchar型，完全透明为0，否则为255
    image=imread(img_name.toUtf8().data());
    //image = cv::imread(img_name.toLatin1().data());
    //namedWindow( "map", WINDOW_NORMAL );
    //cv::setMouseCallback("map", mouseHandler, (void*)image);
    //cv::setMouseCallback("map", mouseHandler, 0);
//    if(!image.empty())
//    {
//        imshow("map",image);
//    }
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
    //close();
    qApp->quit();
}

void MainWindow::open_keyboard_lineEdit()
{
    QLineEdit *line = (QLineEdit *)sender();
    lineEditkeyboard->setLineEdit(line);
    lineEditkeyboard->show();
}

void MainWindow::on_sendcarport_clicked()
{

}
