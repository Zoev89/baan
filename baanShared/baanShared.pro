#-------------------------------------------------
#
# Project created by QtCreator 2014-12-25T19:12:44
#
#-------------------------------------------------

QT       -= core gui

TARGET = baanShared
TEMPLATE = lib
CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11 -I../../baan/baanLib `pkg-config libxml++-2.6 --cflags`

SOURCES += \
    progLib.cxx \
    leesdata.cxx \
    usbinout.cxx

HEADERS += \
    leesdata.h \
    portIO.h \
    progLib.h \
    programDefines.h \
    usbinout.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
