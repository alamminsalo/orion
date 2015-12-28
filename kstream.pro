#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T09:32:05
#
#-------------------------------------------------

QT       += qml quick network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kstream

TEMPLATE = app

DEFINES += APP_NAME=\\\"kstream\\\"\
    _QML

LIBS +=

SOURCES += src/ui/mainwindow.cpp\
    src/main.cpp\
    src/model/channelmanager.cpp \
    src/model/channel.cpp \
    src/util/fileutils.cpp \
    src/ui/streamitem.cpp \
    src/network/networkmanager.cpp \
    src/model/game.cpp

HEADERS  += src/ui/mainwindow.h\
    src/model/channel.h \
    src/model/channelmanager.h \
    src/util/fileutils.h \
    src/ui/streamitem.h \
    src/network/networkmanager.h \
    src/model/game.h

FORMS    += src/ui/mainwindow.ui

OTHER_FILES += \
    resources/logo.svg \
    resources/db/db.sql

QMAKE_CXXFLAGS += -std=c++11 -Wall -O2

unix{
    copydata.commands = $(COPY_DIR) $$PWD/resources $$OUT_PWD
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata
}

DISTFILES +=

RESOURCES += \
    src/qml/qml.qrc
