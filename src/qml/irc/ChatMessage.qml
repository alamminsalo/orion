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
import QtQuick.Layouts 1.0
import "../styles.js" as Styles
import "../components"

Item {
    id: root
    property string user
    property var msg
    property bool isAction
    property string emoteDirPath
    property int fontSize: Styles.titleFont.smaller
    property var pmsg: JSON.parse(msg)

    height: childrenRect.height

    /*
    Component.onCompleted: {
        //console.log("Got: " + msg);
        //console.log("Got toString: " + msg.toString());
        var rmsg = JSON.parse(msg);

        if (rmsg)
        {
            _text.text = "<font color=\""+chat.colors[user]+"\"><a href=\"user:%1\"><b>%1</b></a>".arg(user);
            if (isAction) {
                _text.text += " ";
                parseMsg(rmsg);
            }
            _text.text += "</font>";
            if (!isAction) {
                _text.text += ": ";
                parseMsg(rmsg)
            }
        }
        else
        {
            _text.text = "<font color=\"#FFFFFF\"><b>%1</b></font>".arg(user)
        }
        _text.user = user
    }
    */

    function parseMsg(msg) {
        console.log("We are in parseMsg()");
        console.log("msg typeof is " + typeof(msg));
        //console.log("msg length is " + msg.length.toString());
        console.log("msg tostring is " + msg.toString());

        console.log("typeof msg.length " + typeof(msg.length));
        console.log("msg.length " + msg.length.toString());

        for (var j=0; j < msg.length; j++) {
            var mlist = msg[j];
            console.log("cur mlist entry " + j.toString() + " typeof is " + typeof(mlist));
            if (typeof(mlist) == "number") {
                // it's an emote
                var imgUrl = emoteDirPath + "/" + mlist.toString();

                _text.text += "<img src=\"" + imgUrl + "\"></img>";

            } else {
                mlist = mlist.split(" ")
                var textParts = [];

                for (var i=0; i < mlist.length; i++) {
                    var str = mlist[i]

                    textParts.push(!isUrl(str) ? str : makeUrl(str))
                }
                _text.text += textParts.join(" ");
            }

        }

        console.log("Created text object: " + _text.text);
    }

    function makeUrl(str) {
        var urlPattern = /\b(?:https?):\/\/[a-z0-9-+&@#\/%?=~_|!:,.;]*[a-z0-9-+&@#\/%=~_|]/gim;
        var pseudoUrlPattern = /(^|[^\/])(www\.[\S]+(\b|$))/gim;
        return str.replace(urlPattern, '<a href="$&">$&</a>').replace(pseudoUrlPattern, '$1<a href="http://$2">$2</a>');
    }

    function isUrl(str) {
        return str.match(/^(https?:\/\/)?([\da-z\.-]+)\.([a-z]{2,6})([\/\w \.-]*)*\/?$/)
    }

    CustomFlow {
      anchors {
          left: parent.left
          right: parent.right
      }

      vAlign: vAlignCenter

      Text {
        id: userName
        verticalAlignment: Text.AlignVCenter
        color: Styles.textColor
        font.pixelSize: fontSize
        text: "<font color=\""+chat.colors[user]+"\"><a href=\"user:%1\"><b>%1</b></a></font>".arg(user) + (isAction? "&nbsp;": ":&nbsp;")
      }

      Repeater {
        model: pmsg

        Loader {
          property var msgItem: pmsg[index]
          sourceComponent: {
            if(typeof pmsg[index] == "string") {
              return msgText
            }
            else {
              return imgThing
            }
          }
        }
      }
    }

    property Component msgText: Component {
      Text {
        verticalAlignment: Text.AlignVCenter
        color: isAction? chat.colors[user] : Styles.textColor
        font.pixelSize: fontSize
        text: msgItem
        wrapMode: Text.WordWrap
        textFormat: Text.PlainText
      }
    }
    property Component imgThing: Component {
      Image {
        Component.onCompleted: {
          source = "image://emote/" + msgItem.toString();
        }
        // synchronous to simplify CustomFlow
      }
    }
    /*
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
    */
}
