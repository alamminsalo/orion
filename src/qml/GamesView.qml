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
import "styles.js" as Style

Item {
    id: root
    anchors.fill: parent
    property int gamesCount: 0
    property bool checked: false
    property string query: ""

    function search(clear) {
        if (clear) {
            gamesCount = 0
        }
        g_cman.searchGames(query, gamesCount, 100);
        gamesCount += 100
    }

    function searchChannels(item) {
        searchView.search(":game " + item.title)
        requestSelectionChange(0)
    }

    Connections {
        target: g_cman

        onGamesSearchStarted: {
            _spinner.visible = true
        }

        onGamesUpdated: {
            games.checkScroll()
            _spinner.visible = false
        }
    }

    ViewHeader {
        text: "Games"
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: dp(100)
        color: Style.bg
        z: games.z + 1
        clip: true

        Rectangle {
            property string text: _input.text

            id: searchBox
            height: dp(60)
            width: _input.width + _spacer.width + _button.width
            color: Style.sidebarBg
            radius: 5
            anchors.centerIn: parent
            anchors.margins: dp(10)
            border.color: Style.border
            border.width: dp(1)
            clip: true

            MouseArea {
                id: inputArea
                cursorShape: Qt.IBeamCursor
                width: dp(300)
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }

                TextInput{
                    id: _input
                    color: Style.iconColor
                    anchors.fill: parent
                    clip:true
                    selectionColor: Style.purple
                    focus: true
                    selectByMouse: true
                    font.pixelSize: Style.titleFont.smaller


                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter

                    Keys.onReturnPressed: {
                        query = searchBox.text
                        search(true)
                    }
                }
            }


            Rectangle {
                id: _spacer
                width: dp(1)
                color: Style.border
                anchors {
                    left: inputArea.right
                    top: parent.top
                    bottom: parent.bottom
                }
            }

            Icon {
                id: _button
                icon: "search"
                iconSize: Style.iconSize
                visible: !_spinner.visible
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: _spacer.right
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        query = searchBox.text
                        search(true)
                    }

                    onHoveredChanged: {
                        parent.iconColor = containsMouse ? Style.iconHighlight : Style.iconColor
                    }
                }
            }

            SpinnerIcon {
                id: _spinner
                iconSize: Style.iconSize
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: _spacer.right
                }
                visible: false
            }
        }
    }

    onVisibleChanged: {
        if (visible && !checked){
            if (query === "")
                search()
            checked = true
            timer.start()
        }

        if (visible) {
            _input.focus = true
            games.positionViewAtBeginning()
        }
    }

    ChannelGrid {
        id: games
        tooltipEnabled: true

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: dp(10)
        }

        model: g_games
        delegate: Channel {
            title: model.name
            logo: model.logo
            preview: model.preview
            viewers: model.viewers
            online: true
        }

        function checkScroll(){
            if (atYEnd && model.count() === gamesCount && gamesCount > 0){
                search()
            }
        }

        onItemClicked: {
            root.searchChannels(currentItem)
        }

        onItemRightClicked: {
            _menu.item = currentItem
            _menu.popup()
        }

        onAtYEndChanged: checkScroll()

        Connections {
            target: g_cman

        }

        ContextMenu {
            id: _menu
            MenuItem {
                text: "Search streams"
                //text: "Find streams;search"
                onTriggered: {
                    root.searchChannels(_menu.item)
                }
            }
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
}
