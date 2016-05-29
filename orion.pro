#-------------------------------------------------
#
# Orion: Twitch Client written in C++/QML
#
#-------------------------------------------------

QT     += gui opengl qml quick network widgets webengine

TARGET = orion

DEFINES += APP_NAME=\\\"Orion\\\" \
        #DEBUG_LIBMPV

SOURCES += src/main.cpp\
    src/model/channelmanager.cpp \
    src/model/channel.cpp \
    src/util/fileutils.cpp \
    src/network/networkmanager.cpp \
    src/model/game.cpp \
    src/util/jsonparser.cpp \
    src/model/channellistmodel.cpp \
    src/model/gamelistmodel.cpp \
    src/player/mpvobject.cpp \
    src/power/power.cpp \
    src/systray.cpp \
    src/util/runguard.cpp \
    src/customapp.cpp \
    src/model/vod.cpp \
    src/model/vodlistmodel.cpp \
    src/model/vodmanager.cpp \
    src/notification/notificationmanager.cpp \
    src/player/mpvrenderer.cpp


HEADERS  += src/model/channel.h \
    src/model/channelmanager.h \
    src/util/fileutils.h \
    src/network/networkmanager.h \
    src/model/game.h \
    src/util/jsonparser.h \
    src/model/channellistmodel.h \
    src/model/gamelistmodel.h \
    src/util/m3u8parser.h \
    src/player/mpvobject.h \
    src/player/mpvrenderer.h \
    src/power/power.h \
    src/systray.h \
    src/util/runguard.h \
    src/customapp.h \
    src/model/vod.h \
    src/model/vodlistmodel.h \
    src/model/vodmanager.h \
    src/network/urls.h \
    src/player/mpv/client.h \
    src/player/mpv/opengl_cb.h \
    src/player/mpv/qthelper.hpp \
    src/notification/notificationmanager.h

QMAKE_CXXFLAGS += -Wall -O2

CONFIG += c++11

DISTFILES += src/qml/icon/orion.svg

unix:!macx: {
    QT += dbus
    LIBS += -lmpv
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
    LIBS += -L$${PWD}/libs/ -lmpv.dll
    RC_ICONS = distfiles/orion.ico

    EXTRA_BINFILES = $$PWD/libs/mpv-1.dll \
                    $$PWD/distfiles/qt.conf \
                    $$PWD/libs/libssl32.dll \
                    $$PWD/libs/ssleay32.dll \
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
    LIBS += -framework Foundation
    LIBS += -framework AppKit

    HEADERS += src/notification/notificationsender.h
    #inc as obj-c
    OBJECTIVE_SOURCES +=

    INCLUDEPATH += /System/Library/Frameworks/Foundation.framework/Versions/C/Headers
    INCLUDEPATH += /System/Library/Frameworks/AppKit.framework/Versions/C/Headers

    LIBS += -L$$PWD/../../../../usr/local/Cellar/mpv/0.17.0/lib -lmpv.1.20.0
}

OBJECTIVE_SOURCES += \
    src/notification/notificationsender.mm
