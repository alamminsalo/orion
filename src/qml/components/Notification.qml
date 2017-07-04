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
import QtQuick.Window 2.0
import QtQuick.Controls 2.1
import app.orion 1.0

Window {
    property string title
    property string description
    property string imgSrc
    property real destY

    signal clicked()

    //Locations: 0 - topleft, 1 - topright, 2 - bottomleft, 3 - bottomright
    property int location: Settings.alertPosition

    id: root
    flags: Qt.SplashScreen | Qt.NoFocus | Qt.X11BypassWindowManagerHint | Qt.BypassWindowManagerHint | Qt.WindowStaysOnTopHint | Qt.Popup
    width: 400
    height: 120

    function close(){
        //console.log("Destroying notification")
        root.destroy()
    }

    function setPosition(){
        switch (location){
        case 0:
            x =  50
            y = -height
            destY = 50
            break

        case 1:
            x = Screen.width - width - 50
            y = -height
            destY = 50
            break

        case 2:
            x = 50
            y = Screen.height
            destY = Screen.height - height  - 50
            break

        case 3:
            x = Screen.width - width  - 50
            y = Screen.height
            destY = Screen.height - height - 50
            break
        }
    }

    onVisibleChanged: {
        if (visible){
            setPosition()
            show()
            //raise()
            anim.start()
        }
    }

    NumberAnimation {
        id: anim
        target: root
        properties: "y"
        from: y
        to: destY
        duration: 300
        easing.type: Easing.OutCubic
    }

    Rectangle {
        anchors.fill: parent
        color: "#333"

        Image {
            id: img
            source: imgSrc
            fillMode: Image.PreserveAspectFit
            width: 80
            height: width
            anchors {
                left: parent.left
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }
        }

        Item {
            anchors {
                left: img.right
                right: parent.right
                leftMargin: 10
                rightMargin: 10
                top: img.top
            }
            height: 100
            clip: true

            Label {
                id: titleText
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                text: root.title
                wrapMode: Text.WordWrap
                font.bold: true
                font.pointSize: 10
            }

            Label {
                id: descriptionText
                anchors {
                    top: titleText.bottom
                    left: parent.left
                    right: parent.right
                }
                text: root.description
                wrapMode: Text.WordWrap
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.hide()
            root.clicked()
        }
    }
}
