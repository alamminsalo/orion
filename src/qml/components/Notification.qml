import QtQuick 2.0
import QtQuick.Window 2.0
import "../styles.js" as Styles
import "../style"

Window {
    property string title
    property string description
    property string imgSrc
    property real destY

    signal clicked()

    //Locations: 0 - topleft, 1 - topright, 2 - bottomleft, 3 - bottomright
    property int location: g_cman.getAlertPosition()

    id: root
    flags: Qt.SplashScreen | Qt.NoFocus | Qt.X11BypassWindowManagerHint | Qt.BypassWindowManagerHint | Qt.WindowStaysOnTopHint | Qt.Popup
    width: Dpi.scale(440)
    height: Dpi.scale(140)

    function close(){
        //console.log("Destroying notification")
        root.destroy()
    }

    function setPosition(){
        switch (location){
        case 0:
            x =  Dpi.scale(50)
            y = -height
            destY = Dpi.scale(50)
            break

        case 1:
            x = Screen.width - width  - Dpi.scale(50)
            y = -height
            destY = Dpi.scale(50)
            break

        case 2:
            x = Dpi.scale(50)
            y = Screen.height
            destY = Screen.height - height  - Dpi.scale(50)
            break

        case 3:
            x = Screen.width - width  - Dpi.scale(50)
            y = Screen.height
            destY = Screen.height - height - Dpi.scale(50)
            break
        }
    }

    onVisibleChanged: {
        if (visible){
            setPosition()
            show()
            raise()
            anim.start()
        }
    }

    NumberAnimation {
        id: anim
        target: root
        properties: "y"
        from: y
        to: destY
        duration: 300
        easing.type: Easing.OutCubic
    }

    Rectangle {
        anchors.fill: parent
        color: Styles.bg

        Image {
            id: img
            source: imgSrc
            fillMode: Image.PreserveAspectFit
            width: Dpi.scale(100)
            height: Dpi.scale(100)
            anchors {
                left: parent.left
                leftMargin: Dpi.scale(10)
                verticalCenter: parent.verticalCenter
            }
        }

        Item {
            anchors {
                left: img.right
                right: parent.right
                leftMargin: Dpi.scale(10)
                rightMargin: Dpi.scale(10)
                verticalCenter: parent.verticalCenter
            }
            height: Dpi.scale(100)
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
                font.pointSize: Styles.titleFont.bigger
                //renderType: Text.NativeRendering
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
                font.pointSize: Styles.titleFont.smaller
                font.family: "Droid Sans"
                //renderType: Text.NativeRendering
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.hide()
            root.clicked()
        }
    }
}
