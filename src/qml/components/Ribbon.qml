import QtQuick 2.0
import "../styles.js" as Style

Rectangle {
    property string text
    property bool isSelected
    property string iconStr
    property int borderWidth: 6
    property int iconSize: 20
    property bool highlightOn: true

    id: root
    height: 50
    anchors {
        left: parent.left
        right: parent.right
    }
    color: "transparent"

    function setFocus(isActive){
        border.width = isActive ? (g_toolBox.isOpen ? 14 : 50) : 0
        //borderTop.height = isActive ? 1 : 0
        //borderBottom.height = isActive ? 1 : 0
        color = isActive ? Style.ribbonSelected : "transparent"
        isSelected = isActive
        iconLabel.anchors.centerIn = g_toolBox.isOpen ? null : root
    }

    Connections {
        target: g_toolBox
        onIsOpenChanged: {
            setFocus(isSelected)
        }
    }

    function setHighlight(isActive){
        if (highlightOn){
            if (!isSelected)
                color = isActive ? Style.ribbonHighlight : "transparent"
        } else {
            iconLabel.iconColor = isActive ? Style.iconHighlight : Style.iconColor
        }
    }

    Rectangle {
        id: border
        color: Style.purple
        width: 0
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        Behavior on width {
            NumberAnimation {
                duration: 150
                easing.type: Easing.OutCubic
            }
        }
    }

    MouseArea {
        id: mArea
        anchors.fill: parent
        hoverEnabled: true
        onHoveredChanged: {
            setHighlight(containsMouse)
        }
    }


    Rectangle {
        id: borderTop
        height: 0
        color: Style.border
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
    }

    Rectangle {
        id: borderBottom
        height: 0
        color: Style.border
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    Text {
        id: textLabel
        visible: g_toolBox.isOpen
        anchors.centerIn: parent
        text: root.text
        color: Style.textColor
        font.pixelSize: Style.button.pixelSize
    }

    Icon {
        id: iconLabel
        icon: iconStr
        iconSize: root.iconSize
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
    }

}
