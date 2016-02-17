import QtQuick 2.0
import "../styles.js" as Styles

Item {
    property string text
    property var value
    property var mask
    property int inputWidth: dp(60)

    function getValue(){
        return _input.text
    }

    id: root
    height: dp(60)

    Component.onCompleted: _input.text = value

    Text {
        font.family: "Droid Sans"
        color: Styles.textColor
        font.pointSize: (Styles.titleFont.bigger)
        text: root.text
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: dp(10)
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        renderType: Text.NativeRendering
    }

    Rectangle {
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            margins: dp(10)
        }

        width: root.inputWidth

        //radius: dp(5)
        color: Styles.sidebarBg

        MouseArea{
            anchors.fill: parent
            cursorShape: Qt.IBeamCursor

            TextInput{
                id: _input
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
