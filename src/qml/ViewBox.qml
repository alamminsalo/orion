import QtQuick 2.0
import "components"
import "styles.js" as Style

Rectangle {
    property int selection

    color: Style.bg

    onSelectionChanged: {
        search.visible = false
        favourites.visible = false
        games.visible = false

        switch (selection){
            //Search
            case 0:
                search.visible = true
                break;

            //Fav
            case 1:
                favourites.visible = true
                break

            //Games
            case 2:
                games.visible = true
                if (games.count === 0)
                    games.getGames()
                break
        }
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
            title: model.name
            logo: model.logo
            info: model.info
            viewers: model.viewers
            preview: model.preview
            online: model.online
            containerSize: favourites.cellHeight
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

        model: g_cman.games
        delegate: Channel {
            title: modelData.name
            logo: modelData.logo
            preview: modelData.preview
            viewers: modelData.viewers
            online: true
            containerSize: favourites.cellHeight
        }

        function getGames(){
            if (games.count === games.gamesCount){
                g_cman.getGames();
                games.gamesCount += 25
            }
        }

        onContentYChanged: {
            if (contentHeight - contentY - height <= 0){
                games.getGames();
            }
        }
    }

}
