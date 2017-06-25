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

    //Application main font
    font.family: appFont.name
    Material.accent: Settings.hasAccessToken ? "#ee3862" : "#888"
    Material.theme: Settings.lightTheme ? Material.Light : Material.Dark

    title: "Orion"
    visibility: g_fullscreen && Qt.platform.os !== "android"
                ? Window.FullScreen : Window.AutomaticVisibility

    property variant rootWindow: root
    property variant g_tooltip
    property bool g_contextMenuVisible: false
    property bool g_fullscreen: false
    property var chat: chatdrawer.chat

    function fitToAspectRatio() {
        height = view.width * 0.5625 + topbar.height
    }

    onClosing: {
        if (!Settings.closeToTray) {
            Qt.quit()
        }
    }

    ChatDrawer {
        id: chatdrawer
    }

    Views {
        id: view
        anchors {
            fill: parent
            leftMargin: !chatdrawer.interactive && chatdrawer.edge === Qt.LeftEdge ? chatdrawer.width : 0
            rightMargin: !chatdrawer.interactive && chatdrawer.edge === Qt.RightEdge ? chatdrawer.width : 0
            bottomMargin: Settings.chatEdge === 2 && chatdrawer.position > 0 ? chatdrawer.height : 0
        }

        onRequestSelectionChange: {
            topbar.setView(index)
        }
    }

    header: TopBar {
        id: topbar
        onSelectedViewChanged: {
            view.setSelection(selectedView)
        }
    }

    footer: ToolBar {
        id: connectionErrorRectangle
        leftPadding: !chatdrawer.interactive && chatdrawer.edge === Qt.LeftEdge ? chatdrawer.width : 0
        rightPadding: !chatdrawer.interactive && chatdrawer.edge === Qt.RightEdge ? chatdrawer.width : 0
        Material.background: Material.Amber
        visible: !Network.up

        Label {
            anchors.centerIn: parent
            text: "Connection error"
        }
    }

    Component.onCompleted: {
        if (Qt.platform.os !== "android") {
            height = Screen.height * 0.7
            width = height * 1.2

            setX(Screen.width / 2 - width / 2);
            setY(Screen.height / 2 - height / 2);

            var component = Qt.createComponent("components/Tooltip.qml")
            g_tooltip = component.createObject(root)
        }

        console.log("Pixel density", Screen.pixelDensity)
        console.log("Pixel ratio", Screen.devicePixelRatio)
        console.log("Logical pixel density", Screen.logicalPixelDensity)
        console.log("Orientation", Screen.orientation)

        //Initial view
        topbar.setView(1)
    }

    FontLoader {
        source: "fonts/MaterialIcons-Regular.ttf"
        name: "Material Icons"
    }

    FontLoader {
        id: appFont
        //source: "fonts/overpass-regular.otf"
        source: "fonts/NotoSans-Regular.ttf"
        name: "Noto Sans"
    }

    AppTray {
        id: appTray
    }
}

