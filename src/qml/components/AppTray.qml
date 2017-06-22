import QtQuick 2.5
import QtQuick.Controls 2.1
import Qt.labs.platform 1.0

// Tray icon for orion app

SystemTrayIcon {
    visible: true
    iconSource: "qrc:/icon/orion.ico"

    property bool appVisible: g_rootWindow.visible
    
    menu: Menu {
        MenuItem {
            text: appVisible ? "Hide" : "Show"
            onTriggered: {
                if (appVisible)
                    g_rootWindow.hide()
                else
                    g_rootWindow.show()
            }
        }
        MenuItem {
            text: "Close"
            onTriggered: g_rootWindow.close()
        }
    }
    //        Component.onCompleted: {
    //            if (!available)
    //                console.warn("SystemTrayIcon: Not available on current platform")
    
    //            else if (!supportsMessages)
    //                console.warn("SystemTrayIcon: No message support")
    
    //            else
    //                console.info("SystemTrayIcon: OK")
    //        }
}
