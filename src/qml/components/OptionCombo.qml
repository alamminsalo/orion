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
import QtQuick.Layouts 1.1

RowLayout {
    id: root

    property alias text: label.text
    property alias selection: combo.currentIndex
    property alias model: combo.model
    property alias currentIndex: combo.currentIndex

    signal activated(int index)

    Label {
        id: label
    }

    ComboBox {
        id: combo
        font.pointSize: 10
        Layout.fillWidth: true
        onActivated: root.activated(index)
        flat: true
    }
}
