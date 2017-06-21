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
import "irc"
import "components"
import app.orion 1.0

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

    function fitToAspectRatio() {
        height = view.width * 0.5625 + topbar.height
    }

    onClosing: {
        if (!Settings.closeToTray) {
            Qt.quit()
        }
    }

    Drawer {
        id: chatdrawer
        edge: Settings.swapChat ? Qt.LeftEdge : Qt.RightEdge

        height: g_rootWindow.height - 40
        y: (g_rootWindow.height - height) / 2

        width: 330
        dim: false

        ChatView {
            id: chatview
            anchors.fill: parent
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
        Material.background: Material.Amber
        visible: !Network.up

        UILabel {
            anchors.centerIn: parent
            text: "Connection error"
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

        console.log("Pixel density", Screen.pixelDensity)
        console.log("Pixel ratio", Screen.devicePixelRatio)
        console.log("Logical pixel density", Screen.logicalPixelDensity)
        console.log("Orientation", Screen.orientation)

        pollTimer.start()
    }

    Timer {
        id: pollTimer
        interval: 30000
        running: false
        repeat: true
        triggeredOnStart: true
        onTriggered: {
            ChannelManager.checkFavourites()
        }
    }

    FontLoader {
        source: "fonts/MaterialIcons-Regular.ttf"
        name: "Material Icons"
    }

    FontLoader {
        id: mainFont
        source: "fonts/NotoSans-Regular.ttf"
        name: "Noto Sans"
    }
}

