import QtQuick 2.5
import "../styles.js" as Styles

//Channel.qml
Rectangle {
    property int _id
    property string name
    property string title
    property string logo
    property string info
    property string preview
    property bool online
    property bool favourite: false
    property int viewers
    property string game
    property int imgSize: dp(148)
    property int containerSize: dp(200)

    id: root

    width: containerSize
    height: width
    border.color: "transparent"
    border.width: dp(1)
    antialiasing: false
    clip:true
    color: "transparent"
    radius: dp(5)

    Component.onCompleted: {
        imageShade.refresh()
    }

    onOnlineChanged: {
        imageShade.refresh()
    }

    Rectangle {
        id: container
        height: imgSize
        width: height
        anchors.centerIn: parent
        clip: true
        color: "#000000"

        SpinnerIcon {
            id:_spinner
            iconSize: dp(38)
            anchors.fill: parent
        }

        Image {
            id: channelImage
            source: root.logo
            fillMode: Image.PreserveAspectFit
            width: imgSize
            anchors.centerIn: container

            Component.onCompleted: {
                if (root.scaleImage){
                    width = height
                }
            }

            onProgressChanged: {
                if (progress >= 1.0)
                    _spinner.visible = false
            }

            Behavior on width {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InCubic
                }
            }

            Rectangle {
                id: imageShade
                anchors.fill: parent
                color: "#000000"
                opacity: 0

                function refresh(){
                    opacity = root.online ? 0 : .8
                }

            }
        }

        Icon {
            id: favIcon
            icon: "fav"
            opacity: favourite ? 1 : 0
            iconSize: dp(24)
            iconColor: Styles.purple
            anchors {
                top: container.top
                right: container.right
                margins: dp(10)
            }

            Behavior on opacity{
                NumberAnimation{
                    duration: 200
                    easing.type: Easing.OutCubic
                }
            }
        }

        Rectangle {
            id: infoRect
            color: favourite ? Styles.purple : Styles.shadeColor
            opacity: .85
            height: Math.floor(parent.height * 0.25)

            anchors {
                left: container.left
                right: container.right
                bottom: container.bottom
            }
        }

        Text {
            id: channelTitle
            text: root.title
            elide: Text.ElideRight
            color: online ? Styles.textColor : Styles.iconColor
            anchors.fill: infoRect
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "Droid Sans"
            font.pixelSize: dp(Styles.titleFont.smaller)
            wrapMode: Text.WordWrap
            renderType: Text.NativeRendering
        }
    }


    function setHighlight(isActive){
        //imageShade.visible = !isActive && !root.online
        channelImage.width = isActive ? Math.floor(imgSize * 1.2) : imgSize
        root.color = isActive ? Styles.highlight : "transparent"
        root.border.color = isActive ? Styles.border : "transparent"
    }
}
