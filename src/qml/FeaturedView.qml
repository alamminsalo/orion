import QtQuick 2.0
import QtQuick.Controls 1.4
import "components"

Item {
    anchors.fill: parent

    ViewHeader{
        id: header
        text: "Featured channels"
        z: featured.z + 1
    }

    ChannelGrid {
        property bool checked: false

        id: featured
        tooltipEnabled: true

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom : parent.bottom
            margins: dp(10)
        }

        onVisibleChanged: {
            if (visible && !checked){
                g_cman.getFeatured()
                checked = true
                timer.restart()
            }
        }

        model: g_featured
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
            favourite: model.favourite
        }

        onItemClicked: {
            if (currentItem.online){
                player.getStreams(currentItem)
            }
        }

        onItemRightClicked: {
            _menu.item = currentItem
            _menu.items[0].enabled = _menu.item.online
            var item = _menu.items[2]
            item.text = !_menu.item.favourite ? "Add favourite" : "Remove favourite"
            //item.text = !_menu.item.favourite ? "Add favourite;fav" : "Remove favourite;remove"
            _menu.state = !_menu.item.favourite ? 1 : 2

            _menu.popup()
        }

        ContextMenu {
            id: _menu

            function addRemoveFavourite(){
                if (state === 1){
                    g_cman.addToFavourites(_menu.item._id)
                } else if (state === 2){
                    g_cman.removeFromFavourites(_menu.item._id)
                }
            }

            MenuItem {
                text: "Watch live"
                //text: "Watch;play"
                onTriggered: {
                    if (_menu.item.online){
                        player.getStreams(_menu.item)
                    }
                }
            }

            MenuItem {
                text: "Videos"
                //text: "Videos;video"
                onTriggered: {
                    vods.search(_menu.item)
                }
            }

            MenuItem {
                id: _fav
                onTriggered: {
                    _menu.addRemoveFavourite()
                }
            }


        }

        Timer {
            id: timer
            interval: 30000
            running: false
            repeat: false
            onTriggered: {
                featured.checked = false
            }
        }
    }
}
