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
