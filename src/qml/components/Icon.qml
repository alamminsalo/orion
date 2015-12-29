import QtQuick 2.0
import "fontAwesome.js" as FontAwesome
import "../styles.js" as Styles

Rectangle {
    property string icon
    property int iconSize

    id: root
    height: 24
    width: height
    color: "transparent"

    FontLoader {
        source: "../fonts/fontawesome-webfont.ttf"
    }

    Text {
        anchors.centerIn: root
        font.pixelSize: iconSize || Math.floor(root.height * 0.8)
        color: Styles.iconColor
        font.family: "FontAwesome"
        text: FontAwesome.fromText(icon)
    }
}
