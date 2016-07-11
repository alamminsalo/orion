import QtQuick 2.0
import "../styles.js" as Styles

Item {
    id: root
    property string user
    property string msg

    anchors {
        left: parent.left
        right: parent.right
    }
    height: _text.contentHeight + width * 0.04

    Component.onCompleted: {
        _text.text = "<b>%1</b>: %2".arg(user).arg(msg)
    }

    Text {
        id: _text
        verticalAlignment: Text.AlignVCenter
        color: Styles.textColor
        font.pixelSize: Styles.titleFont.smaller
        wrapMode: Text.WordWrap
        anchors {
            fill: root
            margins: parent.width * 0.02
        }
    }
}
