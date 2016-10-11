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
import "../styles.js" as Styles

Rectangle {
    property string text
    property string iconStr

    signal buttonPressed()

    id: root
    width: dp(70)
    height: dp(30)
    color: Styles.purple

    Component.onCompleted: {
        if (!label.text){
            iconLabel.anchors.centerIn = root
        }
    }

    Text {
        id: label
        anchors.centerIn: parent
        color: Styles.textColor
        text: root.text
        font.pixelSize: Styles.button.size
        //renderType: Text.NativeRendering
    }

    Rectangle {
        id: hoverRect
        color: "#000000"
        anchors.fill: parent
        opacity: 0
    }

    Icon {
        id: iconLabel
        icon: iconStr
        iconSize: 20
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
    }

    MouseArea {
        id: mArea
        anchors.fill: parent
        hoverEnabled: true

        onHoveredChanged: {
            hoverRect.opacity = containsMouse ? .1 : 0
        }

        onPressedChanged: {
            hoverRect.opacity = pressed ? .2 : containsMouse ? .1 : 0
        }

        onClicked: buttonPressed()
    }
}


