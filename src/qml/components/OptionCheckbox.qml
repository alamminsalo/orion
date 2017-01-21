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

Item {
    property string text
    property bool checked: true
    property alias fontSize: text.font.pixelSize

    signal clicked()

    id: root
    height: dp(60)
    width: text.contentWidth + checkbox.width + dp(20) + dp(10)

    Text {
        id: text
        color: Styles.textColor
        font.pixelSize: Styles.titleFont.bigger
        text: root.text
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: dp(10)
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        //renderType: Text.NativeRendering
    }

    Rectangle {
        id: checkbox
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            margins: dp(10)
        }

        width: dp(36)
        radius: dp(2)
        color: Styles.sidebarBg

        MouseArea{
            anchors.fill: parent

            onClicked: {
                root.checked = !root.checked
                root.clicked()
            }

            Icon {
                icon: "check"
                anchors.fill: parent
                visible: root.checked
            }
        }
    }
}
