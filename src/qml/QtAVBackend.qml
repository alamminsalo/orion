import QtQuick 2.5
import QtAV 1.7
import "components"
import "irc"
import "styles.js" as Styles

/* Interface for backend Mpv

Functions needed:
load(src, start)    -- Loads stream src, if given, start sets the starting milliseconds in vods
resume()            -- Forces resume
pause()             -- Forces pause
togglePause()       -- Toggles between playing and pausing
stop()              -- Stops playback
seekTo(ms)          -- Seeks to milliseconds in the current source, works only on vods
setVolume(vol)      -- Number between 0 - 100

Signals needed:
playingResumed()    -- Signaled when playback toggles from paused / stopped to playing
playingPaused()     -- Signaled when playback pauses
playingStopped()    -- Signaled when playback stops (stream ends)
volumeChanged()     -- Signaled when volume changes internally

Variables needed:
status              -- string "PLAYING" | "PAUSED" | "STOPPED"
position            -- Milliseconds in playback
volume              -- volume between 0 - 100

*/

Item {
    id: root

    function load(src, start) {
        stop();

        if (start) {
            position = start
            renderer.setOption("start", "+" + position)
        }

        renderer.command(["loadfile", src])

        togglePause(false)
    }

    function resume() {
        renderer.play()
    }

    function pause() {
        renderer.pause()
    }

    function stop() {
        renderer.stop()
    }

    function togglePause() {
        if (status == "PAUSED" || status == "STOPPED")
            resume()
        else
            pause()
        console.log(status)
    }

    function seekTo(ms) {
        position = ms
    }

    function setVolume(vol) {
        volume = Math.round(vol)
    }

    signal playingResumed()
    signal playingPaused()
    signal playingStopped()
    signal volumeChangedInternally()

    property string status: "STOPPED"
    onStatusChanged: {
        switch (status) {
        case "PLAYING":
            playingResumed();
            break;
        case "PAUSED":
            playingPaused();
            break;
        case "STOPPED":
            playingStopped();
            break;
        }
    }

    property int position: 0
    onPositionChanged: {
        renderer.seek(position)
        renderer.setProperty("playback-time", position)
    }

    property double volume: 100
    onVolumeChanged: {
        //console.log("Volume", volume)
        renderer.setProperty("volume", volume)
    }

    MediaPlayer {
        id: renderer
        anchors.fill: parent

        onStopped: {
            g_powerman.setScreensaver(true);
            status = "STOPPED"
        }

        onPaused: {
            g_powerman.setScreensaver(true);
            status = "PAUSED"
        }

        onPlaying: {
            setWatchingTitle()
            g_powerman.setScreensaver(false);
            status = "PLAYING"
        }

        Component.onCompleted: {
            root.setVolume(100)
        }
    }

    Rectangle {
        color: "black"
        anchors.fill: parent

        VideoOutput {
            id: output
            anchors.fill: parent
            source: renderer
        }

        MouseArea{
            z: output.z + 1
            anchors.fill: parent
            hoverEnabled: true
            propagateComposedEvents: false

            onClicked: {
                if (sourcesBox.open){
                    sourcesBox.close()
                }
            }

            onDoubleClicked: {
                g_fullscreen = !g_fullscreen
            }

            onPositionChanged: {
                header.show()
                footer.show()
                headerTimer.restart()
            }
        }
    }
}
