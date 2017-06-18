/*
 * Copyright © 2015-2016 Antti Lamminsalo
 *
 * This file is part of Orion.
 *
 * Orion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with Orion.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick.Controls 1.4
import QtQuick 2.5

CommonGrid {
    property bool allFavourites : false

    tooltipEnabled: true

    onItemClicked: {
        if (currentItem.online){
            playerView.getStreams(clickedItem)
        } else {
            playerView.getChat(clickedItem)
        }
    }

    onItemRightClicked: {
        _menu.item = clickedItem

        _fav.text = (!allFavourites && !_menu.item.favourite) ? "Follow" : "Unfollow"

        _menu.open()
    }

    onItemTooltipHover: {
        if (selectedItem.online) {
            g_tooltip.text = ""

            if (selectedItem.game){
                g_tooltip.text += "Playing <b>" + selectedItem.game + "</b>"
            } else if (selectedItem.title){
                g_tooltip.text += selectedItem.title
            }

            if (selectedItem.viewers){
                g_tooltip.text += g_tooltip.text.length > 0 ? "<br/>" : ""
                g_tooltip.text += selectedItem.viewers + " viewers"
            }

            if (selectedItem.info){
                g_tooltip.text += g_tooltip.text.length > 0 ? "<br/>" : ""
                g_tooltip.text += selectedItem.info
            }

            g_tooltip.img = selectedItem.preview
            g_tooltip.display(g_rootWindow.x + mX, g_rootWindow.y + mY)
        }
    }

    ContextMenu {
        id: _menu

        MenuItem {
            text: "Watch live"
            onTriggered: {
                playerView.getStreams(_menu.item)
            }
        }

        MenuItem {
            text: "VODs"
            //text: "Videos;video"
            onTriggered: {
                vodsView.search(_menu.item)
            }
        }

        MenuItem {
            text: "Open chat"
            onTriggered: {
                playerView.getChat(_menu.item);
            }
        }

        MenuItem {
            id: _fav
            onTriggered: {
                if (text === "Follow") {
                    g_cman.addToFavourites(_menu.item._id)
                } else {
                    g_cman.removeFromFavourites(_menu.item._id)
                }
            }
        }
    }

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

}
