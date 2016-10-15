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

import QtQuick 2.0
import "../styles.js" as Styles

Item {
    id: root
    property string user
    property string msg
    property int fontSize: Styles.titleFont.smaller

    height: childrenRect.height

    Component.onCompleted: {
        _text.text = "<b>%1</b>".arg(user) + (msg ? ": " : "")

        if (msg)
            parseMsg(msg)
    }

    function parseMsg(msg) {

        var mlist = msg.split(" ")
        var textStr = ""

        for (var i=0; i < mlist.length; i++) {
            var str = mlist[i]

            if (!str)
                continue

            textStr += "%1 ".arg(!isUrl(str) ? str : makeUrl(str))
        }

        _text.text += textStr.trim()

        //console.log("Created text object: " + textStr)
    }

    function makeUrl(str) {
        return "<a href=\"%2%1\">%1</a>"
            .arg(str)
            .arg(str.match(/^(https?:\/\/)/) ? "" : "http://") //Set http:// start if omitted
    }

    function isUrl(str) {
        return str.match(/^(https?:\/\/)?([\da-z\.-]+)\.([a-z]{2,6})([\/\w \.-]*)*\/?$/)
    }

    Text {
        id: _text
        anchors {
            left: parent.left
            right: parent.right
        }
        verticalAlignment: Text.AlignVCenter
        color: Styles.textColor
        font.pixelSize: fontSize
        linkColor: Styles.purple
        wrapMode: Text.WordWrap
        onLinkActivated: Qt.openUrlExternally(link)
        //renderType: Text.NativeRendering
    }
}
