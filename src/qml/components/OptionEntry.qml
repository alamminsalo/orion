import QtQuick 2.0
import "../styles.js" as Styles

Item {
    property string text

    id: root
    height: dp(60)

    Text {
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

    //Add option interface in declaration
}

