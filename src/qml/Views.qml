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
import "components"
import "styles.js" as Styles


Rectangle {
    property int selection
    id: root

    signal requestSelectionChange(int index)

    color: Styles.bg

    function setSelection(sel) {
        if (selection === 5 && playerView.isPlaying() && playerView.enableSmallMode)
            playerView.smallMode = true

        selection = sel
    }

    onSelectionChanged: {
        searchView.visible = false
        favouritesView.visible = false
        gamesView.visible = false
        featuredView.visible = false
        playerView.visible = playerView.smallMode
        settingsView.visible = false
        vodsView.visible = false
        webView.visible = false

        switch (selection){

            //Search
        case 0:
            searchView.visible = true
            gradient.parent = searchView
            searchView.focusInput()
            break

            //Featured
        case 1:
            featuredView.visible = true
            gradient.parent = featuredView
            break

            //Fav
        case 2:
            favouritesView.visible = true
            gradient.parent = favouritesView
            break

            //Games
        case 3:
            gamesView.visible = true
            gradient.parent = gamesView
            break

            //Vods
        case 4:
            vodsView.visible = true
            gradient.parent = vodsView
            break

            //Player
        case 5:
            playerView.visible = true
            playerView.smallMode = false
            break

            //Settings
        case 6:
            settingsView.visible = true
            break

        case 7:
            webView.visible = true
            break
        }
    }

    SearchView {
        id: searchView
        visible: false
    }

    FeaturedView{
        id: featuredView
        visible: false
    }

    FavouritesView{
        id: favouritesView
        visible: false
    }

    GamesView {
        id: gamesView
        visible: false
    }

    VodsView {
        id: vodsView
        visible: false
    }

    OptionsView{
        id: settingsView
        visible: false
    }

    WebView{
        id: webView
        visible: false
    }

    PlayerView {
        id: playerView
        visible: false

        onSmallModeChanged: {
            if (root.selection !== 5)
                visible = false
        }
    }

    //The gradient that is applied to each view
    GradientBottom {
        id: gradient
        parent: searchView
    }

    Rectangle {
        id: connectionErrorRectangle
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 0
        color: Styles.connectionErrorColor
        clip: true

        Text {
            anchors.centerIn: parent
            font.pixelSize: Styles.titleFont.bigger
            color: Styles.errorTextColor
            text: "Connection error"
        }

        Behavior on height {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
    }

    Connections {
        target: netman
        onNetworkAccessChanged: {
            if (up) {
                connectionErrorRectangle.height = 0
            }
            else {
                connectionErrorRectangle.height = dp(50)
            }
        }
    }
}
