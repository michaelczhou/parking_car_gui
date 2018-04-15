#-------------------------------------------------
#
# Project created by QtCreator 2018-03-14T19:42:02
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = parking
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    keyboard/keyboard.cpp \
    commonhelper.cpp \
    udpclient.cpp

HEADERS  += mainwindow.h \
    keyboard/keyboard.h \
    keyboard/ui_keyboard.h \
    commonhelper.h \
    udpclient.h

FORMS    += mainwindow.ui \
    keyboard/keyboard.ui

INCLUDEPATH += /usr/local/include \
                /usr/local/include/opencv \
                /usr/local/include/opencv2

LIBS += /usr/local/lib/libopencv_highgui.so \
        /usr/local/lib/libopencv_core.so    \
        /usr/local/lib/libopencv_imgproc.so \
        /usr/local/lib/libopencv_imgcodecs.so.3.2
