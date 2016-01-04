
import QtQuick 2.5
import QtQuick.Window 2.0
import "../styles.js" as Styles

Window {
    id: root
    flags: Qt.SplashScreen | Qt.NoFocus | Qt.X11BypassWindowManagerHint | Qt.BypassWindowManagerHint

    height: dp(320)
    width: dp(512)

    property string text
    property string img

    Rectangle {
        id: rootRect
        anchors.fill: parent
        color: "#000000"

        SpinnerIcon {
            id:_spinner
            anchors.fill: parent
            iconSize: dp(64)
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
            opacity: 0.7
            height: text.height
        }

        Text {
            id: text
            color: Styles.textColor
            text: root.text
            font.pointSize: dp(Styles.titleFont.smaller)
            anchors{
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            wrapMode: Text.WordWrap
        }
    }

    function display(mX, mY){

        console.log(mX, mY)

        if (g_contextMenuVisible){
            return
        }

        root.x = mX + dp(20)

        if (root.x + root.width > Screen.width)
            root.x -= root.width + dp(40)

        root.y = mY

        root.show()
    }
}
