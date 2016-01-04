import QtQuick 2.0
import QtQuick.Controls 1.4
import "components"

ChannelGrid {
    id: games
    property int gamesCount: 0

    visible: false
    tooltipEnabled: true

    anchors {
        fill: parent
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
}
