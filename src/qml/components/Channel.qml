import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../styles.js" as Style

//Channel.qml
Rectangle {
    property string title
    property string logo
    property string info
    property string preview
    property bool online
    property int viewers

    property int transitionDuration: 100
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


    //color: "transparent"

    Component.onCompleted: {
        colorOverlay.setOverlay(online)
    }

    onOnlineChanged: {
        colorOverlay.setOverlay(online)
    }

//    Rectangle {
//        id: highlight
//        anchors.fill: parent
//        color: "#ffffff"
//        opacity: 0
//        border.width: 2
//        border.color: "black"

//        NumberAnimation on opacity {
//            id: animateOpacity
//            duration: root.transitionDuration
//            easing.type: Easing.InCubic
//        }

//        function setFocus(bool){
//            animateOpacity.stop()
//            if (bool === true){
//                animateOpacity.to = .1
//                animateOpacity.from = 0
//            } else {
//                animateOpacity.from = .1
//                animateOpacity.to = 0
//            }
//            animateOpacity.start()
//        }
//    }

    Rectangle {
        id: container
        height: imgSize
        width: height
        anchors.centerIn: parent
        clip: true

        function setFocus(bool){

            imageShade.opacity = bool ? 0 : .2

            img_depth_anim.stop();
            img_depth_anim.from = channelImage.height
            img_depth_anim.to = bool ? imgSize * 1.1 : imgSize
            img_depth_anim.start()
        }

        Image {
            id: channelImage
            source: root.logo
            height: imgSize
            width: height
            anchors.centerIn: container

            NumberAnimation on height {
                id: img_depth_anim
                duration: root.transitionDuration
                easing.type: Easing.Linear
            }

            Rectangle {
                id: imageShade
                anchors.fill: parent
                color: "#000000"
                opacity: .2

//                NumberAnimation on opacity {
//                    id: img_shade_animation
//                    duration: 0
//                    easing.type: Easing.InCubic
//                }


            }

            ColorOverlay {
                id: colorOverlay
                anchors.fill: channelImage
                source: channelImage

                function setOverlay(isOnline){
                    if (isOnline){
                        colorOverlay.color = "transparent"
                    } else {
                        colorOverlay.color = Style.grayScaleColor
                    }
                }
            }
        }

        Rectangle {
            id: infoRect
            color: Style.shadeColor
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
            color: Style.textColor
            anchors.fill: infoRect
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            font.pixelSize: Style.titleFont.pixelSize
            wrapMode: Text.WordWrap
        }
    }


    function setHighlight(isActive){
        container.setFocus(isActive)
        root.color = isActive ? Style.twitch.highlight : "transparent"
        root.border.color = isActive ? Style.twitch.border : "transparent"
    }
}
