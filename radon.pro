QT       += core gui opengl testlib multimedia

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

win32 {
    INCLUDEPATH += C:\Users\anton\opencv3.1.0\build\include
    LIBS += -LC:\Users\anton\opencv3.1.0\build\x64\vc12\lib
    LIBS += -lopencv_world310d
}
unix {
    LIBS += `pkg-config opencv --cflags --libs`
}
