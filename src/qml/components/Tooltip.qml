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
import QtQuick.Window 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import "../util.js" as Util

Window {
    id: root
    flags: Qt.SplashScreen | Qt.NoFocus | Qt.X11BypassWindowManagerHint | Qt.BypassWindowManagerHint

    width: 512
    height: width * 0.5625

    property alias text: _text.text

    Material.theme: Material.Dark

    Rectangle {
        id: rootRect
        anchors.fill: parent
        color: "#000000"

        BusyIndicator {
            id:_spinner
            anchors.fill: parent
            running: img.state === Image.Loading
        }

        Image {
            id: img
            anchors.fill: parent

        }

        //Container for text
        Rectangle {
            id: header
            opacity: 0.666
            gradient: Gradient {
                GradientStop { position: 0.0; color: "black"}
                GradientStop { position: 0.5; color: "transparent" }
            }

            anchors {
                fill: parent
            }
        }

        Label {
            id: _text
            wrapMode: Text.WordWrap
            width: parent.width
            maximumLineCount: 6
            elide: Text.ElideRight
            clip: true
        }
    }

    function displayChannel(channel, mX, mY) {
        text = ""
        text += "<b>" + channel.title + "</b><br/>";
        text += "Playing " + channel.game + "<br/>"
        if (channel.duration)
            text += "Duration " + Util.getTime(channel.duration) + "<br/>"

        if (channel.createdAt)
            text += (new Date(channel.createdAt)).toLocaleString() + "<br/>";

        text += channel.views + " views<br/>"
        img.source = channel.preview

        display(mX, mY)
    }

    function displayGame(game, mX, mY) {
        text = ""

        if (game.title){
            text += game.title
        }

        if (game.viewers){
            text += text.length > 0 ? "<br/>" : ""
            text += game.viewers + " viewers"
        }
        img.source = game.preview

        display(mX, mY)
    }

    function display(mX, mY){
        if (g_contextMenuVisible){
            return
        }

        root.x = mX + 20

        if (root.x + root.width > Screen.width)
            root.x -= root.width + 40

        root.y = mY

        root.show()
    }
}
