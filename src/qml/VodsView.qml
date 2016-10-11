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
import "components"

Item{
    anchors.fill: parent
    property variant selectedChannel
    property int itemCount: 0

    function search(channel){

        if (!channel || typeof channel == "undefined")
            return

        selectedChannel = {
            "_id": channel._id,
            "name": channel.name,
            "game": channel.game,
            "title": channel.title,
            "online": channel.online,
            "favourite": channel.favourite || g_cman.containsFavourite(channel._id),
            "viewers": channel.viewers,
            "logo": channel.logo,
            "preview": channel.preview,
        }

        header.text = "Videos for " + selectedChannel.title;

        g_vodmgr.search(selectedChannel.name, 0, 35)

        itemCount = 35

        requestSelectionChange(4)
    }

    ViewHeader{
        id: header
        text: "Channel vods"
        z: vods.z + 1
    }

    onVisibleChanged: {
        if (visible) {
            vods.positionViewAtBeginning()
            vods.checkScroll()
        }
    }

    VodGrid {
        id: vods
        tooltipEnabled: true

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom : parent.bottom
            margins: dp(10)
        }

        model: vodsModel
        delegate: Video {
            _id: model.id
            title: model.title
            views: model.views
            preview: model.preview
            duration: model.duration
            game: model.game
        }

        onItemClicked: {
            playerView.getStreams(selectedChannel, selectedItem)
        }

        onItemRightClicked: {
            _menu.item = selectedItem
            _menu.popup()
        }

        ContextMenu {
            id: _menu
            MenuItem {
                text: "Watch video"
                //text: "Watch;play"
                onTriggered: {
                    playerView.getStreams(selectedChannel, _menu.item)
                }
            }
        }

        onAtYEndChanged: checkScroll()

        function checkScroll(){
            if (atYEnd && model.count() === itemCount && itemCount > 0){
                g_vodmgr.search(selectedChannel.name, itemCount, 25)
                itemCount += 25
            }
        }
    }
}
