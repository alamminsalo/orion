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
import QtQuick.Controls.Material 2.1

ToolBar {
    id: root
    property int selectedView: 0
    visible : !g_fullscreen

    //Base font color
    Material.foreground: Material.Grey

    function setView(index) {
        selectedView = index
    }

    TabBar {
        id: tab
        anchors.fill: parent
        currentIndex: selectedView

        property bool showIcons: root.width < 700
        font.pointSize: !showIcons ? 13 : 16
        font.family: showIcons ? "Material Icons" : "Noto Sans"

        TabButton {
            text: !tab.showIcons ? "Channels" : "\ue8b6"
            onClicked: selectedView = 0
        }
        TabButton {
            text: !tab.showIcons ? "Followed" : "\ue87d"
            onClicked: selectedView = 1
        }
        TabButton {
            text: !tab.showIcons ? "Games" : "\ue021"
            onClicked: selectedView = 2
        }
        TabButton {
            text: !tab.showIcons ? "VODs" : "\ue63a"
            onClicked: selectedView = 3
        }
        TabButton {
            text: !tab.showIcons ? "Player" : "\ue038"
            onClicked: selectedView = 4
        }
        TabButton {
            text: !tab.showIcons ? "Settings" : "\ue8b8"
            onClicked: selectedView = 5
        }
    }
}
