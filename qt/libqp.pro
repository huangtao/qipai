#-------------------------------------------------
#
# Project created by QtCreator 2015-11-13T14:07:19
#
#-------------------------------------------------

QT       -= core gui

TARGET = qp
TEMPLATE = lib
CONFIG += staticlib

SOURCES += ../src/card.c \
    ../src/gp.c \
    ../src/card_sort.c

HEADERS += ../src/card.h \
    ../src/gp.h \
    ../src/qipai.h \
    ../src/card_sort.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    ../src/CMakeLists.txt \
    ../src/Makefile
