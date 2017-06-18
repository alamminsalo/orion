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
import "irc"

Page {
    //    anchors {
    //        left: parent.left
    //        bottom: parent.bottom
    //    }

    property int duration: -1
    property var currentChannel
    property var streamMap
    property bool isVod: false
    property bool streamOnline: true
    property string curVodId
    property int lastSetPosition

    property bool cursorHidden: false
    property string currentQualityName

    //Minimode, bit ugly
    property bool smallMode: false
    //property alias enableSmallMode: miniModeCheckBox.checked

    Component.onCompleted: {
        var savedQuality = g_cman.getQuality();
        console.log("Loaded saved quality", savedQuality);
        if (savedQuality) {
            currentQualityName = savedQuality;
        }
    }

    //Animations, need to be declared BEFORE width, height binds
    //    Behavior on width {
    //        enabled: smallMode
    //        NumberAnimation {
    //            duration: 250
    //            easing.type: Easing.OutCubic
    //        }
    //    }

    //    width: smallMode ? parent.width / 3 : parent.width
    //    height: smallMode ? width * 0.5625 : parent.height

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
        g_cman.setQuality(streamName);

        spinner.running = true
    }

    function getStreams(channel, vod, startPos){
        getChannel(channel, vod, true, startPos);
    }

    function getChat(channel) {
        getChannel(channel, null, false, 0);
        if (chatview.status == 0) {
            chatview.status++;
        }
    }

    function getChannel(channel, vod, wantVideo, startPos){

        if (!channel){
            return
        }

        renderer.stop()

        if (wantVideo) {
            if (!vod || typeof vod === "undefined") {
                g_cman.findPlaybackStream(channel.name)
                isVod = false

                duration = -1
            }
            else {
                g_vodmgr.getBroadcasts(vod._id)
                isVod = true
                root.curVodId = vod._id
                root.lastSetPosition = startPos

                duration = vod.duration

                console.log("Setting up VOD, duration " + vod.duration)

                seekBar.setPosition(startPos, duration)
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
                                             "favourite": channel.favourite || g_cman.containsFavourite(channel._id),
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

        onPositionChanged: {
            var newPos = renderer.position;
            chatview.playerPositionUpdate(newPos);
            if (root.isVod) {
                if (Math.abs(newPos - root.lastSetPosition) > 10) {
                    root.lastSetPosition = newPos;
                    g_cman.setVodLastPlaybackPosition(root.currentChannel.name, root.curVodId, newPos);
                }
            }
            seekBar.setPosition(newPos, duration);
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

        BusyIndicator {
            id: spinner
            anchors.centerIn: parent
            running: false
        }
    }

    //    Item {
    //        //Player controls overlay
    //        visible: false
    //        z: playerArea.z + 1

    //        anchors {
    //            top: parent.top
    //            bottom: parent.bottom
    //            left: g_cman.swapChat ? chatview.right : parent.left
    //            right: g_cman.swapChat ? parent.right : chatview.left
    //        }

    //        MouseArea{
    //            anchors.fill: parent
    //            hoverEnabled: true
    //            propagateComposedEvents: false

    //            //Hide cursor when headers hide
    //            cursorShape: cursorHidden ? Qt.BlankCursor : Qt.ArrowCursor

    //            onClicked: {
    //                if (sourcesBox.open){
    //                    sourcesBox.close()
    //                }playBtn
    //            }

    //            onDoubleClicked: {
    //                if (smallMode) {
    //                    requestSelectionChange(4)
    //                } else {
    //                    g_fullscreen = !g_fullscreen
    //                }
    //            }

    //            onPositionChanged: {
    //                header.show()
    //                footer.show()
    //                headerTimer.restart()
    //            }
    //        }

    //        PlayerHeader {playBtn
    //            id: header
    //            visible: !smallMode

    //            MouseArea {
    //                id: mAreaHeader
    //                hoverEnabled: true
    //                anchors.fill: parent
    //                propagateComposedEvents: false
    //            }

    //            anchors {
    //                top: parent.top
    //                left: parent.left
    //                right: parent.right
    //            }

    //            Label {
    //                id: headerText
    //                anchors {
    //                    left: parent.left
    //                    top: parent.top
    //                    bottom: parent.bottom
    //                    right: miniModeContainer.left
    //                    margins: dp(5)
    //                }
    //                fontSizeMode: Text.Fit
    //                verticalAlignment: Text.AlignVCenter
    //                z: root.z + 1
    //            }

    //            Item {
    //                id: miniModeContainer
    //                anchors {
    //                    top: parent.top
    //                    bottom: parent.bottom
    //                    right: favourite.left
    //                    rightMargin: dp(5)
    //                }
    //                width: dp(50)

    //                IconButton {
    //                    id: miniModeCheckBox
    //                    icon: "minimode"
    //                    checkable: true
    //                    checked: true

    //                    anchors.centerIn: parent
    //                }

    //                ToolTip {
    //                    visible: miniModeCheckBox.mouseArea.containsMouse
    //                    delay: 666
    //                    text: "Toggle floating player"
    //                }
    //            }

    //            Item {
    //                id: favourite
    //                anchors {
    //                    top: parent.top
    //                    bottom: parent.bottom
    //                    right: chatButton.left
    //                    rightMargin: dp(5)
    //                }
    //                width: dp(50)



    //            Icon {
    //                id: chatButton
    //                icon: "chat"
    //                anchors {
    //                    top: parent.top
    //                    bottom: parent.bottom
    //                    right: parent.right
    //                }
    //                width: dp(50)
    //                height: width

    //                MouseArea {
    //                    id: chatButtonArea
    //                    anchors.fill: parent
    //                    onClicked: {
    //                        chatview.status++
    //                    }
    //                    hoverEnabled: true

    //                    onHoveredChanged: {
    //                        parent.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
    //                    }

    //                    ToolTip {
    //                        visible: parent.containsMouse
    //                        delay: 666
    //                        text: "Toggle chat"
    //                    }
    //                }
    //            }
    //        }

    //        Icon {
    //            id: stopButton

    //            icon: "remove"

    //            anchors {
    //                top: parent.top
    //                right: parent.right
    //                rightMargin: dp(5)
    //            }

    //            visible: smallMode
    //            width: dp(50)
    //            height: width

    //            MouseArea {
    //                anchors.fill: parent
    //                onClicked: {
    //                    root.smallMode = false
    //                    root.enableSmallMode = false
    //                }
    //                hoverEnabled: true
    //                onHoveredChanged: {
    //                    parent.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
    //                }
    //                propagateComposedEvents: false
    //            }
    //        }
    //    }

    ChatView {
        id: chatview

        edge: Qt.RightEdge
        height: root.height
        //width: 300

        // Use JS for side anchors so we can control the order the anchors are set when we change them.
        // https://doc.qt.io/qt-5/qtquick-positioning-anchors.html#changing-anchors

        function updateAnchors() {
            //            console.log("updateAnchors: g_cman.swapChat", g_cman.swapChat);
            //            if (g_cman.swapChat) {
            //                anchors.right = undefined;
            //                anchors.left = parent.left;
            //            } else {
            //                anchors.left = undefined;
            //                anchors.right = parent.right;
            //            }
        }

        Component.onCompleted: {
            chatview.updateAnchors();
        }

        Connections {
            target: g_cman
            onSwapChatChanged: {
                chatview.updateAnchors();
            }
        }

        width: visible && !smallMode ? chatWidth : 0
        chatWidth: 250 * g_cman.textScaleFactor

        Behavior on width {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
    }

    header: ToolBar {
        padding: 5
        Material.background: Material.background

        RowLayout {
            anchors.fill: parent

            Label {
                id: title
                font.bold: true
                font.pointSize: 10
                Layout.fillWidth: true
            }

            RoundButton {
                id: favBtn
                text: "Follow"
                flat: true

                function update() {
                    highlighted = currentChannel.favourite === true
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

            RoundButton {
                id: chatBtn
                text: "Chat"
                onClicked: chatview.open()
            }
        }
    }

    footer: ToolBar {
        padding: 0
        Material.background: Material.background

        SeekBar {
            id: seekBar
            visible: isVod
            anchors {
                verticalCenter: parent.top
                left: parent.left
                right: parent.right
            }
            onMoved: seekTo(value * position)
        }

        RowLayout {
            anchors.fill: parent

            RoundButton {
                id: playBtn
                //font.family: "Material Icons"
                flat: true
                text: renderer.status != "PLAYING" ? "play" : "pause"
                onClicked: renderer.togglePause()
            }
            RoundButton {
                id: resetBtn
                //font.family: "Material Icons"
                flat:true
                text: "reset"
                onClicked: reloadStream()
            }

            RoundButton {
                id: cropBtn
                text: "crop"
                flat: true
                onClicked: fitToAspectRatio()
            }
            RoundButton {
                id: fsBtn
                text: "FS"
                flat: true
                onClicked: g_fullscreen = !g_fullscreen
            }

            Slider {
                id: volumeSlider

                Component.onCompleted: {
                    value = g_cman.getVolumeLevel() / 100.0
                }

                onValueChanged: {
                    var val = value * 100.0
                    if (Qt.platform === "linux" && player_backend === "mpv")
                        val = Math.round(Math.log(val) / Math.log(100) * 100)

                    renderer.setVolume(val)
                    g_cman.setVolumeLevel(val);
                }
            }

            ComboBox {
                id: sourcesBox

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
