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
import "../styles.js" as Styles
import QtQuick.Controls.Material 2.1

Button {
    id: root
    height: dp(60)
    anchors {
        left: parent.left
        right: parent.right
    }
    color: "transparent"

    function setFocus(isActive){
        border.width = isActive ? (g_toolBox.isOpen ? dp(14) : dp(60)) : 0
        color = isActive ? Styles.ribbonSelected : "transparent"
        textLabel.color = isActive ? Styles.textColor : Styles.iconColor
        isSelected = isActive
        iconLabel.anchors.centerIn = g_toolBox.isOpen ? null : root
        iconLabel.iconColor = isActive ? Styles.iconHighlight : Styles.iconColor
    }

    Connections {
        target: g_toolBox
        onIsOpenChanged: {
            setFocus(isSelected)
        }
    }

    function setHighlight(isActive){
        if (highlightOn){
            if (!isSelected)
                color = isActive ? Styles.ribbonHighlight : "transparent"
        }
        if (!isSelected){
            iconLabel.iconColor = isActive ? Styles.iconHighlight : Styles.iconColor
            textLabel.color = isActive ? Styles.textColor : Styles.iconColor
        }
    }

    Rectangle {
        id: border
        color: Styles.purple
        width: 0
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        Behavior on width {
            NumberAnimation {
                duration: 150
                easing.type: Easing.OutCubic
            }
        }
    }

    Text {
        id: textLabel
        //visible: g_toolBox.isOpen
        anchors.centerIn: parent
        text: root.text
        font.bold: true
        color: Styles.iconColor
        font.pixelSize: Styles.titleFont.regular
    }

    Icon {
        id: iconLabel
        rotation: iconRotated ? -180 : 0
        icon: iconStr
        iconSize: root.iconSize
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
    }
}
