/*
 * Copyright © 2015-2016 Antti Lamminsalo
 *
 * This file is part of Orion.
 *
 * Orion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with Orion.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.5
import "components"
import "irc"
import "styles.js" as Styles

Item {
    anchors {
        left: parent.left
        bottom: parent.bottom
    }

    //Quality values:
    //  4 - source
    //  3 - high
    //  2 - medium
    //  1 - low
    //  0 - mobile
    //property alias quality: sourcesBox.selectedItem
    property int duration: -1
    property var currentChannel
    property var streamMap
    property bool isVod: false
    property bool streamOnline: true

    //Minimode, bit ugly
    property bool smallMode: false
    property alias enableSmallMode: miniModeCheckBox.checked

    //Animations, need to be declared BEFORE width, height binds
    Behavior on width {
        enabled: smallMode
        NumberAnimation {
            duration: 250
            easing.type: Easing.OutCubic
        }
    }

    width: smallMode ? parent.width / 3 : parent.width
    height: smallMode ? width * 0.5625 : parent.height

    onSmallModeChanged: {
        if (smallMode)
            chatview.status = 0
    }

    //Renderer interface
    property alias renderer: loader.item

    id: root

    //Fix minimode header bar
    clip: true

    function isPlaying() {
        return (renderer.status === "PLAYING")
    }

    Connections {
        target: g_cman

        onAddedChannel: {
            console.log("Added channel")
            if (currentChannel && currentChannel._id == chanid){
                currentChannel.favourite = true
                _favIcon.update()
            }
        }

        onDeletedChannel: {
            console.log("Deleted channel")
            if (currentChannel && currentChannel._id == chanid){
                currentChannel.favourite = false
                _favIcon.update()
            }
        }

        onFoundPlaybackStream: {
            loadStreams(streams)
        }
    }

    Connections {
        target: netman

        onNetworkAccessChanged: {
            if (up && currentChannel && !renderer.status !== "PAUSED") {
                //console.log("Network up. Resuming playback...")
                loadAndPlay()
            }
        }

        onStreamGetOperationFinished: {
            //console.log("Received stream status", channelName, online)
            if (channelName === currentChannel.name) {
                if (online && !root.streamOnline) {
                    console.log("Stream back online, resuming playback")
                    loadAndPlay()
                }
                root.streamOnline = online
            }
        }

        onError: {
            switch (error) {

            case "token_error":
            case "playlist_error":
                //Display messagetrue
                setHeaderText("Error getting stream")
                break;

            default:
                break;
            }
        }
    }

    Timer {
        //Polls channel when stream goes down
        id: pollTimer
        interval: 4000
        repeat: true
        onTriggered: {
            if (currentChannel && currentChannel.name)
                netman.getStream(currentChannel.name)
        }
    }


    function loadAndPlay(streamName){
        setWatchingTitle()

        var start = !isVod ? -1 : seekBar.position

        var url = streamMap[streamName]

        console.debug("Loading: ", url)

        renderer.load(url, start)
    }

    function getStreams(channel, vod){

        if (!channel){
            return
        }

        renderer.stop()

        if (!vod || typeof vod === "undefined") {
            g_cman.findPlaybackStream(channel.name)
            isVod = false

            duration = -1
        }
        else {
            g_vodmgr.getBroadcasts(vod._id)
            isVod = true

            duration = vod.duration

            console.log("Setting up VOD, duration " + vod.duration)

            seekBar.setPosition(0, duration)
        }

        currentChannel = {
            "_id": channel._id,
            "name": channel.name,
            "game": isVod ? vod.game : channel.game,
                            "title": isVod ? vod.title : channel.title,
                                             "online": channel.online,
                                             "favourite": channel.favourite || g_cman.containsFavourite(channel._id),
                                             "viewers": channel.viewers,
                                             "logo": channel.logo,
                                             "preview": channel.preview,
        }

        _favIcon.update()
        _label.visible = false
        setWatchingTitle()
        chatview.joinChannel(currentChannel.name)
        pollTimer.restart()

        requestSelectionChange(5)
    }

    function setHeaderText(text) {
        headerText.text = text
    }

    function setWatchingTitle(){
        setHeaderText(currentChannel.title
                      + " playing " + currentChannel.game
                      + (isVod ? " (VOD)" : ""))
    }

    function loadStreams(streams) {

        console.log("DEBUG STREAMS")
        var sourceNames = []
        for (var k in streams) {
            console.log(k + " => " + streams[k])
            sourceNames.push(k)
        }

        streamMap = streams

        //TODO: sort sourceNames => [source , ... , mobile/smallest reso]
        sourcesBox.entries = sourceNames

        sourcesBox.selectFirst()
    }

    function seekTo(position) {
        console.log("Seeking to", position, duration)
        if (isVod){
            renderer.seekTo(position)
        }
    }

    function reloadStream() {
        renderer.stop()
        loadAndPlay()
    }

    Connections {
        target: g_vodmgr
        onStreamsGetFinished: {
            loadStreams(items)
        }
    }

    Connections {
        target: g_rootWindow
        onClosing: {
            renderer.pause()
        }
    }

    Connections {
        target: renderer

        onStatusChanged: {
            //console.log("Renderer status changed to " + renderer.status)
            togglePause.icon = renderer.status != "PLAYING" ? "play" : "pause"
        }

        onVolumeChanged: {
            //console.log("Renderer volume changed to " + renderer.volume)
        }

        onPositionChanged: {
            seekBar.setPosition(renderer.position, duration)
        }

        onPlayingResumed: {
            setWatchingTitle()
            spinner.visible = false
        }

        onPlayingPaused: {
            setHeaderText("Paused")
            spinner.visible = false
        }

        onPlayingStopped: {
            setHeaderText("Playback stopped")
            spinner.visible = false
        }
    }

    Item {
        id: playerArea
        anchors {
            top: parent.top
            left: parent.left
            right: chatview.status < 2 ? chatview.left : parent.right
            bottom: parent.bottom
        }

        Loader {
            id: loader
            anchors.fill: parent

            source: {
                switch (player_backend) {
                case "mpv":
                    return "MpvBackend.qml";

                case "qtav":
                    return "QtAVBackend.qml";

                case "multimedia":
                default:
                    return "MultimediaBackend.qml";
                }
            }

            onLoaded: {
                console.log("Loaded renderer")
            }
        }

        SpinnerIcon {
            id: spinner
            anchors.centerIn: parent
            iconSize: parent.width * 0.1
            visible: false
        }
    }

    Item {
        z: playerArea.z + 1

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: chatview.left
        }

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            propagateComposedEvents: false

            onClicked: {
                if (sourcesBox.open){
                    sourcesBox.close()
                }
            }

            onDoubleClicked: {
                if (smallMode) {
                    requestSelectionChange(5)
                } else {
                    g_fullscreen = !g_fullscreen
                }
            }

            onPositionChanged: {
                header.show()
                footer.show()
                headerTimer.restart()
            }
        }

        PlayerHeader {
            id: header
            //z: playerArea.z + 1

            visible: !smallMode

            MouseArea {
                id: mAreaHeader
                hoverEnabled: true
                anchors.fill: parent
                propagateComposedEvents: false
            }

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            Text {
                id: headerText
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                    right: miniModeContainer.left
                    margins: dp(5)
                }
                fontSizeMode: Text.Fit
                verticalAlignment: Text.AlignVCenter
                color: Styles.textColor
                font.pixelSize: Styles.titleFont.bigger
                z: root.z + 1
            }

            Item {
                id: miniModeContainer
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: favourite.left
                    rightMargin: dp(5)
                }
                width: dp(50)

                IconButton {
                    id: miniModeCheckBox
                    icon: "minimode"
                    checkable: true
                    checked: true

                    anchors.centerIn: parent
                }
            }

            Item {
                id: favourite
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: chatButton.left
                    rightMargin: dp(5)
                }
                width: dp(50)

                Icon {
                    id: _favIcon
                    icon: "fav"

                    anchors.centerIn: parent

                    function update(){
                        if (currentChannel)
                            iconColor= currentChannel.favourite ? Styles.purple : Styles.iconColor
                        else
                            iconColor= Styles.iconColor
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onHoveredChanged: {
                        if (containsMouse){
                            _favIcon.iconColor = Styles.white
                        } else {
                            _favIcon.update()
                        }
                    }

                    onClicked: {
                        if (currentChannel){
                            if (currentChannel.favourite)
                                g_cman.removeFromFavourites(currentChannel._id)
                            else{
                                //console.log(currentChannel)
                                g_cman.addToFavourites(currentChannel._id, currentChannel.name,
                                                       currentChannel.title, currentChannel.info,
                                                       currentChannel.logo, currentChannel.preview,
                                                       currentChannel.game, currentChannel.viewers,
                                                       currentChannel.online)
                            }
                        }
                    }
                }
            }

            Icon {
                id: chatButton
                icon: "chat"
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                    rightMargin: dp(5)
                }
                width: dp(50)
                height: width

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        //chatview.visible = !chatview.visible
                        chatview.status++

                        if (chatview.status > 2)
                            chatview.status = 0
                    }
                    hoverEnabled: true

                    onHoveredChanged: {
                        parent.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
                    }
                }
            }
        }

        PlayerHeader {
            id: footer
            //z: playerArea.z + 1
            visible: !smallMode

            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            MouseArea {
                id: mAreaFooter
                hoverEnabled: true
                anchors.fill: parent
                propagateComposedEvents: false
            }

            Item {
                id: pauseButton
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    leftMargin: dp(5)
                }

                width: dp(50)

                Icon {
                    id: togglePause
                    anchors.centerIn: parent
                    icon: "play"//renderer.status != "PLAYING" ? "play" : "pause"
                }

                MouseArea {
                    id: pauseArea
                    anchors.fill: parent
                    onClicked: {
                        renderer.togglePause()
                    }

                    hoverEnabled: true

                    onHoveredChanged: {
                        togglePause.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
                    }
                }
            }

            Icon {
                id: reloadButton
                icon: "reload"
                anchors {
                    left: pauseButton.right
                    leftMargin: dp(5)
                    verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    id: reloadArea
                    anchors.fill: parent
                    onClicked: reloadStream()
                    hoverEnabled: true

                    onHoveredChanged: {
                        reloadButton.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
                    }
                }
            }

            SeekBar {
                id: seekBar
                visible: isVod

                onUserChangedPosition: {
                    seekTo(position)
                }

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: reloadButton.right
                    right: fitButton.left
                }
            }

            Icon {
                id: fitButton
                icon: "crop"
                anchors {
                    right: vol.left
                    verticalCenter: parent.verticalCenter
                }
                width: !g_fullscreen ? dp(50) : 0
                height: width
                visible: !g_fullscreen

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (!g_fullscreen) {
                            g_rootWindow.fitToAspectRatio()
                        }
                    }
                    hoverEnabled: true

                    onHoveredChanged: {
                        parent.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
                    }
                }
            }

            VolumeSlider {
                id: vol
                z: parent.z + 1

                anchors {
                    right: fsButton.left
                    verticalCenter: parent.verticalCenter
                }
                Component.onCompleted: {
                    vol.value = g_cman.getVolumeLevel()
                }

                onValueChanged: {
                    var val
                    if (Qt.platform === "linux" && player_backend === "mpv")
                        val = Math.max(0, Math.min(100, Math.round(Math.log(value) / Math.log(100) * 100)))
                    else
                        val = Math.max(0, Math.min(100, value))

                    renderer.setVolume(val)
                    g_cman.setVolumeLevel(val);
                }
            }

            Icon {
                id: fsButton
                icon: !g_fullscreen ? "expand" : "compress"
                anchors {
                    right: sourcesBox.left
                    verticalCenter: parent.verticalCenter
                    rightMargin: dp(5)
                }
                width: dp(50)
                height: width

                MouseArea {
                    anchors.fill: parent
                    onClicked: g_fullscreen = !g_fullscreen
                    hoverEnabled: true

                    onHoveredChanged: {
                        parent.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
                    }
                }
            }

            ComboBox {
                //Contains data for sources
                id: sourcesBox
                width: dp(90)
                height: dp(40)
                //names: ["Mobile","Low","Medium","High","Source"]

                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: dp(5)
                }

                onItemChanged: {
                    loadAndPlay(item)
                }
            }
        }

        onVisibleChanged: {
            if (visible){
                header.show()
                footer.show()
            }
        }

        Text {
            id: _label
            text: "No stream currently playing"
            font.pixelSize: Styles.titleFont.bigger
            color: Styles.iconColor
            anchors.centerIn: parent
        }

        Timer {
            id: headerTimer
            interval: 3000
            running: false
            repeat: false
            onTriggered: {
                if (canHideHeaders()) {
                    header.hide()
                    footer.hide()
                }

                else
                    restart()
            }
        }

        Icon {
            id: stopButton

            icon: "remove"

            anchors {
                top: parent.top
                right: parent.right
                rightMargin: dp(5)
            }

            visible: root.smallMode
            width: dp(50)
            height: width

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    root.smallMode = false
                    root.enableSmallMode = false
                }
                hoverEnabled: true
                onHoveredChanged: {
                    parent.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
                }
                propagateComposedEvents: false
            }
        }
    }

    MouseArea {
        anchors.fill: playerArea
        onWheel: {
            if (wheel.angleDelta.y > 0)
            {
                vol.value = renderer.volume + 10
            }
            else
            {
                vol.value = renderer.volume - 10
            }
        }
    }

    ChatView {
        id: chatview

        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        width: visible ? dp(250) : 0

        Behavior on width {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
    }

    function canHideHeaders() {
        if (mAreaHeader.containsMouse)
            return false
        if (mAreaFooter.containsMouse)
            return false
        if (vol.open)
            return false
        if (sourcesBox.open)
            return false
        if (pauseArea.containsMouse)
            return false
        if (seekBar.containsMouse)
            return false
        if (reloadArea.containsMouse)
            return false

        return true
    }
}
