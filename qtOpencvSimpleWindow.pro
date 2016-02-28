#-------------------------------------------------
#
# Project created by QtCreator 2016-02-13T22:52:03
#
#-------------------------------------------------

QT       += core gui opengl testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtOpencvSimpleWindow
TEMPLATE = app
LIBS += `pkg-config opencv --cflags --libs`
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    capturer.cpp \
    radon.cpp

HEADERS  += mainwindow.h \
    capturer.h \
    radon.h

FORMS    += mainwindow.ui

RESOURCES = appresource.qrc
