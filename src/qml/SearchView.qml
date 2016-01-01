import QtQuick 2.0
import "components"
import "styles.js" as Styles

Item {
    width: 100
    height: 62

    anchors.fill: parent

    Rectangle {
        id: searchContainer
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 100
        color: Styles.bg
        z: channels.z + 1

        Rectangle {
            property string text: _input.text

            id: searchBox
            height: 60
            width: _input.width + _spacer.width + _button.width
            color: Styles.sidebarBg
            radius: 5
            anchors.centerIn: parent
            anchors.margins: 10
            border.color: Styles.border
            border.width: 1
            clip: true

            TextInput{
                id: _input
                font.pixelSize: Styles.titleFont.pixelSize
                color: Styles.iconColor
                width: 280
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            Rectangle {
                id: _spacer
                width: 1
                color: Styles.border
                anchors {
                    left: _input.right
                    top: parent.top
                    bottom: parent.bottom
                }
            }

            Icon {
                id: _button
                icon: "search"
                iconSize: 20
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: _spacer.right
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        if (searchBox.text.length > 0)
                            g_cman.searchChannels(searchBox.text, channels.count, 25, true)
                    }

                    onPressedChanged: {
                        parent.iconColor = pressed ? Styles.textColor : Styles.iconColor
                    }
                }
            }
        }
    }

    ChannelGrid {
        id: channels
        tooltipEnabled: true

        anchors {
            top: searchContainer.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        model: g_results
        delegate: Channel {
            title: model.name
            logo: model.logo
            info: model.info
            viewers: model.viewers
            preview: model.preview
            online: model.online
            containerSize: favourites.cellHeight
        }
    }
}
