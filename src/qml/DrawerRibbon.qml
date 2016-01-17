import QtQuick 2.0
import "components"

Ribbon {
    id: root
    signal click
    iconStr: 'chevron_l'
    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.click()
            rotateIcon()
        }

        hoverEnabled: true
        onHoveredChanged: {
            root.setHighlight(containsMouse)
        }
    }
}
