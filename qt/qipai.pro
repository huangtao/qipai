#-------------------------------------------------
#
# Project created by QtCreator 2015-11-13T14:07:19
#
#-------------------------------------------------

QT       -= core gui

CONFIG(debug, debug | release) {
    TARGET = qipai_d
} else {
    TARGET = qipai
}
TEMPLATE = lib
CONFIG += staticlib

SOURCES += ../src/card.c \
    ../src/gp.c \
    ../src/card_algo.c \
    ../src/mj.c \
    ../src/mj_algo.c \
    ../src/mjhz.c \
    ../src/qp_common.c

HEADERS += \
    ../src/card.h \
    ../src/gp.h \
    ../src/qipai.h \
    ../src/card_algo.h \
    ../src/mj.h \
    ../src/mj_algo.h \
    ../src/mjhz.h \
    ../src/qp_common.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

macx {
QMAKE_MAC_SDK = macosx10.12
}

DEBUG_POSTFIX=_d

# vc默认不支持utf-8无BOM中文
msvc:QMAKE_CFLAGS +=/source-charset:utf-8
msvc:QMAKE_CXXFLAGS +=/source-charset:utf-8
msvc:DEFINES += _CRT_SECURE_NO_WARNINGS
#gcc:

contains(QT_ARCH, i386) {
    message("32-bit")
} else {
    message("64-bit")
}

DISTFILES += \
    ../src/CMakeLists.txt \
    ../src/Makefile
