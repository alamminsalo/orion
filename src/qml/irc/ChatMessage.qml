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
    property bool isWhisper
    property string systemMessage
    property int fontSize: Styles.titleFont.smaller
    property var pmsg: JSON.parse(msg)
    property var badgeEntries: JSON.parse(jsonBadgeEntries)
    property var highlightOpacity: 1.0

    property string channelNoticeBackgroundColor: "#444444"

    property bool showUsernameLine: !isChannelNotice || !systemMessage || (pmsg && pmsg.length > 0)
    property bool showSystemMessageLine: isChannelNotice && systemMessage != ""

    property var visibleBadgeEntries: showUsernameLine? badgeEntries : []

    height: childrenRect.height

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
        color: root.channelNoticeBackgroundColor
        opacity: root.highlightOpacity
    }

    Text {
        id: _systemMessageLine
        anchors {
            left: parent.left
            right: parent.right
            margins: {
                leftMargin: dp(2)
                rightMargin: dp(2)
            }
        }

        visible: showSystemMessageLine
        color: Styles.textColor
        text: root.systemMessage
        font.pixelSize: fontSize
        wrapMode: Text.WordWrap

        height: showSystemMessageLine? contentHeight : 0
    }

    CustomFlow {
      id: _messageLineFlow
      ySpacing: 1
      anchors {
          top: _systemMessageLine.bottom
          left: parent.left
          right: parent.right
          margins: {
              leftMargin: dp(2)
              rightMargin: dp(2)
          }
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
        visible: showUsernameLine
        verticalAlignment: Text.AlignVCenter
        color: Styles.textColor
        font.pixelSize: fontSize
        text: "<font color=\""+chat.colors[user]+"\"><a href=\"user:%1\"><b>%1</b></a></font>".arg(user) + (isAction? "&nbsp;": ":&nbsp;")
        onLinkActivated: userLinkActivation(link)

        height: showUsernameLine? contentHeight : 0

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
              if (pmsg[index].imageProvider == "bits") {
                  return bitsImgThing;
              } else {
                  return imgThing;
              }
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
          width: childrenRect.width
          height: childrenRect.height

          Row {
              width: _emoteImg.width + _emoteImgSuffixText.width
              height: _emoteImg.height

              Image {
                id: _emoteImg

                width: sourceSize.width/(chat.getHiDpi()? 2.0 : 1.0)
                height: sourceSize.height/(chat.getHiDpi()? 2.0 : 1.0)

                Component.onCompleted: {
                  source = "image://" + msgItem.imageProvider + "/" + msgItem.imageId;
                }
              }

              Text {
                  id: _emoteImgSuffixText
                  text: msgItem.textSuffix
                  color: msgItem.textSuffixColor
                  font.bold: true
                  verticalAlignment: Text.AlignVCenter
                  height: _emoteImg.height
              }

          }

          ToolTip {
              visible: _emoteImgMouseArea.containsMouse && msgItem.originalText != null
              text: msgItem.originalText
          }
      }
    }
    property Component bitsImgThing: Component {
      MouseArea {
          id: _animatedImgMouseArea
          hoverEnabled: true
          width: _animatedImg.width + _animatedImgSuffixText.width
          height: _animatedImg.height

          Row {
              AnimatedImage {
                id: _animatedImg

                // AnimatedImage doesn't provide a sourceSize properly even when status == AnimatedImage.Ready
                width: 28
                height: 28

                Component.onCompleted: {
                  source = msgItem.sourceUrl;
                }
              }

              Text {
                  id: _animatedImgSuffixText
                  text: msgItem.textSuffix
                  color: msgItem.textSuffixColor
                  font.bold: true
                  verticalAlignment: Text.AlignVCenter
                  height: _animatedImg.height
              }
          }

          ToolTip {
              visible: _animatedImgMouseArea.containsMouse && msgItem.originalText != null
              text: msgItem.originalText
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
            Component.onCompleted: {
              source = badgeEntry.url;
            }

            width: sourceSize.width/badgeEntry.devicePixelRatio
            height: sourceSize.height/badgeEntry.devicePixelRatio

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
            var clickedUser = link.replace('user:',"");
            if (isWhisper) {
                _input.text = "/w " + clickedUser + " " + _input.text;
            } else {
                var value = "@" + clickedUser + ', ';
                if (_input.text === "")
                {
                    _input.text = value
                }
                else {
                    _input.text = _input.text + ' '+ value
                }
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
}
