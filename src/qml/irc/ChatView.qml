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

Rectangle {
    id: root

    implicitHeight: dp(400)
    implicitWidth: dp(200)

    color: Styles.sidebarBg

    onVisibleChanged: {
        if (visible)
            _input.forceActiveFocus()
    }

    function joinChannel(channel) {
        if ("#" + channel != chat.channel) {
            chatModel.clear()
            chat.joinChannel(channel)
        }
    }

    function leaveChannel() {
        chatModel.clear()
        chat.leaveChannel()
    }

    function sendMessage() {
        chat.sendChatMessage(_input.text)
        _input.text = ""
        list.positionViewAtEnd()
    }

    Connections {
        target: g_rootWindow

        onHeightChanged: {
            list.positionViewAtEnd()
        }
    }

    ListView {
        id: list

        property bool lock: true
        property int scrollbuf: 0

        model: ListModel {
            id: chatModel
        }

        clip: true
        highlightFollowsCurrentItem: true
        spacing: dp(10)

        delegate: Component {
            ChatMessage {
                user: model.user
                msg: model.message

                width: root.width
            }
        }

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: spacer.top
        }

        onContentYChanged: {

            if (atYEnd)
                lock = true;
            else if (scrollbuf < 0)
                lock = false
            else
                scrollbuf--
        }
    }

    Rectangle {
        id: spacer
        anchors {
            left: parent.left
            right: parent.right
            bottom: inputArea.top
        }
        height: dp(3)
        color: Styles.border
    }

    Rectangle {
        id: inputArea

        height: dp(30)
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        color: Styles.bg

        MouseArea {
            cursorShape: Qt.IBeamCursor
            anchors {
                fill: parent
            }

            TextInput{
                id: _input
                anchors {
                    fill: parent
                    leftMargin: dp(5)
                    rightMargin: dp(5)
                }
                color: "#ffffff"
                clip:true
                selectionColor: Styles.purple
                focus: true
                selectByMouse: true
                font.pixelSize: Styles.titleFont.smaller
                verticalAlignment: Text.AlignVCenter

                Keys.onReturnPressed: sendMessage()
            }
        }
    }

    Chat {
        id: chat

        onMessageReceived: {
            chatModel.append({"user": user, "message": message})
            list.scrollbuf = 6

            var max = 1000
            //Limit msg count in list
            if (chatModel.count > max) {
                chatModel.remove(0, chatModel.count - max)
            }

            if (list.lock)
                list.positionViewAtEnd()
        }

        onClear: {
            chatModel.clear()
        }
    }
}
