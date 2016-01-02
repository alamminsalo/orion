import QtQuick 2.5

ListView {
    id: rList
    height: model.count * 50

    property variant selection

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
    }
}
