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
import "styles.js" as Styles

//ToolBox.qml
Rectangle {
    id: root
    color: Styles.sidebarBg
    property int _width: dp(180)
    property bool hidden: false
    width: !hidden ? _width : 0
    clip: true
    z: 99

    property bool isOpen: true
    property int selectedView: 0

    Behavior on width {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutCubic
        }
    }

    function toggle(){
        root._width = isOpen ? dp(60) : dp(180)
        isOpen = !isOpen
        toggleButton.rotateIcon()
    }

    function setView(index){
        if (index > -1 && index <= ribbonList.count)
            ribbonList.currentIndex = index
    }

    DrawerRibbon {
        id: toggleButton
        iconStr: 'chevron_l'
        onClick: toggle()
        highlightOn: false
    }

    RibbonList {
        id: ribbonList
        height: model.count * dp(60)

        onCurrentIndexChanged: {
            selectedView = currentIndex
        }

        anchors {
            top: toggleButton.bottom
            left: parent.left
            right: parent.right
        }

        model: ListModel {
            id: model
            ListElement {
                label: "Search"
                icon: "search"
            }
            ListElement {
                label: "Featured"
                icon: "featured"
            }
            ListElement {
                label: "Followed"
                icon: "fav"
            }
            ListElement {
                label: "Games"
                icon: "game"
            }
            ListElement {
                label: "VODs"
                icon: "video"
            }
            ListElement {
                label: "Watch"
                icon: "play"
            }
            ListElement {
                label: "Settings"
                icon: "settings"
            }
        }

        delegate: Ribbon {
            text: label
            iconStr: icon
        }
    }

    Rectangle {
        id: border
        width: dp(1)
        color: Styles.border
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
    }
}
