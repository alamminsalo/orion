import QtQuick 2.0
import "components"
import "styles.js" as Style

Rectangle {
    property int selection: 0

    color: Style.twitch.bg

    function setView(index){
        switch (index){
            case 0:
                favourites.visible = true
                games.visible = false
                break

            case 1:
                favourites.visible = false
                games.visible = true
                g_cman.getGames()
                break
        }
        selection = index
    }

    ChannelGrid {
        id: favourites
        cellSize: 200
        tooltipEnabled: true
        visible: false

        anchors {
            fill: parent
            margins: 10
        }

        model: g_cman.channels
        delegate: Channel {
            title: modelData.name
            logo: modelData.logo
            info: modelData.info
            viewers: modelData.viewers
            preview: modelData.preview
            online: modelData.online
            containerSize: favourites.cellHeight
        }
    }

    ChannelGrid {
        id: games
        cellSize: 200
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
    }

}
