#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T09:32:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kstream
TEMPLATE = app

LIBS += -lcurl -pthread

SOURCES += mainwindow.cpp\
    main.cpp\
    ../channelmanager.cpp \
    ../channel.cpp \
    ../connector.cpp \
    ../fileutils.cpp \
    ../threadman.cpp \
    streamitem.cpp

HEADERS  += mainwindow.h\
    ../channel.h \
    ../channelmanager.h \
    ../connector.h \
    ../fileutils.h \
    ../global.h \
    ../threadman.h \
    streamitem.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    logo.svg

QMAKE_CXXFLAGS += -std=c++0x
