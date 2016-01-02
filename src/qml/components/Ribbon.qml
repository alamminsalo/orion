import QtQuick 2.5
import "../styles.js" as Styles

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
        color = isActive ? Styles.ribbonSelected : "transparent"
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
                color = isActive ? Styles.ribbonHighlight : "transparent"
        } else {
            iconLabel.iconColor = isActive ? Styles.iconHighlight : Styles.iconColor
        }
    }

    Rectangle {
        id: border
        color: Styles.purple
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
        color: Styles.border
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
    }

    Rectangle {
        id: borderBottom
        height: 0
        color: Styles.border
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
        color: Styles.textColor
        font.pixelSize: Styles.button.pixelSize
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
