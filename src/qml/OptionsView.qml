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
            id: closeToTrayOption
            anchors {
                top: alertPosition.bottom
                left: parent.left
                right: parent.right
            }
            checked: g_cman.isCloseToTray()
            onClicked: {
                g_cman.setCloseToTray(checked)
            }
            text: "Close to tray"
        }

        OptionEntry {
            id: loginOption
            text: "Twitch account"
            anchors {
                top: closeToTrayOption.bottom
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
