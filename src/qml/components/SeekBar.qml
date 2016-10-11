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

import "../styles.js" as Styles
import "../util.js" as Util

Item {
    property int duration
    property int position

    id: root

    signal userChangedPosition(int position)

    function setPosition(position, duration){

        //if (root.duration != duration)
            root.duration = duration

        //if (root.position != position)
            root.position = position


        var fraction = position / duration
        fillBar.width = Math.floor(fraction * seekBar.width)

        time.updateTime()
    }

    onDurationChanged: {
        time.duration = Util.getTime(duration)
    }

    onPositionChanged: {
        time.position = Util.getTime(position)
    }

    Rectangle {
        id: seekBar
        color: Styles.seekBar

        anchors {
            left: parent.left
            right: parent.right
            leftMargin: dp(10)
            rightMargin: dp(10)

            verticalCenter: parent.verticalCenter
        }

        height: dp(6)

        Rectangle {
            color: "white"
            id: fillBar

            width: 0
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
        }
    }

    MouseArea {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: seekBar.left
            right: seekBar.right
            topMargin: dp(10)
            bottomMargin: dp(10)
        }

        propagateComposedEvents: false
        onClicked: {
            userChangedPosition((mouseX / seekBar.width) * duration)
        }
    }

    Item {

        property string duration
        property string position

        id: time
        anchors {
            top: parent.top
            topMargin: dp(6)
            horizontalCenter: parent.horizontalCenter
        }

        height: _time.contentHeight
        width: _time.contentWidth

        function updateTime() {
            _time.text = position + "/" + duration
        }

        Text {
            id: _time
            text: "hh:mm:ss / hh:mm:ss"
            color: Styles.iconColor
            font.bold: true
            font.pixelSize: Styles.titleFont.extrasmall
            wrapMode: Text.WordWrap
            //renderType: Text.NativeRendering
        }
    }
}
