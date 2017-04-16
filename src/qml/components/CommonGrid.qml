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

//ChannelList.qml
GridView {
    property variant selectedItem
    property bool tooltipEnabled: false
    property string title

    signal itemClicked(int index, Item clickedItem)
    signal itemRightClicked(int index, Item clickedItem)
    signal itemTooltipHover(int index, real mX, real mY)

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
            // this is longer than the 800ms QML press and hold time so that a press and hold will take precedence
            interval: 900
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
                        root.itemTooltipHover(index, mX, mY);
                    }
                }
            }
        }

        onClicked: {
            // Note that click/press doesn't necessarily set grid's current item so we shouldn't use currentIndex
            // TODO: rework this if something better than a single-point click solution is available for touchscreens
            var clickedIndex = indexAt(mouse.x + root.contentX, mouse.y + root.contentY);
            if (clickedIndex !== -1){
                var clickedItem = itemAt(mouse.x + root.contentX, mouse.y + root.contentY);
                if (mouse.button === Qt.LeftButton)
                    itemClicked(clickedIndex, clickedItem)
                else if (mouse.button === Qt.RightButton){
                    itemRightClicked(clickedIndex, clickedItem)
                }
            }
        }

        onPressed: {
            //console.log("pressed");
            tooltipTimer.restart();
        }

        onPressAndHold: {
            //console.log("pressed and held");
            var clickedIndex = indexAt(mouse.x + root.contentX, mouse.y + root.contentY);
            if (clickedIndex !== -1){
                var clickedItem = itemAt(mouse.x + root.contentX, mouse.y + root.contentY);
                itemRightClicked(clickedIndex, clickedItem);
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
