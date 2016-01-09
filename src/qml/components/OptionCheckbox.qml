import QtQuick 2.0
import "../styles.js" as Styles

Item {
    property string text
    property bool checked: true

    id: root
    height: dp(60)

    Text {
        font.family: "Droid Sans"
        color: Styles.textColor
        font.pixelSize: Styles.titleFont.bigger
        text: root.text
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

        width: dp(36)

        radius: dp(5)
        color: Styles.sidebarBg

        MouseArea{
            anchors.fill: parent

            Icon {
                icon: "check"
                anchors.fill: parent
                iconSize: dp(24)
                visible: root.checked
            }
        }
    }
}
