#-------------------------------------------------
#
# Orion: Twitch Client written in C++/QML
#
#-------------------------------------------------

QT     += gui opengl qml quick network widgets webengine

TARGET = orion

DEFINES += APP_NAME=\\\"Orion\\\"

SOURCES += src/main.cpp\
    src/model/channelmanager.cpp \
    src/model/channel.cpp \
    src/util/fileutils.cpp \
    src/network/networkmanager.cpp \
    src/model/game.cpp \
    src/util/jsonparser.cpp \
    src/model/channellistmodel.cpp \
    src/model/gamelistmodel.cpp \
    src/power/power.cpp \
    src/systray.cpp \
    src/util/runguard.cpp \
    src/customapp.cpp \
    src/model/vod.cpp \
    src/model/vodlistmodel.cpp \
    src/model/vodmanager.cpp \
    src/notification/notificationmanager.cpp \


HEADERS  += src/model/channel.h \
    src/model/channelmanager.h \
    src/util/fileutils.h \
    src/network/networkmanager.h \
    src/model/game.h \
    src/util/jsonparser.h \
    src/model/channellistmodel.h \
    src/model/gamelistmodel.h \
    src/util/m3u8parser.h \
    src/power/power.h \
    src/systray.h \
    src/util/runguard.h \
    src/customapp.h \
    src/model/vod.h \
    src/model/vodlistmodel.h \
    src/model/vodmanager.h \
    src/network/urls.h \
    src/notification/notificationmanager.h

#Communi IRC plugin
CONFIG += communi
COMMUNI += core

#Backend for player
BACKEND = mpv

equals(BACKEND, mpv) {
    #DEFINES += DEBUG_LIBMPV
    DEFINES += MPV_PLAYER
    SOURCES +=  src/player/mpvrenderer.cpp \
                src/player/mpvobject.cpp

    HEADERS +=  src/player/mpv/client.h \
                src/player/mpv/opengl_cb.h \
                src/player/mpv/qthelper.hpp \
                src/player/mpvobject.h \
                src/player/mpvrenderer.h \

    unix:!macx: LIBS += -lmpv
    win32: LIBS += -L$${PWD}/libs/ -lmpv.dll
    macx: LIBS += -L$$PWD/../../../../usr/local/Cellar/mpv/0.17.0/lib -lmpv
}

equals(BACKEND, qtav) {
    QT += av
    DEFINES += QTAV_PLAYER
}

equals(BACKEND, multimedia) {
    QT += multimedia
    DEFINES += MULTIMEDIA_PLAYER
    macx: {
        LIBS += -framework AVFoundation
        INCLUDEPATH += /System/Library/Frameworks/AVFoundation.framework/Versions/Current/Headers
    }
}

QMAKE_CXXFLAGS += -Wall -O2

CONFIG += c++11
#CONFIG += console

DISTFILES += src/qml/icon/orion.svg \
    src/qml/player.html

unix:!macx: {
    QT += dbus

    HEADERS += src/notification/notificationsender.h
    SOURCES +=  src/notification/notificationsender.cpp

    #Copy some files over to destination dir
    CONFIG(release): {
        copydata.commands = $(COPY) $$PWD/distfiles/* $$OUT_PWD
        first.depends = $(first) copydata
        export(first.depends)
        export(copydata.commands)

        QMAKE_EXTRA_TARGETS += first copydata
    }

    #QMAKE_POST_LINK += $quote($(COPY) $PWD/distfiles * $OUT_PWD
}

RESOURCES += \
    src/qml/qml.qrc

# Copies the given files to the destination directory
defineTest(copyToDestdir) {
    files = $$1

    for(FILE, files) {
        DDIR = $$DESTDIR

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

win32: {
    RC_ICONS = distfiles/orion.ico

    EXTRA_BINFILES = $$PWD/libs/ssleay32.dll \
                    $$PWD/libs/libeay32.dll

    EXTRA_BINFILES_WIN = $${EXTRA_BINFILES}
    EXTRA_BINFILES_WIN ~= s,/,\\,g
    DESTDIR_WIN = $$OUT_PWD/release
    DESTDIR_WIN ~= s,/,\\,g
    for(FILE,EXTRA_BINFILES_WIN){
                QMAKE_POST_LINK +=$$quote(cmd /c copy /y $${FILE} $${DESTDIR_WIN}$$escape_expand(\n\t))
    }

}

#CONFIG(release): DEFINES += QT_NO_DEBUG_OUTPUT

macx: {
    ICON = $$PWD/distfiles/orion.icns
    LIBS += -framework Foundation -framework CoreServices
    LIBS += -framework AppKit

    HEADERS += src/notification/notificationsender.h
    #inc as obj-c
    OBJECTIVE_SOURCES +=

    INCLUDEPATH += /System/Library/Frameworks/Foundation.framework/Versions/C/Headers
    INCLUDEPATH += /System/Library/Frameworks/AppKit.framework/Versions/C/Headers
}

OBJECTIVE_SOURCES += \
    src/notification/notificationsender.mm
