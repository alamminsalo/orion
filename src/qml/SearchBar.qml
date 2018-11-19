import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.1
import "components"

ToolBar {
    id: root
    padding: 20
    height: searchBtn.checked ? implicitHeight : 0

    Material.theme: rootWindow.Material.theme
    Material.background: rootWindow.Material.background

    property alias text: _input.text
    property alias input: _input
    signal submit()

    Behavior on height {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }

    // Workaround when resizing window and height == 0, Layout will not be updated otherwise
    onWidthChanged: spacer.maximumWidth = width
    
    RowLayout {
        anchors.fill: parent
        TextField {
            id: _input
            onAccepted: submit()
            inputMethodHints: Qt.ImhNoPredictiveText
            selectByMouse: true
            Layout.fillWidth: true
            visible: searchBtn.checked
            font.pointSize: 16
            onVisibleChanged:  {
                if (visible)
                    forceActiveFocus()
            }
            maximumLength: 48
        }

        Item {
            id: spacer
            Layout.fillWidth: true
            property real maximumWidth: 0
            Component.onCompleted: maximumWidth = parent.width
            Layout.maximumWidth: maximumWidth
            visible: !_input.visible
        }

        IconButtonFlat {
            id: clearBtn
            Layout.alignment: Qt.AlignRight
            text: "\ue14c"
            visible: _input.visible && _input.text.length > 0
            onClicked: _input.clear()
        }

        RoundButton {
            Layout.alignment: Qt.AlignRight
            id: searchBtn
            text: "\ue8b6"
            Material.foreground: "#fff"
            font.pointSize: 20
            padding: 20
            font.family: "Material Icons"
            checkable: true
            checked: false
            focusPolicy: Qt.NoFocus

            onClicked: {
                if (!checked) {
                    submit()
                }
            }

            onDoubleClicked: {
                if (!isMobile()){
                    //Clear search bar and submit, this makes it easy to quickly search for featured streams, games
                    _input.clear()
                    submit()
                }
            }
        }
    }
}
