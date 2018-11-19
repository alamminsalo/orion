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
import QtQuick.Window 2.4
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtGraphicalEffects 1.0

import "../util.js" as Util

Item {
    id: root

    width: 256 * Screen.devicePixelRatio
    height: width * (img.implicitHeight || 72) / (img.implicitWidth || 120)

    property alias text: _text.text

    property point position: Qt.point(0, 0)
    property real mouseX: position.x
    property real mouseY: position.y
    property Item target


    function show() {
        tooltip.visible = true
    }

    function hide() {
        tooltip.visible = false
    }

    onMouseXChanged: {
        var px = Util.globalPosition(target).x;
        if (px > position.x || position.x >= px + target.width) {
            position = position
            hide()
        }
    }

    onMouseYChanged: {
        var py = Util.globalPosition(target).y;
        if (py > position.y || position.y >= py + target.height) {
            position = position
            hide()
        }
    }

    ToolTip {
        z: 1
        id: tooltip
        width: root.width
        height: root.height
        Material.theme: Material.Dark
        modal: false
        dim: false

        Component.onCompleted: if (Object.hasOwnProperty("enabled")) enabled = false

        property point base: Util.globalPosition(root, 0, 0)
        x: mouseX - base.x + 5
        y: mouseY - base.y + 5

        enter: Transition { }
        exit: Transition { }
        background: Item { }

        Rectangle {
            id: rootRect
            anchors.fill: parent
            color: "#000000"
            clip: true

            BusyIndicator {
                id:_spinner
                visible: running
                hoverEnabled: false
                anchors.fill: parent
                running: img.state === Image.Loading
            }

            Image {
                id: img
                anchors.fill: parent
            }

            Label {
                z: 1
                padding: 5
                font.pointSize: 12
                id: _text
                wrapMode: Text.Wrap
                width: parent.width
                maximumLineCount: 6
                elide: Text.ElideRight
                background: RectangularGlow {
                    width: _text.contentWidth + _text.padding * 2
                    height: _text.contentHeight + _text.padding * 2
                    opacity: 0.666
                    color: "black"
                    glowRadius: 25
                    spread: 0
                    cornerRadius: _text.padding
                }
            }
        }
    }

    function displayChannel(channel, getPosition) {
        text = ""
        text += "<b>" + channel.title + "</b><br/>";
        text += channel.game + "<br/>"
        text += channel.viewers + " viewers"
        img.source = channel.preview

        display(channel, getPosition)
    }

    function displayVod(channel, getPosition) {
        text = ""
        text += "<b>" + channel.title + "</b><br/>";
        text += channel.game + "<br/>"
        if (channel.duration)
            text += "Duration " + Util.getTime(channel.duration) + "<br/>"

        if (channel.createdAt)
            text += (new Date(channel.createdAt)).toLocaleString() + "<br/>";

        text += channel.views + " views"
        img.source = channel.preview

        display(channel, getPosition)
    }

    function displayGame(game, getPosition) {
        text = ""

        if (game.title){
            text += "<b>" + game.title + "</b>"
        }

        if (game.viewers){
            text += text.length > 0 ? "<br/>" : ""
            text += game.viewers + " viewers"
        }
        img.source = game.preview

        display(game, getPosition)
    }

    function display(item, getPosition){
        if (g_contextMenuVisible){
            return
        }
        root.target = item
        root.position = Qt.binding(getPosition);
        root.show()
    }
}
