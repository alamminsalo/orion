
import QtQuick 2.5
import QtQuick.Window 2.0
import "../styles.js" as Styles

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
        color: "#000000"

        SpinnerIcon {
            id:_spinner
            anchors.fill: parent
            iconSize: 64
        }

        Image {
            id: img
            source: root.img
            anchors.fill: parent
            onProgressChanged: {
                if (progress >= 1.0)
                    _spinner.visible = false
            }

            onSourceChanged: {
                _spinner.visible = true
            }
        }

        //Container for text
        Rectangle {
            id: header
            anchors.fill: text
            color: Styles.shadeColor
            opacity: 0.5
            height: text.height
        }

        Text {
            id: text
            color: Styles.textColor
            text: root.text
            font.pixelSize: Styles.titleFont.pixelSize
            anchors{
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            wrapMode: Text.WordWrap
        }
    }

    function display(mX, mY){

        if (g_contextMenuVisible){
            return
        }

        root.x = mX + 20

        if (root.x + root.width > Screen.width)
            root.x -= root.width + 40

        root.y = mY

        root.show()
    }
}
