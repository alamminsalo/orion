import QtQuick 2.0

//ChannelList.qml
GridView{
    property variant selectedItem
    property int cellSize: 200
    property bool tooltipEnabled: false
    signal itemClicked(int index)

    id: root

    cellHeight: cellSize
    maximumFlickVelocity: 800
    cellWidth: cellHeight

    function setFocus(){
        g_tooltip.hide()

        if (mArea.containsMouse) {
            root.currentIndex = indexAt(contentX + mArea.mouseX, contentY + mArea.mouseY)
            if (tooltipEnabled)
                tooltipTimer.restart()
        }
    }

    onContentYChanged: setFocus()
    onContentXChanged: setFocus()

    MouseArea{
        id: mArea
        anchors.fill: parent
        hoverEnabled: true

        onMouseXChanged: setFocus()
        onMouseYChanged: setFocus()

        Timer {
            id: tooltipTimer
            interval: 800
            running: false
            repeat: false
            onTriggered: {
                if (tooltipEnabled){
                    g_tooltip.hide()

                    var mX = mArea.mouseX
                    var mY = mArea.mouseY
                    var p = mArea.parent

                    //Traverse dom tree to root, adding x,y-values from objects
                    while (p){
                        mX += p.x
                        mY += p.y
                        p = p.parent
                    }

                    if (mArea.containsMouse && indexAt(contentX + mX, contentY + mY) > -1 && selectedItem && selectedItem.online)
                        g_tooltip.displayChannel(g_rootWindow.x + mX,
                                                 g_rootWindow.y + mY,
                                                 selectedItem)
                }
            }
        }

        onClicked: {
            if (currentItem){
                itemClicked(currentIndex)
            }
        }
    }

    onCurrentItemChanged: {
        if (selectedItem && typeof selectedItem.setHighlight === 'function')
            selectedItem.setHighlight(false)

        if (currentItem && typeof currentItem.setHighlight === 'function'){
            selectedItem = currentItem
            selectedItem.setHighlight(true)
        }
    }
}
