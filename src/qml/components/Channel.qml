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
import QtQuick.Layouts 1.3
import "../util.js" as Util

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

    Material.foreground: rootWindow.Material.foreground

    Behavior on Material.elevation {
        NumberAnimation {
            duration: 200
        }
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
                visible: running
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

            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 5
                spacing: 0
                Label {
                    visible: online && viewers > 0
                    id: liveBanner
                    Layout.minimumHeight: parent.height
                    Material.foreground: "red"
                    font.pointSize: 8
                    text: "\ue061"
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
                    font.family: "Material Icons"
                    padding: 5
                    rightPadding: 0
                    background: Rectangle {
                        color: Material.background
                        opacity: 0.8
                    }
                }
                Label {
                    visible: online && viewers > 0
                    font.pointSize: 8
                    Layout.minimumHeight: parent.height
                    padding: 5
                    text: viewers
                    verticalAlignment: Text.AlignVCenter
                    background: Rectangle {
                        color: Material.background
                        opacity: 0.8
                    }
                }
                Label {
                    id: favIcon
                    Layout.minimumHeight: parent.height
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    text: "\ue87d"
                    font.family: "Material Icons"
                    padding: 5
                    opacity: favourite ? 1 : 0
                    Material.foreground: Material.accent
                    font.pointSize: 10
                    visible: showFavIcon && favourite

                    Behavior on opacity { PropertyAnimation { } }
                    background: Rectangle {
                        color: Material.background
                        opacity: 0.8
                    }
                }
                Item {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 0
                }
                Item {
                    visible: game
                    Layout.maximumWidth: fullGameLabel.implicitWidth
                    Layout.preferredWidth: fullGameLabel.implicitHeight
                    Layout.minimumWidth: abbrGameLabel.implicitWidth
                    Layout.fillWidth: true
                    Layout.minimumHeight: parent.height
                    Layout.alignment: Qt.AlignRight
                    Label {
                        anchors.right: parent.right
                        visible: parent.width >= implicitWidth
                        id: fullGameLabel
                        text: game
                        font.pointSize: 8
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignRight
                        padding: 5
                        background: Rectangle {
                            color: Material.background
                            opacity: 0.8
                        }
                    }
                    Label {
                        function abbreviate(str) {
                            if (!str) return str;
                            return str.replace(/[',\.]/g, '')
                                .replace(/[A-Z]{4,}/g, function(s) { return s[0]; })
                                .replace(/[a-z][A-Z]/g, function(s) { return s[0] + ' ' + s[1]; })
                                .replace(/\B[a-z]/g, '')
                                .replace(/\s/g, '');
                        }
                        anchors.right: parent.right
                        visible: !fullGameLabel.visible
                        id: abbrGameLabel
                        text: abbreviate(game)
                        font.pointSize: 8
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignRight
                        padding: 5
                        background: Rectangle {
                            color: Material.background
                            opacity: 0.8
                        }
                    }
                }
            }

            Rectangle {
                id: imageShade
                anchors.fill: image
                color: "#000000"
                opacity: root.online ? 0 : .8
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
                font.family: rootWindow.font.family // Somehow doesn't get updated on font change...
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

    property bool isCurrent: GridView.isCurrentItem || false
    onIsCurrentChanged: {
        if (image.isLandscape)
            image.height = isCurrent ? containerSize : imageSize
        else
            image.width = isCurrent ? containerSize : imageSize

        innerPane.Material.elevation = isCurrent ? 12 : 0
    }
}
