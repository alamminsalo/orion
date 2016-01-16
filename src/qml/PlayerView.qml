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
    property var qualityMap
    property bool fs: false

    id: root

    function play(channel){

        if (!channel){
            return
        }

        renderer.command(["stop"])
        g_cman.findPlaybackStream(channel.name)

        paused = false
        renderer.play()

        currentChannel = channel

        _label.visible = false
        spinner.visible = true

        header.text = "Currently watching: " + currentChannel.title + " playing " + currentChannel.game
    }

    function togglePause(){
        paused = !paused

        if (paused)
            renderer.pause()

        else
            renderer.play()
    }

    Connections {
        target: g_cman
        onFoundPlaybackStream: {

            qualityMap = streams

            var desc = true
            while (!qualityMap[quality] || qualityMap[quality].length <= 0){

                if (quality <= 0)
                    desc = false

                if (quality == 4 && !desc)
                    break;

                quality += desc ? -1 : 1
            }

            sourcesBox.entries = qualityMap

            if (qualityMap[quality]){
                header.text = "Currently watching: " + currentChannel.title
                        +   " playing " + currentChannel.game

                renderer.command(["loadfile", qualityMap[quality]])

                spinner.visible = false

                sourcesBox.setIndex(quality)
            }
        }
    }

    PlayerHeader{
        text: "Currently watching: N/A"
        id: header
        z: renderer.z + 1

        MouseArea {
            id: mAreaHeader
            hoverEnabled: true
            anchors.fill: parent
            propagateComposedEvents: false
        }

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
    }

    PlayerHeader {
        id: footer
        z: renderer.z + 1
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        MouseArea {
            id: mAreaFooter
            hoverEnabled: true
            anchors.fill: parent
            propagateComposedEvents: false
        }

        Item {
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                leftMargin: dp(20)
            }

            width: dp(50)

            Icon {
                id: fsToggle
                anchors.centerIn: parent
                icon: paused ? "play" : "pause"

            }

            MouseArea {
                anchors.fill: parent
                onClicked: togglePause()
                hoverEnabled: true

                onHoveredChanged: {
                    fsToggle.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
                }
            }
        }



        VolumeSlider {
            id: vol
            z: parent.z + 1
            anchors {
                right: sourcesBox.left
                verticalCenter: parent.verticalCenter
                rightMargin: dp(20)
            }
            onValueChanged: {
                var val = Math.max(0,Math.min(100, Math.round(Math.log(value) / Math.log(100) * 100)))
                renderer.setProperty("volume", val)
            }
        }

        ComboBox {
            id: sourcesBox
            width: dp(100)
            height: dp(50)

            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: dp(20)
            }

            onIndexChanged: {
                if (index != quality){
                    quality = index
                    play(currentChannel)
                }
            }
        }
    }

    onVisibleChanged: {
        if (visible){
            header.show()
            footer.show()
        }
    }

    MpvObject {
        id: renderer
        anchors {
            fill: parent
        }

        visible: parent.visible

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                if (sourcesBox.open){
                    sourcesBox.close()
                }
            }

            onDoubleClicked: {
                g_fullscreen = !g_fullscreen
                //renderer.command(["set","pause", "no"])
            }

            onPositionChanged: {
                header.show()
                footer.show()
                headerTimer.restart()
            }
        }

        onPlayingStopped: {
            header.text = "Playback stopped"
            g_powerman.setScreensaver(true);
        }

        onPlayingPaused: {
            header.text = "Paused"
            g_powerman.setScreensaver(true);
        }

        onPlayingResumed: {
            header.text = "Currently watching: " + currentChannel.title
                    +   " playing " + currentChannel.game
            spinner.visible = false

            g_powerman.setScreensaver(false);
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


    Timer {
        id: headerTimer
        interval: 5000
        running: false
        repeat: false
        onTriggered: {
            if (mAreaHeader.containsMouse || mAreaFooter.containsMouse || vol.open || sourcesBox.open){
                restart()
            }
            else {
                header.hide()
                footer.hide()
            }
        }
    }
}
