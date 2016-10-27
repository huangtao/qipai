#-------------------------------------------------
#
# Project created by QtCreator 2015-11-13T14:07:19
#
#-------------------------------------------------

QT       -= core gui

TARGET = qipai
TEMPLATE = lib
CONFIG += staticlib

SOURCES += ../src/card.c \
    ../src/gp.c \
    ../src/card_algo.c \
    ../src/mj.c \
    ../src/mj_algo.c \
    ../src/mjhz.c \
    ../src/qp_comm.c

HEADERS += \
    ../src/card.h \
    ../src/gp.h \
    ../src/qipai.h \
    ../src/card_algo.h \
    ../src/mj.h \
    ../src/mj_algo.h \
    ../src/mjhz.h \
    ../src/qp_comm.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

macx {
QMAKE_MAC_SDK = macosx10.12
}

DISTFILES += \
    ../src/CMakeLists.txt \
    ../src/Makefile
