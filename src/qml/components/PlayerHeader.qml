import QtQuick 2.5
import "../styles.js" as Styles

Item {
    property string text
    property int thickness: dp(60)
    property string color: Styles.shadeColor
    id: root

    height: thickness

    Rectangle {
        id: shade
        color: root.color
        opacity: .8
        anchors.fill: parent
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
