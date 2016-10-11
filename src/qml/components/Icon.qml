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
import "../fonts/fontAwesome.js" as FontAwesome
import "../styles.js" as Styles

Rectangle {
    property string icon
    property int iconSize: Styles.iconSize
    property color iconColor: Styles.iconColor

    id: root
    height: dp(24)
    width: height
    color: "transparent"

    Behavior on rotation {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }

    FontLoader {
        source: "../fonts/fontawesome-webfont.ttf"
    }

    Text {
        anchors.centerIn: parent
        font.pixelSize: iconSize
        color: iconColor
        font.family: "FontAwesome"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        text: FontAwesome.fromText(icon)
    }
}
