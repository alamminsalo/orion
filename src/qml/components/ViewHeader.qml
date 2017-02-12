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
import "../styles.js" as Style

Rectangle {
    property string text
    id: root

    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }
    height: dp(60)
    color: Style.bg

    Item {
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: dp(20)
        }
        width: _text.width

        Text {
            id: _text
            anchors {
                top: parent.top
                left: parent.left
            }

            color: Style.textColor
            font.pixelSize: Style.titleFont.bigger
            text: root.text
            z: root.z + 1
            font.bold: true
            //renderType: Text.NativeRendering
        }
    }
}
