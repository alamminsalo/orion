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
            playerView.getStreams(currentItem)
        } else {
            playerView.getChat(currentItem)
        }
    }

    onItemRightClicked: {
        _menu.item = currentItem

        _watchLive.enabled = _menu.item.online

        _fav.text = (!allFavourites && !_menu.item.favourite) ? "Follow" : "Unfollow"
        _menu.state = (!allFavourites && !_menu.item.favourite) ? 1 : 2

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
            id: _watchLive
            text: "Watch live"
            //text: "Watch;play"
            onTriggered: {
                if (_menu.item.online){
                    playerView.getStreams(_menu.item)
                }
            }
        }

        MenuItem {
            text: "Past broadcasts"
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
                _menu.addRemoveFavourite()
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
