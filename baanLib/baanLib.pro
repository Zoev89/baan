#-------------------------------------------------
#
# Project created by QtCreator 2014-12-25T15:34:40
#
#-------------------------------------------------

QT       -= core gui

TARGET = baanLib
TEMPLATE = lib
CONFIG += staticlib

DEFINES += BAANLIB_LIBRARY


QMAKE_CXXFLAGS += -std=c++11 -fPIC -I../../baanShared `pkg-config libxml++-2.6 --cflags`
QMAKE_LFLAGS += -rdynamic -fPIC -L/usr/X11R6/lib
LIBS = -lfltk2_images -lfltk2    -lX11 -lXi -lXinerama -lXft -lpthread -lm  -lXext  -lsupc++ -lstdc++ -lrt -lusb -lboost_thread -lboost_system -lboost_filesystem `pkg-config libxml++-2.6 --libs` -ldl -lfontconfig -lXrender

SOURCES += \
    baanDoc.cxx \
    baanMessage.cxx \
    baanOverzicht.cxx \
    baanQueue.cxx \
    baanSemaphoreLinux.cxx \
    baanTimeLinux.cxx \
    baanTreinen.cxx \
    baanView.cxx \
    baanWT.cxx \
    baanZoek.cxx \
    blok.cxx \
    driewegWissel.cxx \
    engelseWissel.cxx \
    foutStatistiek.cxx \
    hardwareCom.cxx \
    ioOverzicht.cxx \
    kruising.cxx \
    lamp.cxx \
    ontkoppelaar.cxx \
    programma.cxx \
    regelaar.cxx \
    socketCom.cxx \
    spoorInformatie.cxx \
    standaardWissel.cxx \
    td.cxx \
    wissel.cxx \
    errorPrint.cxx \
    baanLib.cxx \
    baanUtility.cxx \
    TelefoonConnectie.cxx \
    draaiSchijf.cxx \
    IoBits.cxx

HEADERS +=\
    baanDoc.h \
    baanMessage.h \
    baanOverzicht.h \
    baanQueue.h \
    baanSemaphore.h \
    baanTime.h \
    baanTreinen.h \
    baanTypes.h \
    baanView.h \
    baanWT.h \
    baanZoek.h \
    blok.h \
    concurrentQueue.h \
    driewegWissel.h \
    engelseWissel.h \
    errorPrint.h \
    foutStatistiek.h \
    hardwareCom.h \
    ioOverzicht.h \
    kruising.h \
    lamp.h \
    ontkoppelaar.h \
    programma.h \
    regelaar.h \
    rwbmp.h \
    socketCom.h \
    spoorInformatie.h \
    standaardWissel.h \
    td.h \
    wissel.h \
    baan.h \
    baanLib.h \
    IMessage.h \
    IBaanWT.h \
    IBaanDoc.h \
    IBaanTreinen.h \
    IBlok.h \
    IWissel.h \
    IWissels.h \
    IMainScreenControls.h \
    IMainWindowDrawing.h \
    IOverzicht.h \
    baanUtility.h \
    IRegelaarView.h \
    IRegelaarViewUpdates.h \
    IBlokInst.h \
    IBaanMessage.h \
    IView.h \
    IViewInternal.h \
    IWisselDialoog.h \
    ITreinenDialoog.h \
    ILampInstDialoog.h \
    IKopRichtingDialoog.h \
    INieuwIODialoog.h \
    IAddBlokDialoog.h \
    IRegelaarInstellingenDialoog.h \
    Rectangle.h \
    TelefoonConnectie.h \
    ITelefoonConnectie.h \
    ITd.h \
    IErrorPrint.h \
    draaiSchijf.h \
    IIoBits.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../baanShared/release/ -lbaanShared
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../baanShared/debug/ -lbaanShared
else:unix: LIBS += -L$$OUT_PWD/../baanShared/ -lbaanShared

INCLUDEPATH += $$PWD/../baanShared
DEPENDPATH += $$PWD/../baanShared

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../baanShared/release/libbaanShared.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../baanShared/debug/libbaanShared.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../baanShared/release/baanShared.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../baanShared/debug/baanShared.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../baanShared/libbaanShared.a
