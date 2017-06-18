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

Window {
    id: root
    flags: Qt.SplashScreen | Qt.NoFocus | Qt.X11BypassWindowManagerHint | Qt.BypassWindowManagerHint

    height: 320
    width: 512

    property string text
    property string img

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
            source: root.img
            anchors.fill: parent
        }

        //Container for text
        Pane {
            id: header
            opacity: 0.88

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            Label {
                id: text
                text: root.text
                wrapMode: Text.WordWrap
            }
        }
    }

    function display(mX, mY){

        if (g_contextMenuVisible){
            return
        }

        root.x = mX + dp(20)

        if (root.x + root.width > Screen.width)
            root.x -= root.width + dp(40)

        root.y = mY

        root.show()
    }
}
