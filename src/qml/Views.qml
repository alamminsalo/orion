import QtQuick 2.5
import "components"
import "styles.js" as Styles

Rectangle {
    property int selection

    signal requestSelectionChange(int index)

    color: Styles.bg

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
                break;
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

    FavouritesView{
        id: favourites
        visible: false
    }

    GamesView {
        id: games
        visible: false
    }

}
