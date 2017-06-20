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
import app.orion.channels 1.0

Page {
    id: root
    padding: 20

    property int itemWidth: 300

    Flow {
        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }

        flow: Flow.TopToBottom
        spacing: 20

        GroupBox {
            title: "Notification settings"
            padding: 10
            width: root.itemWidth
            Layout.alignment: Qt.AlignCenter

            ColumnLayout {
                Switch {
                    id: alertOption
                    checked: ChannelManager.isAlert()
                    onClicked: {
                        ChannelManager.setAlert(checked)
                    }
                    text: "Enable notifications"
                }

                Switch {
                    id: notificationsOption
                    enabled: alertOption.checked

                    checked: ChannelManager.offlineNotifications
                    onClicked: {
                        ChannelManager.offlineNotifications = !ChannelManager.offlineNotifications
                    }
                    text: "Show offline notifications"
                }

                OptionCombo {
                    id: alertPosition
                    selection: ChannelManager.getAlertPosition()
                    onSelectionChanged: ChannelManager.setAlertPosition(selection)

                    text: "Notification position"
                    model: ["Top Left", "Top Right", "Bottom Left", "Bottom Right"]
                }
            }
        }

        GroupBox {
            title: "Tray options"
            padding: 10
            width: root.itemWidth
            Layout.alignment: Qt.AlignCenter

            ColumnLayout {
                Switch {
                    id: minStartupOption
                    checked: ChannelManager.isMinimizeOnStartup()
                    onClicked: {
                        ChannelManager.setMinimizeOnStartup(checked)
                    }
                    text: "Start minimized"
                }

                Switch {
                    id: closeToTrayOption
                    checked: ChannelManager.isCloseToTray()
                    onClicked: {
                        ChannelManager.setCloseToTray(checked)
                    }
                    text: "Close to tray"
                }
            }
        }

        GroupBox {
            title: "Chat options"
            padding: 10
            width: root.itemWidth
            Layout.alignment: Qt.AlignCenter

            ColumnLayout {
                Switch {
                    id: chatSwapOption
                    checked: ChannelManager.swapChat
                    onClicked: {
                        ChannelManager.swapChat = !ChannelManager.swapChat
                    }
                    text: "Swap Chat Side"
                }

                OptionCombo {
                    id: textScaleFactor
                    text: "Chat Text Scale Factor"
                    model: ["0.5", "0.75", "1.0", "1.25", "1.5", "1.75", "2.0", "2.5", "3.0"]

                    Component.onCompleted: {
                        var entries = model;

                        var val = ChannelManager.textScaleFactor;
                        for (var i = 0; i < entries.length; i++) {
                            var cur = entries[i];
                            if (parseFloat(cur) == val) {
                                selection = i
                                break;
                            }
                        }
                    }

                    onSelectionChanged: {
                        ChannelManager.textScaleFactor = parseFloat(selection)
                    }
                }
            }
        }

        GroupBox {
            title: "Twitch login"
            padding: 10
            width: root.itemWidth
            Layout.alignment: Qt.AlignCenter

            RowLayout {
                id: loginOption

                Label {
                    id: twitchName
                    text: "Not logged in"
                }

                Button {
                    id: connectButton
                    property bool loggedIn: false
                    text: loggedIn ? "Log out" : "Log in"
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                    }
                    onClicked: {
                        if (!loggedIn) {
                            httpServer.start();
                            var url = "https://api.twitch.tv/kraken/oauth2/authorize?response_type=token&client_id=" + netman.getClientId()
                                    + "&redirect_uri=http://localhost:8979"
                                    + "&scope=user_read%20user_subscriptions%20user_follows_edit%20chat_login%20user_blocks_read%20user_blocks_edit"
                                    + "&force_verify=true";
                            Qt.openUrlExternally(url);


                        }
                        else {
                            ChannelManager.setAccessToken("")
                            ChannelManager.checkFavourites()
                            twitchName.text = "Not logged in"
                        }
                    }

                    Connections {
                        target: ChannelManager
                        onAccessTokenUpdated: {
                            connectButton.loggedIn = ChannelManager.isAccessTokenAvailable()
                        }
                        onUserNameUpdated: {
                            twitchName.text = "Logged in as " + name
                        }
                    }
                }
            }
        }

        GroupBox {
            title: "Version info"
            padding: 10
            width: root.itemWidth
            Layout.alignment: Qt.AlignCenter

            Label {
                text: app_version
            }
        }
    }
}
