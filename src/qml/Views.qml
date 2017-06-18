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
import "components"
import "styles.js" as Styles


SwipeView {
    id: root

    signal requestSelectionChange(int index)
    interactive: false

    function setSelection(sel) {
        currentIndex = sel
    }

    SearchView {
        id: searchView
    }

    FeaturedView{
        id: featuredView
    }

    FollowedView{
        id: favouritesView
    }

    GamesView {
        id: gamesView
    }

    VodsView {
        id: vodsView
    }

    PlayerView {
        id: playerView

//        onSmallModeChanged: {
//            if (root.selection !== 5)
//                visible = false
//        }
    }

    OptionsView{
        id: settingsView
    }

    //The gradient that is applied to each view
//    GradientBottom {
//        id: gradient
//        parent: searchView
//    }

//    Rectangle {
//        id: connectionErrorRectangle
//        anchors {
//            top: parent.top
//            left: parent.left
//            right: parent.right
//        }
//        height: 0
//        color: Styles.connectionErrorColor
//        clip: true

//        Text {
//            anchors.centerIn: parent
//            font.pixelSize: Styles.titleFont.bigger
//            color: Styles.errorTextColor
//            text: "Connection error"
//        }

//        Behavior on height {
//            NumberAnimation {
//                duration: 200
//                easing.type: Easing.OutCubic
//            }
//        }
//    }

//    function updateForNetworkAccess(up) {
//        if (up) {
//            connectionErrorRectangle.height = 0
//        }
//        else {
//            connectionErrorRectangle.height = dp(50)
//        }
//    }

//    Component.onCompleted: {
//        updateForNetworkAccess(netman.networkAccess());
//    }

//    Connections {
//        target: netman
//        onNetworkAccessChanged: {
//            updateForNetworkAccess(up);
//        }
//    }
}
