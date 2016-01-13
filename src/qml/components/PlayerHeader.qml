import QtQuick 2.0
import "../styles.js" as Styles

Item {
    property string text
    property int thickness: 100
    property string color: Styles.shadeColor
    id: root

    height: dp(thickness)

    Rectangle {
        id: shade
        color: root.color
        opacity: .8
        anchors.fill: parent
    }

    Item {
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: dp(20)
        }
        width: _text.width

        Text {
            id: _text
            anchors.centerIn: parent
            color: Styles.textColor
            font.pixelSize: dp(30)
            text: root.text
            font.family: "Droid Sans"
            z: root.z + 1
        }
    }

    Behavior on height {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutCubic
        }
    }

    function show(){
        height = dp(thickness)
        headerTimer.restart()
    }

    function hide(){
        height = -dp(thickness)
    }

    Component.onCompleted: hide()

    Timer {
        id: headerTimer
        interval: 5000
        running: false
        repeat: false
        onTriggered: {
            hide()
        }
    }
}
