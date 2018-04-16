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
#include "keyboard/keyboard.h"
#include <QUdpSocket>
#include <QLabel>
#include <QPushButton>
#include "udpclient.h"
#include "commonhelper.h"

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
    void on_sendcarport_clicked();
    void open_keyboard_lineEdit();

    void on_connect_pushButton_clicked();

    void on_quit_pushButton_clicked();

    void updateReceiveText(const QString string);

    void updateStateBar(QString state, QVariant inNum, QVariant outNum);

private:
    Ui::MainWindow *ui;
    void connectNet();
    void disConnectNet();
    bool isConnect;
    cv::Mat image;
    QImage img;                  //将用到的定义全部声明，应该没有必要，我习惯
    QString img_name;
    Keyboard *lineEditkeyboard;

    UDPClient client;
    /** 工具类 */
    CommonHelper chelper;
    /** 状态标签 */
    QLabel *statusLabel;
    /** 总接收数量显示标签 */
    QLabel *receiveLabel;
    /** 总发送数量显示标签 */
    QLabel *sendLabel;
    /** 状态栏 计数清零 按钮 */
    QPushButton *clearCounterButton;
    /** 状态栏 时间标签 */
    QLabel *timeLabel;
    /** 接收总数 */
    quint64 mReceiveNum;
    /** 发送总数 */
    quint64 mSendNum;
    /** 远程IP */
    QString mRemoteIp;
    /** 远程端口 */
    int mRemotePort;
    /** 本地端口 */
    int mLocalPort;

    void doSettings(bool isWrite);
    void init();
};

#endif // MAINWINDOW_H
