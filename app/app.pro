QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = GuiGnuplot
TEMPLATE = app


DESTDIR = $$PWD/../build

RESOURCES += \
    Resources.qrc

FORMS += \
    mainwindow.ui

HEADERS += \
    command.h \
    commandGenerator.h \
    enums.h \
    gnuplotProcess.h \
    highlighter.h \
    lineParsing.h \
    mainwindow.h \
    parameter.h \
    script.h

SOURCES += \
    command.cpp \
    commandGenerator.cpp \
    gnuplotProcess.cpp \
    highlighter.cpp \
    lineParsing.cpp \
    main.cpp \
    mainwindow.cpp \
    parameter.cpp \
    script.cpp
