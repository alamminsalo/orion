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

.pragma library

//Returns time in HH:MM:SS presentation, arg in secs
function getTime(totalSec){

    if (!totalSec)
        return "NaN"

    var days = Math.floor(parseInt(totalSec / (3600 * 24)))
    var hours = parseInt(totalSec / 3600) % 24;
    var minutes = parseInt(totalSec / 60) % 60;
    var seconds = totalSec % 60;

    var result = "";

    if (days > 0)
        result += days + ":";
    if (days > 0 || hours > 0)
        result += hours + ":";
    result += (minutes < 10 ? "0" + minutes : minutes) + ":";
    result += (seconds  < 10 ? "0" + seconds : seconds);

    return result
}

function endsWith(s, suffix) {
    return s.length >= suffix.length && s.substring(s.length - suffix.length) === suffix;
}
