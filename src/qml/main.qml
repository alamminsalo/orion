import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import "components"
import "styles.js" as Style

Window {
    id: root
    visible: true
    width: 1200
    height: 768

    property variant g_rootWindow: root
    property variant g_tooltip
    property variant g_toolBox: tools

    Rectangle {
        anchors.fill: parent

        ToolBox {
            id: tools
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
        }

        Rectangle {
            color: Style.bg

            anchors {
                left: tools.right
                top: parent.top
                right: parent.right
                bottom: parent.bottom
            }

            Rectangle {
                id: border
                width: 1
                color: Style.border
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }
            }

            ViewBox {
                id: view
                selection: tools.selectedView
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: border.right
                    right: parent.right
                }
                onRequestSelectionChange: {
                    g_toolBox.setView(index)
                }
            }
        }
    }

    Component.onCompleted: {
        var component = Qt.createComponent("components/Tooltip.qml")
        g_tooltip = component.createObject(root);

        g_cman.checkFavourites()
        pollTimer.start()

        //Show favourites view
        g_toolBox.setView(1)
    }

    Timer {
        id: pollTimer
        interval: 15000
        running: false
        repeat: true
        onTriggered: {
            g_cman.checkFavourites()
        }
    }
}

