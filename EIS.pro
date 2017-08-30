#-------------------------------------------------
#
# Project created by QtCreator 2016-06-17T21:09:27
#
#-------------------------------------------------

QT       += core gui\
            charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EIS
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    impedance.cpp \
    plotgraph.cpp \
    table_reader.cpp \
    circuittree.cpp \
    dataseriestable.cpp

HEADERS  += mainwindow.h \
    impedance.h \
    plotgraph.h \
    table_reader.h \
    circuittree.h \
    dataseriestable.h

RESOURCES += Icons.qrc

