QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = GuiGnuplotTests
TEMPLATE = app

CONFIG += console

SOURCES += TestMain.cpp \
    CommandGeneratorTest.cpp \
    CommandTest.cpp \
    LineParsingTest.cpp \
    ParameterTest.cpp \
    ScriptTest.cpp \
    ../app/command.cpp \
    ../app/commandGenerator.cpp \
    ../app/gnuplotProcess.cpp \
    ../app/lineParsing.cpp \
    ../app/parameter.cpp \
    ../app/script.cpp \
    ../app/highlighter.cpp

HEADERS += \
    ../app/command.h \
    ../app/commandGenerator.h \
    ../app/enums.h \
    ../app/gnuplotProcess.h \
    ../app/lineParsing.h \
    ../app/parameter.h \
    ../app/script.h \
    ../app/highlighter.h

INCLUDEPATH += \
    ../app \
    ../lib

DESTDIR = $$PWD/../build

