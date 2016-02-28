TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS +=  ../baanShared/libbaanShared.a -L/usr/X11R6/lib  -lX11 -lpthread -lm  -lXext  -lsupc++ -lstdc++ -lrt -lusb -lboost_system -lboost_thread -lboost_filesystem `pkg-config libxml++-2.6 --libs` -ldl

SOURCES += \
    print.cxx

INCLUDEPATH += $$PWD/../baanShared
DEPENDPATH += $$PWD/../baanShared
