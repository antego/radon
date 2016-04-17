QT       += core gui opengl testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = radon
TEMPLATE = app


SOURCES += main.cpp\
            mainwindow.cpp \
            capturer.cpp \
            radon.cpp \
            radon_scanner.cpp \
            inverse_radon_scanner.cpp

HEADERS += mainwindow.h \
            capturer.h \
            radon.h \
            radon_scanner.h \
            inverse_radon_scanner.h

FORMS += mainwindow.ui

RESOURCES = appresource.qrc

INCLUDEPATH += C:\Users\anton\opencv2.4.11\opencv\build\include
LIBS += -LC:\Users\anton\opencv2.4.11\opencv\build\x64\vc12\lib
LIBS += -lopencv_core2411d -lopencv_highgui2411d -lopencv_imgproc2411d
