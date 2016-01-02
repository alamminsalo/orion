import QtQuick 2.0
import "fontAwesome.js" as FontAwesome
import "../styles.js" as Style

Item {
    property string icon
    property int iconSize
    property color iconColor: Style.iconColor

    id: root
    height: 24
    width: height

    FontLoader {
        source: "../fonts/fontawesome-webfont.ttf"
    }

    Text {
        anchors.centerIn: root
        font.pixelSize: iconSize || root.height
        color: iconColor
        font.family: "FontAwesome"
        text: FontAwesome.fromText(icon)
    }
}
