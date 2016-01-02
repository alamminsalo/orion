import QtQuick 2.5
import QtQuick.Controls 1.4
import "../styles.js" as Styles

//ToolBox.qml
Rectangle {
    id: root
    color: Styles.sidebarBg
    width: 200

    property bool isOpen: true
    property int selectedView: 0

    Behavior on width {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutCubic
        }
    }

    function toggle(){
        root.width = isOpen ? 50 : 200
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
