import QtQuick 2.0
import "../styles.js" as Styles

Item {
    property int value: 100
    property int _value
    property bool open: false
    property bool mute: false

    width: dp(50)
    height: dp(50)

    function refresh(){
        open = iconArea.containsMouse || sliderArea.containsMouse || sliderArea.pressed
    }

    MouseArea {
        id: iconArea
        anchors.fill: parent
        hoverEnabled: true
        onHoveredChanged: {
            icon.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
            refresh()
        }

        onClicked: {
            mute = !mute
            if (mute){
                _value = value
                value = 0
            } else {
                value = _value
            }
        }
    }

    Icon {
        id: icon
        icon: value > 50 ? "volume" : value > 0 ? "volume-med" : "volume-off"
        anchors {
            left: parent.left
            leftMargin: dp(10)
            verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        id: slider
        color: Styles.shadeColor
        height: open ? dp(160) : 0
        clip: true

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.top
        }

        Behavior on height {
            NumberAnimation {
                duration: 100
            }
        }

        MouseArea {
            id: sliderArea

            anchors.fill: parent
            hoverEnabled: true
            onHoveredChanged: {
                refresh()
            }

            onMouseYChanged: {
                if (pressed){
                    var yPos = Math.max(sliderBar.y, Math.min(sliderBar.y + sliderBar.height, mouseY))
                    var _val = ((sliderBar.height - yPos + sliderBar.y)/ sliderBar.height) * 100
                    value = Math.max(0, Math.min(100,_val))
                    mute = false
                }
            }

            onPressedChanged: {
                refresh()
            }
        }


        Rectangle {
            color: Styles.iconColor
            id: sliderBar
            width: dp(4)
            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
                topMargin: dp(20)
                bottomMargin: dp(20)
            }

            Rectangle {
                id: handle
                color: "white"
                width: dp(17)
                height: dp(6)
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                y: parent.height - ((value / 100) * parent.height) - (height / 2)
            }
        }
    }

}
