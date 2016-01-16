import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Controls 1.1
import "styles.js" as Styles


ApplicationWindow {
    id: root
    visible: true
    width: dp(1600)
    height: dp(1200)
    title: "Orion"
    visibility: g_fullscreen ? "FullScreen" : "Windowed"

    property variant g_rootWindow: root
    property variant g_tooltip
    property variant g_toolBox: sidebar
    property bool g_contextMenuVisible: false
    property bool g_fullscreen: false

    function dp(number){
        return Math.ceil(number * g_ppi / 157.29)
    }

    FontLoader{
        source: "qrc:/fonts/DroidSans.ttf"
    }

    Item {
        anchors.fill: parent

        SideBar {
            id: sidebar
            visible: !g_fullscreen
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

            Views {
                id: view
                selection: sidebar.selectedView
                anchors.fill: parent
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
        g_toolBox.setView(2)
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

