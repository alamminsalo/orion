import QtQuick.Controls 1.4
import QtQuick 2.0
import "components"

Item{
    anchors.fill: parent
    property variant selectedChannel
    property int itemCount: 0

    function search(channel){
        selectedChannel = channel;

        g_vodmgr.search(selectedChannel.name, 0, 50)

        itemCount = 50

        requestSelectionChange(4)
    }

    ViewHeader{
        id: header
        text: ""
        z: favourites.z + 1
    }

    onSelectedChannelChanged: {
        header.text = "Videos for " + selectedChannel.title
    }

    onVisibleChanged: {
        vods.checkScroll()
    }

    ChannelGrid {
        id: vods
        tooltipEnabled: true

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom : parent.bottom
            margins: dp(10)
        }

        model: vodsModel
        delegate: Video {
            _id: model.id
            title: model.title
            views: model.views
            preview: model.preview
            duration: model.duration
            game: model.game
        }

        onItemClicked: {
            player.play(selectedChannel, selectedItem)
        }

        onItemRightClicked: {
            _menu.item = selectedItem
            _menu.popup()
        }

        ContextMenu {
            id: _menu
            MenuItem {
                text: "Watch;play"
                onTriggered: {
                    player.play(selectedChannel, _menu.item)
                }
            }
        }

        onAtYEndChanged: checkScroll()

        function checkScroll(){
            if (atYEnd && model.count() === itemCount && itemCount > 0){
                g_vodmgr.search(selectedChannel.name, itemCount, 25)
                itemCount += 25
            }
        }
    }
}
