import QtQuick 2.5
import QtMultimedia 5.6
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
seekTo(pos)         -- Seeks to milliseconds in the current source, works only on vods
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
        console.log("Loading src", src, start)
        stop();

        if (start !== undefined) {
            seekTo(start)
        }

        renderer.source = src

        resume()
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
    }

    function seekTo(pos) {
        renderer.seek(pos * 1000)
        //root.position = ms
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
        //console.log("Position", position)
    }

    property double volume: 100
    onVolumeChanged: {
        //console.log("Volume", volume)
        renderer.volume = volume / 100.0
    }

    MediaPlayer {
        id: renderer

        onStopped: {
            root.status = "STOPPED"
            root.playingStopped()
            g_powerman.setScreensaver(true);
        }

        onPaused: {
            root.status = "PAUSED"
            root.playingPaused()
            g_powerman.setScreensaver(true);
        }

        onPlaying: {
            root.status = "PLAYING"
            root.playingResumed()
            g_powerman.setScreensaver(false);
        }

        onPositionChanged: {
            var pos = position / 1000
            if (root.position !== pos)
                root.position = pos
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
    }
}
