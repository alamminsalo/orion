import QtQuick 2.0
import "components"
import mpv 1.0

import "styles.js" as Styles

Item {
    anchors.fill: parent

    //Quality values:
    //  4 - source
    //  3 - high
    //  2 - medium
    //  1 - low
    //  0 - mobile
    property int quality: 4
    property bool paused: false
    property var currentChannel

    function play(channel){
        renderer.command(["stop"])
        g_cman.findPlaybackStream(channel.name, quality)

        paused = false
        renderer.command(["set","pause", "no"])

        currentChannel = channel

        _label.visible = false
        spinner.visible = true

        header.text = "Currently watching: " + currentChannel.title
                +   " playing " + currentChannel.game
    }

    Connections {
        target: g_cman
        onFoundPlaybackStream: {
            header.text = "Currently watching: " + currentChannel.title
                    +   " playing " + currentChannel.game
            renderer.command(["loadfile", stream])
            spinner.visible = false
        }

//        onCacheUpdated: {
//            renderer.setProperty("cache-secs", g_cman.getCache())
//        }
    }

    PlayerHeader{
        text: "Currently watching: N/A"
        id: header
        z: renderer.z + 1
    }

    onVisibleChanged: {
        if (visible)
            header.show()
    }

    MpvObject {
        id: renderer
        anchors {
            fill: parent
        }

        visible: parent.visible

//        Component.onCompleted: {
//            renderer.setProperty("cache-secs", g_cman.getCache())
//        }

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                paused = !paused
                renderer.command(["set","pause", paused ? "yes" : "no"])
            }

            onPositionChanged: {
                header.show()
            }
        }

        onPlayingStopped: {
            header.text = "Playback stopped"
        }

        onPlayingPaused: {
            header.text = "Paused"
        }

        onPlayingResumed: {
            header.text = "Currently watching: " + currentChannel.title
                    +   " playing " + currentChannel.game
            spinner.visible = false
        }

        onBufferingStarted: {
            spinner.visible = true
        }
    }

    Text {
        id: _label
        text: "No stream currently playing"
        font.pointSize: dp(40)
        font.family: "Droid Sans"
        color: Styles.iconColor
        anchors.centerIn: parent
    }

    SpinnerIcon {
        id: spinner
        visible: false
        anchors.centerIn: parent
        iconSize: dp(100)
    }
}
