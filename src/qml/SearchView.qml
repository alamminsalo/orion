import QtQuick 2.0
import "components"
import "styles.js" as Styles

Item {
    width: 100
    height: 62

    anchors.fill: parent

    Item {
        id: searchContainer
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 100

        Item {
            anchors {
                centerIn: parent
            }

            height: 70
            width: 300


            Rectangle {
                id: searchBox
                property string text: _input.text
                color: Styles.twitch.sidebarBg
                radius: 5
                anchors.fill: parent
                anchors.margins: 10
                border.color: Styles.twitch.border
                border.width: 1

                TextInput{
                    font.pixelSize: Styles.titleFont.pixelSize
                    color: Styles.iconColor
                    id: _input
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

//        Rectangle {
//            anchors {
//                left: parent.left
//                right: parent.right
//                bottom: parent.bottom
//            }
//            height: 1
//            color: Styles.twitch.border
//        }
    }

    ChannelGrid {
        id: channels
    }
}
