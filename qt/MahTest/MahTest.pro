#-------------------------------------------------
#
# Project created by QtCreator 2016-09-27T09:46:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MahTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        ../../src/mjhz.c \
        ../../src/mj.c

HEADERS  += mainwindow.h \
        ../../src/mjhz.h \
        ../../src/qpdef.h \
        ../../src/mj.h

FORMS    += mainwindow.ui

RESOURCES += \
    mahtest.qrc
