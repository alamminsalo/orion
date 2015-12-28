import QtQuick 2.0
import "../styles.js" as Style

Rectangle {

    property int id: index
    property string text: label
    property bool isSelected

    height: 36
    anchors {
        left: parent.left
        right: parent.right
    }
    color: "transparent"

    function setFocus(isActive){
        border_anim.stop()
        border_anim.from = border.width
        border_anim.to = isActive ? 8 : 0
        border_anim.start()
        borderTop.height = isActive ? 1 : 0
        borderBottom.height = isActive ? 1 : 0
        color = isActive ? Style.twitch.bg : "transparent"
        isSelected = isActive
    }

    function setHighlight(isActive){
        if (!isSelected)
            color = isActive ? Style.twitch.bg : "transparent"
    }

    Rectangle {
        id: border
        color: Style.twitch.purple
        width: 0
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        NumberAnimation on width {
            id: border_anim
            duration: 150
            easing.type: Easing.OutCubic
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onHoveredChanged: {
            setHighlight(containsMouse)
        }
    }

    Text {
        anchors.centerIn: parent
        text: parent.text
        color: Style.textColor
        font.pixelSize: Style.button.pixelSize
    }

    Rectangle {
        id: borderTop
        height: 0
        color: Style.twitch.border
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
    }

    Rectangle {
        id: borderBottom
        height: 0
        color: Style.twitch.border
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

}
