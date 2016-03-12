import QtQuick 2.0
import "components"
import mpv 1.0

import "styles.js" as Styles

Item {
    anchors.fill: parent

    //Quality values:
    //  4 - source
    //  3 - high
    //  2 - medium
    //  1 - low
    //  0 - mobile
    property int quality: 4
    property bool paused: false
    property int duration: -1
    property var currentChannel
    property var qualityMap
    property bool fs: false
    property bool isVod: false

    id: root

    function play(channel, vod){

        if (!channel){
            return
        }

        renderer.command(["stop"])

        //console.log(typeof vod)

        if (!vod || typeof vod === "undefined") {
            g_cman.findPlaybackStream(channel.name)
            isVod = false

            duration = -1
        }
        else {
            g_vodmgr.getStreams(vod._id)
            isVod = true

            duration = vod.duration

            console.log("Setting up VOD, duration " + vod.duration)
        }

        paused = false
        //renderer.play()

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
        spinner.visible = true

        setWatchingTitle()

        requestSelectionChange(5)
    }

    function setWatchingTitle(){
        header.text = currentChannel.title
                + " playing " + currentChannel.game
                + (isVod ? " (VOD)" : "")
    }

    function pause(){
        paused = !paused

        if (paused)
            renderer.pause()
        else
            renderer.play()
    }

    Connections {
        target: g_cman
        onFoundPlaybackStream: {
            loadStreams(streams)
        }
    }

    function loadStreams(streams) {
        qualityMap = streams

        var desc = true
        while (!qualityMap[quality] || qualityMap[quality].length <= 0){

            if (quality <= 0)
                desc = false

            if (quality == 4 && !desc)
                break;

            quality += desc ? -1 : 1
        }

        sourcesBox.entries = qualityMap

        if (qualityMap[quality]){
            setWatchingTitle()

            renderer.command(["loadfile", qualityMap[quality]])

            spinner.visible = false

            sourcesBox.setIndex(quality)
        }

        renderer.play()
    }

    function seekTo(percentage) {
        if (isVod){
            var pos = duration / 100 * percentage
            renderer.setProperty("playback-time", pos)
        }
    }

    Connections {
        target: g_vodmgr
        onStreamsGetFinished: {
            loadStreams(g_vodmgr.getResults())
        }
    }

    Connections {
        target: g_rootWindow
        onClosing: {
            if (!paused)
                pause()
        }
    }

    PlayerHeader{
        text: "Currently watching: N/A"
        id: header
        z: renderer.z + 1

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

        Item {
            id: favourite
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
                rightMargin: dp(10)
            }
            width: dp(50)

            Icon {
                id: _favIcon
                icon: "fav"

                anchors.centerIn: parent

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
                }

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
                            console.log(currentChannel)
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

    }

    PlayerHeader {
        id: footer
        z: renderer.z + 1
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
                leftMargin: dp(10)
            }

            width: dp(50)

            Icon {
                id: togglePause
                anchors.centerIn: parent
                icon: paused ? "play" : "pause"
            }

            MouseArea {
                id: pauseArea
                anchors.fill: parent
                onClicked: pause()
                hoverEnabled: true

                onHoveredChanged: {
                    togglePause.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
                }
            }
        }

        Item {

            visible: isVod

            anchors {
                top: parent.top
                bottom: parent.bottom
                left: pauseButton.right
                right: vol.left
            }

            Rectangle {
                id: seekBar
                color: Styles.seekBar

                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: dp(10)
                    rightMargin: dp(10)

                    verticalCenter: parent.verticalCenter
                }

                height: dp(6)

                Rectangle {
                    color: "white"
                    id: fillBar
                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                    }

                    Connections {
                        target: renderer
                        onPositionChanged: {
                            var width = Math.max(0,Math.min(position / duration * seekBar.width, seekBar.width))

                            fillBar.width = width
                        }
                    }
                }
            }

            MouseArea {
                anchors {
                    fill: parent
                    topMargin: dp(10)
                    bottomMargin: dp(10)
                }

                propagateComposedEvents: false
                onClicked: {
                    var percentage = Math.max(0, Math.min(mouseX / seekBar.width * 100, 100))
                    seekTo(percentage)
                }
            }
        }


        VolumeSlider {
            id: vol
            z: parent.z + 1
            anchors {
                right: sourcesBox.left
                verticalCenter: parent.verticalCenter
                rightMargin: dp(10)
            }
            onValueChanged: {
                var val = Math.max(0,Math.min(100, Math.round(Math.log(value) / Math.log(100) * 100)))
                renderer.setProperty("volume", val)
            }
        }

        ComboBox {
            id: sourcesBox
            width: dp(100)
            height: dp(50)
            names: ["Mobile","Low","Medium","High","Source"]

            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: dp(10)
            }

            onIndexChanged: {
                if (index != quality){
                    quality = index
                    play(currentChannel)
                }
            }
        }
    }

    onVisibleChanged: {
        if (visible){
            header.show()
            footer.show()
        }
    }

    MpvObject {
        id: renderer
        anchors {
            fill: parent
        }

        visible: parent.visible

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                if (sourcesBox.open){
                    sourcesBox.close()
                }
            }

            onDoubleClicked: {
                g_fullscreen = !g_fullscreen
                //renderer.command(["set","pause", "no"])
            }

            onPositionChanged: {
                header.show()
                footer.show()
                headerTimer.restart()
            }
        }

        onPlayingStopped: {
            header.text = "Playback stopped"
            g_powerman.setScreensaver(true);
        }

        onPlayingPaused: {
            header.text = "Paused"
            g_powerman.setScreensaver(true);
        }

        onPlayingResumed: {
            setWatchingTitle()
            spinner.visible = false

            g_powerman.setScreensaver(false);
        }

        onBufferingStarted: {
            spinner.visible = true
        }
    }

    Text {
        id: _label
        text: "No stream currently playing"
        font.pointSize: dp(Styles.bigFont)
        font.family: "Droid Sans"
        color: Styles.iconColor
        anchors.centerIn: parent
        renderType: Text.NativeRendering
    }

    SpinnerIcon {
        id: spinner
        visible: false
        anchors.centerIn: parent
        iconSize: dp(80)
    }


    Timer {
        id: headerTimer
        interval: 4000
        running: false
        repeat: false
        onTriggered: {
            if (mAreaHeader.containsMouse || mAreaFooter.containsMouse || vol.open || sourcesBox.open || pauseArea.containsMouse){
                restart()
            }
            else {
                header.hide()
                footer.hide()
            }
        }
    }
}
