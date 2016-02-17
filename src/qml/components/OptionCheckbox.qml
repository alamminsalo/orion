import QtQuick 2.0
import "../styles.js" as Styles

Item {
    property string text
    property bool checked: true

    signal clicked()

    id: root
    height: dp(60)

    Text {
        font.family: "Droid Sans"
        color: Styles.textColor
        font.pointSize: dp(Styles.titleFont.bigger)
        text: root.text
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: dp(10)
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        renderType: Text.NativeRendering
    }

    Rectangle {
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            margins: dp(10)
        }

        width: dp(36)
        radius: dp(2)
        color: Styles.sidebarBg

        MouseArea{
            anchors.fill: parent

            onClicked: {
                root.checked = !root.checked
                root.clicked()
            }

            Icon {
                icon: "check"
                anchors.fill: parent
                iconSize: dp(20)
                visible: root.checked
            }
        }
    }
}
