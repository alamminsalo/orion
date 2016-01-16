import QtQuick 2.0
import QtQuick.Controls 1.4
import "components"
import "styles.js" as Style

Item {
    id: root
    anchors.fill: parent
    property int gamesCount: 0
    property bool checked: false

    ViewHeader{
        id: header
        text: "All games"
        z: games.z + 1
    }

    onVisibleChanged: {
        if (visible && !checked){
            g_cman.getGames(0, 25, true)
            gamesCount = 25
            checked = true
            timer.start()
        }
    }

    ChannelGrid {
        id: games
        tooltipEnabled: true

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: dp(10)
        }

        model: g_games
        delegate: Channel {
            title: model.name
            logo: model.logo
            preview: model.preview
            viewers: model.viewers
            online: true
        }

        function checkScroll(){
            if (atYEnd && model.count() === gamesCount && gamesCount > 0){
                g_cman.getGames(gamesCount, 25, false);
                gamesCount += 25
            }
        }

        onItemClicked: {
            search.search(":game " + currentItem.title)
            requestSelectionChange(0)
        }

        onItemRightClicked: {
            _menu.item = currentItem
            _menu.popup()
        }

        onAtYEndChanged: checkScroll()

        Connections {
            target: g_cman
            onGamesUpdated: {
                games.checkScroll()
            }
        }

        ContextMenu {
            id: _menu
            MenuItem {
                text: "Find streams;search"
                onTriggered: {
                    search.search(":game " + _menu.item.title)
                    requestSelectionChange(0)
                }
            }
        }

        Timer {
            id: timer
            interval: 30000
            running: false
            repeat: false
            onTriggered: {
                root.checked = false
            }
        }
    }
}
