
import QtQuick 2.0
import QtQuick.Window 2.0

Window {
    id: root
    flags: Qt.SplashScreen | Qt.NoFocus

    property string title: "Channel title"
    property string description: "Description goes here."
    property string img: "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"

    property variant currentChannel

    Component.onCompleted: {
        if (img.height > 320){
            root.height = img.height
            root.width = img.width
        } else {
            root.height = 320
            root.width = 480
        }
    }

    Rectangle {
        id: rootRect
        anchors.fill: parent

        Image {
            id: img
            source: root.img
            anchors.fill: parent

            Rectangle {
                id: header
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }

                color: "#000000"
                opacity: 0.5
                height: 64
            }

            Text {
                id: title
                opacity: 1
                color: "white"
                text: root.title
                width: parent.width
                anchors{
                    top: header.top
                    left: header.left
                }
                wrapMode: Text.WordWrap
            }

            Text {
                id: description
                opacity: 1
                color: "white"
                width: parent.width
                text: root.description
                anchors {
                    top: title.bottom
                    left: parent.left
                }
                wrapMode: Text.WordWrap
            }
        }


    }

    function displayChannel(mX, mY, channel){
        if (channel && channel !== currentChannel){
            currentChannel = channel
            root.img = channel.online ? channel.preview : "https://raw.githubusercontent.com/alamminsalo/kstream/dev/resources/preview/offline.png"
            //root.img = channel.preview
            root.title = channel.title
            root.description = channel.info
            root.x = mX
            root.y = mY
            root.show()
        }
    }
}
