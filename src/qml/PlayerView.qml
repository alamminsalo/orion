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

    id: root

    function play(channel){

        if (!channel){
            return
        }

        renderer.command(["stop"])
        g_cman.findPlaybackStream(channel.name)

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

            qualityMap = streams

//            console.log(streams)
//            for(var i in streams){
//                console.log(i)
//            }

            var desc = true
            while (!qualityMap[quality] || qualityMap[quality].length <= 0){

                if (quality <= 0)
                    desc = false

                if (quality == 4 && !desc)
                    break;

                quality += desc ? -1 : 1
            }

            console.log("resolved quality:", quality)
            //streamLabel.update(quality)

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

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
    }

    PlayerHeader {
        id: footer
        //color: "white"
        z: renderer.z + 1
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
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
                paused = !paused
                renderer.command(["set","pause", paused ? "yes" : "no"])
            }

            onPositionChanged: {
                header.show()
                footer.show()
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
