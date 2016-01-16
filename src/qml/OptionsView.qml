import QtQuick 2.0
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

            OptionTextInput {
                id: cacheOption
                enabled: false
                visible: false
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }

                text: "Buffer lenght (s)"
                value: g_cman.getCache()
                mask: "99"
            }

            OptionCheckbox {
                id: alertOption
                visible: g_notifications_enabled
                anchors {
                    top: cacheOption.bottom
                    left: parent.left
                    right: parent.right
                }
                checked: g_cman.isAlert()
                onClicked: {
                    g_cman.setAlert(checked)
                }
                text: "Enable notifications"
            }
        }
}
