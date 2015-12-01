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
    ../src/ddz.c \
    ../src/deck.c \
    ../src/dn.c \
    ../src/gp.c \
    ../src/hand.c \
    ../src/ht_str.c \
    ../src/sort_card.c \
    ../src/texas.c \
    ../src/card_player.c

HEADERS += ../src/card.h \
    ../src/ddz.h \
    ../src/deck.h \
    ../src/dn.h \
    ../src/gp.h \
    ../src/hand.h \
    ../src/ht_lch.h \
    ../src/ht_str.h \
    ../src/mj.h \
    ../src/qipai.h \
    ../src/sort_card.h \
    ../src/texas.h \
    ../src/card_player.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    ../src/CMakeLists.txt \
    ../src/Makefile
