import QtQuick 2.5
import "../styles.js" as Styles

Rectangle {
    property string text
    property bool isSelected: false
    property string iconStr
    property int borderWidth: 6
    property int iconSize: Styles.iconSize
    property bool highlightOn: false
    property bool iconRotated: false

    function rotateIcon(){
        iconRotated = !iconRotated
    }

    id: root
    height: dp(60)
    anchors {
        left: parent.left
        right: parent.right
    }
    color: "transparent"

    function setFocus(isActive){
        border.width = isActive ? (g_toolBox.isOpen ? dp(14) : dp(60)) : 0
        color = isActive ? Styles.ribbonSelected : "transparent"
        textLabel.color = isActive ? Styles.textColor : Styles.iconColor
        isSelected = isActive
        iconLabel.anchors.centerIn = g_toolBox.isOpen ? null : root
        iconLabel.iconColor = isActive ? Styles.iconHighlight : Styles.iconColor
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
        }
        if (!isSelected){
            iconLabel.iconColor = isActive ? Styles.iconHighlight : Styles.iconColor
            textLabel.color = isActive ? Styles.textColor : Styles.iconColor
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

    Text {
        id: textLabel
        visible: g_toolBox.isOpen
        anchors.centerIn: parent
        text: root.text
        font.bold: true
        font.family: "Droid Sans"
        color: Styles.iconColor
        font.pointSize: Styles.titleFont.smaller
        ////renderType: Text.NativeRendering
    }

    Icon {
        id: iconLabel
        rotation: iconRotated ? -180 : 0
        icon: iconStr
        iconSize: root.iconSize
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
    }
}
