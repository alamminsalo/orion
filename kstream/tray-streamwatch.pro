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
    streamitem.cpp \
    tray.cpp

HEADERS  += mainwindow.h\
    ../channel.h \
    ../channelmanager.h \
    ../connector.h \
    ../fileutils.h \
    ../threadman.h \
    streamitem.h \
    tray.h \
    ../rapidjson/include/rapidjson/internal/pow10.h \
    ../rapidjson/include/rapidjson/internal/stack.h \
    ../rapidjson/include/rapidjson/internal/strfunc.h \
    ../rapidjson/include/rapidjson/document.h \
    ../rapidjson/include/rapidjson/filestream.h \
    ../rapidjson/include/rapidjson/prettywriter.h \
    ../rapidjson/include/rapidjson/rapidjson.h \
    ../rapidjson/include/rapidjson/reader.h \
    ../rapidjson/include/rapidjson/stringbuffer.h \
    ../rapidjson/include/rapidjson/writer.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    resources/logo.svg

QMAKE_CXXFLAGS += -std=c++11 -O2

copydata.commands = $(COPY_DIR) $$PWD/resources $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

DISTFILES += \
    resources/scripts/play.sh
