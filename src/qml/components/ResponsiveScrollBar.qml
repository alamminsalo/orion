import QtQuick 2.5
import QtQuick.Controls 2.1

ScrollBar {
    id: root;
    property real scrollspeed: 120
    signal scrollBegin()
    signal scrollEnd()
    Connections {
        target: root
        onPressedChanged: {
            if (pressed)
                scrollBegin();
            else
                scrollEnd();
        }
    }
    MouseArea {
        Component.onCompleted: parent = parent.parent
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        z: -1 // allow other MouseAreas to set cursorShape
        onWheel: {
            wheel.accepted = true;
            if (parent.flicking) parent.cancelFlick();
            root.scrollBegin();
            if (parent.contentY !== undefined) {
                var dy = wheel.angleDelta.y / 120 * root.scrollspeed;
                var begin = parent.originY || 0;
                var contentHeight = parent.contentHeight || parent.contentItem.height;
                var end = Math.max(begin, begin + contentHeight - parent.height);
                parent.contentY = Math.min(Math.max(begin, parent.contentY - dy), end);
            }
            root.scrollEnd();
        }
    }
}
