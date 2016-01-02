import QtQuick 2.0
import "components"
import "styles.js" as Style

Rectangle {
    property int selection

    signal requestSelectionChange(int index)

    color: Style.bg

    onSelectionChanged: {
        search.visible = false
        favourites.visible = false
        games.visible = false

        switch (selection){
            //Search
            case 0:
                search.visible = true
                search.focusInput()
                break;

            //Fav
            case 1:
                favourites.visible = true
                break

            //Games
            case 2:
                games.visible = true
                if (games.gamesCount === 0){
                    g_cman.getGames(0, 25, true)
                    games.gamesCount = 25
                }
        }
    }

    onVisibleChanged: {
        if (visible)
            games.checkScroll()
    }

    SearchView {
        id: search
        visible: false
    }

    ChannelGrid {
        id: favourites
        tooltipEnabled: true
        visible: false

        anchors {
            fill: parent
            margins: 10
        }

        model: g_favourites
        delegate: Channel {
            name: model.serviceName
            title: model.name
            logo: model.logo
            info: model.info
            viewers: model.viewers
            preview: model.preview
            online: model.online
            containerSize: favourites.cellHeight
        }

        onItemClicked: {
            g_cman.removeFromFavourites(currentItem.name)
        }
    }

    ChannelGrid {
        id: games
        property int gamesCount: 0

        visible: false
        tooltipEnabled: true

        anchors {
            fill: parent
            margins: 10
        }

        model: g_games
        delegate: Channel {
            title: model.name
            logo: model.logo
            preview: model.preview
            viewers: model.viewers
            online: true
            containerSize: favourites.cellHeight
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

        onAtYEndChanged: checkScroll()

        Connections {
            target: g_cman
            onGamesUpdated: {
                games.checkScroll()
            }
        }
    }

}
