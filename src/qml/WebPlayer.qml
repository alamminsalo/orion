import QtQuick 2.0
import QtWebEngine 1.1
import "components"
import "styles.js" as Styles

//NOT USED CURRENTLY

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


    onHeightChanged: adjustSize()
    onWidthChanged: adjustSize()

    function adjustSize() {
        if (!web.loading) {
            web.runJavaScript("player.setHeight("+height+")")
            web.runJavaScript("player.setWidth("+width+")")
        }
    }

    function setVolume(volume){
        console.log("Setting volume: " +volume)
        web.runJavaScript("player.setVolume("+volume+")")
    }

    function setQuality(q) {
        web.runJavaScript("player.setQuality('"+q+"')")
    }

    function refreshDuration() {
        web.runJavaScript("player.getCurrentTime()", function(time) {
            seekBar.setPosition(parseInt(time), duration)
        })
    }

    function loadAndPlay(){
        setWatchingTitle()

        var position = !isVod ? 0 : seekBar.position

        //console.log(position)

        if (isVod)
            seekTo(position)

        var q
        switch (quality) {
            case 4:
                q = "chunked"
                break;

            case 3:
                q = "high"
                break;

            case 2:
                q = "medium"
                break;

            case 1:
                q = "low"
                break;

            default:
                q = "mobile"
        }

        //Stop everything earlier
        console.debug("Loading: ", q)
        setQuality(q)

        pause()
        pause()
    }

    function seekTo(pos) {
        web.runJavaScript("player.seek("+pos+")")
    }

    function getStreams(channel, vod) {

        if (!vod || typeof vod === "undefined") {
            web.runJavaScript("player.setChannel('"+channel.name+"')")

            isVod = false

            duration = -1
        }
        else {
            web.runJavaScript("player.setVideo('"+vod._id+"')")
            isVod = true

            duration = vod.duration

            console.log("Setting up VOD, duration " + vod.duration)

            seekBar.position = 0
        }

        g_cman.findPlaybackStream(channel.name)
        paused = false

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

        web.runJavaScript("player.getQualities()", function(streams) {
            sourcesBox.entries = streams
        })

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
            web.runJavaScript("player.pause()")
        else {
            web.runJavaScript("player.play()")
            pTimer.start()
        }
    }

    WebEngineView {
        id: web
        anchors.fill: parent
        url: "player.html"


        onLoadingChanged: {
            if (!loading) {
                adjustSize()
                vol.initialize()
            }
        }
    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: false

        onClicked: {
            if (sourcesBox.open){
                sourcesBox.close()
            }

            clickTimer.start()
        }

        onDoubleClicked: {
            g_fullscreen = !g_fullscreen
            clickTimer.dblClicked = true
        }

        onPositionChanged: {
            header.show()
            footer.show()
            headerTimer.restart()
        }
    }

    PlayerHeader {
        text: "Currently watching: N/A"
        id: header
        z: web.z + 1

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
    }

    PlayerHeader {
        id: footer
        z: web.z + 1
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

        SeekBar {
            id: seekBar
            visible: isVod

            onUserChangedPosition: {
                seekTo(position)
            }

            anchors {
                top: parent.top
                bottom: parent.bottom
                left: pauseButton.right
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
            width: dp(50)
            height: width

            MouseArea {
                anchors.fill: parent
                onClicked: g_rootWindow.fitToAspectRatio()
                hoverEnabled: true

                onHoveredChanged: {
                    parent.iconColor = containsMouse ? Styles.textColor : Styles.iconColor
                }
            }
        }

        VolumeSlider {
            id: vol
            z: parent.z + 1

            property bool initialized: false

            anchors {
                right: fsButton.left
                verticalCenter: parent.verticalCenter
            }

            function initialize(vol) {
                value = vol || 100
                initialized = true
            }

            onValueChanged: {
                if (initialized) {
                    var val = Math.max(0, Math.min(100, value))

                    setVolume(val / 100)
                }
            }
        }

        Icon {
            id: fsButton
            icon: !g_fullscreen ? "expand" : "compress"
            anchors {
                right: sourcesBox.left
                verticalCenter: parent.verticalCenter
                rightMargin: dp(10)
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
            id: sourcesBox
            width: dp(90)
            height: dp(40)
            names: ["Mobile","Low","Medium","High","Source"]

            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: dp(10)
            }

            onIndexChanged: {
                if (index != quality){
                    quality = index
                    loadAndPlay()
                }
            }
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

    //Workaround to buggy interaction in combined pause / fullscreen on click / dblclick
    Timer {
        property bool dblClicked: false
        id: clickTimer
        interval: 400
        running: false
        repeat: false
        onTriggered: {
            if (!dblClicked)
                pause()
        }

        onRunningChanged: {
            if (running)
                dblClicked = false
        }
    }

    Timer {
        id: pTimer
        interval: 1000
        running: false
        repeat: true
        onTriggered: {
            refreshDuration()
        }
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
            sourcesBox.setIndex(quality)

            loadAndPlay()
        }
    }
}
