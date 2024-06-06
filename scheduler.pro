QT += core gui widgets

CONFIG += c++11

TARGET = algorithms_integration
TEMPLATE = app

SOURCES += main.cpp \
    sjfwidget.cpp \
    srtfwidget.cpp \
    rrwidget.cpp \
    bankerswidget.cpp 

HEADERS += sjfwidget.h \
    srtfwidget.h \
    rrwidget.h \
    bankerswidget.h 