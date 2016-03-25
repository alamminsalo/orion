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
        searchView.visible = false
        favouritesView.visible = false
        gamesView.visible = false
        featuredView.visible = false
        playerView.visible = false
        settingsView.visible = false
        vodsView.visible = false

        switch (selection){

        //Search
        case 0:
            searchView.visible = true
            searchView.focusInput()
            break

        //Featured
        case 1:
            featuredView.visible = true
            break

        //Fav
        case 2:
            favouritesView.visible = true
            break

        //Games
        case 3:
            gamesView.visible = true
            break

        //Vods
        case 4:
            vodsView.visible = true
            break

        //Player
        case 5:
            playerView.visible = true
            break

        //Settings
        case 6:
            settingsView.visible = true
            break
        }
    }

    SearchView {
        id: searchView
        visible: false
    }

    FeaturedView{
        id: featuredView
        visible: false
    }

    FavouritesView{
        id: favouritesView
        visible: false
    }

    GamesView {
        id: gamesView
        visible: false
    }

    VodsView {
        id: vodsView
        visible: false
    }

    PlayerView{
        id: playerView
        visible: false
    }

    OptionsView{
        id: settingsView
        visible: false
    }



    Rectangle {
        id: _gradient
        visible: !playerView.visible
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
