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
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import Qt.labs.settings 1.0 as Labs
import "irc"
import "components"
import app.orion 1.0

ApplicationWindow {
    id: root
    visible: true

    property alias app: root

    // Application main font
    font.family: Settings.font || appFont.name

    height: 600
    width: 700
    minimumHeight: 480
    minimumWidth: 480

    Labs.Settings {
        id: windowSettings
        property var windowX
        property var windowY
        property var windowWidth
        property var windowHeight
        property var windowVisibility
        function update() {
            Qt.callLater(function() {
                if (visibility === Window.Windowed) {
                    windowX = x
                    windowY = y
                    windowWidth = width
                    windowHeight = height
                }
                windowVisibility = visibility
            })
        }
        Component.onCompleted: {
            if (isMobile()) return
            if (visibility === Window.Windowed) {
                x = windowX !== undefined ? windowX : x
                y = windowY !== undefined ? windowY : y
                width = windowWidth !== undefined ? windowWidth : width
                height = windowHeight !== undefined ? windowHeight : height
                visibility = windowVisibility === Window.Maximized ? windowVisibility : visibility
            }
            root.onXChanged.connect(update)
            root.onYChanged.connect(update)
            root.onWidthChanged.connect(update)
            root.onHeightChanged.connect(update)
            root.onVisibilityChanged.connect(update)
        }
    }

    // Style settings
    Material.theme: Settings.lightTheme ? Material.Light : Material.Dark

    title: "Orion"

    visibility: (!isMobile() && Settings.minimizeOnStartup) ? Window.Minimized : Window.AutomaticVisibility

    property int restoredVisibility: Window.AutomaticVisibility
    onAppFullScreenChanged: {
        if (visibility != Window.FullScreen) {
            restoredVisibility = visibility
        }
        visibility = appFullScreen ? Window.FullScreen : restoredVisibility
    }
    onVisibilityChanged: {
        if (!isMobile()) {
            appFullScreen = visibility === Window.FullScreen
        }
    }

    property variant rootWindow: root
    property variant g_tooltip
    property bool g_contextMenuVisible: false
    property bool appFullScreen: isMobile() ? (view.playerVisible && !isPortraitMode) : false
    property var chat: chatdrawer.chat
    property bool isPortraitMode: Screen.primaryOrientation === Qt.PortraitOrientation
                                  || Screen.primaryOrientation === Qt.InvertedPortraitOrientation

    function fitToAspectRatio() {
        height = Math.floor(view.width * 0.5625)
    }

    function isMobile() {
        return {android: true, ios: true, winphone: true}[Qt.platform.os] || false;
    }

    onClosing: {
            Qt.quit()
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
        // remove binding. this is required to allow proper behaviour with Settings.minimizeOnStartup
        visibility = visibility

        if (!isMobile()) {
            var component = Qt.createComponent("components/GridTooltip.qml")
            if(component.status === Component.Ready) {
            g_tooltip = component.createObject(root)
            } else {
                console.error(component.errorString())
            }
        }

        console.log("Pixel density", Screen.pixelDensity)
        console.log("Pixel ratio", Screen.devicePixelRatio)
        console.log("Logical pixel density", Screen.logicalPixelDensity)
        console.log("Orientation", Screen.orientation)

        //Initial view
        if (!Settings.hasAccessToken) {
            topbar.setView(5)
        } else {
            topbar.setView(1)
        }

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

        if (Settings.keepOnTop){
            root.flags |= Qt.WindowStaysOnTopHint;
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

    Popup {
        id: dialog
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        modal: true
        dim: true
        property string text
        property var callback
        function showMessage(message, accepted) {
            text = message
            callback = accepted
            dialog.open()
        }
        background: Pane { Material.elevation: 13 }
        function accept() {
            if (callback) callback()
            dialog.close()
        }
        ColumnLayout {
            Label {
                font.bold: true
                font.pointSize: 13
                Layout.fillWidth: true
                text: dialog.text
            }
            RowLayout {
                spacing: 5
                Item { Layout.fillWidth: true }
                Button { text: "No"; onPressed: dialog.close() }
                Button {
                    text: "Yes";
                    onPressed: dialog.accept()
                    focus: true
                    Keys.onReturnPressed: accept()
                    Keys.onEnterPressed: accept()
                }
            }
        }

    }

    function addToFavourites(channel, callback) {
        dialog.showMessage("Do you want to follow " + (channel.name || "this channel") + "?", function() {
            ChannelManager.addToFavourites(channel._id, channel.name,
                                                       channel.title, channel.info,
                                                       channel.logo, channel.preview,
                                                       channel.game, channel.viewers,
                                                       channel.online)
            channel.favourite = true
            if (callback) callback()
        })
    }

    function removeFromFavourites(channel, callback) {
        dialog.showMessage("Do you want to stop following " + (channel.name || "this channel") + "?", function() {
            ChannelManager.removeFromFavourites(channel._id)
            channel.favourite = false
            if (callback) callback()
        })
    }

    Connections{
        target: Settings
        onKeepOnTopChanged:{
            if (Settings.keepOnTop){
                root.flags |= Qt.WindowStaysOnTopHint;
            }else{
                root.flags &= ~Qt.WindowStaysOnTopHint;
            }
        }
    }
}

