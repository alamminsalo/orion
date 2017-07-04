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

import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.1
import "components"

import app.orion 1.0

Page {
    id: root
    property int itemCount: 0
    property bool checked: false
    property string query: searchBar.text

    header: SearchBar {
        id: searchBar
        input.placeholderText: "Search for channels"
        onSubmit: search()
    }

    function search(str, offset, limit, clear){
        if (str)
            searchBar.text = str

        str = str || searchBar.text
        console.log("Searching channels: " + str)

        offset = offset || 0
        limit = limit || 25

        if (typeof clear === 'undefined'){
            clear = true
        }

        ChannelManager.searchChannels(str, offset, limit, clear)
        if (clear){
            itemCount = limit
        }
    }

    onVisibleChanged: {
        if (visible) {
            channels.positionViewAtBeginning()
            channels.checkScrolled()

            if (query.length <= 0 && !checked) {
                search()
                checked = true
                timer.restart()
            }
        }
    }

    Connections {
        target: ChannelManager
        onResultsUpdated: {
            channels.adjustItemCount(numAdded);
            channels.checkScrolled(total)
            busyIndicator.running = false
        }

        onSearchingStarted: {
            busyIndicator.running = true
        }
    }

    ChannelGrid {
        id: channels

        anchors.fill: parent

        model: g_results

        function checkScrolled(total){
            if (total != null && itemCount >= total) {
                return;
            }
            if (atYEnd && model.count() === itemCount && itemCount > 0){
                search(query, itemCount, 25, false);
                itemCount += 25
            }
        }

        function adjustItemCount(numAdded) {
            // we pre-increased itemCount by the expected size of the result but it's possible that
            // some results were not included due to duplicates filtering
            if (numAdded !== 25) {
                itemCount += numAdded - 25;
            }
        }

        onAtYEndChanged: {
            if (visible)
                checkScrolled()
        }
    }

    BusyIndicator {
        id: busyIndicator
        running: false
        anchors.centerIn: parent
    }

    Timer {
        id: timer
        interval: 30000
        running: false
        repeat: false
        onTriggered: {
            root.checked = false
        }
    }
}
