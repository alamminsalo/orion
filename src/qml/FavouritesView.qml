import QtQuick.Controls 1.4
import QtQuick 2.0
import "components"

ChannelGrid {
    id: favourites
    tooltipEnabled: true

    anchors {
        fill: parent
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
            player.play(currentItem.name)
            requestSelectionChange(4)
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
            text: "Watch;play"
            onTriggered: {
                if (_menu.item.online){
                    player.play(_menu.item.name)
                    requestSelectionChange(4)
                }
            }
        }
        MenuItem {
            text: "Info;info"
        }
        MenuItem {
            text: "Remove;remove"
            onTriggered: {
                g_cman.removeFromFavourites(_menu.item._id)
            }
        }
    }
}
