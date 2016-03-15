
.pragma library

//Returns time in HH:MM:SS presentation, arg in secs
function getTime(totalSec){

    if (!totalSec)
        return "NaN"

    var hours = parseInt(totalSec / 3600) % 24;
    var minutes = parseInt(totalSec / 60) % 60;
    var seconds = totalSec % 60;

    var result = "";

    if (hours > 0)
        result += hours + ":";
    result += (minutes < 10 ? "0" + minutes : minutes) + ":";
    result += (seconds  < 10 ? "0" + seconds : seconds);

    return result
}
