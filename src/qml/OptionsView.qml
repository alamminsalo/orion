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

    //NotificationMaker is easier to just put here because all the options are near
    NotificationMaker{
        id: notifMaker
        position: alertPosition.selection
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

    }
}
