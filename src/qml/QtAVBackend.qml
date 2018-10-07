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
import QtAV 1.7

/* Interface for backend Mpv

Functions needed:
load(src, start)    -- Loads stream src, if given, start sets the starting milliseconds in vods
resume()            -- Forces resume
pause()             -- Forces pause
togglePause()       -- Toggles between playing and pausing
stop()              -- Stops playback
seekTo(pos)         -- Seeks to milliseconds in the current source, works only on vods
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

    function load(src, start) {
        console.log("Loading src", src, start)
        status = "BUFFERING"

        stop();

        renderer.source = src
        renderer.startPosition = 0

        if (start >= 0) {
            seekTo(start)
        }

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
        status = "BUFFERING"
        renderer.startPosition = pos * 1000
        renderer.seek(pos * 1000)
        root.position = pos
    }

    function setVolume(vol) {
        volume = Math.round(vol)
    }

    function getDecoder() {
        var defaultDecoders = renderer.videoCodecs
        var decoder = [ "auto" ]
        decoder = decoder.concat(defaultDecoders);
        return decoder
    }

    function setDecoder(idx) {
        var decoderName = getDecoder()[idx]

        var opt = renderer.videoCodecOptions
        opt["copyMode"] = Qt.platform.os == "android" ? "OptimizedCopy" : "ZeroCopy"
        renderer.videoCodecOptions = opt

        if (decoderName === "auto") {
            if (g_instance === "child") {
                // disable automatic hw acceleration for multiple instances (usually results in lag)
                renderer.videoCodecPriority = [ "FFmpeg" ]
            } else if (Qt.platform.os == "windows") {
                renderer.videoCodecPriority = [ "DXVA", "FFmpeg" ]
            } else if (Qt.platform.os == "winrt" || Qt.platform.os == "winphone") {
                renderer.videoCodecPriority = [ "D3D11", "FFmpeg" ]
            } else if (Qt.platform.os == "osx" || Qt.platform.os == "ios") {
                renderer.videoCodecPriority = [ "VideoToolbox", "FFmpeg" ]
            } else if(Qt.platform.os == "android") {
                renderer.videoCodecPriority = [ "MediaCodec", "FFmpeg" ]
            } else if (Qt.platform.os == "linux") {
                renderer.videoCodecPriority = [ "VAAPI", "FFmpeg" ]
            }
        } else {
            renderer.videoCodecPriority = [ decoderName ]
        }
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

        function updateStatus() {
            if (status === MediaPlayer.Buffering) {
                root.status = "BUFFERING"
            } else if (playbackState === MediaPlayer.PlayingState) {
                root.status = "PLAYING"
            } else if (playbackState === MediaPlayer.PausedState) {
                root.status = "PAUSED"
            } else if (playbackState === MediaPlayer.StoppedState) {
                root.status = "STOPPED"
            }
        }

        onStatusChanged: {
            updateStatus()
        }

        onStopped: {
            root.status = "STOPPED"
            root.playingStopped()
        }

        onPaused: {
            root.status = "PAUSED"
            root.playingPaused()
        }

        onPlaying: {
            root.status = "PLAYING"
            root.playingResumed()
        }

        onSeekFinished: {
            updateStatus()
        }

        onPositionChanged: {
            if (root.status !== "PLAYING") return
            var pos = position / 1000
            if (root.position !== pos && (startPosition === 0 || position > 0))
                root.position = pos
        }
    }

    Rectangle {
        color: "black"
        anchors.fill: parent

        VideoOutput2 {
            id: output
            anchors.fill: parent
            source: renderer
        }
    }
}
