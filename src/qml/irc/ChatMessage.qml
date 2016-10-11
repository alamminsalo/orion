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

import QtQuick 2.0
import "../styles.js" as Styles

Item {
    id: root
    property string user
    property string msg

    anchors {
        left: parent.left
        right: parent.right
    }
    height: _text.contentHeight + width * 0.04

    Component.onCompleted: {
        _text.text = "<b>%1</b>: %2".arg(user).arg(msg)
    }

    Text {
        id: _text
        verticalAlignment: Text.AlignVCenter
        color: Styles.textColor
        font.pixelSize: Styles.titleFont.smaller
        wrapMode: Text.WordWrap
        anchors {
            fill: root
            margins: parent.width * 0.02
        }
    }
}
