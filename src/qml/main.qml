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

import QtQuick 2.8
import QtQuick.Window 2.2
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import "irc"
import "components"
import app.orion 1.0

ApplicationWindow {
    id: root
    visible: true

    // Application main font
    font.family: Settings.font || appFont.name

    // Style settings
    Material.theme: Settings.lightTheme ? Material.Light : Material.Dark

    title: "Orion"
    visibility: appFullScreen ? Window.FullScreen : Window.AutomaticVisibility

    property variant rootWindow: root
    property variant g_tooltip
    property bool g_contextMenuVisible: false
    property bool appFullScreen: isMobile() ? (view.playerVisible && !isPortraitMode) : false
    property var chat: chatdrawer.chat
    property bool isPortraitMode: Screen.primaryOrientation === Qt.PortraitOrientation
                                  || Screen.primaryOrientation === Qt.InvertedPortraitOrientation

    function fitToAspectRatio() {
        height = Math.floor(view.width * 0.5625 + topbar.height)
    }

    function isMobile() {
        return Qt.platform.os === "android"
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
            bottomMargin: chatdrawer.isBottom && chatdrawer.position > 0 ? chatdrawer.height : 0
        }

        onCurrentIndexChanged: {
            if (chatdrawer.isBottom) {
                if (!playerVisible)
                    chatdrawer.close()
                else if (isMobile()) {
                    if (isPortraitMode)
                        chatdrawer.open()
                }
            }
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
        Material.background: Material.Primary
        visible: !Network.up

        Label {
            anchors.centerIn: parent
            text: "Connection error"
        }
    }

    Component.onCompleted: {
        if (!isMobile()) {
            var component = Qt.createComponent("components/Tooltip.qml")
            g_tooltip = component.createObject(root)
        }

        console.log("Pixel density", Screen.pixelDensity)
        console.log("Pixel ratio", Screen.devicePixelRatio)
        console.log("Logical pixel density", Screen.logicalPixelDensity)
        console.log("Orientation", Screen.orientation)

        //Initial view
        topbar.setView(1)

        //Check version
        if (Settings.versionCheckEnabled) {
            Network.checkVersion()
            Network.versionCheckEnded.connect(function(version, url){
                if (version && Settings.isNewerVersion(version)) {
                    newVersionLabel.text = "Version " + version + ".\nGo to download page?"
                    versionPopup.url = url
                    versionPopup.open()
                }
            });
        }
    }

    Dialog {
        id: versionPopup
        property string url
        modal: true
        dim: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        visible: false
        title: "New version available!"
        Label {
            id: newVersionLabel
            font.bold: true
        }
        standardButtons: Dialog.No | Dialog.Yes
        onAccepted: {
            if (url)
                Qt.openUrlExternally(url)
        }
    }

    FontLoader {
        source: "fonts/MaterialIcons-Regular.ttf"
        name: "Material Icons"
    }

    FontLoader {
        id: appFont

        source: "fonts/NotoSans-Regular.ttf"
        name: "Noto Sans"
    }

    Loader {
        active: !isMobile()
        sourceComponent: AppTray{}
    }
}

