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

ListView {
    id: rList

    property variant selection
    property variant earlierItem

    onCurrentItemChanged: {
        if (selection && typeof selection.setFocus === 'function'){
            selection.setFocus(false)
        }

        if (currentItem && typeof currentItem.setFocus === 'function'){
            selection = currentItem
            selection.setFocus(true)
        }
    }

    MouseArea {
        id: mArea
        anchors.fill: parent
        hoverEnabled: true

        onClicked: {
            var index = parent.indexAt(mouseX, mouseY)
            if (index > -1)
                rList.currentIndex = index
        }

        function setHover(){
            if (containsMouse){
                var item = itemAt(mouseX, mouseY)

                if (earlierItem)
                    earlierItem.setHighlight(false)

                earlierItem = item

                if (earlierItem)
                    earlierItem.setHighlight(true)
            } else if (earlierItem)
                earlierItem.setHighlight(false)
        }

        onHoveredChanged: setHover()
        onPositionChanged: setHover()
    }
}
