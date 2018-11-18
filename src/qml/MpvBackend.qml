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
import "util.js" as Util

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
        }

        renderer.setOption("audio-client-name", "Orion");
        renderer.setOption("title", description);

        renderer.command(["loadfile", src, "replace"])
        resume()
    }

    function resume() {
        renderer.command(["set", "pause", "no"])
    }

    function pause() {
        renderer.command(["set", "pause", "yes"])
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
        status = "BUFFERING"
        renderer.setProperty("playback-time", sec)
        root.position = sec;
    }

    function setVolume(vol) {
        volume = Math.round(vol)
    }

    function getDecoder() {
        var defaultDecoders = []
        if (Qt.platform.os == "windows") {
            defaultDecoders = [ "dxva2-copy", "d3d11va-copy", "cuda-copy", "nvdec-copy", "no" ]
        } else if (Qt.platform.os == "osx" || Qt.platform.os == "ios") {
            defaultDecoders = [ "videotoolbox", "no" ]
        } else if(Qt.platform.os == "android") {
            defaultDecoders = [ "mediacodec_embed", "no" ]
        } else if (Qt.platform.os == "linux") {
            defaultDecoders = [ "vaapi-copy", "vdpau-copy", "cuda-copy", "nvdec-copy", "no" ]
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

    property double volume: 100
    onVolumeChanged: {
        renderer.setProperty("volume", volume)
    }

    MpvObject {
        id: renderer

        anchors.fill: parent

        function updateStatus() {
            if (idleActive) {
                if (root.status !== "STOPPED") root.playingStopped()
                root.status = "STOPPED"
            } else if (bufferingState >= 100 && !seeking) {
                if (!getProperty("pause")) {
                    if (root.status != "PLAYING") root.playingResumed()
                    root.status = "PLAYING"
                } else if (coreIdle) {
                    if (root.status === "BUFFERING") command(["frame-step"])
                    if (root.status !== "PAUSED") root.playingPaused()
                    root.status = "PAUSED"
                } else {
                    root.status = "BUFFERING"
                }
            } else {
                root.status = "BUFFERING"
            }
        }

        onPlaybackTimeChanged: root.position = playbackTime
        onCoreIdleChanged: Qt.callLater(updateStatus)
        onBufferingStateChanged: Qt.callLater(updateStatus)
        onIdleActiveChanged: Qt.callLater(updateStatus)
        onSeekingChanged: Qt.callLater(updateStatus)
        onVolumeChanged: root.volumeChangedInternally()

        property real bufferingState: 0
        property bool coreIdle: true
        property bool idleActive: true
        property bool seeking: false
        property real volume: getProperty("volume")
        property real playbackTime: 0

        Timer {
            interval: 1000
            repeat: true
            running: !renderer.idleActive
            onTriggered: {
                // https://github.com/mpv-player/mpv/issues/4195
                renderer.playbackTime = renderer.getProperty("playback-time") || 0
            }
        }

        Component.onCompleted: {
            renderer.observeProperty("cache-buffering-state", function(value) { bufferingState = value });
            renderer.observeProperty("core-idle", function(value) { coreIdle = value });
            renderer.observeProperty("idle-active", function(value) { idleActive = value });
            renderer.observeProperty("seeking", function(value) { seeking = value });
            renderer.observeProperty("volume", function(value) { volume = value });
            renderer.observeProperty("playback-time", function(value) { playbackTime = value || 0 });

            root.setVolume(Math.round(volume));
            root.volumeChangedInternally()
        }
    }
}
