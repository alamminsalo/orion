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
import QtQuick.Controls.Material 2.1

Pane {
    property string _id
    property string title
    property string preview
    property int views
    property int position
    property int duration
    property string game
    property string createdAt

    property int imgSize: dp(148)
    property int containerSize: 180

    id: root

    Material.elevation : 0
    width: containerSize
    height: width
    antialiasing: false

    Rectangle {
        id: container
        height: imgSize
        width: height
        anchors.centerIn: parent
        color: "#000000"

        BusyIndicator {
            id:_spinner
            anchors.centerIn: parent
            anchors.verticalCenterOffset: -infoRect.height / 2
            running: channelImage.progress < 1
        }

        Image {
            id: channelImage
            source: preview
            fillMode: Image.PreserveAspectFit
            width: imgSize
            anchors.centerIn: container

            Component.onCompleted: {
                if (root.scaleImage){
                    width = height
                }
            }
        }

        Pane {
            id: infoRect
            opacity: .85
            height: Math.floor(parent.height * 0.25)

            anchors {
                left: container.left
                right: container.right
                bottom: container.bottom
            }

            Label {
                id: channelTitle
                text: root.title
                font.bold: true
                elide: Text.ElideRight
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }
    }


    function setHighlight(isActive){
        root.Material.elevation = isActive ? 10 : 0
    }
}
