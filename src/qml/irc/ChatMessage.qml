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
import QtQuick.Controls 2.0
import "../styles.js" as Styles
import "../components"

Item {
    id: root
    property string user
    property var msg
    property bool isAction
    property string jsonBadgeEntries
    property string emoteDirPath
    property bool isChannelNotice
    property string systemMessage
    property int fontSize: Styles.titleFont.smaller
    property var pmsg: JSON.parse(msg)
    property var badgeEntries: JSON.parse(jsonBadgeEntries)
    property var visibleBadgeEntries: userName.visible? badgeEntries : []
    property var highlightOpacity: 1.0

    property string systemMessageBackgroundColor: "#333333"

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
            if (typeof(mlist) == "object") {
                // it's an emote
                var emoteId = mlist.emoteId;
                var emoteText = mlist.emoteText;

                var imgUrl = emoteDirPath + "/" + emoteId.toString();

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
        var pref = "";
        if (str.length && (str.charAt(0) === " ")) {
            pref = "&nbsp;";
            str = str.substring(1);
        }

        var urlPattern = / ?\b(?:https?):\/\/[a-z0-9-+&@#\/%?=~_|!:,.;]*[a-z0-9-+&@#\/%=~_|]/gim;
        var pseudoUrlPattern = /(^|[^\/])(www\.[\S]+(\b|$))/gim;
        var out = pref + str.replace(urlPattern, '<a href="$&">$&</a>').replace(pseudoUrlPattern, '$1<a href="http://$2">$2</a>');

        //console.log("makeUrl", str, out);
        return out;
    }

    function isUrl(str) {
        var result = str.match(/^ ?(https?:\/\/)?([\da-z\.-]+)\.([a-z]{2,6})([\/\w \.-]*)*\/?$/);
        //console.log("isUrl", str, result);
        return result
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            top: _systemMessageLine.top
            bottom: _messageLineFlow.bottom
        }

        visible: isChannelNotice
        color: root.systemMessageBackgroundColor
        opacity: root.highlightOpacity
    }

    Text {
        id: _systemMessageLine
        anchors {
            left: parent.left
            right: parent.right
        }

        visible: root.isChannelNotice && root.systemMessage != ""
        color: Styles.textColor
        text: root.systemMessage
        font.pixelSize: fontSize
        wrapMode: Text.WordWrap

        height: visible? contentHeight : 0
    }

    CustomFlow {
      id: _messageLineFlow
      ySpacing: 1
      anchors {
          top: _systemMessageLine.bottom
          left: parent.left
          right: parent.right
      }

      vAlign: vAlignCenter

      Repeater {
        model: visibleBadgeEntries

        Loader {
          property var badgeEntry: visibleBadgeEntries[index]
          sourceComponent: {
            return badgeItem
          }
        }
      }

      Text {
        id: userName
        // if this ChatMessage is a channel notice with no user message text, don't show a user chat line
        visible: !root.isChannelNotice || !root.systemMessage || (pmsg && pmsg.length > 0)
        verticalAlignment: Text.AlignVCenter
        color: Styles.textColor
        font.pixelSize: fontSize
        text: "<font color=\""+chat.colors[user]+"\"><a href=\"user:%1\"><b>%1</b></a></font>".arg(user) + (isAction? "&nbsp;": ":&nbsp;")
        onLinkActivated: userLinkActivation(link)

        height: visible? contentHeight : 0

        MouseArea {
            anchors.fill: parent
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            acceptedButtons: Qt.NoButton
        }
      }

      Repeater {
        model: pmsg

        Loader {
          property var msgItem: pmsg[index]
          sourceComponent: {
            if(typeof pmsg[index] == "string") {
              if (isUrl(pmsg[index])) {
                return msgLink;
              } else {
                return msgText;
              }
            } else {
              return imgThing;
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
    property Component msgLink: Component {
      Text {
        verticalAlignment: Text.AlignVCenter
        color: isAction? chat.colors[user] : Styles.textColor
        font.pixelSize: fontSize
        text: makeUrl(msgItem)
        textFormat: Text.RichText
        wrapMode: Text.WordWrap
        onLinkActivated: externalLinkActivation(link)

        MouseArea {
            anchors.fill: parent
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            acceptedButtons: Qt.NoButton
        }
      }
    }
    property Component imgThing: Component {
      MouseArea {
          id: _emoteImgMouseArea
          hoverEnabled: true
          width: _emoteImg.width
          height: _emoteImg.height
          Image {
            id: _emoteImg
            // synchronous to simplify CustomFlow
            Component.onCompleted: {
              source = "image://emote/" + msgItem.emoteId.toString();
            }

            ToolTip {
                visible: _emoteImgMouseArea.containsMouse && msgItem.emoteText != null
                text: msgItem.emoteText
            }
          }
      }
    }

    property Component badgeItem: Component {
      MouseArea {
          id: _badgeImgMouseArea
          hoverEnabled: true
          width: _badgeImg.width + dp(2)
          height: _badgeImg.height
          Image {
            id: _badgeImg
            // synchronous to simplify CustomFlow
            Component.onCompleted: {
              source = badgeEntry.url;
            }

            onStatusChanged: {
                if (status == Image.Ready) {
                    _messageLineFlow.updatePositions();
                }
            }

            ToolTip {
                visible: _badgeImgMouseArea.containsMouse && badgeEntry.name != null
                text: badgeEntry.name
            }
          }

          property bool clickable: badgeEntry.click_action == "visit_url"

          cursorShape: clickable ? Qt.PointingHandCursor : Qt.ArrowCursor

          onClicked: {
              console.log("badge clicked, action", badgeEntry.click_action);
              if (badgeEntry.click_action == "visit_url") {
                  var link = badgeEntry.click_url;
                  console.log("Launching url", link);
                  Qt.openUrlExternally(link);
              }
          }
      }
    }

    function userLinkActivation(link)
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
    }

    function externalLinkActivation(link)
    {
        //console.log("externalLinkActivation", link, "passed");
        if (link.substr(0,5) !== "user:")
        {
            //console.log("opening link")
            Qt.openUrlExternally(link)
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
