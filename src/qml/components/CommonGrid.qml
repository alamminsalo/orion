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
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

//ChannelList.qml
GridView {
    id: root

    property bool tooltipEnabled: !isMobile()

    signal updateTriggered()
    signal itemClicked(int index, Item clickedItem)
    signal itemRightClicked(int index, Item clickedItem, real mX, real mY)
    signal itemTooltipHover(Item item, var getPosition)
    signal itemDoubleClicked(int index, Item clickedItem)

    highlightFollowsCurrentItem: false
    cellWidth: width / Math.floor(width / Math.min(190, width / 2)) - 1
    cellHeight: cellWidth
    maximumFlickVelocity: 1200
    currentIndex: -1

    ScrollIndicator.vertical: ScrollIndicator { visible: isMobile() }
    ScrollBar.vertical: ResponsiveScrollBar {
        id: scrollbar
        visible: !isMobile()
        scrollspeed: root.cellHeight / 2
    }

    add: FadeUpTransition {}

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
            if (tooltipEnabled)
                tooltipTimer.restart()
        } else {
            if (g_tooltip)
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
        return Qt.point(mX, mY)
    }

    RotatingButton {
        id: updateIndicator
        property int maxY: 50
        running: y === maxY
        y: Math.min(-parent.contentY - 90, maxY)
        anchors.horizontalCenter: parent.horizontalCenter
        flat: false
        font.pointSize: 25
        text: "\ue5d5"
    }

    onDragEnded: {
        if (updateIndicator.running) {
            updateTriggered()
            console.log("Update trigger")
        }
    }

    onCurrentItemChanged: {
        if (g_tooltip)
            g_tooltip.hide()
        tooltipTimer.stop()
    }

    onContentXChanged: g_tooltip.hide()
    onContentYChanged: g_tooltip.hide()

    MouseArea{
        id: mArea
        anchors.fill: parent
        hoverEnabled: !isMobile()
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPositionChanged: setFocus()

        onHoveredChanged: {
            if (!containsMouse && g_tooltip){
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
                if (g_tooltip && tooltipEnabled && !mArea.pressed){
                    var pt = root.mapFromItem(mArea, mArea.mouseX, mArea.mouseY)
                    pt = root.mapToItem(root, pt.x + root.contentX, pt.y + root.contentY)

                    var item = root.itemAt(pt.x, pt.y)

                    if (item){
                        root.itemTooltipHover(item, getMouseCoords);
                    }
                }
            }
        }

        Timer {
            id: _ct
            property var foo
            interval: 100
            repeat: false
            onTriggered: {
                if (foo && typeof foo === 'function')
                    foo()
                _ct.foo = undefined
            }
        }

        onClicked: {
            var foo;
            var clickedIndex = indexAt(contentX + mouseX, contentY + mouseY);
            root.currentIndex = clickedIndex;
            if (clickedIndex !== -1){
                var clickedItem = itemAt(mouse.x + root.contentX, mouse.y + root.contentY);
                if (mouse.button === Qt.LeftButton) {
                    foo = function(){ itemClicked(clickedIndex, clickedItem) }
                } else if (mouse.button === Qt.RightButton){
                    foo = function(){ itemRightClicked(clickedIndex, clickedItem, mouse.x, mouse.y) }
                }
            }
            _ct.foo = foo
            _ct.restart()
        }

        onDoubleClicked: {
            _ct.stop()
            _ct.foo = undefined

            var clickedIndex = indexAt(mouse.x + root.contentX, mouse.y + root.contentY);
            if (clickedIndex !== -1){
                var clickedItem = itemAt(mouse.x + root.contentX, mouse.y + root.contentY);
                if (mouse.button === Qt.LeftButton) {
                    itemDoubleClicked(clickedIndex, clickedItem)
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
                itemRightClicked(clickedIndex, clickedItem, mouse.x, mouse.y);
            }
        }
    }
}
