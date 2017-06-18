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

//Channel.qml
Pane {
    property int _id
    property string name
    property string title
    property string logo
    property string info
    property string preview
    property bool online
    property bool favourite: false
    property int viewers
    property string game
    property int imgSize: 148
    property int containerSize: 180

    id: root

    width: containerSize
    height: width

    antialiasing: false
    Material.elevation: 0

    Behavior on Material.elevation {
        NumberAnimation {
            duration: 200
        }
    }

    Component.onCompleted: {
        imageShade.refresh()
    }

    onOnlineChanged: {
        imageShade.refresh()
    }

    Rectangle {
        id: container
        height: imgSize
        width: height
        anchors.centerIn: parent
        clip: true
        color: "#000000"

        BusyIndicator {
            id:_spinner
            anchors.centerIn: parent
            anchors.verticalCenterOffset: -infoRect.height / 2
            running: channelImage.progress < 1
        }

        Image {
            id: channelImage
            source: root.logo
            fillMode: Image.PreserveAspectFit
            width: imgSize
            anchors.centerIn: container

            Component.onCompleted: {
                if (root.scaleImage){
                    width = height
                }
            }

            Rectangle {
                id: imageShade
                anchors.fill: parent
                color: "#000000"
                opacity: 0

                function refresh(){
                    opacity = root.online ? 0 : .8
                }
            }
        }

        Label {
            id: favIcon
            text: "\ue87d"
            font.family: "Material Icons"
            opacity: favourite ? 1 : 0
            color: Material.color(Material.accent)
            font.pointSize: 14
            anchors {
                top: container.top
                right: container.right
                margins: 10
            }

            Behavior on opacity{
                NumberAnimation{
                    duration: 200
                    easing.type: Easing.OutCubic
                }
            }
        }

        Pane {
            id: infoRect
            opacity: .85
            height: parent.height * 0.25

            anchors {
                left: container.left
                right: container.right
                bottom: container.bottom
            }

            Label {
                id: channelTitle
                text: root.title
                elide: Text.ElideRight
                //color: online ? Styles.textColor : Styles.iconColor
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                font.bold: true
            }
        }
    }


    function setHighlight(isActive){
        //imageShade.visible = !isActive && !root.online
        //channelImage.width = isActive ? Math.floor(imgSize * 1.2) : imgSize
        root.Material.elevation = isActive ? 10 : 0
        //root.color = isActive ? Styles.highlight : "transparent"
        //root.border.color = isActive ? Styles.border : "transparent"
    }
}
