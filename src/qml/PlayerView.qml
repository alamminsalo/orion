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
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.1
import "components"

import app.orion 1.0

Page {

    property int duration: -1
    property var currentChannel
    property var streamMap
    property bool isVod: false
    property bool streamOnline: true
    property string curVodId
    property int lastSetPosition
    property bool headersVisible: true
    property string currentQualityName

    //Minimode, bit ugly
    property bool smallMode: false
    //property alias enableSmallMode: miniModeCheckBox.checked

    Component.onCompleted: {
        var savedQuality = Settings.quality;
        console.log("Loaded saved quality", savedQuality);
        if (savedQuality) {
            currentQualityName = savedQuality;
        }
    }

    //Renderer interface
    property alias renderer: loader.item

    id: root

    //Fix minimode header bar
    clip: true

    Connections {
        target: ChannelManager

        onAddedChannel: {
            console.log("Added channel")
            if (currentChannel && currentChannel._id == chanid){
                currentChannel.favourite = true
                favBtn.update()
            }
        }

        onDeletedChannel: {
            console.log("Deleted channel")
            if (currentChannel && currentChannel._id == chanid){
                currentChannel.favourite = false
                favBtn.update()
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
                loadAndPlay(currentQualityName)
            }
        }

        onStreamGetOperationFinished: {
            //console.log("Received stream status", channelId, currentChannel._id, online)
            if (channelId === currentChannel._id) {
                if (online && !root.streamOnline) {
                    console.log("Stream back online, resuming playback")
                    loadAndPlay(currentQualityName)
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
            if (currentChannel && currentChannel._id)
                netman.getStream(currentChannel._id)
        }
    }


    function loadAndPlay(streamName){
        var description = setWatchingTitle();

        var start = !isVod ? -1 : seekBar.position

        var url = streamMap[streamName]

        console.debug("Loading: ", url)

        renderer.load(url, start, description)

        currentQualityName = streamName
        Settings.quality = streamName;

        spinner.running = true
    }

    function getStreams(channel, vod, startPos){
        getChannel(channel, vod, true, startPos);
    }

    function getChat(channel) {
        getChannel(channel, null, false, 0);
//        if (chatview.status == 0) {
//            chatview.status++;
//        }
    }

    function getChannel(channel, vod, wantVideo, startPos){

        if (!channel){
            return
        }

        renderer.stop()

        if (wantVideo) {
            if (!vod || typeof vod === "undefined") {
                ChannelManager.findPlaybackStream(channel.name)
                isVod = false

                duration = -1
            }
            else {
                VodManager.getBroadcasts(vod._id)
                isVod = true
                root.curVodId = vod._id
                root.lastSetPosition = startPos

                duration = vod.duration

                console.log("Setting up VOD, duration " + vod.duration)

                seekBar.value = startPos
            }
        } else {
            isVod = false;
        }

        currentChannel = {
            "_id": channel._id,
            "name": channel.name,
            "game": isVod ? vod.game : channel.game,
                            "title": isVod ? vod.title : channel.title,
                                             "online": channel.online,
                                             "favourite": channel.favourite || ChannelManager.containsFavourite(channel._id),
                                             "viewers": channel.viewers,
                                             "logo": channel.logo,
                                             "preview": channel.preview,
        }

        favBtn.update()
        setWatchingTitle()

        if (isVod) {
            var startEpochTime = (new Date(vod.createdAt)).getTime() / 1000.0;

            console.log("typeof vod._id is", typeof(vod._id))

            if (vod._id.charAt(0) !== "v") {
                console.log("unknown vod id format in", vod._id);
            } else {
                var vodIdNum = parseInt(vod._id.substring(1));
                console.log("replaying chat for vod", vodIdNum, "starting at", startEpochTime);
                chatview.replayChat(currentChannel.name, currentChannel._id, vodIdNum, startEpochTime, startPos);
            }
        } else {
            chatview.joinChannel(currentChannel.name, currentChannel._id);
        }

        pollTimer.restart()

        spinner.running = true

        requestSelectionChange(4)
    }

    function setHeaderText(text) {
        title.text = text
    }

    function setWatchingTitle(){
        var description = currentChannel.title
                + (currentChannel.game ? " playing " + currentChannel.game : "")
                + (isVod ? " (VOD)" : "");
        setHeaderText(description);
        return description;
    }

    function loadStreams(streams) {
        var sourceNames = []
        for (var k in streams) {
            sourceNames.splice(0, 0, k) //revert order
        }

        streamMap = streams
        sourcesBox.model = sourceNames

        if (currentQualityName && streamMap[currentQualityName]) {
            sourcesBox.selectItem(currentQualityName);
            loadAndPlay(currentQualityName)
        } else {
            sourcesBox.currentIndex = 0
        }
    }

    function seekTo(position) {
        console.log("Seeking to", position, duration)
        if (isVod){
            chatview.playerSeek(position)
            renderer.seekTo(position)
        }
    }

    function reloadStream() {
        renderer.stop()
        loadAndPlay(currentQualityName)
    }

    Connections {
        target: VodManager
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

        onPositionChanged: {
            var newPos = renderer.position;
            chatview.playerPositionUpdate(newPos);
            if (root.isVod) {
                if (Math.abs(newPos - root.lastSetPosition) > 10) {
                    root.lastSetPosition = newPos;
                    VodManager.setVodLastPlaybackPosition(root.currentChannel.name, root.curVodId, newPos);
                }
            }
            seekBar.value = newPos
        }

        onPlayingResumed: {
            setWatchingTitle()
            spinner.running = false
        }

        onPlayingPaused: {
            setHeaderText("Paused")
            spinner.running = false
        }

        onPlayingStopped: {
            setHeaderText("Playback stopped")
            spinner.running = false
        }
    }

    Item {
        id: playerArea
        anchors.fill: parent

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

        MouseArea {
            id: pArea
            anchors.fill: parent
            onDoubleClicked: g_fullscreen = !g_fullscreen
            hoverEnabled: true
            cursorShape: headersVisible ? Qt.ArrowCursor : Qt.BlankCursor
            onPositionChanged: {
                if (!timer.running)
                    root.headersVisible = true
                timer.restart()
            }
        }

        Timer {
            id: timer
            interval: 3000
            running: false
            repeat: false
            onTriggered: {
                if (!footerArea.containsMouse && !headerArea.containsMouse) {
                    headersVisible = false
                }
            }
        }

        BusyIndicator {
            id: spinner
            anchors.centerIn: parent
            running: false
        }
    }

    header: ToolBar {
        visible: root.headersVisible
        Material.background: Material.background
        leftPadding: 5
        rightPadding: 5

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            id: headerArea
        }

        RowLayout {
            anchors.fill: parent

            Label {
                id: title
                font.bold: true
                font.pointSize: 10
                Layout.fillWidth: true
                horizontalAlignment: Qt.AlignHCenter
                clip: true
            }

            IconButtonFlat {
                id: favBtn
                text: "\ue87d"

                function update() {
                    highlighted = currentChannel.favourite === true
                }

                onClicked: {
                    if (currentChannel){
                        if (currentChannel.favourite)
                            ChannelManager.removeFromFavourites(currentChannel._id)
                        else{
                            //console.log(currentChannel)
                            ChannelManager.addToFavourites(currentChannel._id, currentChannel.name,
                                                   currentChannel.title, currentChannel.info,
                                                   currentChannel.logo, currentChannel.preview,
                                                   currentChannel.game, currentChannel.viewers,
                                                   currentChannel.online)
                        }
                    }
                }
            }

            IconButtonFlat {
                id: chatBtn
                onClicked: chatdrawer.open()
                text: "\ue0ca"
            }
        }
    }

    footer: ToolBar {
        visible: root.headersVisible
        leftPadding: 5
        rightPadding: 5
        Material.background: Material.background

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            id: footerArea
        }

        Slider {
            id: seekBar
            from: 0
            to: duration
            visible: isVod
            anchors {
                verticalCenter: parent.top
                left: parent.left
                right: parent.right
            }
            onMoved: seekTo(value)
        }

        RowLayout {
            anchors.fill: parent

            IconButtonFlat {
                id: playBtn
                text: renderer.status != "PLAYING" ? "\ue037" : "\ue034"
                onClicked: renderer.togglePause()
            }
            IconButtonFlat {
                id: resetBtn
                text: "\ue5d5"
                onClicked: reloadStream()
            }

            //spacer
            Item {
                Layout.minimumWidth: 0
                Layout.fillWidth: true
            }

            IconButtonFlat {
                id: cropBtn
                text: "\ue3be"
                onClicked: fitToAspectRatio()
                visible: parent.width > 440
            }

            IconButtonFlat {
                id: fsBtn
                text: "\ue5d0"
                onClicked: g_fullscreen = !g_fullscreen
                visible: parent.width > 380
            }

            Slider {
                id: volumeSlider
                from: 0
                to: 100
                Layout.maximumWidth: 100

                Component.onCompleted: {
                    value = Settings.volumeLevel
                }

                onValueChanged: {
                    var val = value
                    if (Qt.platform === "linux" && player_backend === "mpv")
                        val = Math.round(Math.log(val) / Math.log(100))

                    renderer.setVolume(val)
                    Settings.volumeLevel = val;
                }
            }

            ComboBox {
                id: sourcesBox
                font.pointSize: 9
                font.bold: true

                onActivated: {
                    loadAndPlay(sourcesBox.model[index])
                }

                function selectItem(name) {
                    for (var i in sourcesBox.model) {
                        if (sourcesBox.model[i] === name) {
                            currentIndex = i;
                            return;
                        }
                    }
                    //None found, attempt to select first item
                    currentIndex = 0
                }
            }
        }
    }
}
