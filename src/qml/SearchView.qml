import QtQuick 2.5
import QtQuick.Controls 1.4
import "components"
import "styles.js" as Styles

Item {
    property int itemCount: 0

    anchors.fill: parent

    function focusInput(){
        _input.forceActiveFocus()
    }

    function search(str, offset, limit, clear){
        _label.visible = false
        str = str || _input.text
        offset = offset || 0
        limit = limit || 25

        if (typeof clear === 'undefined'){
            clear = true
        }

        if (str.length > 0){
            _input.text = str
            g_cman.searchChannels(str, offset, limit, clear)
            if (clear){
                itemCount = limit
            }
        }
    }

    onVisibleChanged: {
        if (visible)
            channels.checkScrolled()
    }

    Connections {
        target: g_cman
        onResultsUpdated: {
            _spinner.visible = false
            _button.visible = true
            channels.checkScrolled()
        }

        onSearchingStarted: {
            _spinner.visible = true
            _button.visible = false
        }
    }

    Rectangle {
        id: searchContainer
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: dp(100)
        color: Styles.bg
        z: channels.z + 1
        clip: true

        Rectangle {
            property string text: _input.text

            id: searchBox
            height: dp(60)
            width: _input.width + _spacer.width + _button.width
            color: Styles.sidebarBg
            radius: 5
            anchors.centerIn: parent
            anchors.margins: dp(10)
            border.color: Styles.border
            border.width: dp(1)
            clip: true

            TextInput{
                id: _input
                color: Styles.iconColor
                width: dp(300)
                clip:true
                selectionColor: Styles.purple
                focus: true
                selectByMouse: true

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

                Keys.onReturnPressed: search()

                MouseArea {
                    onDoubleClicked: _input.selectAll()
                }
            }

            Rectangle {
                id: _spacer
                width: dp(1)
                color: Styles.border
                anchors {
                    left: _input.right
                    top: parent.top
                    bottom: parent.bottom
                }
            }

            Icon {
                id: _button
                icon: "search"
                iconSize: dp(20)
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: _spacer.right
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        search()
                    }

                    onHoveredChanged: {
                        parent.iconColor = containsMouse ? Styles.iconHighlight : Styles.iconColor
                    }
                }
            }

            SpinnerIcon {
                id: _spinner
                iconSize: dp(20)
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: _spacer.right
                }
                visible: false
            }
        }
    }

    ChannelGrid {
        id: channels
        tooltipEnabled: true

        anchors {
            top: searchContainer.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        model: g_results
        delegate: Channel {
            _id: model.id
            name: model.serviceName
            title: model.name
            logo: model.logo
            info: model.info
            viewers: model.viewers
            preview: model.preview
            online: model.online
            game: model.game
            favourite: model.favourite
        }

        function checkScrolled(){
            if (atYEnd && model.count() === itemCount && itemCount > 0){
                search(_input.text, itemCount, 25, false);
                itemCount += 25
            }
        }

        onAtYEndChanged: checkScrolled()

        onItemClicked: {
            //Play
        }

        onItemRightClicked: {
            _menu.item = currentItem

            var item = _menu.items[1]
            item.text = !_menu.item.favourite ? "Add favourite;fav" : "Remove favourite;remove"
            _menu.state = !_menu.item.favourite ? 1 : 2

            _menu.popup()
        }

        ContextMenu {
            id: _menu

            function addRemoveFavourite(){
                if (state === 1){
                    g_cman.addToFavourites(_menu.item._id)
                } else if (state === 2){
                    g_cman.removeFromFavourites(_menu.item._id)
                }
            }

            MenuItem {
                text: "Watch;play"
                onTriggered: {
                    g_cman.openStream(_menu.item.name)
                }
            }

            MenuItem {
                id: _fav
                onTriggered: {
                    _menu.addRemoveFavourite()
                }
            }
        }

        Text {
            id: _label
            text: "Search for channels"
            font.pointSize: dp(40)
            color: Styles.iconColor
            anchors.centerIn: parent
        }
    }
}
