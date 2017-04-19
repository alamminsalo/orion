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

Rectangle {
    property string _id
    property string title
    property string preview
    property int views
    property int duration
    property string game
    property string createdAt

    property int imgSize: dp(148)
    property int containerSize: dp(200)

    id: root

    width: containerSize
    height: width
    border.color: "transparent"
    border.width: dp(1)
    antialiasing: false
    clip:true
    color: "transparent"
    radius: dp(5)

    Rectangle {
        id: container
        height: imgSize
        width: height
        anchors.centerIn: parent
        clip: true
        color: "#000000"

        SpinnerIcon {
            id:_spinner
            iconSize: 30
            anchors.fill: parent
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

            onProgressChanged: {
                if (progress >= 1.0)
                    _spinner.visible = false
            }

            Behavior on width {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InCubic
                }
            }
        }

        Rectangle {
            id: infoRect
            color: Styles.shadeColor
            opacity: .85
            height: Math.floor(parent.height * 0.25)

            anchors {
                left: container.left
                right: container.right
                bottom: container.bottom
            }
        }

        Text {
            id: channelTitle
            text: root.title
            elide: Text.ElideRight
            color: Styles.textColor
            anchors.fill: infoRect
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Styles.titleFont.smaller
            wrapMode: Text.WordWrap
            //renderType: Text.NativeRendering
        }
    }


    function setHighlight(isActive){
        channelImage.width = isActive ? Math.floor(imgSize * 1.2) : imgSize
        root.color = isActive ? Styles.highlight : "transparent"
        root.border.color = isActive ? Styles.border : "transparent"
    }
}
