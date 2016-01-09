import QtQuick 2.0
import "../styles.js" as Styles

Item {
    property string text
    property string value
    property var mask
    property int inputWidth: dp(60)

    id: root
    height: dp(60)

    Text {
        font.family: "Droid Sans"
        color: Styles.textColor
        font.pixelSize: Styles.titleFont.bigger
        text: root.text
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: dp(10)
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Rectangle {
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            margins: dp(10)
        }

        width: root.inputWidth

        radius: dp(5)
        color: Styles.sidebarBg

        MouseArea{
            anchors.fill: parent
            cursorShape: Qt.IBeamCursor

            TextInput{
                id: _input
                text: root.value
                color: Styles.iconColor
                anchors.fill: parent
                clip:true
                selectionColor: Styles.purple
                focus: true
                selectByMouse: true
                font.pointSize: dp(Styles.titleFont.bigger)
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                inputMask: mask
            }
        }
    }
}
