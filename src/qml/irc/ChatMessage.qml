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
        var ACTION_PREFIX = "\u0001ACTION";
        var ACTION_SUFFIX = "\u0001";

        function startswith(s, a) {
            return s.length >= a.length && s.substring(0, a.length) == a;
        }

        function endswith(s, a) {
            return s.length >= a.length && s.substring(s.length - a.length) == a;
        }

        if (msg)
        {
            if (startswith(msg, ACTION_PREFIX) && endswith(msg, ACTION_SUFFIX)) {
                var action = msg.substring(ACTION_PREFIX.length, msg.length - ACTION_SUFFIX.length);
                _text.text = "<font color=\""+chat.colors[user]+"\"><a href=\"user:%1\"><b>%1</b></a> %2</font>".arg(user).arg(action);
            } else {
                _text.text = "<font color=\""+chat.colors[user]+"\"><a href=\"user:%1\"><b>%1</b></a></font>".arg(user) + (msg ? ": " : "");
                parseMsg(msg)
            }
        }
        else
            _text.text = "<font color=\"#FFFFFF\"><b>%1</b></font>".arg(user) + (msg ? ": " : "")
        _text.user = user
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
        var urlPattern = /\b(?:https?):\/\/[a-z0-9-+&@#\/%?=~_|!:,.;]*[a-z0-9-+&@#\/%=~_|]/gim;
        var pseudoUrlPattern = /(^|[^\/])(www\.[\S]+(\b|$))/gim;
        return str.replace(urlPattern, '<a href="$&">$&</a>').replace(pseudoUrlPattern, '$1<a href="http://$2">$2</a>');
    }

    function isUrl(str) {
        return str.match(/^(https?:\/\/)?([\da-z\.-]+)\.([a-z]{2,6})([\/\w \.-]*)*\/?$/)
    }

    Text {
        id: _text
        property string user: ""
        anchors {
            left: parent.left
            right: parent.right
        }
        verticalAlignment: Text.AlignVCenter
        color: Styles.textColor
        font.pixelSize: fontSize
        linkColor: Styles.purple
        wrapMode: Text.WordWrap
        onLinkActivated: function(link)
        {
            if (link.substr(0,5) === "user:")
            {
                var value = "@"+link.replace('user:',"")+', '
                if (_input.text === "")
                {
                    _input.text = value
                }
                else {
                    _input.text = _input.text + ' '+ value
                }

            }
            else
            {
                Qt.openUrlExternally(link)
            }
        }

    }
}
