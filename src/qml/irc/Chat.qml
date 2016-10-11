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

    signal messageReceived(string user, string message)
    signal clear()

    property string accesstoken: g_cman.accesstoken
    onAccesstokenChanged: {
        console.log("Setting chat password")
        chat.password = "oauth:" + accesstoken
        reconnect()
    }

    property var channel: undefined

    Connections {
        target: g_cman
        onUserNameUpdated: {
            console.log("Setting chat username: " + name)
            chat.name = name
            reconnect()
        }
    }

    function joinChannel(channelName) {
        chat.join(channelName)
        clear()
    }

    function leaveChannel() {
        if (chat.connected)
            chat.disconnect()
        clear()
    }

    function sendChatMessage(message) {
        if (chat.connected)
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
            root.messageReceived(user, message)
        }

        onNoticeReceived: {
            root.messageReceived("--NOTIFICATION--", message)
        }
    }
}
