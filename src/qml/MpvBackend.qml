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
import mpv 1.0

/* Interface for backend Mpv

Functions needed:
load(src, start)    -- Loads stream src, if given, start sets the starting milliseconds in vods
resume()            -- Forces resume
pause()             -- Forces pause
togglePause()       -- Toggles between playing and pausing
stop()              -- Stops playback
seekTo(ms)          -- Seeks to milliseconds in the current source, works only on vods
setVolume(vol)      -- Number between 0 - 100
getDecoder()        -- Return list of video decoders
setDecoder(idx)     -- Set video decoder

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

    function load(src, start, description) {
        console.log("Loading src", src, start)
        status = "BUFFERING"

        stop();

        if (start >= 0) {
            position = start
            renderer.setOption("start", "+" + position)
            lastStartPosition = position;
            streamOffsetCalibrated = false;
            streamOffset = 0;
        }

        renderer.setOption("audio-client-name", "Orion");
        renderer.setOption("title", description);

        renderer.command(["loadfile", src])

        resume()
    }

    function resume() {
        renderer.play(false)
    }

    function pause() {
        renderer.pause()
    }

    function stop() {
        renderer.command(["stop"])
    }

    function togglePause() {
        if (status == "PAUSED" || status == "STOPPED")
            resume()
        else
            pause()
        console.log(status)
    }

    function seekTo(sec) {
        var adjustedSec = sec;
        if (streamOffsetCalibrated) {
            adjustedSec += streamOffset;
        }
        renderer.setProperty("playback-time", adjustedSec)
        root.position = sec;
    }

    function setVolume(vol) {
        if (Qt.platform.os === "linux")
            volume = Math.round(Math.log(vol) / Math.log(100))
        else
            volume = Math.round(vol)
    }

    function getDecoder() {
        var defaultDecoders = []
        if (Qt.platform.os == "windows") {
            defaultDecoders = [ "dxva2-copy", "d3d11va-copy", "cuda-copy", "no" ]
        } else if (Qt.platform.os == "osx" || Qt.platform.os == "ios") {
            defaultDecoders = [ "videotoolbox", "no" ]
        } else if(Qt.platform.os == "android") {
            defaultDecoders = [ "mediacodec_embed", "no" ]
        } else if (Qt.platform.os == "linux") {
            defaultDecoders = [ "vaapi-copy", "vdpau-copy", "no" ]
        }
        return [ "auto" ].concat(defaultDecoders)
    }

    function setDecoder(idx) {
        var decoderName = getDecoder()[idx]
        renderer.setProperty("hwdec", decoderName)
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

    property int lastStartPosition;
    property bool streamOffsetCalibrated: false;
    property int streamOffset: 0;

    property double volume: 100
    onVolumeChanged: {
        //console.log("Volume", volume)
        renderer.setProperty("volume", volume)
    }

    Timer {
        id: positionTimer
        interval: 1000
        running: false
        repeat: true
        onTriggered: {
            if (root.status === "PLAYING")
                root.position += 1
        }
    }

    MpvObject {
        id: renderer

        anchors.fill: parent

        onBufferingStarted: {
            root.status = "BUFFERING"
        }

        onPlayingStopped: {
            root.status = "STOPPED"
            positionTimer.stop()
        }

        onPlayingPaused: {
            root.status = "PAUSED"
            positionTimer.stop()
        }

        onPlayingResumed: {
            root.status = "PLAYING"
            positionTimer.start()
        }

        onPositionChanged: {
            var adjustedPosition = position;

            if (!root.streamOffsetCalibrated && status == "PLAYING") {
                root.streamOffset = position - lastStartPosition;
                root.streamOffsetCalibrated = true;
                console.log("MpvBackend stream offset", root.streamOffset);
            }

            if (root.streamOffsetCalibrated) {
                adjustedPosition -= root.streamOffset;
            }

            if (root.position !== adjustedPosition)
                root.position = adjustedPosition

            positionTimer.restart()
        }

        Component.onCompleted: {
            root.setVolume(Math.round(renderer.getProperty("volume")))
            root.volumeChangedInternally()
        }
    }
}
