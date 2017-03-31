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
import QtQuick.Controls 2.0
import "../styles.js" as Styles

Rectangle {
    id: root

    property string text: "Picker"
    property bool loading: false
    property string filterTextProperty

    property ListModel model
    property ListModel _innerModel
    property ListModel _filteredModel: ListModel {}

    property bool focusOnVisible: false

    property var _filterIndexMap

    signal itemClicked(int index);
    signal closeRequested();

    function focusFilterInput() {
        if (_filterTextInput.visible) {
            _filterTextInput.selectAll();
            _filterTextInput.focus = true;
        }
    }

    onVisibleChanged: {
        if (focusOnVisible && visible) {
            focusFilterInput();
        }
    }

    SpinnerIcon {
        id:_spinner
        anchors.fill: parent
        iconSize: 60
        visible: root.loading
        z: 2
    }

    Text {
        id: text
        color: Styles.textColor
        text: root.text
        font.pixelSize: Styles.titleFont.smaller
        anchors{
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        wrapMode: Text.WordWrap
        //renderType: Text.NativeRendering
    }

    function updateFilter() {
        if (_filterTextInput.text == "") {
            _innerModel = model;
            _filterIndexMap = null;
        } else {
            _filteredModel.clear();
            _innerModel = _filteredModel;
            _filterIndexMap = [];
            var filterText = _filterTextInput.text.toLowerCase();
            for (var i = 0; i < model.count; i++) {
                var obj = model.get(i);
                var text = obj[filterTextProperty].toLowerCase();
                var io = text.indexOf(filterText);
                var visible = io != -1;
                //console.log("updating filter", text, filterText, io, visible);
                if (visible) {
                    _innerModel.append(obj);
                    _filterIndexMap.push(i);
                }
            }
        }

    }

    function _visibleItemClicked(index) {
        var actualIndex = index;
        if (_filterIndexMap != null) {
            if (index >= _filterIndexMap.length) {
                // index out of range
                return;
            }
            actualIndex = _filterIndexMap[index];
        }
        itemClicked(actualIndex);
    }

    GridView {
        id: _emotesGrid

        anchors {
            top: spacer.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right

            topMargin: dp(5)
            bottomMargin: dp(5)
            leftMargin: dp(17)
            rightMargin: dp(17)
        }
        model: root._innerModel

        cellWidth: dp(42)
        cellHeight: dp(42)

        delegate: Item {
            height: dp(42)
            width: dp(42)

            anchors.margins: dp(1)

            MouseArea {
                id: _imageMouseArea
                anchors.fill: parent
                hoverEnabled: true

                Rectangle {
                    color: _imageMouseArea.containsMouse? "#dddddd" : "#ffffff"
                    anchors.fill: parent
                }

                Image {
                    id: _itemImage
                    source: model.imageUrl
                    anchors.fill: parent
                    fillMode: Image.Pad
                    //asynchronous: true
                }

                /*
                Text {
                    text: model.imageUrl
                }
                */

                ToolTip {
                    visible: _imageMouseArea.containsMouse && root.filterTextProperty != null
                    //delay: 666
                    text: model[root.filterTextProperty]
                }

                onClicked: {
                    root._visibleItemClicked(index);
                }
            }
        }
    }

    MouseArea {
        id: _filterTextInputArea

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            leftMargin: dp(5)
            rightMargin: dp(5)
        }

        height: dp(40)

        cursorShape: Qt.IBeamCursor

        TextInput {
            id: _filterTextInput

            z: 1

            anchors {
                fill: parent
            }

            onTextChanged: {
                updateFilter();
            }
            visible: root.filterTextProperty != null

            clip:true
            selectionColor: Styles.purple
            focus: true
            selectByMouse: true
            font.pixelSize: Styles.titleFont.smaller
            verticalAlignment: Text.AlignVCenter

            Keys.onReturnPressed: {
                console.log("filterTextInput enter pressed");
                root._visibleItemClicked(0);
            }

            Keys.onEscapePressed: {
                console.log("filterTextInput escape pressed");
                root.closeRequested();
            }
        }

        Rectangle {
            anchors.fill: _filterTextInput
            color: "#ffffff"
        }
    }

    Rectangle {
        id: spacer
        anchors {
            left: parent.left
            right: parent.right
            bottom: _filterTextInputArea.bottom
        }
        height: inputArea.visible ? dp(1) : 0
        color: "#777777"

        opacity: root._opacity
    }

    function display(mX, mY){

        if (g_contextMenuVisible){
            return
        }

        root.x = mX + dp(20)

        if (root.x + root.width > Screen.width)
            root.x -= root.width + dp(40)

        root.y = mY

        root.show()
    }
}
