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

Item {
    property int itemCount: 0

    anchors.fill: parent

    function focusInput(){
        _input.forceActiveFocus()
    }

    function search(str, offset, limit, clear){
        str = str || _input.text

        if (str.length === 0){
            return
        }

        offset = offset || 0
        limit = limit || 25

        if (typeof clear === 'undefined'){
            clear = true
        }

        if (str.length > 0){
            _input.text = str
            g_cman.searchChannels(str, offset, limit, clear)
            if (clear){
                itemCount = limit
            }
        }

        _label.visible = false
    }

    onVisibleChanged: {
        if (visible) {
            channels.positionViewAtBeginning()
            channels.checkScrolled()
        }
    }

    Connections {
        target: g_cman
        onResultsUpdated: {
            _spinner.visible = false
            _button.visible = true
            channels.checkScrolled()
        }

        onSearchingStarted: {
            _spinner.visible = true
            _button.visible = false
        }
    }

    ViewHeader {
        text: "Search channels"
        id: searchContainer
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: dp(100)
        color: Styles.bg
        z: channels.z + 1
        clip: true

        Rectangle {
            property string text: _input.text

            id: searchBox
            height: dp(60)
            width: _input.width + _spacer.width + _button.width
            color: Styles.sidebarBg
            radius: 5
            anchors.centerIn: parent
            anchors.margins: dp(10)
            border.color: Styles.border
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
                    color: Styles.iconColor
                    anchors.fill: parent
                    clip:true
                    selectionColor: Styles.purple
                    focus: true
                    selectByMouse: true
                    font.pixelSize: Styles.titleFont.smaller


                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter

                    Keys.onReturnPressed: search()
                }
            }


            Rectangle {
                id: _spacer
                width: dp(1)
                color: Styles.border
                anchors {
                    left: inputArea.right
                    top: parent.top
                    bottom: parent.bottom
                }
            }

            Icon {
                id: _button
                icon: "search"
                iconSize: Styles.iconSize
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: _spacer.right
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        search()
                    }

                    onHoveredChanged: {
                        parent.iconColor = containsMouse ? Styles.iconHighlight : Styles.iconColor
                    }
                }
            }

            SpinnerIcon {
                id: _spinner
                iconSize: Styles.iconSize
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: _spacer.right
                }
                visible: false
            }
        }
    }

    ChannelGrid {
        id: channels
        tooltipEnabled: true

        anchors {
            top: searchContainer.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: dp(10)
        }

        model: g_results
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

        function checkScrolled(){
            if (atYEnd && model.count() === itemCount && itemCount > 0){
                search(_input.text, itemCount, 25, false);
                itemCount += 25
            }
        }

        onAtYEndChanged: {
            if (visible)
                checkScrolled()
        }

        onItemClicked: {
            if (currentItem.online){
                playerView.getStreams(currentItem)
            }
        }

        onItemRightClicked: {
            _menu.item = currentItem

            _menu.items[0].enabled = _menu.item.online

            var item = _menu.items[2]
            item.text = !_menu.item.favourite ? "Follow" : "Unfollow"
            _menu.state = !_menu.item.favourite ? 1 : 2

            _menu.popup()
        }

        ContextMenu {
            id: _menu

            function addRemoveFavourite(){
                if (state === 1){
                    g_cman.addToFavourites(_menu.item._id)
                } else if (state === 2){
                    g_cman.removeFromFavourites(_menu.item._id, _menu.item.title)
                }
            }

            MenuItem {
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
                id: _fav
                onTriggered: {
                    _menu.addRemoveFavourite()
                }
            }
        }

        Text {
            id: _label
            text: "Search for channels"
            font.pixelSize: Styles.titleFont.large
            color: Styles.iconColor
            anchors.centerIn: parent
            ////renderType: Text.NativeRendering
        }
    }
}
