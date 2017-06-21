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

    property int itemWidth: 300

    RowLayout {
        anchors.fill: parent

        Flow {
            Layout.fillHeight: true
            Layout.alignment: Layout.Center
            Layout.maximumWidth: parent.width

            flow: Flow.TopToBottom
            spacing: 15

            GroupBox {
                title: "Notification settings"
                padding: 10
                width: root.itemWidth
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
                        font.family: mainFont.name

                        checked: Settings.offlineNotifications
                        onClicked: {
                            Settings.offlineNotifications = !Settings.offlineNotifications
                        }
                        text: "Show offline notifications"
                    }

                    OptionCombo {
                        id: alertPosition
                        width: parent.width
                        selection: Settings.alertPosition
                        onActivated: Settings.alertPosition = index

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
                title: "Chat options"
                padding: 10
                width: root.itemWidth
                Layout.alignment: Qt.AlignCenter

                Column {
                    Switch {
                        id: chatSwapOption
                        checked: Settings.swapChat
                        onClicked: {
                            Settings.swapChat = !Settings.swapChat
                        }
                        text: "Swap chat side"
                    }

//                    UILabel {
//                        text: "Font scale"
//                    }

//                    SpinBox {
//                        id: textScaleFactor
//                        padding: 0
//                        font.pointSize: 10
//                        from: 50
//                        to: 300
//                        stepSize: 25

//                        property real realValue: value / 100
//                        value: Settings.textScaleFactor * 100

//                        textFromValue: function(value, locale) {
//                            return Number(value / 100).toLocaleString(locale, 'f', 2)
//                        }

//                        valueFromText: function(text, locale) {
//                            return Number.fromLocaleString(locale, text) * 100
//                        }

//                        onValueModified: {
//                            Settings.textScaleFactor = realValue
//                        }
//                    }
                }
            }

            GroupBox {
                title: "Twitch login"
                padding: 10
                width: root.itemWidth
                Layout.alignment: Qt.AlignCenter

                RowLayout {
                    UILabel {
                        id: twitchName
                        text: "Not logged in"
                        Layout.fillWidth: true
                        clip: true
                    }

                    Button {
                        id: connectButton
                        property bool loggedIn: Settings.hasAccessToken
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
                UILabel {
                    text: Settings.appName() + " " + Settings.appVersion()
                }
            }
        }
    }
}
