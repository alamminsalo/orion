import QtQuick.Controls 1.4
import QtQuick 2.0
import "components"

ChannelGrid {
    id: favourites
    tooltipEnabled: true

    anchors {
        fill: parent
        margins: 10
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

    onItemRightClicked: {
        _menu.item = currentItem
        _menu.popup()
    }

    ContextMenu {
        id: _menu
        MenuItem {
            text: "Watch;play"
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
