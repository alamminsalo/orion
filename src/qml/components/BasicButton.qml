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

Rectangle {
    id: root
    property string text
    property string bgColor: Styles.grayButtonColor
    property string borderColor: "#ffffff"

    width: content.contentWidth + dp(10)
    height: content.contentHeight + dp(10)

    signal clicked()

    color: bgColor

    border {
        width: dp(1)
        color: borderColor
    }

    Text {
        id: content
        color: Styles.textColor
        font.pixelSize: Styles.titleFont.bigger
        text: root.text
        anchors {
            centerIn: parent
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    MouseArea {
        anchors.fill: parent

        hoverEnabled: true

        onHoveredChanged: {
            if (containsMouse)
                root.bgColor = Styles.purple
            else
                root.bgColor = Styles.grayButtonColor
        }

        onClicked: root.clicked()
    }
}
