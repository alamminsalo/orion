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


import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2

Page {
    id: root

    Material.background: "#fff"
    property bool loading: false
    property string filterTextProperty

    property real devicePixelRatio: 1.0

    property ListModel model
    property ListModel _innerModel
    property ListModel _filteredModel: ListModel {}
    property var _filterIndexMap: []

    onVisibleChanged:  {
        if (visible && !isMobile()) {
            _filterTextInput.forceActiveFocus()
        }
        _emotesGrid.currentIndex = -1
        _emotesGrid.contentY = _emotesGrid.originY
    }

    signal itemClicked(int index);
    signal closeRequested();
    signal moveFocusDown();

    function focusEntersFromBottom() {
        if (_emotesGrid.currentIndex === -1) {
            _emotesGrid.currentIndex = _emotesGrid.lastRowNum() * _emotesGrid.rowSize();
        }
        _emotesGrid.forceActiveFocus();
    }

    function focusFilterInput() {
        if (_filterTextInput.visible) {
            _filterTextInput.selectAll();
            _filterTextInput.forceActiveFocus();
        }
    }

    BusyIndicator {
        id:_spinner
        visible: running
        hoverEnabled: false
        anchors.centerIn: parent
        height: 60
        running: root.loading
        opacity: 0.5
        z: 2
    }

    function updateFilter() {
        if (_filterTextInput.text == "") {
            _innerModel = model;
            _filterIndexMap = [];
        } else {
            _filteredModel.clear();
            _innerModel = _filteredModel;
            var newFilterIndexMap = [];
            var filterText = _filterTextInput.text.toLowerCase();
            for (var i = 0; i < model.count; i++) {
                var obj = model.get(i);
                var text = obj[filterTextProperty].toLowerCase();
                var io = text.indexOf(filterText);
                var visible = io != -1;
                //console.log("updating filter", text, filterText, io, visible);
                if (visible) {
                    _innerModel.append(obj);
                    newFilterIndexMap.push(i);
                }
            }
            _filterIndexMap = newFilterIndexMap // update property bindings
        }
    }

    function _visibleItemClicked(index) {
        var actualIndex = index;
        if (_filterIndexMap.length > 0) {
            if (0 > index || index >= _filterIndexMap.length) {
                // index out of range
                return false;
            }
            actualIndex = _filterIndexMap[index];
        }
        itemClicked(actualIndex);
        return true
    }

    Component {
        id: highlight
        Rectangle {
            width: _emotesGrid.cellWidth; height: _emotesGrid.cellHeight
            color: Material.color(Material.primary)
            radius: 5
            x: _emotesGrid.currentItem ? _emotesGrid.currentItem.x : -1
            y: _emotesGrid.currentItem ? _emotesGrid.currentItem.y : -1
            z: 10
            opacity: 0.5
            visible: _emotesGrid.currentIndex !== -1
            Behavior on x { NumberAnimation { duration: 150 } }
            Behavior on y { NumberAnimation { duration: 150 } }
            EmoteTooltip {
                visible: (_emotesGrid.activeFocus || gridKeyHandler.activeFocus) && index !== -1
                property int index: _filterIndexMap[_emotesGrid.currentIndex] || _emotesGrid.currentIndex
                text: index !== -1 ? model.get(index)[root.filterTextProperty] : ""
            }
        }
    }

    Item {
        // workaround for https://bugreports.qt.io/browse/QTBUG-68711
        id: gridKeyHandler
        Keys.forwardTo: [handler, _filterTextInput]
        Keys.onShortcutOverride: {
            event.accepted = true;
        }
        Item {
            id: handler

            Keys.onEscapePressed: {
                root.closeRequested();
            }

            Keys.onPressed: {
                switch (event.key) {
                case Qt.Key_Home:
                    _emotesGrid.currentIndex = 0;
                    break;
                case Qt.Key_End:
                    _emotesGrid.currentIndex = _emotesGrid.count - 1;
                    break;
                case Qt.Key_PageDown:
                    _emotesGrid.currentIndex = Math.min(_emotesGrid.currentIndex + _emotesGrid.rowSize() * _emotesGrid.pageRows(), _emotesGrid.count - 1);
                    break;
                case Qt.Key_PageUp:
                    _emotesGrid.currentIndex = Math.max(_emotesGrid.currentIndex - _emotesGrid.rowSize() * _emotesGrid.pageRows(), 0);
                    break;
                default:
                    event.accepted = false
                    break;
                }
            }

            Keys.onRightPressed: _emotesGrid.currentIndex = Math.min(_emotesGrid.count-1, _emotesGrid.currentIndex+1)
            Keys.onLeftPressed: _emotesGrid.currentIndex = Math.max(0, _emotesGrid.currentIndex-1)
            Keys.onUpPressed: {
                if (_emotesGrid.count == 0 || (_emotesGrid.rowNum(_emotesGrid.currentIndex) === 0) && _filterTextInput.visible) {
                    if (_emotesGrid.currentIndex === 0)
                        _emotesGrid.currentIndex = -1
                    _filterTextInput.forceActiveFocus();
                } else {
                    _emotesGrid.currentIndex = Math.max(0, _emotesGrid.currentIndex - _emotesGrid.rowSize());
                }
            }
            Keys.onDownPressed: {
                if (_emotesGrid.count == 0 || (_emotesGrid.rowNum(_emotesGrid.currentIndex) === _emotesGrid.lastRowNum())) {
                    moveFocusDown();
                } else {
                    _emotesGrid.currentIndex = Math.min(_emotesGrid.currentIndex + _emotesGrid.rowSize(), _emotesGrid.count - 1);
                }
            }
            Keys.onTabPressed: {
                if (_emotesGrid.currentIndex === 0) {
                    _emotesGrid.currentIndex = -1
                }
                moveFocusDown()
            }

            function _onReturnPressed(event) {
                if (root._visibleItemClicked(_emotesGrid.currentIndex) && (event.modifiers & Qt.ShiftModifier) !== Qt.ShiftModifier) {
                    closeRequested();
                }
            }

            Keys.onReturnPressed: _onReturnPressed(event)
            Keys.onEnterPressed: _onReturnPressed(event)
        }
    }

    GridView {
        id: _emotesGrid
        clip: true
        focus: true
        currentIndex: -1

        onActiveFocusChanged: if (activeFocus) gridKeyHandler.forceActiveFocus()

        anchors {
            fill: parent
            topMargin: 2
            bottomMargin: 2
            leftMargin: 2 + ((parent.width - 4) % cellWidth) / 2 // centerAlign
        }

        ScrollBar.vertical: ResponsiveScrollBar {
            id: scrollbar
            visible: false
            width: 0
            scrollspeed: 1.5 * parent.cellHeight
        }

        model: root._innerModel

        cellWidth: 26 * Screen.devicePixelRatio
        cellHeight: cellWidth

        highlight: highlight
        highlightFollowsCurrentItem: true

        function rowSize() {
            return Math.floor(width / cellWidth);
        }

        function rowNum(i) {
            return Math.floor(i / rowSize());
        }

        function lastRowNum() {
            return rowNum(_emotesGrid.count - 1);
        }

        function pageRows() {
            return Math.floor(height / cellHeight);
        }

        MouseArea {
            id: _imageMouseArea
            visible: !_emotesGrid.dragging
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: hoveringIndex != -1 ? Qt.PointingHandCursor : Qt.ArrowCursor
            property int hoveringIndex: containsMouse ? _emotesGrid.indexAt(_emotesGrid.contentX + mouseX, _emotesGrid.contentY + mouseY) : -1
            onClicked: {
                root._visibleItemClicked(hoveringIndex);
                moveFocusDown()
            }
        }

        delegate: Item {
            id: cellItem
            height: _emotesGrid.cellHeight
            width: _emotesGrid.cellWidth

            anchors.margins: 1

            Rectangle {
                color: _imageMouseArea.hoveringItem === parent ? "#dddddd" : "#ffffff"
                anchors.fill: parent
            }

            Image {
                id: _itemImage
                source: model.imageUrl
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                asynchronous: true

                width: sourceSize.width / root.devicePixelRatio
                height: sourceSize.height / root.devicePixelRatio
            }


//                Label {
//                    text: model.imageUrl
//                }

            EmoteTooltip {
                visible: _emotesGrid.visible && index !== -1 && _imageMouseArea.hoveringIndex === index
                text: model[root.filterTextProperty]
            }
        }
    }

    header: ToolBar {
        Material.theme: Material.Light
        Material.background: "white"
        padding: 5

        TextField {
            id: _filterTextInput
            placeholderText: "Filter emotes"
            Material.foreground: "black"
            inputMethodHints: Qt.ImhNoPredictiveText
            selectByMouse: true
            anchors.fill: parent

            onTextChanged: {
                var prevIndex = _emotesGrid.currentIndex
                updateFilter();
                if (prevIndex !== -1) {
                    _emotesGrid.currentIndex = 0 // todo: find correct new index
                }
            }

            function _onReturnPressed(event) {
                if (root._visibleItemClicked(0) && (event.modifiers & Qt.ShiftModifier) !== Qt.ShiftModifier) {
                    closeRequested()
                }
            }

            Keys.onEnterPressed: _onReturnPressed(event)
            Keys.onReturnPressed: _onReturnPressed(event)

            Keys.onShortcutOverride: {
                event.accepted = true;
            }

            Keys.onSpacePressed: event.accepted = true

            Keys.onEscapePressed: {
                //console.log("filterTextInput escape pressed");
                root.closeRequested();
            }

            Keys.onDownPressed: {
                _emotesGrid.forceActiveFocus();
                if (_emotesGrid.currentIndex === -1) {
                    _emotesGrid.currentIndex = 0;
                }
            }

            Keys.onTabPressed: {
                if (_emotesGrid.currentIndex === -1) {
                    _emotesGrid.currentIndex = 0;
                }
                _emotesGrid.forceActiveFocus()
            }
        }
    }

    function display(mX, mY){
        root.x = mX + 20

        if (root.x + root.width > Screen.width)
            root.x -= root.width + 40

        root.y = mY

        root.show()
    }
}
