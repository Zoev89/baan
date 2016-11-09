TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS +=  ../baanShared/libbaanShared.a -L/usr/X11R6/lib  -lX11 -lpthread -lm  -lXext  -lsupc++ -lstdc++ -lrt -lusb -lboost_system -lboost_thread -lboost_filesystem `pkg-config libxml++-2.6 --libs` -ldl
CodeCoverage {
QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0

LIBS += \
    -lgcov
}

SOURCES += \
    print.cxx

INCLUDEPATH += $$PWD/../baanShared
DEPENDPATH += $$PWD/../baanShared
