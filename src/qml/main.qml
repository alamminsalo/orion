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

import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

import app.orion.channels 1.0

ApplicationWindow {
    id: root
    visible: true

    title: "Orion"
    flags: Qt.FramelessWindowHint | Qt.SplashScreen
    visibility: g_fullscreen ? "FullScreen" : windowstate

    property variant g_rootWindow: root
    property variant g_tooltip
    property bool g_contextMenuVisible: false
    property bool g_fullscreen: false
    onG_fullscreenChanged: {
        if (g_fullscreen)
            windowstate = visibility
    }

    property var windowstate: "Windowed"

    function dp(number){
        return number;
        //return Dpi.scale(number)
    }

    function fitToAspectRatio() {
        height = view.width * 0.5625 + topbar.height
    }

    onClosing: {
        if (!ChannelManager.isCloseToTray()) {
            Qt.quit()
        }
    }

    Connections {
        target: g_tray
        onShowTriggered: {
            if (root.visible)
                root.hide()
            else
                root.show()
        }
    }

    Connections {
        target: g_guard
        onAnotherProcessTriggered: {
            root.show()
            root.raise()
        }
    }

    header: TopBar {
        id: topbar
        onSelectedViewChanged: {
            view.setSelection(selectedView)
        }
    }

    Views {
        id: view
        anchors.fill: parent
        onRequestSelectionChange: {
            topbar.setView(index)
        }
    }

    footer: ToolBar {
        id: connectionErrorRectangle

        Label {
            anchors.centerIn: parent
            text: "Connection error"
        }
    }

    function updateForNetworkAccess(up) {
        if (up) {
            connectionErrorRectangle.visible = false
        }
        else {
            connectionErrorRectangle.visible = true
        }
    }

    Connections {
        target: netman
        onNetworkAccessChanged: {
            updateForNetworkAccess(up);
        }
    }

    Component.onCompleted: {
        height=Screen.height * 0.7
        width=height * 1.2

        setX(Screen.width / 2 - width / 2);
        setY(Screen.height / 2 - height / 2);

        var component = Qt.createComponent("components/Tooltip.qml")
        g_tooltip = component.createObject(root)

        //Initial view
        topbar.setView(2)


        if (ChannelManager.isMinimizeOnStartup())
            root.hide();

        console.log("Pixel density", Screen.pixelDensity)
        console.log("Pixel ratio", Screen.devicePixelRatio)
        console.log("Logical pixel density", Screen.logicalPixelDensity)
        console.log("Orientation", Screen.orientation)

        ChannelManager.checkFavourites()
        pollTimer.start()

        updateForNetworkAccess(netman.networkAccess());
    }

    Timer {
        id: pollTimer
        interval: 30000
        running: false
        repeat: true
        onTriggered: {
            ChannelManager.checkFavourites()
        }
    }

    FontLoader {
        source: "fonts/MaterialIcons-Regular.ttf"
        name: "Material Icons"
    }

    FontLoader {
        source: "fonts/NotoSans-Regular.ttf"
        name: "Noto Sans"
    }
}

