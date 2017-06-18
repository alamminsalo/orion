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
import "styles.js" as Styles

Page {
    id: root

    ColumnLayout {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        Switch {
            id: alertOption
            checked: g_cman.isAlert()
            onClicked: {
                g_cman.setAlert(checked)
            }
            text: "Enable notifications"
        }

        Switch {
            id: notificationsOption
            enabled: alertOption.checked

            checked: g_cman.offlineNotifications
            onClicked: {
                g_cman.offlineNotifications = !g_cman.offlineNotifications
            }
            text: "Show offline notifications"
        }

        OptionCombo {
            id: alertPosition
            selection: g_cman.getAlertPosition()
            onSelectionChanged: g_cman.setAlertPosition(selection)

            text: "Notification position"
            model: ["Top Left", "Top Right", "Bottom Left", "Bottom Right"]
        }

        Switch {
            id: minStartupOption
            checked: g_cman.isMinimizeOnStartup()
            onClicked: {
                g_cman.setMinimizeOnStartup(checked)
            }
            text: "Start minimized"
        }

        Switch {
            id: closeToTrayOption
            checked: g_cman.isCloseToTray()
            onClicked: {
                g_cman.setCloseToTray(checked)
            }
            text: "Close to tray"
        }

        Switch {
            id: chatSwapOption
            checked: g_cman.swapChat
            onClicked: {
                g_cman.swapChat = !g_cman.swapChat
            }
            text: "Swap Chat Side"
        }

        OptionCombo {
            id: textScaleFactor
            text: "Chat Text Scale Factor"
            model: ["0.5", "0.75", "1.0", "1.25", "1.5", "1.75", "2.0", "2.5", "3.0"]

            Component.onCompleted: {
                var entries = model;

                var val = g_cman.textScaleFactor;
                for (var i = 0; i < entries.length; i++) {
                    var cur = entries[i];
                    if (parseFloat(cur) == val) {
                        selection = i
                        break;
                    }
                }
            }

            onSelectionChanged: {
                g_cman.textScaleFactor = parseFloat(selection)
            }
        }

        RowLayout {
            id: loginOption

            Label {
                id: twitchName
                text: "Twitch account"
            }

            Button {
                id: connectButton

                text: g_cman.isAccessTokenAvailable() ? "Log out" : "Log in"
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                }
                onClicked: {
                    if (!g_cman.isAccessTokenAvailable()) {
                        httpServer.start();
                        var url = "https://api.twitch.tv/kraken/oauth2/authorize?response_type=token&client_id=" + netman.getClientId()
                                + "&redirect_uri=http://localhost:8979"
                                + "&scope=user_read%20user_subscriptions%20user_follows_edit%20chat_login%20user_blocks_read%20user_blocks_edit"
                                + "&force_verify=true";
                        Qt.openUrlExternally(url);


                    }
                    else {
                        g_cman.setAccessToken("")
                        g_cman.checkFavourites()
                        twitchName.text = "Twitch account"
                    }
                }

                Connections {
                    target: g_cman
                    onAccessTokenUpdated: {
                        connectButton.loggedIn = g_cman.isAccessTokenAvailable()
                    }
                    onUserNameUpdated: {
                        twitchName.text = "Logged in as " + name
                    }
                }
            }
        }
    }

    footer: ToolBar {
        padding: 10
        Label {
            text: app_version
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
