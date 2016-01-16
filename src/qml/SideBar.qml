import QtQuick 2.5
import QtQuick.Controls 1.4
import "components"
import "styles.js" as Styles

//ToolBox.qml
Rectangle {
    id: root
    color: Styles.sidebarBg
    property int _width: dp(230)
    width: visible ? _width : 0
    clip: true
    z: 99

    property bool isOpen: true
    property int selectedView: 0

    Behavior on width {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutCubic
        }
    }

    function toggle(){
        root._width = isOpen ? dp(60) : dp(230)
        isOpen = !isOpen
    }

    function setView(index){
        if (index > -1 && index <= ribbonList.count)
            ribbonList.currentIndex = index
    }

    DrawerRibbon {
        id: toggleButton
        iconStr: 'chevron_l'
        onClick: toggle()
        highlightOn: false
    }

    RibbonList {
        id: ribbonList
        height: model.count * dp(60)

        onCurrentIndexChanged: {
            selectedView = currentIndex
        }

        anchors {
            top: toggleButton.bottom
            left: parent.left
            right: parent.right
        }

        model: ListModel {
            id: model
            ListElement {
                label: "Search"
                icon: "search"
            }
            ListElement {
                label: "Featured"
                icon: "featured"
            }
            ListElement {
                label: "Favourites"
                icon: "fav"
            }
            ListElement {
                label: "Games"
                icon: "game"
            }
            ListElement {
                label: "Watch"
                icon: "play"
            }
        }

        delegate: Ribbon {
            text: label
            iconStr: icon
        }

        Component.onCompleted: {
            //Add more components with OR as they are added
            if (g_notifications_enabled){
                model.append({"label":"Settings", "icon":"settings"})
            }
        }
    }

    Rectangle {
        id: border
        width: dp(1)
        color: Styles.border
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
    }
}
