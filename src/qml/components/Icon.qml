import QtQuick 2.5
import "../fonts/fontAwesome.js" as FontAwesome
import "../styles.js" as Styles

Item {
    property string icon
    property int iconSize
    property color iconColor: Styles.iconColor

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
