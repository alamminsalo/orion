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
import "components"
import "styles.js" as Styles
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.1

//ToolBox.qml
ToolBar {
    id: root
    property int selectedView: 0
    visible : !g_fullscreen

    Behavior on width {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutCubic
        }
    }

    function setView(index) {
        selectedView = index
    }

    RowLayout {
        anchors.fill: parent
        //spacing: 0

        Button {
            text: "Search"
            onClicked: selectedView = 0
            checked: selectedView == 0
            checkable: checked
            Layout.fillWidth: true
            flat: true
        }
        Button {
            text: "Featured"
            onClicked: selectedView = 1
            checked: selectedView ==1
            checkable: checked
            Layout.fillWidth: true
            flat:true
        }
        Button {
            text: "Followed"
            onClicked: selectedView = 2
            checked: selectedView ==2
            checkable: checked
            Layout.fillWidth: true
            flat:true
        }
        Button {
            text: "Games"
            onClicked: selectedView = 3
            checked: selectedView ==3
            checkable: checked
            Layout.fillWidth: true
            flat:true
        }
        Button {
            text: "VODs"
            onClicked: selectedView = 4
            checked: selectedView == 4
            checkable: checked
            Layout.fillWidth: true
            flat:true
        }
        Button {
            text: "Player"
            onClicked: selectedView = 5
            checked: selectedView == 5
            checkable: checked
            Layout.fillWidth: true
            flat:true
        }
        Button {
            text: "Settings"
            onClicked: selectedView = 6
            checked: selectedView == 6
            checkable: checked
            Layout.fillWidth: true
            flat:true
        }
    }
}
