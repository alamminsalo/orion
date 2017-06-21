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

import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import "../components"
import "../util.js" as Util

import app.orion 1.0

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
    property real fontSize: Settings.textScaleFactor * 12
    property var pmsg: JSON.parse(msg)
    property var badgeEntries: JSON.parse(jsonBadgeEntries)
    property real highlightOpacity: 1.0

    property string channelNoticeBackgroundColor: "#444444"

    property bool showUsernameLine: !isChannelNotice || !systemMessage || (pmsg && pmsg.length > 0)
    property bool showSystemMessageLine: isChannelNotice && systemMessage != ""

    property var visibleBadgeEntries: showUsernameLine? badgeEntries : []

    height: childrenRect.height


    onFontSizeChanged: {
        // defer updatePositions so that bindings to the font size have a chance to recalculate before the re-layout
        Qt.callLater(function() {
            _messageLineFlow.updatePositions()
        })
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

    UILabel {
        id: _systemMessageLine
        anchors {
            left: parent.left
            right: parent.right
            margins: {
                leftMargin: 2
                rightMargin: 2
            }
        }

        visible: showSystemMessageLine
        text: root.systemMessage
        font.pointSize: fontSize
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
              leftMargin: 2
              rightMargin: 2
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

      UILabel {
        id: userName
        // if this ChatMessage is a channel notice with no user message text, don't show a user chat line
        visible: showUsernameLine
        verticalAlignment: Text.AlignVCenter
        font.pointSize: fontSize
        text: "<font color=\""+chat.colors[user]+"\"><a href=\"user:%1\"><b>%1</b></a></font>".arg(user) + (isAction? "&nbsp;": ":&nbsp;")
        onLinkActivated: userLinkActivation(link)

        height: showUsernameLine? contentHeight : 0

        // Disabled as performance regression
//        MouseArea {
//            anchors.fill: parent
//            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
//            acceptedButtons: Qt.NoButton
//        }
      }

      Repeater {
        model: pmsg

        Loader {
          property var msgItem: pmsg[index]
          sourceComponent: {
            if(typeof pmsg[index] == "string") {
              if (Util.isUrl(pmsg[index])) {
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
      UILabel {
        verticalAlignment: Text.AlignVCenter
        color: isAction? chat.colors[user] : Material.foreground
        font.pointSize: fontSize
        text: msgItem
        wrapMode: Text.WordWrap
        textFormat: Text.PlainText
      }
    }
    property Component msgLink: Component {
      Text {
        verticalAlignment: Text.AlignVCenter
        color: isAction? chat.colors[user] : Material.foreground
        font.pointSize: fontSize
        text: Util.makeUrl(msgItem)
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

                width: sourceSize.width/(hiDPI ? 2.0 : 1.0) * Settings.textScaleFactor
                height: sourceSize.height/(hiDPI ? 2.0 : 1.0) * Settings.textScaleFactor

                Component.onCompleted: {
                  source = "image://" + msgItem.imageProvider + "/" + msgItem.imageId;
                }
              }

              UILabel {
                  id: _emoteImgSuffixText
                  text: msgItem.textSuffix
                  color: msgItem.textSuffixColor
                  font.bold: true
                  font.pointSize: fontSize
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
                width: 28 * Settings.textScaleFactor
                height: 28 * Settings.textScaleFactor

                Component.onCompleted: {
                  source = msgItem.sourceUrl;
                }
              }

              Text {
                  id: _animatedImgSuffixText
                  text: msgItem.textSuffix
                  color: msgItem.textSuffixColor
                  font.bold: true
                  font.pointSize: fontSize
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
          width: _badgeImg.width + 2
          height: _badgeImg.height
          Image {
            id: _badgeImg
            Component.onCompleted: {
              source = badgeEntry.url;
            }

            width: sourceSize.width / badgeEntry.devicePixelRatio * Settings.textScaleFactor
            height: sourceSize.height / badgeEntry.devicePixelRatio * Settings.textScaleFactor

//            onStatusChanged: {
//                if (status == Image.Ready) {
//                    _messageLineFlow.updatePositions();
//                }
//            }

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
