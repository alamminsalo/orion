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

//TODO: change name to something like 'StreamSelectorComboBox'

Rectangle {
    property var entries: []
    property alias open: list.visible

    id: root

    signal itemChanged(string item)

    function close(){
        list.visible = false
    }

    function nameWeight(str) {
        //Returns weight for given str, used in sorting
        switch(str) {

        //Source always on top
        case "source": return 999999;

        //Rest in order
        case "audio_only": return -1;
        case "mobile": return 0;
        case "low": return 1;
        case "medium": return 2;
        case "high": return 3;

        //Number evaluation
        default:
            var numbers = str.split("p")

            var value = parseInt(numbers[0])

            if (numbers.length > 1)
                value += parseInt(numbers[1])

            return value
        }
    }

    function sortEntries(fn) {
        entries.sort(function(a, b) {return nameWeight(a) - nameWeight(b)});
    }

    onEntriesChanged: {
        sortEntries(nameWeight);

        console.log("Setting new entries")
        srcModel.clear()
        for (var i = entries.length - 1; i > -1; i--){
            if (entries[i] && entries[i].length > 0){
                srcModel.append( { "itemIndex": i})
            }
        }
    }

    function findIndex(str) {
        for (var i=0; i < entries.length; i++) {
            console.log(entries[i])
            if (entries[i].localeCompare(str) === 0) {
                console.log(str + " => " + i)

                //Somehow entries end up backwards in
                return (entries.length - 1) - i;
            }
        }
    }

    function selectFirst() {
        list.currentIndex = 0
    }

    color: Styles.shadeColor

    Rectangle {
        id: rect
        color: "transparent"
        width: root.width
        height: root.height
        border {
            color: Styles.border
            width: dp(1)
        }
        anchors {
            verticalCenter: parent.verticalCenter
        }

        Text {
            id: label
            text: list.currentItem ? list.currentItem.text : "..."
            anchors.centerIn: parent
            color: Styles.textColor
            font.pixelSize: Styles.titleFont.extrasmall
            font.bold: true
        }
    }


    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onClicked: {
            list.visible = !list.visible
        }

        onHoveredChanged: {
            rect.border.color = containsMouse ? Styles.textColor : Styles.border
        }
    }

    ListView {

        property var hoveredItem

        id: list
        interactive: false
        anchors {
            bottom: rect.top
        }

        onCurrentIndexChanged: {
            console.log(currentItem.text)
            root.itemChanged(currentItem.text)
        }

        width: dp(90)
        height: srcModel.count * root.height

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                if (list.itemAt(mouseX, mouseY)){
                    list.currentIndex = list.indexAt(mouseX, mouseY)
                }

                list.visible = false
            }

            onPositionChanged: {
                var item = list.itemAt(mouseX, mouseY)
                if (item && list.hoveredItem !== item){
                    if (list.hoveredItem)
                        list.hoveredItem.color = Styles.shadeColor
                    item.color = Styles.ribbonHighlight
                    list.hoveredItem = item
                }
            }
        }

        visible: false

        model: ListModel {
            id: srcModel
        }

        delegate: Rectangle {
            property int index: itemIndex
            property alias text: _txt.text
            width: root.width
            height: root.height

            color: Styles.shadeColor

            Text {
                id: _txt
                text: entries[parent.index] ? entries[parent.index] : ""
                anchors.centerIn: parent
                color: Styles.textColor
                font.pixelSize: Styles.titleFont.smaller
            }
        }
    }
}
