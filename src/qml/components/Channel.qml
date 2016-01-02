import QtQuick 2.5
import QtGraphicalEffects 1.0
import "../styles.js" as Styles

//Channel.qml
Rectangle {
    property string name
    property string title
    property string logo
    property string info
    property string preview
    property bool online
    property int viewers
    property string game

    property int imgSize: 148
    property int containerSize

    id: root

    width: containerSize
    height: width
    border.color: "transparent"
    border.width: 1
    antialiasing: false
    clip:true
    color: "transparent"
    radius: 5

    Component.onCompleted: {
        colorOverlay.setOverlay(online)
    }

    onOnlineChanged: {
        colorOverlay.setOverlay(online)
    }

    Item {
        id: container
        height: imgSize
        width: height
        anchors.centerIn: parent
        clip: true

        SpinnerIcon {
            id:_spinner
            iconSize: 38
            anchors.fill: parent
        }

        Image {
            id: channelImage
            source: root.logo
            height: imgSize
            width: height
            anchors.centerIn: container

            onProgressChanged: {
                if (progress >= 1.0)
                    _spinner.visible = false
            }

            Behavior on height {
                id: _heightTransition
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InCubic
                }
            }

            Rectangle {
                id: imageShade
                anchors.fill: parent
                color: "#000000"
                opacity: .2
            }

            ColorOverlay {
                id: colorOverlay
                anchors.fill: channelImage
                source: channelImage

                function setOverlay(isOnline){
                    if (isOnline){
                        colorOverlay.color = "transparent"
                    } else {
                        colorOverlay.color = Styles.grayScaleColor
                    }
                }
            }
        }

        Rectangle {
            id: infoRect
            color: Styles.shadeColor
            opacity: .66
            height: Math.floor(parent.height * 0.25)

            anchors{
                left: container.left
                right: container.right
                bottom: container.bottom
            }
        }

        Text {
            id: channelTitle
            text: root.title
            color: Styles.textColor
            anchors.fill: infoRect
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            font.pixelSize: Styles.titleFont.pixelSize
            wrapMode: Text.WordWrap
        }
    }


    function setHighlight(isActive){
        imageShade.opacity = isActive ? 0 : .2
        channelImage.height = isActive ? Math.floor(imgSize * 1.16) : imgSize
        root.color = isActive ? Styles.highlight : "transparent"
        root.border.color = isActive ? Styles.border : "transparent"
    }
}
