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

    function play(name){
        renderer.command(["stop"])
        g_cman.findPlaybackStream(name, quality)

        paused = false
        renderer.command(["set","pause", paused ? "yes" : "no"])

        _label.visible = false
        spinner.visible = true
    }

    Connections {
        target: g_cman
        onFoundPlaybackStream: {
            renderer.command(["loadfile", stream])
            spinner.visible = false
        }
    }

    MpvObject {
        id: renderer
        anchors.fill: parent
        visible: parent.visible

        MouseArea{
            anchors.fill: parent
            onClicked: {
                paused = !paused
                renderer.command(["set","pause", paused ? "yes" : "no"])
            }
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
