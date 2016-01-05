#-------------------------------------------------
#
# Orion: Twitch Client written in C++/QML
#
#-------------------------------------------------

QT       += qml quick network

TARGET = orion

TEMPLATE = app

DEFINES += APP_NAME=\\\"orion\\\" \
        _QML \

LIBS += -L"$$_PRO_FILE_PWD_/thirdparty/libsnore" -lsnore-qt5

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


FORMS    +=

OTHER_FILES += \
    resources/logo.svg

QMAKE_CXXFLAGS += -std=c++11 -Wall -O2

DISTFILES +=

RESOURCES += \
    src/qml/qml.qrc

CONFIG(release): DEFINES += QT_NO_DEBUG_OUTPUT
