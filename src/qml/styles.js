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

var ribbonHighlight= "#161619"
var ribbonSelected= "#212125"
var purple= "#7550ba"
var sidebarBg= "#101014"
var highlight= "#101014"
var highlight2= "#222228"
var border= "#32323e"
var borderBright= "#3d3d44"
var bg = "#19191f"
var textColor = "#ffffff"
var shadeColor = "#000000"
var grayScaleColor = "#aa262626"
var iconColor = "#aaaaaa"
var iconHighlight= "#dddddd"
var selectedTextBg= "#212125"
var disabled = "#444444"
var grayButtonColor = "#333333"

var seekBar = "#444444"

var white = "#ffffff"

var connectionErrorColor = "#ff8c00"
var errorTextColor = "#ffffff"

var initialized = false

var dpiMultiplier = 1
//Do this before using!
function applyDpi(dpi) {

    if (!initialized) {
        console.debug("Setting fonts dpi: ", dpi)

        iconSize *= dpi
        iconSizeBigger *= dpi

        titleFont.extrasmall *= dpi
        titleFont.smaller *= dpi
        titleFont.regular *= dpi
        titleFont.bigger *= dpi
        titleFont.large *= dpi

        button.size *= dpi

        initialized = true

        dpiMultiplier = dpi
    }
}

function scale(val) {
    return Math.ceil(val * dpiMultiplier)
}

//NOTE: these values are scaled and need no further scaling

var iconSize = 20
var iconSizeBigger = iconSize + 4

var titleFont = {
    extrasmall: 13,
    smaller: 15,
    regular: 17,
    bigger: 19,
    large: 21
}

var button = {
    size: 18
}
