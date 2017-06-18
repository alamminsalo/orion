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

function decodeHtml(html) {
    var entities = {
        "amp": "&",
        "lt": "<",
        "gt": ">",
        "quot": "\""
    }

    var cur = 0;
    var parts = [];
    while (true) {
        var pos = html.indexOf("&", cur);
        if (pos == -1) {
            break;
        }

        parts.push(html.substring(cur, pos));

        var end = html.indexOf(";", pos + 1);
        if (end == -1) {
            console.log("unterminated entity " + html.substring(pos));
            break;
        }

        var entityName = html.substring(pos + 1, end);
        var value = entities[entityName];

        if (!entityName) {
            console.log("unknown entity " + entityName);
            break;
        }

        parts.push(value);

        cur = end + 1;
    }
    parts.push(html.substring(cur));
    return parts.join("");
}

function encodeHtml(unsafe) {
    // per https://stackoverflow.com/questions/6234773/can-i-escape-html-special-chars-in-javascript
    return unsafe
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#039;");
}

function inverseRegex(s) {
    var out = [];
    var unconfirmed = "";
    var showDebug = false;
    for (var i = 0; i < s.length; i++) {
        var cur = s.charAt(i);
        switch (cur) {
        case "\\":
            cur = s.charAt(++i);
            out.push(unconfirmed)
            unconfirmed = cur;
            break;
        case "?":
            // previous was optional
            // assume nope
            unconfirmed = "";
            break;
        case "(":
            // recurse on this part of the regex until | or ) at this depth
            var start = i + 1;
            var end = null;
            var ch;
            var running = true;
            var depth = 0;
            while (running) {
                ch = s.charAt(++i);
                switch (ch) {
                case "\\":
                    i++;
                    break;
                case "(":
                    depth++;
                    break;
                case ")":
                    if (depth == 0) {
                        if (end == null) {
                            end = i;
                        }
                        running = false;
                    } else {
                        depth--;
                    }
                    break;
                case "|":
                    if (depth == 0) {
                        if (end == null) {
                            end = i;
                        }
                    }
                    break;
                }
            }
            out.push(unconfirmed);
            var regexPart = s.substring(start, end);
            // console.log(s, "recursing on", regexPart);
            // showDebug = true;
            unconfirmed = inverseRegex(regexPart);
            break;
        case "[":
            cur = s.charAt(++i);
            if (cur == "\\") {
                cur = s.charAt(++i);
            }

            var end = s.indexOf("]", i + 1);
            if (end == -1) {
                console.log("unterminated [");
                showDebug = true;
            }
            i = end;

            out.push(unconfirmed);
            unconfirmed = cur;
            break;
        default:
            out.push(unconfirmed);
            unconfirmed = cur;
        }
    }

    out.push(unconfirmed);
    out = out.join("");

    /*
    // test the generated text
    var testFailed;

    try {
        var r = new RegExp(s);
        var match = r.exec(out);
        testFailed = match == null || match[0] != out;
    } catch (e) {
        console.log(e, out);
        testFailed = true;
    }
    if (testFailed || showDebug) {
        // mismatch
        console.log("Converted regex " + s + " output " + out + (testFailed? " doesn't match": ""));
    }
    */

    return out;
}
