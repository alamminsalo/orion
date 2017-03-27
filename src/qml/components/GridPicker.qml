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
import "../styles.js" as Styles

Window {
    id: root
    flags: Qt.SplashScreen | Qt.NoFocus | Qt.X11BypassWindowManagerHint | Qt.BypassWindowManagerHint

    height: dp(320)
    width: dp(512)

    property string text: "Picker"
    property bool loading: false

    property var source

    Rectangle {
        id: rootRect
        anchors.fill: parent
        color: "#000000"

        SpinnerIcon {
            id:_spinner
            anchors.fill: parent
            iconSize: 60
            visible: loading
        }

        //Container for text
        Rectangle {
            id: header
            anchors.fill: text
            color: Styles.shadeColor
            opacity: 0.7
            height: text.height
        }

        Flow {
            /*
            anchors: {
                fill: parent
            }
            */
            Repeater {
              model: source

              Loader {
                height: 25
                width: 25
                property var msgItem: source[index]
                sourceComponent: imgThing
              }
            }
        }


        Text {
            id: text
            color: Styles.textColor
            text: root.text
            font.pixelSize: Styles.titleFont.smaller
            anchors{
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            wrapMode: Text.WordWrap
            //renderType: Text.NativeRendering
        }

        property Component imgThing: Component {
          Image {
            Component.onCompleted: {
              source = "image://emote/" + msgItem[0];
            }
            asynchronous: true
          }
        }

    }

    function display(mX, mY){

        if (g_contextMenuVisible){
            return
        }

        root.x = mX + dp(20)

        if (root.x + root.width > Screen.width)
            root.x -= root.width + dp(40)

        root.y = mY

        root.show()
    }
}
