import QtQuick 2.5
import "../fonts/fontAwesome.js" as FontAwesome
import "../styles.js" as Styles

Rectangle {
    property string icon
    property int iconSize
    property color iconColor: Styles.iconColor

    id: root
    height: dp(24)
    width: height
    color: "transparent"

    Behavior on rotation {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }

    FontLoader {
        source: "../fonts/fontawesome-webfont.ttf"
    }

    Text {
        anchors.centerIn: parent
        font.pixelSize: iconSize ? iconSize : root.height
        color: iconColor
        font.family: "FontAwesome"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        text: FontAwesome.fromText(icon)
    }
}
