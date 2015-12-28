import QtQuick 2.0
import QtQuick.Controls 1.1
import "../styles.js" as Style

//ToolBox.qml
Rectangle {
    id: root
    color: Style.twitch.sidebarBg
    width: 200

    property bool isOpen: false
    property int selectedView: 0

    Component.onCompleted: {
        toggle()
    }

    NumberAnimation on width {
        id: toolbox_drawer_anim
        duration: 200
        easing.type: Easing.OutCubic
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
           toggle()
        }
    }

    function toggle(){
        toolbox_drawer_anim.stop()
        toolbox_drawer_anim.from = width
        toolbox_drawer_anim.to = isOpen ? 50 : 200
        toolbox_drawer_anim.start()
        isOpen = !isOpen
    }


    Item {
        id: searchContainer
        anchors {
            left: parent.left
            right: parent.right
        }
        height: 200
    }

    RibbonList {
        id: ribbonList

        onSelectionChanged: {
            root.selectedView = selection.id
            console.log(root.selectedView)
        }

        anchors {
            top: searchContainer.bottom
            left: parent.left
            right: parent.right
        }

        model: ListModel {
            ListElement {
                index: 0
                label: "Favourites"
            }
            ListElement {
                index: 1
                label: "Games"
            }
        }
    }
}
