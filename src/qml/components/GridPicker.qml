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

    property bool loading: false
    property string filterTextProperty

    property real devicePixelRatio: 1.0

    property ListModel model
    property ListModel _innerModel
    property ListModel _filteredModel: ListModel {}

    property var _filterIndexMap

    signal itemClicked(int index);
    signal closeRequested();
    signal moveFocusDown();

    function focusEntersFromBottom() {
        if (_emotesGrid.rowNum(_emotesGrid.currentIndex) != _emotesGrid.lastRowNum()) {
            _emotesGrid.currentIndex = _emotesGrid.lastRowNum() * _emotesGrid.rowSize();
        }
        _emotesGrid.focus = true;
    }

    function focusFilterInput() {
        if (_filterTextInput.visible) {
            _filterTextInput.selectAll();
            _filterTextInput.focus = true;
        }
    }

    SpinnerIcon {
        id:_spinner
        anchors.fill: parent
        iconSize: 60
        visible: root.loading
        opacity: 0.5
        z: 2
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

    Component {
        id: highlight
        Rectangle {
            width: _emotesGrid.cellWidth; height: _emotesGrid.cellHeight
            color: "lightsteelblue"; radius: 5
            x: _emotesGrid.currentItem.x
            y: _emotesGrid.currentItem.y
            z: 10
            opacity: 0.5
            visible: _emotesGrid.focus
            Behavior on x { SpringAnimation { spring: 3; damping: 0.2 } }
            Behavior on y { SpringAnimation { spring: 3; damping: 0.2 } }
        }
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

        highlight: highlight
        highlightFollowsCurrentItem: true

        Keys.onReturnPressed: {
            root._visibleItemClicked(_emotesGrid.currentIndex);
        }

        function rowSize() {
            return Math.floor(width / cellWidth);
        }

        function rowNum(i) {
            return Math.floor(i / rowSize());
        }

        function lastRowNum() {
            return rowNum(_emotesGrid.count - 1);
        }

        Keys.onUpPressed: {
            //console.log("item", currentIndex, "current row", rowNum(currentIndex));
            if (_emotesGrid.count == 0 || (rowNum(currentIndex) == 0) && _filterTextInput.visible) {
                _filterTextInput.focus = true;
            } else {
                event.accepted = false;
            }
        }

        Keys.onDownPressed: {
            if (_emotesGrid.count == 0 || (rowNum(currentIndex) == lastRowNum())) {
                moveFocusDown();
            } else {
                event.accepted = false;
            }
        }

        Keys.onEscapePressed: {
            root.closeRequested();
        }

        Keys.onPressed: {
            switch (event.key) {
            case Qt.Key_Home:
                _emotesGrid.currentIndex = 0;
                event.accepted = true;
                break;
            case Qt.Key_End:
                _emotesGrid.currentIndex = _emotesGrid.count - 1;
                event.accepted = true;
                break;
            }
        }

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
                    fillMode: Image.PreserveAspectFit
                    //asynchronous: true

                    width: sourceSize.width / root.devicePixelRatio
                    height: sourceSize.height / root.devicePixelRatio
                }

                /*
                Text {
                    text: model.imageUrl
                }
                */

                ToolTip {
                    visible: (_imageMouseArea.containsMouse || (_emotesGrid.focus && index == _emotesGrid.currentIndex)) && root.filterTextProperty != null
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
                //console.log("filterTextInput enter pressed");
                root._visibleItemClicked(0);
            }

            Keys.onEscapePressed: {
                //console.log("filterTextInput escape pressed");
                root.closeRequested();
            }

            Keys.onDownPressed: {
                _emotesGrid.focus = true;
                if (_emotesGrid.rowNum(_emotesGrid.currentIndex) != 0) {
                    _emotesGrid.currentIndex = 0;
                }
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
