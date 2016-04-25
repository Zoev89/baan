#-------------------------------------------------
#
# Project created by QtCreator 2015-05-31T16:16:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = baanUI

QMAKE_CXXFLAGS += -std=c++11 -fPIC `pkg-config libxml++-2.6 --cflags`
TEMPLATE = app
LIBS +=  -lgmock -lgmock_main ../baanLib/libbaanLib.a ../baanShared/libbaanShared.a -L/usr/X11R6/lib  -lX11 -lpthread -lm  -lXext  -lsupc++ -lstdc++ -lrt -lusb -lboost_system -lboost_thread -lboost_filesystem `pkg-config libxml++-2.6 --libs` -ldl


SOURCES += main.cpp\
        mainwindow.cpp \
    UIMainScreenControls.cpp \
    UIMainWindowDrawing.cpp \
    UITreinenDialoog.cpp \
    UILampInstDialoog.cpp \
    UIRegelaarViewUpdates.cpp \
    UIBaanLibCreator.cpp \
    QBitmapLabel.cpp \
    UIRegelaars.cpp \
    UIRegelaar.cpp \
    QtKopRichtingDialoog.cpp \
    QtBlokInstDialoog.cpp \
    QtWisselDialoog.cpp \
    QtDraaiAansluiting.cpp \
    QtNieuwIODialoog.cpp \
    QtDraaiSchijfTuning.cpp \
    QtAddBlokDialoog.cpp \
    QtMessage.cpp \
    QtRegelaarInstellingenDialoog.cpp

HEADERS  += mainwindow.h \
    UIMainScreenControls.h \
    UIMainWindowDrawing.h \
    UITreinenDialoog.h \
    UILampInstDialoog.h \
    UIRegelaarViewUpdates.h \
    UIBaanLibCreator.h \
    QBitmapLabel.h \
    UIRegelaars.h \
    UIRegelaar.h \
    QtKopRichtingDialoog.h \
    QtBlokInstDialoog.h \
    QtWisselDialoog.h \
    QtDraaiAansluiting.h \
    QtNieuwIODialoog.h \
    QtDraaiSchijfTuning.h \
    QtAddBlokDialoog.h \
    QtMessage.h \
    QtRegelaarInstellingenDialoog.h

FORMS    += mainwindow.ui \
    UIRegelaar.ui \
    QtKopRichtingDialoog.ui \
    QtBlokInstDialoog.ui \
    QtWisselDialoog.ui \
    QtDraaiAansluiting.ui \
    QtNieuwIODialoog.ui \
    QtDraaiSchijfTuning.ui \
    QtAddBlokDialoog.ui \
    QtMessage.ui \
    QtRegelaarInstellingenDialoog.ui

# Specify that we depend on the library (which, logically would be implicit from
# the fact that we are linking to it)
PRE_TARGETDEPS += ../baanLib/libbaanLib.a


INCLUDEPATH += $$PWD/../baanLib $$PWD/../baanShared
DEPENDPATH += $$PWD/../baanLib $$PWD/../baanShared
