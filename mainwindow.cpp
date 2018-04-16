#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <QtGui>
#include <QUdpSocket>
#include "udpclient.h"
#include <QLabel>
#include <QDate>
#include "commonhelper.h"
#include <QSettings>

using namespace cv;
using namespace std;

IplImage* image = 0;
IplImage* dst = 0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug("%s", __func__);
    ui->setupUi(this);

    /* 读取配置文件 */
    doSettings(false);

    /* 设置默认通讯模式 */
    //ui->tcpclient_radioButton->setChecked(true);
    /** 目前设置为UDP为默认方式 */
    ui->udp_radioButton->setChecked(true);
    /** 设置远程主机IP地址 获取本机IP */
    ui->remoteIP_lineEdit->setText(mRemoteIp);
    /* 设置远程端口号 */
    /* TODO: 将其设置为不能以0开头 */
    ui->remoteport_spinBox->setRange(1024,9999);
    ui->remoteport_spinBox->setValue(mRemotePort);
    /* 设置本地端口号 */
    ui->localport_spinBox->setRange(1024,9999);
    ui->localport_spinBox->setValue(mLocalPort);

    isConnect = false;

    // 状态栏
    statusLabel = new QLabel;
    statusLabel->setMinimumSize(260, 20); // 设置标签最小大小
    statusLabel->setFrameShape(QFrame::WinPanel); // 设置标签形状
    statusLabel->setFrameShadow(QFrame::Sunken); // 设置标签阴影
    ui->statusBar->addWidget(statusLabel);
    statusLabel->setText("UDP通信停止");
    statusLabel->setAlignment(Qt::AlignHCenter);

    // 时间 TODO:要进行更新
    timeLabel = new QLabel;
    timeLabel->setMinimumSize(90, 20); // 设置标签最小大小
    timeLabel->setMaximumWidth(90);
    timeLabel->setFrameShape(QFrame::WinPanel); // 设置标签形状
    timeLabel->setFrameShadow(QFrame::Sunken); // 设置标签阴影
    ui->statusBar->addWidget(timeLabel);
    timeLabel->setText(QDate::currentDate().toString("yyyy-MM-dd"));

    // 更新接收到的数据
    connect(&client, SIGNAL(valueChanged(QString)), this, SLOT(updateReceiveText(QString)));
    connect(&client,
            SIGNAL(updateState(QString, QVariant, QVariant)),
            this, SLOT(updateStateBar(QString, QVariant, QVariant)));

    init();

    mReceiveNum = mSendNum = 0;

    //键盘
    lineEditkeyboard = new Keyboard();
    connect( ui->input ,SIGNAL(selectionChanged()),this ,SLOT(open_keyboard_lineEdit()));
}

void MainWindow::doSettings(bool isWrite)
{
    QSettings settings("Yzs_think", "Application");
    const QString REMOTE_IP = "remoteip";
    const QString REMOTE_PORT = "remoteport";
    const QString LOCAL_PORT = "localport";
    if(isWrite) {
        settings.setValue(REMOTE_IP, mRemoteIp);
        settings.setValue(REMOTE_PORT, mRemotePort);
        settings.setValue(LOCAL_PORT, mLocalPort);
    } else {
        mRemoteIp = settings.value(REMOTE_IP, chelper.getLocalHostIP().toString()).toString();
        mRemotePort = settings.value(REMOTE_PORT, 1234).toInt();
        mLocalPort = settings.value(LOCAL_PORT, 2468).toInt();
    }
}


void MainWindow::connectNet()
{
    qDebug("%s", __func__);

    mRemoteIp = ui->remoteIP_lineEdit->text();
    mRemotePort = ui->remoteport_spinBox->text().toInt();
    mLocalPort = ui->localport_spinBox->text().toInt();
    updateStateBar("UDP通信 " + mRemoteIp + ":" + QString().number(mRemotePort),
                   QVariant(QVariant::Int), QVariant(QVariant::Int));

    // No.1
    isConnect = true;
    // 将状态设置为 通
    ui->state_label->setText("通");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::blue);
    ui->state_label->setPalette(pa);

    // 将按钮设置为　断开网络
    ui->connect_pushButton->setText("断开网络");

    // 禁用远程端口，本地端口，远程IP
    ui->remoteIP_lineEdit->setEnabled(false);
    ui->remoteport_spinBox->setEnabled(false);
    ui->localport_spinBox->setEnabled(false);
    // 使能button
    //ui->handSend_pushButton->setEnabled(true);

    client.udpStart(chelper.getLocalHostIP(), mLocalPort, QHostAddress(mRemoteIp), mRemotePort);
}

void MainWindow::updateReceiveText(const QString string)
{
    QString oldString = ui->receive_textBrowser->toPlainText();
    ui->receive_textBrowser->setText(oldString + string + "\n");

    // 将光标移动到最后位置
    QTextCursor tmpCursor = ui->receive_textBrowser->textCursor();
    tmpCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 4);
    ui->receive_textBrowser->setTextCursor(tmpCursor);
}

/**
 * @brief MainWindow::updateStateBar
 * @param state 状态
 * @param inNum 接收数值
 * @param outNum 发送数值
 */
void MainWindow::updateStateBar(QString state, QVariant inNum, QVariant outNum)
{
    if(!state.isNull())
        statusLabel->setText(state);
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
/**
 * 初始化UDP时调用该函数
 * @brief MainWindow::init
 */
void MainWindow::init()
{
    qDebug("%s", __func__);
    // No.1
    isConnect = false;
    // 将状态设置为 断
    ui->state_label->setText("断");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    ui->state_label->setPalette(pa);

    // 将按钮设置为　连接网络
    ui->connect_pushButton->setText("连接网络");

    // 使能远程端口，本地端口，远程IP
    ui->remoteIP_lineEdit->setEnabled(true);
    ui->remoteport_spinBox->setEnabled(true);
    ui->localport_spinBox->setEnabled(true);
    // 禁用button
    //ui->handSend_pushButton->setEnabled(false);
    //
    client.udpStop(NULL, NULL, NULL);

    updateStateBar("本地IP: " + chelper.getLocalHostIP().toString() + " 无连接",
                   QVariant(QVariant::Int), QVariant(QVariant::Int));
}

/**
 * 断开UDP时调用该函数
 * @brief MainWindow::disConnectNet
 */
void MainWindow::disConnectNet()
{
    qDebug("%s", __func__);
    // No.1
    isConnect = false;
    // 将状态设置为 断
    ui->state_label->setText("断");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    ui->state_label->setPalette(pa);

    // 将按钮设置为　连接网络
    ui->connect_pushButton->setText("连接网络");

    // 使能远程端口，本地端口，远程IP
    ui->remoteIP_lineEdit->setEnabled(true);
    ui->remoteport_spinBox->setEnabled(true);
    ui->localport_spinBox->setEnabled(true);
    // 禁用button
    //ui->handSend_pushButton->setEnabled(false);
    //
    client.udpStop(NULL, NULL, NULL);


    updateStateBar(tr("UDP通信停止"), QVariant(QVariant::Int), QVariant(QVariant::Int));
}

MainWindow::~MainWindow()
{
    //doSettings(true);
    delete ui;
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


void MainWindow::open_keyboard_lineEdit()
{
    QLineEdit *line = (QLineEdit *)sender();
    lineEditkeyboard->setLineEdit(line);
    lineEditkeyboard->show();
}

void MainWindow::on_sendcarport_clicked()
{

}

void MainWindow::on_connect_pushButton_clicked()
{
    qDebug("%s", __func__);
    // 如果当前网络是连接状态　调用断开连接函数
    if(isConnect) {
        disConnectNet();
    } else { // 否则调用连接函数
        connectNet();
    }
}

void MainWindow::on_quit_pushButton_clicked()
{
    qApp->quit();
}


