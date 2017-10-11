#-------------------------------------------------
#
# Project created by QtCreator 2016-06-17T21:09:27
#
#-------------------------------------------------

QT       += core gui\
            charts\
            xml


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EVA
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/impedance.cpp \
    src/plotgraph.cpp \
    src/table_reader.cpp \
    src/dataseriestable.cpp \
    src/fittingwindow.cpp \
    src/plotdualgraph.cpp \
    src/circuitelement.cpp \
    src/circuitmodel.cpp \
    src/circuitdiagram.cpp \
    src/parametertablemodel.cpp

HEADERS  += src/mainwindow.h \
    src/impedance.h \
    src/plotgraph.h \
    src/table_reader.h \
    src/dataseriestable.h \
    src/fittingwindow.h \
    src/plotdualgraph.h \
    src/circuitelement.h \
    src/circuitmodel.h \
    src/circuitdiagram.h \
    src/parametertablemodel.h

RESOURCES += resources/Icons.qrc


