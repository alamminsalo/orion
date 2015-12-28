import QtQuick 2.0

//ChannelList.qml
GridView{
    property var selectedChannel

    id: root
    focus: true
    cellHeight: 200
    cellWidth: cellHeight

    Component.onCompleted: {

    }

    model: g_cman.channels
    delegate: Channel {
        title: modelData.name
        logo: modelData.logo
        info: modelData.info
        preview: modelData.preview
        online: modelData.online
        containerSize: cellHeight
    }

    MouseArea{
        id: mArea
        anchors.fill: parent
        hoverEnabled: true
        onMouseXChanged:; onMouseYChanged: {
            var mX = mouseX
            var mY = mouseY
            var index = indexAt(mX, mY)
            var rows = Math.floor(root.count / (Math.floor(root.width / root.cellWidth)))

            //console.log("Grid rows: ", rows)

            if (containsMouse) {
                if (index > -1) {
//                    while (index > currentIndex){
//                        moveCurrentIndexRight()
//                    }
//                    while (index < currentIndex){
//                        moveCurrentIndexLeft()
//                    }
                    root.currentIndex = index
                }

            } else {
                g_tooltip.hide()
            }
            tooltipTimer.restart()
        }

        Timer {
            id: tooltipTimer
            interval: 1000
            running: false
            repeat: false
            onTriggered: {
                var mX = mArea.mouseX
                var mY = mArea.mouseY
                //var index = root.indexAt(mX,mY)

                if (mArea.containsMouse && selectedChannel && selectedChannel.online)
                    g_tooltip.displayChannel(g_rootWindow.x + root.parent.x + 10 + parent.mouseX,
                                             g_rootWindow.y + root.parent.y + 10 + parent.mouseY,
                                             selectedChannel)
                else g_tooltip.hide()
            }
        }
    }

    onCurrentItemChanged: {
        if (selectedChannel && typeof selectedChannel.setHighlight === 'function')
            selectedChannel.setHighlight(false)

        if (currentItem && typeof currentItem.setHighlight === 'function'){
            selectedChannel = currentItem
            selectedChannel.setHighlight(true)
        }
    }

}
