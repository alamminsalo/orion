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

Item {
    property string text
    property int thickness: dp(60)
    property string color: Styles.shadeColor
    id: root

    height: thickness

    Rectangle {
        id: shade
        color: root.color
        opacity: .8
        anchors.fill: parent
    }

    Behavior on height {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutCubic
        }
    }

    function show(){
        height = thickness
    }

    function hide(){
        height = -thickness
    }

    Component.onCompleted: hide()
}
