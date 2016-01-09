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
            height: parent.height

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
                value: "10"
                mask: "99"
            }

            OptionCheckbox {
                anchors {
                    top: cacheOption.bottom
                    left: parent.left
                    right: parent.right
                }

                text: "Enable alerts"
            }
        }
    }
}
