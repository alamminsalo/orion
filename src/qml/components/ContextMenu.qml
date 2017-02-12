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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "../styles.js" as Styles

Menu {
    property variant item
    property int state

    id: root
    visible: false

    function display(item){
        root.item = item
    }

    onAboutToShow: {
        g_contextMenuVisible = true
    }
    onAboutToHide: {
        g_contextMenuVisible = false
    }

//    style: MenuStyle {
//        frame: Rectangle {
//                color: Styles.sidebarBg
//        }

//        itemDelegate {

//            label: Text {
//                id:_label
//                text: styleData.text.split(";")[0]
//                color: styleData.enabled ? Styles.textColor : Styles.disabled
//                font.pixelSize: Styles.titleFont.smaller
//                verticalAlignment: Text.AlignVCenter
//                anchors {
////                    top: _icon.top
////                    bottom: _icon.bottom
////                    left: _icon.right
//                }
//            }

//            background: Rectangle {
//                color: "#ffffff"
//                opacity: 0.1
//                visible: styleData.selected && styleData.enabled
//            }
//        }
//    }
}
