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

// Allows rows to be vertically aligned
Flow {
    property int verticalAlignment: Qt.AlignTop

    QtObject {
        id: d
        property bool debugGeometry: false
        function assignGeometry() {
            for (var i = 0; i < visibleChildren.length; i++) {
                if (visibleChildren[i] && !visibleChildren[i].background) {
                    try {
                        var background = Qt.createQmlObject("import QtQuick 2.0; Rectangle { anchors.fill: parent; color: 'transparent'; border.color: 'green'; border.width: 1 }", visibleChildren[i], "debugGeometry");
                        if (!Object.hasOwnProperty(visibleChildren[i], 'background')) {
                            Object.defineProperty(visibleChildren[i], 'background', { value: background })
                        } else {
                            visibleChildren[i].background = background
                        }
                    } catch(e) {
                        // type error for QtObject and such. Uncheckable before instanceof in Qt5.10
                    }
                }
            }
        }
        function align() {
            if (verticalAlignment === Qt.AlignTop)
                return;
            var heightMap = {}
            var i;
            for (i = 0; i < visibleChildren.length; i++) {
                var visibleChild = visibleChildren[i];
                heightMap[visibleChild.y] = Math.max(heightMap[visibleChild.y] || 0, visibleChild.implicitHeight || visibleChild.childrenRect.height || visibleChild.height);
            }
            if (verticalAlignment === Qt.AlignAbsolute) {
                // adjust height of each item to the height of its line, allowing the items to align themselves
                for (i = 0; i < visibleChildren.length; i++) {
                    visibleChildren[i].height = heightMap[visibleChildren[i].y];
                }
            } else if (verticalAlignment === Qt.AlignBottom) {
                // adjust each item vertically to the bottom of its line
                for (i = 0; i < visibleChildren.length; i++) {
                    visibleChildren[i].y += (heightMap[visibleChildren[i].y] - visibleChildren[i].height);
                }
            } else if (verticalAlignment === Qt.AlignVCenter || verticalAlignment === Qt.AlignCenter) {
                // adjust each item to the vertical center of its line
                for (i = 0; i < visibleChildren.length; i++) {
                    visibleChildren[i].y += (heightMap[visibleChildren[i].y] - visibleChildren[i].height) / 2;
                }
            }
        }
    }

    Component.onCompleted: {
        // positioningComplete introduced in Qt 5.9, attach dynamically to stay somewhat backward-compatible
        if (this.onPositioningComplete) {
            this.onPositioningComplete.connect(function() {
                d.align()
                if (d.debugGeometry)
                    d.assignGeometry()
            })
        }
    }
}
