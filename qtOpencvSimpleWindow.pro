#-------------------------------------------------
#
# Project created by QtCreator 2016-02-13T22:52:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtOpencvSimpleWindow
TEMPLATE = app
LIBS += `pkg-config opencv --cflags --libs`


SOURCES += main.cpp\
        mainwindow.cpp \
    worker.cpp

HEADERS  += mainwindow.h \
    worker.h

FORMS    += mainwindow.ui
