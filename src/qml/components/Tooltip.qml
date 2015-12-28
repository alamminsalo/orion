
import QtQuick 2.0
import QtQuick.Window 2.0
import "../styles.js" as Style

Window {
    id: root
    flags: Qt.SplashScreen | Qt.NoFocus
    height: 200
    width: 333

    property string title
    property string description
    property string img
    property int viewers: 0

    Rectangle {
        id: rootRect
        anchors.fill: parent

        Image {
            id: img
            source: root.img
            anchors.fill: parent
        }

        Rectangle {
            id: header
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            color: Style.shadeColor
            opacity: 0.5
            height: 60
        }

        Text {
            id: title
            opacity: 1
            color: Style.textColor
            text: "<b>" + root.title + ":</b> " + (root.viewers ? root.viewers + " viewers" : "")
            width: parent.width
            font.pixelSize: Style.titleFont.pixelSize
            anchors{
                top: header.top
                left: header.left
            }
            wrapMode: Text.WordWrap
        }

        Text {
            id: description
            opacity: 1
            color: Style.textColor
            width: parent.width
            text: root.description
            font.pixelSize: Style.titleFont.pixelSize
            anchors {
                top: title.bottom
                left: parent.left
            }
            wrapMode: Text.WordWrap
        }


    }

    function displayChannel(mX, mY, channel){
        if (channel){

            root.img = channel.preview
            root.title = channel.title
            root.description = channel.info
            root.viewers = channel.viewers

            root.x = mX
            root.y = mY

            header.height = root.description ? 60 : 40

            root.show()
        }
    }
}
