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

Flow {
    id: root
    property string user
    property string msg
    property int fontSize: Styles.titleFont.smaller

    spacing: fontSize * 0.3

    Component.onCompleted: {
        _text.text = "<b>%1</b>".arg(user) + (msg ? ": " : "")

        if (msg)
            parseMsg(msg)
    }

    function parseMsg(msg) {
        var mlist = msg.split(" ")

        for (var i=0; i < mlist.length; i++) {
            var str = mlist[i]

            if (!str)
                continue

            var dtext = Qt.createQmlObject('import QtQuick 2.0; Text {}', root, "dynText" + i);
            dtext.text = str
            dtext.color= Styles.textColor
            dtext.font.pixelSize = fontSize
            //dtext.wrapMode = Text.WordWrap

            if (isUrl(str)) {
                console.log("IS URL")
                var mArea = Qt.createQmlObject('import QtQuick 2.0; MouseArea {anchors.fill: parent;}', dtext, "mArea" + i);
                mArea.cursorShape = Qt.PointingHandCursor;
                mArea.clicked.connect(function() {
                    Qt.openUrlExternally(str)
                })
                dtext.font.underline = true
                dtext.color = Styles.purple
            }
        }
    }

    function isUrl(str) {
        return str.match(/https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,6}\b([-a-zA-Z0-9@:%_\+.~#?&//=]*)/)
    }

    Text {
        id: _text
        verticalAlignment: Text.AlignVCenter
        color: Styles.textColor
        font.pixelSize: fontSize
        //wrapMode: Text.WordWrap
    }
}
