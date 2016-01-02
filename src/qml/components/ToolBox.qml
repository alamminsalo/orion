import QtQuick 2.0
import QtQuick.Controls 1.1
import "../styles.js" as Style

//ToolBox.qml
Rectangle {
    id: root
    color: Style.sidebarBg
    width: 200

    property bool isOpen: true
    property int selectedView: 0

    NumberAnimation on width {
        id: toolbox_drawer_anim
        duration: 200
        easing.type: Easing.OutCubic
    }

    function toggle(){
        toolbox_drawer_anim.stop()
        toolbox_drawer_anim.from = width
        toolbox_drawer_anim.to = isOpen ? 50 : 200
        toolbox_drawer_anim.start()
        isOpen = !isOpen
        toggleButton.iconStr = isOpen ? 'chevron_l' : 'chevron_r'
    }

    function setView(index){
        if (index > -1 && index <= ribbonList.count)
            ribbonList.currentIndex = index
    }

    Ribbon {
        id: toggleButton
        iconStr: 'chevron_l'
        MouseArea {
            anchors.fill: parent
            onClicked: toggle()
        }
        highlightOn: false
    }

    RibbonList {
        id: ribbonList

        onCurrentIndexChanged: {
            selectedView = currentIndex
        }

        anchors {
            top: toggleButton.bottom
            left: parent.left
            right: parent.right
        }

        model: ListModel {
            ListElement {
                label: "Search"
                icon: "search"
            }
            ListElement {
                label: "Favourites"
                icon: "fav"
            }
            ListElement {
                label: "Games"
                icon: "game"
            }
        }

        delegate: Ribbon {
            text: label
            iconStr: icon
        }
    }
}
