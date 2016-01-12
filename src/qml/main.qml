import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import "styles.js" as Styles


Window {
    id: root
    visible: true
    width: dp(1600)
    height: dp(1200)
    title: "Orion"

    property variant g_rootWindow: root
    property variant g_tooltip
    property variant g_toolBox: sidebar
    property bool g_contextMenuVisible: false

    function dp(number){
        return Math.ceil(number * g_ppi / 157.29)
    }

    FontLoader{
        source: "qrc:/fonts/droidsans/DroidSans.ttf"
    }

    Item {
        anchors.fill: parent

        SideBar {
            id: sidebar
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
        }

        Item {
            anchors {
                left: sidebar.right
                top: parent.top
                right: parent.right
                bottom: parent.bottom
            }

            Rectangle {
                id: border
                width: dp(1)
                color: Styles.border
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }
            }

            Views {
                id: view
                selection: sidebar.selectedView
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

        setX(Screen.width / 2 - width / 2);
        setY(Screen.height / 2 - height / 2);

        var component = Qt.createComponent("components/Tooltip.qml")
        g_tooltip = component.createObject(root)

        g_cman.checkFavourites()
        pollTimer.start()

        //Initial view
        g_toolBox.setView(1)
    }

    Timer {
        id: pollTimer
        interval: 30000
        running: false
        repeat: true
        onTriggered: {
            g_cman.checkFavourites()
        }
    }
}

