
import QtQuick 2.0
import QtQuick.Window 2.0
import "../styles.js" as Style

Window {
    id: root
    flags: Qt.SplashScreen | Qt.NoFocus
    height: 240
    width: 384

    property string text
    property string img

    Rectangle {
        id: rootRect
        anchors.fill: parent

        Image {
            id: img
            source: root.img
            anchors.fill: parent
        }

        //Container for text
        Rectangle {
            id: header
            anchors.fill: text
            color: Style.shadeColor
            opacity: 0.5
            height: text.height
        }

        Text {
            id: text
            color: Style.textColor
            text: root.text
            font.pixelSize: Style.titleFont.pixelSize
            anchors{
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            wrapMode: Text.WordWrap
        }
    }

    function display(mX, mY){
        root.x = mX + 20

        if (root.x + root.width > Screen.width)
            root.x -= root.width + 40

        root.y = mY

        root.show()
    }
}
