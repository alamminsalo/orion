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
import "components"
import "styles.js" as Styles

Item{
    anchors.fill: parent
    id: root

    ViewHeader {
        id: header
        text: "Settings"
    }

    Item {
        height: parent.height
        width: dp(360)

        anchors {
            top: header.bottom
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }

        OptionCheckbox {
            id: alertOption
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            checked: g_cman.isAlert()
            onClicked: {
                g_cman.setAlert(checked)
            }
            text: "Enable notifications"
        }

        OptionCombo {
            id: alertPosition
            selection: g_cman.getAlertPosition()

            onSelectionChanged: {
                g_cman.setAlertPosition(selection)
            }

            anchors {
                top: alertOption.bottom
                left: parent.left
                right: parent.right
            }
            text: "Notification position"
        }

        OptionCheckbox {
            id: minStartupOption
            anchors {
                top: alertPosition.bottom
                left: parent.left
                right: parent.right
            }
            checked: g_cman.isMinimizeOnStartup()
            onClicked: {
                g_cman.setMinimizeOnStartup(checked)
            }
            text: "Start minimized"
        }

        OptionCheckbox {
            id: closeToTrayOption
            anchors {
                top: minStartupOption.bottom
                left: parent.left
                right: parent.right
            }
            checked: g_cman.isCloseToTray()
            onClicked: {
                g_cman.setCloseToTray(checked)
            }
            text: "Close to tray"
        }
	
        OptionCheckbox {
            id: chatSwapOption
            anchors {
                top: closeToTrayOption.bottom
                left: parent.left
                right: parent.right
            }
            checked: g_cman.swapChat
            onClicked: {
                g_cman.swapChat = !g_cman.swapChat
            }
            text: "Swap Chat Side"
        }

        OptionEntry {
            id: loginOption
            text: "Twitch account"
            anchors {
                top: chatSwapOption.bottom
                left: parent.left
                right: parent.right
            }
            BasicButton {
                id: connectButton

                property bool loggedIn: g_cman.isAccessTokenAvailable()
                text: loggedIn ? "Log out" : "Log in"
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                }
                onClicked: {
                    if (!loggedIn)
                        webView.requestAccessToken()
                    else {
                        webView.logout()
                        loginOption.text = "Twitch account"
                    }
                }

                Connections {
                    target: g_cman
                    onAccessTokenUpdated: {
                        connectButton.loggedIn = g_cman.isAccessTokenAvailable()
                    }
                    onUserNameUpdated: {
                        loginOption.text = "Logged in as " + name
                    }
                }
            }
        }
    }
}
