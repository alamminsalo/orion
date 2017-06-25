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
Item {
    property int _id
    property string name
    property string title
    property string logo
    property string info
    property string preview
    property bool online
    property bool favourite
    property int viewers
    property string game
    property int containerSize: width - 10
    property int imageSize: containerSize - 20
    property bool showFavIcon: true

    id: root
    implicitWidth: 180
    height: width

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

    Pane {
        id: innerPane
        Material.elevation: 0
        height: containerSize
        width: height
        anchors.centerIn: parent
        padding: 0
        Material.theme: rootWindow.Material.theme

        Rectangle {
            id: container
            clip: true
            color: "black"
            anchors.fill: parent
            anchors.margins: 10

            BusyIndicator {
                id:_spinner
                anchors.centerIn: parent
                anchors.verticalCenterOffset: -title.height / 2
                running: image.progress < 1
            }

            Image {
                id: image
                source: root.logo
                property bool isLandscape: sourceSize.width >= sourceSize.height

                fillMode: isLandscape ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                width: isLandscape ? undefined : imageSize
                height: isLandscape ? imageSize : undefined
                anchors.centerIn: parent

                Behavior on height {
                    enabled: image.isLandscape
                    NumberAnimation {
                        duration: 100
                    }
                }
                Behavior on width {
                    enabled: !image.isLandscape
                    NumberAnimation {
                        duration: 100
                    }
                }
            }

            Rectangle {
                id: imageShade
                anchors.fill: image
                color: "#000000"
                opacity: 0

                function refresh(){
                    opacity = root.online ? 0 : .8
                }
            }

            Label {
                id: favIcon
                text: "\ue87d"
                font.family: "Material Icons"
                opacity: favourite ? 1 : 0
                Material.foreground: Material.accent
                font.pointSize: 14
                visible: showFavIcon
                anchors {
                    top: parent.top
                    right: parent.right
                    margins: 10
                }

                Behavior on opacity{
                    NumberAnimation{
                        duration: 200
                        easing.type: Easing.OutCubic
                    }
                }
            }

            Rectangle {
                id: titleBg
                color: Material.background
                opacity: 0.8
                anchors {
                    bottom: parent.bottom
                    left: image.left
                    right: image.right
                }
                height: 33
            }

            Label {
                id: title
                text: root.title
                elide: Text.ElideRight
                anchors {
                    top: titleBg.top
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
                fontSizeMode: Text.Fit
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                maximumLineCount: 2
            }
        }
    }

    function setHighlight(isActive){
        if (image.isLandscape)
            image.height = isActive ? containerSize : imageSize
        else
            image.width = isActive ? containerSize : imageSize

        innerPane.Material.elevation = isActive ? 12 : 0
    }
}
