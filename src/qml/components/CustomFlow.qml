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

import QtQuick 2.0

Item {
    /*
     * A replacement for Flow that allows rows to be
     * center or bottom aligned
     */

    id: root

    property bool cComplete: false;

    onChildrenChanged: {
        updatePositions();
    }

    Component.onCompleted: {
        cComplete = true;
        updatePositions();
    }

    property int xSpacing: 0;
    property int leftMargin: 0;
    property int rightMargin: 0;
    property int ySpacing: 0;
    property int topMargin: 0;
    property int bottomMargin: 0;
    property string vAlignBottom: "VAlignBottom";
    property string vAlignCenter: "VAlignCenter";
    property string vAlignTop: "VAlignTop";
    property string vAlign: vAlignBottom;

    onVAlignChanged: {
        updatePositions();
    }

    onWidthChanged: {
        updatePositions();
    }

    function updatePositions() {
        if (!cComplete) {
            return;
        }

        var items = root.children;

        var xSize = root.width;

        var lineStart = 0;
        var xPos = leftMargin;
        var yPos = topMargin;

        function nextLine(lineEnd) {
            //console.log("nextLine", lineEnd);
            var item;

            var lineFactor;
            switch (vAlign) {
            case vAlignBottom:
                lineFactor = 1.0;
                break;
            case vAlignCenter:
                lineFactor = 0.5;
                break;
            default:
                lineFactor = 0;
            }

            // get max height
            var maxHeight = 0;
            if (lineFactor != 0) {
                for (var j = lineStart; j < lineEnd; j++) {
                    item = items[j];
                    maxHeight = Math.max(maxHeight, item.height);
                }
            }

            // position the items vertically
            for (var j = lineStart; j < lineEnd; j++) {
                item = items[j];
                var itemHeight = item.height;
                var deltaHeight = maxHeight - itemHeight;
                item.y = yPos + deltaHeight * lineFactor;
                //console.log("vertical positioning", j, item, "yPos", yPos, "maxHeight", maxHeight, "itemHeight", itemHeight, "deltaHeight", deltaHeight, "result", item.y);
            }

            // go to next line
            lineStart = lineEnd;
            xPos = leftMargin;
            yPos += maxHeight + ySpacing;
        }


        for (var i = 0; i < items.length; ++i) {
            var item = items[i];
            //showKeys(item);
            var itemWidth = item.width;
            var itemFitsOnLine = xPos + itemWidth + rightMargin <= xSize;
            //console.log("laying out i", i, item, "xPos", xPos, "width", itemWidth, "xSize", xSize);
            if (i > lineStart && !itemFitsOnLine) {
                nextLine(i);
            }
            item.x = xPos;
            xPos += itemWidth + xSpacing;
        }
        if (lineStart < items.length) {
            nextLine(items.length);
        }
        root.height = yPos + bottomMargin;
    }
}
