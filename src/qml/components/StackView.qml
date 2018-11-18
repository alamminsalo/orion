import QtQuick 2.5
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

Container {
    id: container
    anchors.fill: parent
    property bool interactive: true

    QtObject {
        id: d
        property bool interactive: true
        function workaroundListViewBug() {
            //ListView often shows nothing after maximizing and restoring window. This invalidates the view and fixes the rendering issue. Qt 5.11, Windows 10
            var index = container.currentIndex
            list.contentX += 1
            list.contentY += 1
            list.contentX -= 1
            list.contentY -= 1
            if (index != container.currentIndex) {
                container.setCurrentIndex(index)
            }
        }
    }

    function isItemInView(item) {
        if (!item) return false
        while (item.visible && item.parent) {
            if (item === currentItem) {
                return true;
            }
            item = item.parent;
        }
        return false;
    }

    onCurrentIndexChanged: {
        if (currentIndex != list.currentIndex) {
            d.interactive = false
            list.currentIndex = currentIndex
            d.interactive = true
        }
    }

    onWidthChanged: Qt.callLater(d.workaroundListViewBug)
    onHeightChanged: Qt.callLater(d.workaroundListViewBug)

    onCountChanged: {
        for (var i = 0; i < count; i++) {
            if (itemAt(i).width === 0) itemAt(i).width = Qt.binding(function() { return container.parent.width; })
            if (itemAt(i).height === 0) itemAt(i).height = Qt.binding(function() { return container.parent.height; })
        }
    }

    contentItem: ListView {
        id: list
        anchors.fill: parent
        interactive: container.interactive && d.interactive
        clip: true
        snapMode: ListView.SnapOneItem
        orientation: Qt.Horizontal
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 0
        highlightResizeDuration: 0
        model: container.contentModel
    }
}
