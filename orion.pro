#-------------------------------------------------
#
# Orion: Twitch Client written in C++/QML
#
# Deployment: create dir 'lib' to build dir and copy over the needed libraries
# (libmpv, snorenotify)
#
#-------------------------------------------------

QT       += qml quick network

TARGET = orion

DEFINES += APP_NAME=\\\"Orion\\\" \
        ENABLE_NOTIFY \
        #DEBUG_LIBMPV

LIBS += -lsnore-qt5 \
        -lmpv

SOURCES += src/main.cpp\
    src/model/channelmanager.cpp \
    src/model/channel.cpp \
    src/util/fileutils.cpp \
    src/network/networkmanager.cpp \
    src/model/game.cpp \
    src/util/jsonparser.cpp \
    src/model/channellistmodel.cpp \
    src/model/gamelistmodel.cpp \
    src/notifications/notification.cpp \
    src/player/mpvobject.cpp


HEADERS  += src/model/channel.h \
    src/model/channelmanager.h \
    src/util/fileutils.h \
    src/network/networkmanager.h \
    src/model/game.h \
    src/util/jsonparser.h \
    src/model/channellistmodel.h \
    src/model/gamelistmodel.h \
    src/notifications/notification.h \
    src/util/m3u8parser.h \
    src/player/mpvobject.h \
    src/player/mpvrenderer.h

QMAKE_CXXFLAGS += -std=c++11 -Wall -O2

DISTFILES += src/qml/icon/orion.svg

#Copy some files over to destination dir

CONFIG(release): {
    copydata.commands = $(COPY) $$PWD/distfiles/* $$OUT_PWD
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)

    QMAKE_EXTRA_TARGETS += first copydata
}

RESOURCES += \
    src/qml/qml.qrc

#CONFIG(release): DEFINES += QT_NO_DEBUG_OUTPUT
