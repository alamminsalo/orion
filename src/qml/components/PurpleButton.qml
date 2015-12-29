import QtQuick 2.0
import "../styles.js" as Style

Rectangle {
    property string text
    property string iconStr

    id: root
    width: 70
    height: 30
    color: Style.twitch.purple

    Component.onCompleted: {
        if (!label.text){
            iconLabel.anchors.centerIn = root
        }
    }

    Text {
        id: label
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

    Icon {
        id: iconLabel
        icon: iconStr
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
    }

    MouseArea {
        id: mArea
        anchors.fill: parent
        hoverEnabled: true

        onPressedChanged:; onHoveredChanged: {
            hoverRect.opacity = pressed ? .1 : 0
            hoverRect.opacity = containsMouse ? .1 : 0
        }
    }
}


