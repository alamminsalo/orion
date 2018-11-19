import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

// Custom Drawer that anchors on its parent instead of the window
Item {
    id: root

    visible: view.partiallyOpen
    property bool opened: view.fullyOpen

    onVisibleChanged: {
        if (visible != view.partiallyOpen) {
            (visible ? open : close)();
        }
        visible = Qt.binding(function() { return view.partiallyOpen })
    }

    onOpenedChanged: {
        if (opened != view.fullyOpen) {
            (opened ? open : close)();
        }
        opened = Qt.binding(function() { return view.fullyOpen })
    }

    property real dragMargin: Qt.styleHints.startDragDistance
    property int edge: Qt.LeftEdge

    property alias interactive: view.interactive
    readonly property alias position: view.position
    property alias dim: popup.dim
    property alias background: contentItem.background

    property bool modal: false
    property int closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    Keys.onEscapePressed: if ((closePolicy & Popup.CloseOnEscape) === Popup.CloseOnEscape) close()

    function open() {
        view.open()
    }

    function close() {
        view.close()
    }

    /* manages children */
    property Item contentItem: Control {
        id: contentItem
        visible: view.partiallyOpen
        parent: root.parent
        width: root.width
        height: root.height

        property int contentItemLength: root.children.length
        onContentItemLengthChanged: {
            Qt.callLater(function() {
                for (var i = 0; i < root.children.length; i++) {
                    root.children[i].parent = contentItem
                }
            })
        }
        background: Rectangle {
            color: Material.background
        }
    }

    MouseArea {
        parent: root.parent
        visible: root.modal && view.partiallyOpen
        z: 1
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        onPressed: {
            var closeOnPressOutside = (root.closePolicy & Popup.CloseOnPressOutside) == Popup.CloseOnPressOutside;
            var closeOnPressOutsideParent = (root.closePolicy & Popup.CloseOnPressOutsideParent) == Popup.CloseOnPressOutsideParent;
            if (closeOnPressOutside || closeOnPressOutsideParent)
                root.close()
        }
        onReleased: {
            var closeOnReleaseOutside = (root.closePolicy & Popup.closeOnReleaseOutside) === Popup.closeOnReleaseOutside;
            var closeOnReleaseOutsideParent = (root.closePolicy & Popup.closeOnReleaseOutsideParent) === Popup.closeOnReleaseOutsideParent;
            if (closeOnReleaseOutside || closeOnReleaseOutsideParent)
                root.close()
        }
        hoverEnabled: true
        preventStealing: true
        propagateComposedEvents: false
    }

    Item {
        parent: root.parent
        x: root.x
        y: root.y
        width: root.parent.width - x
        height: root.parent.height - y

        Popup {
            id: popup
            visible: root.dragMargin > 0 || view.partiallyOpen // don't show if closed without dragMargin
            clip: root.dragMargin > 0 && !view.partiallyOpen // clip if closed with dragMargin

            x: edge === Qt.RightEdge ? root.parent.width - root.width - view.xOffset : 0
            y: edge === Qt.BottomEdge ? root.parent.height - root.height - view.yOffset : 0
            z: -100 // under all other popups

            width: view.orientation == Qt.Horizontal ? Math.min(root.width, view.position * view.width + root.dragMargin) : root.width
            height: view.orientation == Qt.Vertical ? Math.min(root.height, view.position * view.height + root.dragMargin) : root.height

            topPadding: 0
            bottomPadding: 0
            rightPadding: 0
            leftPadding: 0

            dim: root.modal

            modal: false
            closePolicy: Popup.NoAutoClose

            enter: Transition { }
            exit: Transition { }
            background: Item { }

            Item {
                x: view.xOffset
                y: view.yOffset
                width: root.width
                height: root.height

                ListView {
                    id: view

                    width: root.width
                    height: root.height

                    readonly property string contentProp: orientation === Qt.Horizontal ? "contentX" : "contentY"
                    readonly property real xOffset: edge === Qt.RightEdge ? -Math.max(0, view.originX + root.width - view.contentX - root.dragMargin) : 0
                    readonly property real yOffset: edge === Qt.BottomEdge ? -Math.max(0, view.originY + root.height - view.contentY - root.dragMargin) : 0

                    function calcPos(index) {
                        var size = orientation == Qt.Horizontal ? width : height
                        var offset = orientation == Qt.Horizontal ? originX : originY
                        var openMult = edge === Qt.LeftEdge || edge === Qt.TopEdge ? 0 : 1
                        var closeMult = edge === Qt.LeftEdge || edge === Qt.TopEdge ? 1 : 0
                        if (index === 0) {
                            return openMult * size + offset
                        } else {
                            return closeMult * size + offset
                        }
                    }

                    function open() {
                        snapToIndex(0)
                    }

                    function close() {
                        snapToIndex(1)
                    }

                    function snapToIndex(index) {
                        if (index === -1 || dragging)
                            return
                        targetIndex = index
                        cancelFlick()
                        var pos = calcPos(index)
                        if (snapAnimation.running && pos === snapAnimation.to)
                            return
                        snapAnimation.stop()
                        snapAnimation.to = pos
                        snapAnimation.start()
                    }

                    property int targetIndex: 1
                    Component.onCompleted: snapToIndex(targetIndex)

                    readonly property bool partiallyOpen: position > 0
                    readonly property bool fullyOpen: (snapAnimation.running || view.moving || view.dragging) ? position === 1 : targetIndex === 0

                    NumberAnimation {
                        running: false
                        target: view
                        id: snapAnimation
                        property: view.contentProp
                        easing.type: Easing.OutCubic
                        duration: 200
                    }

                    property real position: {
                        var size = orientation === Qt.Horizontal ? contentX : contentY
                        var closed = calcPos(1)
                        var open = calcPos(0)
                        return Math.min(1, Math.max(0, Math.abs((size - closed) / (closed - open))))
                    }

                    // force position to right position if it involuntarily changes (e.g by resizing)
                    function forceToIndex(index) {
                        if (dragging || moving || snapAnimation.running) {
                            return
                        }
                        targetIndex = index
                        if (orientation === Qt.Horizontal) {
                            contentX = calcPos(index)
                            contentY = originY
                        } else {
                            contentX = originX
                            contentY = calcPos(index)
                        }
                    }

                    onMovementEnded: forceToIndex(position < 0.5 ? 1 : 0)
                    onAtYBeginningChanged: forceToIndex(targetIndex)
                    onAtYEndChanged: forceToIndex(targetIndex)
                    onAtXBeginningChanged: forceToIndex(targetIndex)
                    onAtXEndChanged: forceToIndex(targetIndex)

                    orientation: edge === Qt.RightEdge || edge === Qt.LeftEdge ? Qt.Horizontal : Qt.Vertical
                    layoutDirection: edge === Qt.LeftEdge || edge === Qt.TopEdge ? Qt.LeftToRight : Qt.RightToLeft
                    verticalLayoutDirection: edge === Qt.LeftEdge || edge === Qt.TopEdge ? ListView.TopToBottom : ListView.BottomToTop
                    flickableDirection: orientation === Qt.Horizontal ? Flickable.HorizontalFlick : Flickable.VerticalFlick
                    boundsBehavior: Flickable.StopAtBounds
                    snapMode: ListView.SnapToItem

                    model: 2
                    delegate: Item {
                        id: currentItem

                        clip: root.clip

                        width: root.width
                        height: root.height

                        Component.onCompleted: {
                            if (index == 0) {
                                root.contentItem.parent = currentItem
                            }
                        }

                        Component.onDestruction: {
                            if (index == 0) {
                                root.contentItem.parent = root.parent
                            }
                        }
                    }
                }
            }
        }
    }
}

