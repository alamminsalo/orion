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
import "components"
import mpv 1.0
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
        console.log("Loading src", src, start)
        stop();

        if (start !== undefined) {
            position = start
            renderer.setOption("start", "+" + position)
        }

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

    function seekTo(ms) {
        renderer.setProperty("playback-time", ms)
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
            if (root.position !== position)
                root.position = position
        }

        Component.onCompleted: {
            root.setVolume(Math.round(renderer.getProperty("volume")))
            root.volumeChangedInternally()
        }
    }
}
