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

    property int lastStartPosition;
    property bool streamOffsetCalibrated: false;
    property int streamOffset: 0;

    property double volume: 100
    onVolumeChanged: {
        //console.log("Volume", volume)
        renderer.setProperty("volume", volume)
    }


    MpvObject {
        id: renderer

        anchors.fill: parent

        onPlayingStopped: {
            g_powerman.setScreensaver(true);
            status = "STOPPED"
        }

        onPlayingPaused: {
            g_powerman.setScreensaver(true);
            status = "PAUSED"
        }

        onPlayingResumed: {
            g_powerman.setScreensaver(false);
            status = "PLAYING"
        }

        onBufferingStarted: {
            //spinner.visible = true
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
        }

        Component.onCompleted: {
            root.setVolume(Math.round(renderer.getProperty("volume")))
            root.volumeChangedInternally()
        }
    }
}
