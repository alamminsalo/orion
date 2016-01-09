import QtQuick 2.0
import "components"
import "styles.js" as Styles

Item{
    anchors.fill: parent
    id: root

    ViewHeader{
        id: header
        text: "Settings"
    }

    Item {
        anchors {
            top: header.bottom
            left: parent.left
            bottom: parent.bottom
            right: parent.right
            margins: dp(40)
        }

        Rectangle {
            height: settings.height
            width: settings.width
            anchors {
                centerIn: parent
            }
            color: "transparent"
            radius: dp(5)
            border.color: Styles.border
            border.width: dp(1)

            ListView {
                id: settings
                height: model.count * dp(60) + dp(10)
                width: dp(500) + dp(10)
                anchors.margins: dp(10)

                model: ListModel{
                    ListElement {
                        label: "Player cache (s)"
                        type: "input"
                        initialValue: "10"
                    }
                    ListElement {
                        label: "Show popups"
                        type: "checkbox"
                        initialValue: "0"
                    }
                }

                delegate: Item {
                    height: dp(60)

                    property string value: initialValue

                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    Text {
                        text: label
                        font.family: "Droid Sans"
                        color: Styles.textColor
                        font.pixelSize: Styles.titleFont.bigger
                        anchors {
                            left: parent.left
                            top: parent.top
                            bottom: parent.bottom
                            leftMargin: dp(10)
                        }
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Rectangle {
                        anchors {
                            right: parent.right
                            top: parent.top
                            bottom: parent.bottom
                            margins: dp(10)
                        }

                        radius: dp(5)

                        width: type === "input" ? dp(60) : dp(40)
                        color: Styles.sidebarBg

                        MouseArea{
                            anchors.fill: parent
                            cursorShape: type === "input" ? Qt.IBeamCursor : Qt.ArrowCursor
                        }

                        TextInput{
                            visible: type === "input"
                            id: _input
                            color: Styles.iconColor
                            anchors.fill: parent
                            clip:true
                            selectionColor: Styles.purple
                            focus: true
                            selectByMouse: true
                            font.pointSize: dp(Styles.titleFont.bigger)
                            text: value
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Icon {
                            id: _check
                            visible: type === "checkbox"
                            anchors.fill: parent
                            icon: "check"
                            iconSize: dp(20)
                        }
                    }
                }
            }
        }
    }
}
