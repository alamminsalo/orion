import QtQuick 2.0
import "../styles.js" as Style

Rectangle {
    property string text
    id: root

    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }
    height: dp(60)
    color: Style.bg

    Item {
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: dp(20)
        }
        width: _text.width

        Text {
            id: _text
            anchors {
                top: parent.top
                left: parent.left
            }

            color: Style.textColor
            font.pixelSize: dp(30)
            text: root.text
            font.family: "Droid Sans"
            z: root.z + 1
            renderType: Text.NativeRendering
        }
    }
}
