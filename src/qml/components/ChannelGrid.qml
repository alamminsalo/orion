import QtQuick 2.0

//ChannelList.qml
GridView{
    property variant selectedItem
    property int cellSize: 200
    property bool tooltipEnabled: false

    signal itemClicked(int index)
    signal itemRightClicked(int index)

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

    function getMouseCoords(){
        var mX = mArea.mouseX
        var mY = mArea.mouseY
        var p = mArea.parent

        //Traverse dom tree to root, adding x,y-values from objects
        while (p){
            mX += p.x
            mY += p.y
            p = p.parent
        }
        return {x: mX, y: mY}
    }

    onContentYChanged: setFocus()
    onContentXChanged: setFocus()

    MouseArea{
        id: mArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton

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

                    var mouseCoords = getMouseCoords()
                    var mX = mouseCoords.x
                    var mY = mouseCoords.y

                    var index = root.indexAt(mX + root.contentX, mY + root.contentY)

                    if (mArea.containsMouse && index && selectedItem && selectedItem.online){

                        g_tooltip.text = ""//(selectedItem.title ? "<p><b>"+selectedItem.title+"</b></p>" : "")
                                + (selectedItem.game ? "<p>Playing " + selectedItem.game + "</p>" : "")
                                + (selectedItem.viewers ? "<p>" + selectedItem.viewers + " viewers</p>" : "")
                                + (selectedItem.info ? "<p>" + selectedItem.info + "</p>" : "")

                        g_tooltip.img = selectedItem.preview
                        g_tooltip.display(g_rootWindow.x + mX, g_rootWindow.y + mY)
                    }
                }
            }
        }

        onClicked: {
            if (currentItem){
                if (mouse.button === Qt.LeftButton)
                    itemClicked(currentIndex)
                else if (mouse.button === Qt.RightButton){
                    itemRightClicked(currentIndex)
                }
            }
        }
    }

    onCurrentItemChanged: {
        if (selectedItem && typeof selectedItem.setHighlight === 'function')
            selectedItem.setHighlight(false)

        selectedItem = currentItem

        if (selectedItem && typeof selectedItem.setHighlight === 'function'){
            selectedItem.setHighlight(true)
        }
    }
}
