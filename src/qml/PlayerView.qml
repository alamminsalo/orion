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

    //Android only
    onHeadersVisibleChanged: {
        if (isMobile() && view.playerVisible) {
            topbar.visible = headersVisible
        }
    }

    Material.theme: rootWindow.Material.theme

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
        target: Network

        onNetworkAccessChanged: {
            if (up && currentChannel && !renderer.status !== "PAUSED") {
                //console.log("Network up. Resuming playback...")
                loadAndPlay()
            }
        }

        onStreamGetOperationFinished: {
            //console.log("Received stream status", channelId, currentChannel._id, online)
            if (channelId === currentChannel._id) {
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
            if (currentChannel && currentChannel._id)
                Network.getStream(currentChannel._id)
        }
    }


    function loadAndPlay(){
        var description = setWatchingTitle();

        var start = !isVod ? -1 : seekBar.position

        var url = streamMap[Settings.quality]

        console.debug("Loading: ", url)

        renderer.load(url, start, description)
    }

    function getStreams(channel, vod, startPos){
        getChannel(channel, vod, true, startPos);
    }

    function getChat(channel) {
        getChannel(channel, null, false, 0);
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
                                             "preview": channel.preview
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
                chat.replayChat(currentChannel.name, currentChannel._id, vodIdNum, startEpochTime, startPos);
            }
        } else {
            chat.joinChannel(currentChannel.name, currentChannel._id);
        }

        pollTimer.restart()

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

        sourcesBox.selectItem(Settings.quality);
        loadAndPlay()
    }

    function seekTo(position) {
        console.log("Seeking to", position, duration)
        if (isVod){
            chat.playerSeek(position)
            renderer.seekTo(position)
        }
    }

    function reloadStream() {
        renderer.stop()
        loadAndPlay()
    }

    Connections {
        target: VodManager
        onStreamsGetFinished: {
            loadStreams(items)
        }
    }

    Connections {
        target: rootWindow
        onClosing: {
            renderer.stop()
        }
    }

    Connections {
        target: renderer

        onPositionChanged: {
            var newPos = renderer.position;
            chat.playerPositionUpdate(newPos);
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
        }

        onPlayingPaused: {
            setHeaderText("Paused")
        }

        onPlayingStopped: {
            setHeaderText("Playback stopped")
        }

        onStatusChanged: {
            PowerManager.screensaver = (renderer.status !== "PLAYING")
        }
    }

    Item {
        id: playerArea
        anchors.fill: parent

        Loader {
            id: loader
            anchors.fill: parent

            source: {
                switch (Settings.appPlayerBackend()) {
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

        BusyIndicator {
            anchors.centerIn: parent
            running: renderer.status === "BUFFERING"
        }
    }

    MouseArea {
        id: pArea
        anchors.fill: playerArea

        function refreshHeaders(){
            if (!hideTimer.running)
                root.headersVisible = true
            hideTimer.restart()
        }

        onVisibleChanged: refreshHeaders()
        onPositionChanged: refreshHeaders()

        Rectangle {
            id: clickRect
            anchors.centerIn: parent
            width: 0
            height: width
            radius: height / 2
            opacity: 0

            Label {
                id: clickRectIcon
                text: renderer.status !== "PLAYING" ? "\ue037" : "\ue034"
                anchors.centerIn: parent
                font.family: "Material Icons"
                font.pointSize: parent.width * 0.5
            }

            ParallelAnimation {
                id: _anim
                running: false

                NumberAnimation {
                    target: clickRect
                    property: "width"
                    from: 0
                    to: pArea.width * 0.6
                    duration: 1500
                    easing.type: Easing.OutCubic
                }
                NumberAnimation {
                    target: clickRect
                    property: "opacity"
                    from: 0.5
                    to: 0
                    duration: 666
                    easing.type: Easing.OutCubic
                }
            }

            function run() {
                _anim.restart()
            }
        }

        onClicked: {
            clickRect.run()

            if (root.headersVisible && bottomBar.height > 50)
                clickTimer.restart()
            else
                refreshHeaders()
        }
        onDoubleClicked: {
            if (!isMobile()) {
                clickTimer.stop()
                appFullScreen = !appFullScreen
            }
        }
        hoverEnabled: true
        propagateComposedEvents: true
        cursorShape: headersVisible ? Qt.ArrowCursor : Qt.BlankCursor

        Timer {
            //Dbl click timer
            id: clickTimer
            interval: 440
            repeat: false
            onTriggered: {
                renderer.togglePause();
            }
        }

        Timer {
            id: hideTimer
            interval: 2000
            running: false
            repeat: false
            onTriggered: {
                var itemUnder = pArea.childAt(pArea.mouseX, pArea.mouseY)
                root.headersVisible = pArea.containsMouse && (itemUnder === bottomBar || itemUnder === headerBar)
            }
        }

        ToolBar {
            id: headerBar
            Material.foreground: rootWindow.Material.foreground
            background: Rectangle {
                color: root.Material.background
                opacity: 0.8
            }

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            clip: true
            height: root.headersVisible ? 55 : 0

            Behavior on height {
                NumberAnimation {
                    easing.type: Easing.OutCubic
                }
            }

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: 5
                    rightMargin: 5
                }

                Label {
                    id: title
                    font.pointSize: 9
                    Layout.fillWidth: true
                    horizontalAlignment: Qt.AlignHCenter
                    clip: true
                    font.bold: true
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
                    visible: !isMobile()
                    onClicked: {
                        if (chatdrawer.position <= 0)
                            chatdrawer.open()
                        else
                            chatdrawer.close()
                    }
                    text: chat.hasUnreadMessages ? "\ue87f" : "\ue0ca"
                }
            }
        }

        ToolBar {
            id: bottomBar
            Material.foreground: rootWindow.Material.foreground
            background: Rectangle {
                color: root.Material.background
                opacity: 0.8
            }

            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            clip: true
            height: root.headersVisible ? 55 : 0

            Behavior on height {
                NumberAnimation {
                    easing.type: Easing.OutCubic
                }
            }

            RowLayout {
                anchors {
                    fill: parent
                    rightMargin: 5
                    leftMargin: 5
                }

                IconButtonFlat {
                    id: playBtn
                    text: renderer.status !== "PLAYING" ? "\ue037" : "\ue034"
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
                    visible: !appFullScreen && !isMobile() && !chat.visible && parent.width > 440
                    text: "\ue3bc"
                    onClicked: fitToAspectRatio()
                }

                IconButtonFlat {
                    id: fsBtn
                    visible: !isMobile()
                    text: !appFullScreen ? "\ue5d0" : "\ue5d1"
                    onClicked: appFullScreen = !appFullScreen
                }

                IconButtonFlat {
                    id: volumeBtn
                    visible: !isMobile() && parent.width > 390
                    property real mutedValue: 100.0
                    text: volumeSlider.value > 0 ?
                              (volumeSlider.value > 50 ? "\ue050" : "\ue04d")
                            : "\ue04f"
                    onClicked: {
                        if (volumeSlider.value > 0) {
                            mutedValue = volumeSlider.value
                            volumeSlider.value = 0
                        }
                        else {
                            volumeSlider.value = mutedValue
                        }
                    }
                }

                Slider {
                    id: volumeSlider
                    from: 0
                    to: 100
                    visible: !isMobile()
                    Layout.maximumWidth: 90
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
                    flat: true
                    Layout.fillWidth: true
                    Layout.maximumWidth: 140
                    Layout.minimumWidth: 100

                    onActivated: {
                        Settings.quality = sourcesBox.model[index]
                        loadAndPlay()
                        pArea.refreshHeaders()
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

        Slider {
            id: seekBar
            from: 0
            to: duration
            visible: isVod && headersVisible
            padding: 0
            anchors {
                verticalCenter: bottomBar.top
                left: parent.left
                right: parent.right
            }
            onPressedChanged: {
                if (!pressed)
                    seekTo(value)
            }
        }
    }
}
