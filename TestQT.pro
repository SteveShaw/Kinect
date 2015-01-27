#-------------------------------------------------
#
# Project created by QtCreator 2015-01-07T09:57:55
#
#-------------------------------------------------

QT       += core
QT       += multimedia

QT       -= gui

TARGET = TestQT
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    kinectcapture.cpp \
    worker.cpp \
    writer.cpp

HEADERS += \
    kinectcapture.h \
    worker.h \
    writer.h

INCLUDEPATH += "C:/Program Files/Microsoft SDKs/Kinect/v2.0_1409/inc"


win32:CONFIG(release, debug|release): LIBS += -L"C:/Program Files/Microsoft SDKs/Kinect/v2.0_1409/lib/x86" -lkinect20
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:/Program Files/Microsoft SDKs/Kinect/v2.0_1409/lib/x86" -lkinect20


INCLUDEPATH += E:/ZMQ/include

win32:CONFIG(release, debug|release): LIBS += -LE:/ZMQ/lib -llibzmq-v120-mt-4_0_4
else:win32:CONFIG(debug, debug|release): LIBS += -LE:/ZMQ/lib -llibzmq-v120-mt-gd-4_0_4


win32:CONFIG(release, debug|release): LIBS += -LE:/opencv/opencv/build/lib/Release -lopencv_core2410 -lopencv_highgui2410
else:win32:CONFIG(debug, debug|release): LIBS += -LE:/opencv/opencv/build/lib/Debug -lopencv_core2410d -lopencv_highgui2410d

INCLUDEPATH += E:/opencv/opencv/build/include
