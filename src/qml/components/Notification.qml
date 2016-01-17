import QtQuick 2.0
import QtQuick.Window 2.0
import "../styles.js" as Styles

Window {
    property string title
    property string description
    property string imgSrc
    property real destY

    id: root
    flags: Qt.SplashScreen | Qt.NoFocus | Qt.X11BypassWindowManagerHint | Qt.BypassWindowManagerHint

    function close(){
        //console.log("Destroying notification")
        root.destroy()
    }

    Component.onCompleted: {
        show()
        y = destY
    }

    Behavior on y {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Styles.bg

        Image {
            id: img
            source: imgSrc
            fillMode: Image.PreserveAspectFit
            width: dp(100)
            height: dp(100)
            anchors {
                left: parent.left
                leftMargin: dp(10)
                verticalCenter: parent.verticalCenter
            }
        }

        Item {
            anchors {
                left: img.right
                right: parent.right
                leftMargin: dp(10)
                rightMargin: dp(10)
                verticalCenter: parent.verticalCenter
            }
            height: dp(100)
            clip: true

            Text {
                id: titleText
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                text: root.title
                color: Styles.textColor
                font.family: "Droid Sans"
                wrapMode: Text.WordWrap
                font.bold: true
                font.pointSize: dp(Styles.titleFont.bigger)
            }

            Text {
                id: descriptionText
                anchors {
                    top: titleText.bottom
                    left: parent.left
                    right: parent.right
                }
                text: root.description
                wrapMode: Text.WordWrap
                color: Styles.textColor
                font.pointSize: dp(Styles.titleFont.smaller)
                font.family: "Droid Sans"
            }
        }
    }
}
