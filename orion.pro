#-------------------------------------------------
#
# Orion: Twitch Client written in C++/QML
#
#-------------------------------------------------

QT       += qml quick network

TARGET = orion

DEFINES += APP_NAME=\\\"Orion\\\"

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
    src/libmpv/mpvobject.cpp


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
    src/libmpv/mpvobject.h \
    src/libmpv/mpvrenderer.h


FORMS    +=

OTHER_FILES += \
    resources/logo.svg

QMAKE_CXXFLAGS += -std=c++11 -Wall -O2

DISTFILES +=

RESOURCES += \
    src/qml/qml.qrc

#CONFIG(release): DEFINES += QT_NO_DEBUG_OUTPUT
