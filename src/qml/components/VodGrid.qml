import QtQuick 2.5

import "../util.js" as Util

//ChannelList.qml
GridView {
    property variant selectedItem
    property bool tooltipEnabled: false
    property string title

    signal itemClicked(int index)
    signal itemRightClicked(int index)

    id: root

    highlightFollowsCurrentItem: false
    cellHeight: dp(200)
    maximumFlickVelocity: 800
    cellWidth: cellHeight

    add: Transition {
        NumberAnimation {
            properties: "y"
            from: contentY-200
            duration: 200
            easing.type: Easing.OutCubic
        }
    }

    remove: Transition {
        NumberAnimation {
            property: "opacity"
            to: 0
            duration: 200
            easing.type: Easing.OutCubic
        }
    }

    function setFocus(){

        if (mArea.containsMouse) {
            root.currentIndex = indexAt(contentX + mArea.mouseX, contentY + mArea.mouseY)
            if (tooltipEnabled)
                tooltipTimer.restart()

        } else {
            g_tooltip.hide()
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

    onSelectedItemChanged: {
        if (g_tooltip)
            g_tooltip.hide()
        tooltipTimer.stop()
    }

    MouseArea{
        id: mArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPositionChanged: setFocus()

        onHoveredChanged: {
            if (!containsMouse){
                g_tooltip.hide()
                tooltipTimer.stop()
            }
        }

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

                    if (mArea.containsMouse && selectedItem){

                        g_tooltip.text = ""

                        g_tooltip.text += "<b>" + selectedItem.title + "</b><br/>";

                        g_tooltip.text += "Playing " + selectedItem.game + "<br/>"
                        if (selectedItem.duration)
                            g_tooltip.text += "Duration " + Util.getTime(selectedItem.duration) + "<br/>"

                        g_tooltip.text += selectedItem.views + " views<br/>"

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
