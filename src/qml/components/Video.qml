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

Channel {
    property string _id
    property int views
    property int duration
    property int position
    property string createdAt
    property string seekPreviews
    online: true

    Label {
        id: resumePlaybackIcon
        font.family: "Material Icons"
        text: "\ue923"
        font.pointSize: 20
        anchors {
            top: parent.top
            right: parent.right
            margins: parent.width * 0.2
        }

        Material.foreground: Material.accent
        visible: position > 0
    }
}
