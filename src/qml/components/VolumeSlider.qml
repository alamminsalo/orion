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
import QtQuick.Controls 2.1

Item {
    property double value: 100
    property double _value
    property bool open: false
    property bool mute: false

    width: dp(50)
    height: dp(50)

    MouseArea {
        anchors.fill: parent
        onWheel: {
            if (wheel.angleDelta.y > 0)
            {
                value += 10
            }
            else
            {
                value -= 10
            }
        }
    }

    onValueChanged: {
        if (value > 100)
            value = 100;
        else if (value < 0)
            value = 0;
    }

    function refresh(){
        open = iconArea.containsMouse || sliderArea.containsMouse || sliderArea.pressed
    }

    MouseArea {
        id: iconArea
        anchors.fill: parent
        hoverEnabled: true
        onHoveredChanged: {
            //icon.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
            refresh()
        }

        onClicked: {
            mute = !mute
            if (mute){
                _value = value
                value = 0
            } else {
                value = _value
            }
        }
    }

    Icon {
        id: icon
        icon: value > 50 ? "volume" : value > 0 ? "volume-med" : "volume-off"
        anchors {
            centerIn: parent
        }
        //iconSize: Styles.iconSizeBigger
    }

    Rectangle {
        id: slider
        //color: Styles.shadeColor
        height: open ? dp(160) : 0
        clip: true

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.top
        }

        Behavior on height {
            NumberAnimation {
                duration: 100
            }
        }

        MouseArea {
            id: sliderArea

            anchors.fill: parent
            hoverEnabled: true
            onHoveredChanged: {
                refresh()
            }

            onMouseYChanged: {
                if (pressed){
                    var yPos = Math.max(sliderBar.y, Math.min(sliderBar.y + sliderBar.height, mouseY));
                    var _val = ((sliderBar.height - yPos + sliderBar.y)/ sliderBar.height) * 100;
                    value = _val;
                    mute = false;
                }
            }

            onPressedChanged: {
                refresh()
            }
        }


        Rectangle {
            //color: Styles.iconColor
            id: sliderBar
            width: dp(4)

            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
                topMargin: dp(20)
                bottomMargin: dp(20)
            }

            Rectangle {
                id: handle
                color: "white"
                width: dp(17)
                height: dp(6)
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                y: parent.height - ((value / 100) * parent.height) - (height / 2)
            }
        }
    }

}
