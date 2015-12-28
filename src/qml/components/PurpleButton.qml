import QtQuick 2.0
import "../styles.js" as Style

Rectangle {
    property string text

    id: root
    width: 70
    height: 30
    color: Style.twitch.purple


    Text {
        anchors.centerIn: parent
        color: Style.textColor
        text: root.text
        font.pixelSize: Style.button.pixelSize
    }

    Rectangle {
        id: hoverRect
        color: "#000000"
        anchors.fill: parent
        opacity: 0
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onPressedChanged: {
            hoverRect.opacity = pressed ? .1 : 0
        }
    }
}


