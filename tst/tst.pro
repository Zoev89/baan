TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -I../../baanLib -I../../baan/baanShared `pkg-config libxml++-2.6 --cflags`
QMAKE_LFLAGS += -L/usr/X11R6/lib
LIBS =  -lgmock -lgmock_main ../baanLib/libbaanLib.a ../baanShared/libbaanShared.a -L/usr/X11R6/lib  -lX11 -lpthread -lm  -lXext  -lsupc++ -lstdc++ -lrt -lusb -lboost_system -lboost_thread -lboost_filesystem `pkg-config libxml++-2.6 --libs` -ldl

SOURCES += \
    baanZoek.cxx \
    baanLibTest.cxx \
    TdTest.cxx \
    draaischijfTest.cxx \
    DispatcherTest.cpp \
    engelseWisselTest.cxx

HEADERS += \
    IMessageMock.h \
    IMainScreenControlsMock.h \
    IMainWindowDrawingMock.h \
    ITreinenDialoogMock.h \
    IBlokInstMock.h \
    IWisselDialoogMock.h \
    ILampInstDialoogMock.h \
    IKopRichtingDialoogMock.h \
    INieuwIODialoogMock.h \
    IAddBlokDialoogMock.h \
    IBaanTreinenMock.h \
    IBaanMessageMock.h \
    IViewMock.h \
    AllMocks.h \
    IRegelaarViewUpdateMock.h \
    IRegelaarInstellingenDialoogMock.h \
    ITelefoonConnectieMock.h \
    ITdMock.h \
    IErrorPrintMock.h \
    baanLibSetup.h \
    IThreadSleepMock.h \
    IHardwareComMock.h \
    IDraaiSchijfTuningMock.h


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../baanLib/release/ -lbaanLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../baanLib/debug/ -lbaanLib
else:unix: LIBS += -L$$OUT_PWD/../baanLib/ -lbaanLib

INCLUDEPATH += $$PWD/../baanLib
DEPENDPATH += $$PWD/../baanLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../baanLib/release/libbaanLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../baanLib/debug/libbaanLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../baanLib/release/baanLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../baanLib/debug/baanLib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../baanLib/libbaanLib.a
