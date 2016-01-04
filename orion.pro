#-------------------------------------------------
#
# Orion: Twitch Client written in C++/QML
#
#-------------------------------------------------

QT       += qml quick network

TARGET = Orion

TEMPLATE = app

DEFINES += APP_NAME=\\\"orion\\\"\
        _QML

LIBS +=

SOURCES += src/main.cpp\
    src/model/channelmanager.cpp \
    src/model/channel.cpp \
    src/util/fileutils.cpp \
    src/network/networkmanager.cpp \
    src/model/game.cpp \
    src/util/jsonparser.cpp \
    src/model/channellistmodel.cpp \
    src/model/gamelistmodel.cpp \
    src/ui/notification.cpp \

HEADERS  += src/model/channel.h \
    src/model/channelmanager.h \
    src/util/fileutils.h \
    src/network/networkmanager.h \
    src/model/game.h \
    src/util/jsonparser.h \
    src/model/channellistmodel.h \
    src/model/gamelistmodel.h \
    src/ui/notification.h \


FORMS    += src/ui/mainwindow.ui

OTHER_FILES += \
    resources/logo.svg

QMAKE_CXXFLAGS += -std=c++11 -Wall -O2

DISTFILES +=

RESOURCES += \
    src/qml/qml.qrc\

unix: LIBS += -L$$PWD/../../../../usr/local/lib64/ -lsnore-qt5

INCLUDEPATH += $$PWD/../../../../usr/local/lib64
DEPENDPATH += $$PWD/../../../../usr/local/lib64
