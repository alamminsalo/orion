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

StackView {
    id: root
    property alias searchView : searchView
    property alias favouritesView : favouritesView
    property alias gamesView : gamesView
    property alias vodsView : vodsView
    property alias playerView : playerView
    property alias settingsView : settingsView
    property bool playerVisible : playerView.visible

    function requestSelectionChange(index) {
        setCurrentIndex(index);
    }

    function isItemInView(item) {
        while(item.parent) {
            if (currentItem === item) {
                return true
            }
            item = item.parent
        }
        return false
    }

    SearchView {
        id: searchView
    }

    FollowedView {
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
    }

    OptionsView{
        id: settingsView
    }
}
