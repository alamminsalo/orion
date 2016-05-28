import QtQuick 2.0
import "../styles.js" as Styles

Rectangle {
    id: root
    property string text
    property string bgColor: Styles.grayButtonColor
    property string borderColor: "#ffffff"

    width: content.contentWidth + dp(10)
    height: content.contentHeight + dp(10)

    signal clicked()

    color: bgColor

    border {
        width: dp(1)
        color: borderColor
    }

    Text {
        id: content
        color: Styles.textColor
        font.pixelSize: Styles.titleFont.bigger
        text: root.text
        anchors {
            centerIn: parent
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    MouseArea {
        anchors.fill: parent

        hoverEnabled: true

        onHoveredChanged: {
            if (containsMouse)
                root.bgColor = Styles.purple
            else
                root.bgColor = Styles.grayButtonColor
        }

        onClicked: root.clicked()
    }
}
