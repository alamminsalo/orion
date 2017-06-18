import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.1
import "components"

ToolBar {
    id: root
    padding: 20
    height: searchBtn.checked ? implicitHeight : 0

    property alias text: _input.text
    property alias input: _input
    signal submit()

    Behavior on height {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }
    
    Material.background: Material.background
    Material.foreground: Material.Grey
    
    RowLayout {
        anchors.fill: parent
        TextField {
            id: _input
            onAccepted: submit()
            inputMethodHints: Qt.ImhNoPredictiveText
            Layout.fillWidth: true
            visible: root.height === root.implicitHeight
            font.pointSize: 16
            onVisibleChanged:  {
                if (visible)
                    forceActiveFocus()
            }
        }
        
        Item {
            Layout.fillWidth: true
            visible : !_input.visible
        }
        
        RoundButton {
            id: searchBtn
            anchors.top: parent.top
            anchors.right: parent.right
            text: "\ue8b6"
            Material.foreground: "#fff"
            font.pointSize: 20
            padding: 20
            font.family: "Material Icons"
            checkable: true
            checked: false
        }
    }
}
