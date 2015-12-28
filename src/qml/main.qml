import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import "components"
import "styles.js" as Style

Window {
    id: root
    visible: true
    width: 800
    height: 600

    property variant g_rootWindow: root
    property variant g_tooltip

    Rectangle {
        anchors.fill: parent

        ToolBox {
            id: tools
            width: 200
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
        }

        Rectangle {
            color: Style.twitch.bg

            anchors {
                left: tools.right
                top: parent.top
                right: parent.right
                bottom: parent.bottom
            }

            Item {
                id: spacer_l
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }
                width: 10
            }

            Item {
                id: spacer_t
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                height: 10
            }

            ChannelList {
                id: channels
                anchors {
                    top: spacer_t.bottom
                    bottom: parent.bottom
                    left: spacer_l.right
                    right: parent.right
                }
            }
        }
    }



    Component.onCompleted: {
        var component = Qt.createComponent("components/Tooltip.qml")
        g_tooltip = component.createObject(root);

        g_cman.checkStreams()
        pollTimer.start()
    }

    Timer {
        id: pollTimer
        interval: 10000
        running: false
        repeat: true
        onTriggered: {
            g_cman.checkStreams()
        }
    }
}

