import QtQuick 2.5
import "components"
import "styles.js" as Styles

import mpv 1.0


Rectangle {
    property int selection
    id: root

    signal requestSelectionChange(int index)

    color: Styles.bg

    onSelectionChanged: {
        search.visible = false
        favourites.visible = false
        games.visible = false
        featured.visible = false
        player.visible = false
        settings.visible = false

        switch (selection){

        //Search
        case 0:
            search.visible = true
            search.focusInput()
            break

        //Featured
        case 1:
            featured.visible = true
            break

        //Fav
        case 2:
            favourites.visible = true
            break

        //Games
        case 3:
            games.visible = true
            if (games.gamesCount === 0){
                g_cman.getGames(0, 25, true)
                games.gamesCount = 25
            }
            break

        //Player
        case 4:
            player.visible = true
            break

        //Settings
        case 5:
            settings.visible = true
            break
        }
    }

//    onVisibleChanged: {
//        if (visible)
//            games.checkScroll()
//    }

    SearchView {
        id: search
        visible: false
    }

    FeaturedView{
        id: featured
        visible: false
    }

    FavouritesView{
        id: favourites
        visible: false
    }

    GamesView {
        id: games
        visible: false
    }

    PlayerView{
        id: player
        visible: false
    }

    OptionsView{
        id: settings
        visible: false
    }



    Rectangle {
        id: _gradient
        visible: !player.visible
        height: root.height * .33
        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 1.0; color: "#111111" }
        }

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    onHeightChanged: _gradient.height = height * .33

}
