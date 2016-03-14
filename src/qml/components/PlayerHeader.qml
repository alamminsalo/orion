import QtQuick 2.0
import "../styles.js" as Styles

Item {
    property string text
    property int thickness: dp(80)
    property string color: Styles.shadeColor
    id: root

    height: thickness

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
            font.pointSize: Styles.titleFont.bigger
            text: root.text
            font.family: "Droid Sans"
            z: root.z + 1
            //renderType: Text.NativeRendering
        }
    }

    Behavior on height {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutCubic
        }
    }

    function show(){
        height = thickness
    }

    function hide(){
        height = -thickness
    }

    Component.onCompleted: hide()
}
