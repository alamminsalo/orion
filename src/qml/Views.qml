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

StackLayout {
    id: root

    signal requestSelectionChange(int index)

    function setSelection(sel) {
        currentIndex = sel
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
