import QtQuick 2.5
import "components"
import "styles.js" as Styles

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

Icon {
    id: root
    property bool checked: false
    property bool checkable: false

    signal clicked()

    onCheckedChanged: {
        root.iconColor = checked ? Styles.purple : Styles.iconColor
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onHoveredChanged: {
            if (!root.checked)
                root.iconColor = containsMouse ? Styles.white : Styles.iconColor
        }

        onClicked: {
            root.clicked()

            if (root.checkable){
                root.checked = !root.checked
            }
        }
    }
}
