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
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import "components"
import app.orion 1.0

Page {
    id: root
    padding: 20

    Flickable {
        anchors.fill: parent
        contentHeight: col.height
        contentWidth: width
        ScrollIndicator.vertical: ScrollIndicator{}
        flickableDirection: Flickable.VerticalFlick

        ColumnLayout {
            id: col
            width: parent.width
            spacing: 15

            GroupBox {
                title: "Notification settings"
                visible: !isMobile()
                padding: 10
                Layout.fillWidth: true
                Layout.maximumWidth: 500
                Layout.alignment: Qt.AlignCenter

                Column {
                    width: parent.width

                    Switch {
                        id: alertOption
                        checked: Settings.alert

                        onClicked: {
                            Settings.alert = checked
                        }
                        text: "Enable notifications"
                    }

                    Switch {
                        id: notificationsOption
                        enabled: alertOption.checked

                        checked: Settings.offlineNotifications
                        onClicked: {
                            Settings.offlineNotifications = !Settings.offlineNotifications
                        }
                        text: "Show offline notifications"
                    }
                    //                    OptionCombo {
                    //                        id: alertPosition
                    //                        width: parent.width
                    //                        visible: Qt.platform.os === "windows"
                    //                        selection: Settings.alertPosition
                    //                        onActivated: Settings.alertPosition = index

                    //                        text: "Notification position"
                    //                        model: ["Top Left", "Top Right", "Bottom Left", "Bottom Right"]
                    //                    }
                }
            }

            GroupBox {
                title: "Tray options"
                visible: !isMobile()
                padding: 10
                Layout.fillWidth: true
                Layout.maximumWidth: 500
                Layout.alignment: Qt.AlignCenter

                Column {
                    Switch {
                        id: minStartupOption
                        checked: Settings.minimizeOnStartup
                        onClicked: {
                            Settings.minimizeOnStartup = !Settings.minimizeOnStartup
                        }
                        text: "Start minimized"
                    }

                    Switch {
                        id: closeToTrayOption
                        checked: Settings.closeToTray
                        onClicked: {
                            Settings.closeToTray = checked
                        }
                        text: "Close to tray"
                    }
                }
            }

            GroupBox {
                title: "User interface"
                padding: 10
                Layout.fillWidth: true
                Layout.maximumWidth: 500
                Layout.alignment: Qt.AlignCenter

                Column {
                    width: parent.width

                    Switch {
                        id: themeOption
                        text: "Enable dark theme"
                        checked: !Settings.lightTheme
                        onClicked: Settings.lightTheme = !checked
                    }

                    RowLayout {
                        width: parent.width

                        OptionCombo {
                            id: fontOption
                            text: "Font"
                            model: Qt.fontFamilies()
                            Layout.fillWidth: true

                            property string fontName: Settings.font || appFont.name
                            onFontNameChanged: {
                                setCurrentIndex(fontName)
                            }

                            function setCurrentIndex(name) {
                                for (var i=0; i < model.length; i++) {
                                    if (model[i] === name) {
                                        fontOption.selection = i
                                        break;
                                    }
                                }
                            }

                            onActivated: {
                                Settings.font = model[index]
                            }
                        }

                        Button {
                            text: "Reset"
                            font.pointSize: 9
                            onClicked: {
                                Settings.font = appFont.name
                            }
                        }
                    }

                    OptionCombo {
                        id: chatEdgeOption
                        text: "Chat position"
                        visible: !isMobile()
                        model: ["Left", "Right", "Bottom"]
                        selection: Settings.chatEdge
                        onActivated: Settings.chatEdge = index
                    }

                    Switch {
                        visible: !isMobile()
                        id: keepOnTopOption
                        text: "Keep on top"
                        checked: Settings.keepOnTop
                        onClicked: Settings.keepOnTop = checked
                    }

                    Switch {
                        visible: !isMobile()
                        id: mulipleInstancesOption
                        text: "Allow multiple instances"
                        checked: Settings.multipleInstances
                        onClicked: Settings.multipleInstances = checked
                    }

                    RowLayout {
                        width: parent.width

                        OptionCombo {
                            id: openglOption
                            text: "OpenGL (needs restart)"
                            model: {
                                var opengl = [ ]
                                if (Qt.platform.os === "windows") {
                                    opengl = ["angle", "angle (d3d11)", "angle (d3d9)", "angle (warp)"]
                                } else {
                                    opengl = ["opengl es"]
                                }
                                opengl = opengl.concat(["desktop", "software"])
                                return opengl
                            }
                            Layout.fillWidth: true
                            property var defaultValue

                            onActivated: {
                                Settings.opengl = model[currentIndex]
                            }

                            Component.onCompleted: {
                                defaultValue = Settings.opengl
                                selectItem(defaultValue)
                            }

                            function selectItem(name) {
                                for (var i in model) {
                                    if (model[i] === name) {
                                        currentIndex = i;
                                        return;
                                    }
                                }
                                //None found, attempt to select first item
                                currentIndex = 0
                            }
                        }

                        Button {
                            text: "Reset"
                            font.pointSize: 9
                            onClicked: {
                                openglOption.selectItem(openglOption.defaultValue)
                            }
                        }
                    }

                }
            }

            GroupBox {
                title: "Twitch login"
                padding: 10
                Layout.fillWidth: true
                Layout.maximumWidth: 500
                Layout.alignment: Qt.AlignCenter

                RowLayout {
                    width: parent.width

                    Label {
                        id: twitchName
                        text: "Not logged in"
                        Layout.fillWidth: true
                        clip: true
                    }

                    Button {
                        id: connectButton
                        property bool loggedIn: Settings.hasAccessToken
                        highlighted: loggedIn
                        font.pointSize: 9
                        text: loggedIn ? "Log out" : "Log in"
                        onClicked: {
                            if (!loggedIn) {
                                LoginService.start();
                                var url = "https://api.twitch.tv/kraken/oauth2/authorize?response_type=token&client_id=" + Network.getClientId()
                                        + "&redirect_uri=http://localhost:8979"
                                        + "&scope=user_read%20user_subscriptions%20user_follows_edit%20chat_login%20user_blocks_read%20user_blocks_edit"
                                        + "&force_verify=true";
                                Qt.openUrlExternally(url);


                            }
                            else {
                                Settings.accessToken = ""
                                ChannelManager.checkFavourites()
                                twitchName.text = "Not logged in"
                            }
                        }

                        Connections {
                            target: ChannelManager
                            onUserNameUpdated: {
                                twitchName.text = name
                            }
                        }
                    }
                }
            }

            GroupBox {
                title: "Version info"
                padding: 10
                Layout.fillWidth: true
                Layout.maximumWidth: 500
                Layout.alignment: Qt.AlignCenter
                Label {
                    text: Settings.appName() + " " + Settings.appVersion()
                }
            }
        }
    }
}
