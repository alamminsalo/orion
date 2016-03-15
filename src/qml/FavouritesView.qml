import QtQuick.Controls 1.4
import QtQuick 2.0
import "components"

Item{
    anchors.fill: parent

    ViewHeader{
        id: header
        text: "My favourites"
        z: favourites.z + 1
    }

    ChannelGrid {
        id: favourites
        tooltipEnabled: true

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom : parent.bottom
            margins: dp(10)
        }

        model: g_favourites
        delegate: Channel {
            _id: model.id
            name: model.serviceName
            title: model.name
            logo: model.logo
            info: model.info
            viewers: model.viewers
            preview: model.preview
            online: model.online
            game: model.game
        }

        onItemClicked: {
            if (currentItem.online){
                player.getStreams(currentItem)
            }
        }

        onItemRightClicked: {
            _menu.item = currentItem
            _menu.items[0].enabled = _menu.item.online
            _menu.popup()
        }

        ContextMenu {
            id: _menu
            MenuItem {
                //text: "Watch;play"
                text: "Watch live"
                onTriggered: {
                    if (_menu.item.online){
                        player.getStreams(_menu.item)
                    }
                }
            }
            MenuItem {
                //text: "Videos;video"
                text: "Past broadcasts"
                onTriggered: {
                    vods.search(_menu.item)
                }
            }
            MenuItem {
                text: "Remove favourite"
                //text: "Remove;remove"
                onTriggered: {
                    g_cman.removeFromFavourites(_menu.item._id)
                }
            }
        }
    }
}

