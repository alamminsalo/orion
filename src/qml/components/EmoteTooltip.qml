import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

ToolTip {
    y: -height - 5
    Component.onCompleted: if (Object.hasOwnProperty(this, "enabled")) this.enabled = false
    background: Rectangle {
        radius: 3
        color: Qt.darker(Material.background, 1.3)
        border.width: 1
        border.color: Qt.lighter(color, 2)
    }
    padding: 5
    enter: Transition { }
    exit: Transition { }
}
