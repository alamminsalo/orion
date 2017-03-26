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
//import Communi 3.0
import aldrog.twitchtube.ircchat 1.0


Item {
    id: root

    signal messageReceived(string user, variant message, string chatColor, bool subscriber, bool turbo, bool isAction)
    signal setEmotePath(string value)
    signal notify(string message)
    signal clear()

    property alias isAnonymous: chat.anonymous
    property var channel: undefined
    property var singleShot: undefined

    Component.onCompleted: {
        chat.initProviders()
    }

    Connections {
        target: g_cman

        onLogin: {
            //console.log("Login command: ", username, password)
            chat.name = username
            chat.password = password.length > 0 ? "oauth:" + password : ""

            chat.reopenSocket()
        }
    }

    function joinChannel(channelName) {
        chat.join(channelName)
        root.channel = channelName
        messageReceived("Joined channel #" + channelName, null, "", false, false, false)
    }

    function leaveChannel() {
        chat.leave()
    }

    function sendChatMessage(message) {
        chat.sendMessage(message)
    }

    function reconnect() {
        leaveChannel()
        if (root.channel)
            joinChannel(root.channel)
    }

    IrcChat {
        id: chat

        onConnectedChanged: {
            if (connected) {
                console.log("Connected to chat")
                if (root.channel) {
                    joinChannel(root.channel)
                }
            } else {
                console.log("Disconnected from chat")
            }
        }

        onMessageReceived: {
            root.setEmotePath(emoteDirPath)
            root.messageReceived(user, message, chatColor, subscriber, turbo, isAction)
        }

        onNoticeReceived: {
            root.messageReceived("--NOTIFICATION--", message, null, null, false, false)
        }
    }
}
