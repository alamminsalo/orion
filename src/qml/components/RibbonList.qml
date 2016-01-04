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
        onMouseXChanged: setHover()
        onMouseYChanged: setHover()
    }
}
