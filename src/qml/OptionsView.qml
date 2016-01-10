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

        anchors {
            top: header.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        Item {
            width: dp(360)
            height: parent.height / 1.5

            anchors {
                left: parent.left
                right: parent.right
                centerIn: parent
            }

            OptionTextInput {
                id: cacheOption
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
                anchors {
                    top: cacheOption.bottom
                    left: parent.left
                    right: parent.right
                }
                checked: g_cman.isAlert()
                text: "Enable alerts"
            }

            Item {
                //Spacer and submit button
                height: dp(120)
                anchors {
                    left: parent.left
                    right: parent.right
                    top: alertOption.bottom
                }

                PurpleButton {
                    text: "Apply"
                    width: dp(90)
                    height: dp(40)
                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                    }

                    border.color: Styles.iconColor
                    border.width: dp(1)

                    onButtonPressed: {
                        g_cman.setCache(cacheOption.getValue())
                        g_cman.setAlert(alertOption.checked)
                    }
                }
            }
        }
    }
}
