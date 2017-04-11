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

    signal messageReceived(string user, variant message, string chatColor, bool subscriber, bool turbo, bool isAction, var badges, bool isChannelNotice, string systemMessage)
    signal setEmotePath(string value)
    signal notify(string message)
    signal clear()
    signal emoteSetIDsChanged(var emoteSetIDs)
    signal downloadComplete()
    signal channelBadgeUrlsLoaded(string channel, var badgeUrls)
    signal channelBadgeBetaUrlsLoaded(string channel, var badgeSetData)

    property alias isAnonymous: chat.anonymous
    property var channel: undefined
    property var channelId: undefined
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

        onChannelBadgeUrlsLoaded: {
            console.log("onChannelBadgeUrlsLoaded", "channel", channel, "badgeUrls", badgeUrls);
            root.channelBadgeUrlsLoaded(channel, badgeUrls);
        }

        onChannelBadgeBetaUrlsLoaded: {
            console.log("onChannelBadgeBetaUrlsLoaded", "channel", channel, "badgeSetData", badgeSetData);
            root.channelBadgeBetaUrlsLoaded(channel, badgeSetData);
        }
    }

    function joinChannel(channelName, channelId) {
        chat.join(channelName)
        root.channel = channelName
        root.channelId = channelId
        messageReceived("Joined channel #" + channelName, null, "", false, false, false, {}, true, "")
        g_cman.loadChannelBadgeUrls(channelName);
        g_cman.loadChannelBetaBadgeUrls(channelId);
    }

    function leaveChannel() {
        chat.leave()
    }

    function sendChatMessage(message, relevantEmotes) {
        chat.sendMessage(message, relevantEmotes)
    }

    function bulkDownloadEmotes(emotes) {
        return chat.bulkDownloadEmotes(emotes);
    }

    function reconnect() {
        leaveChannel()
        if (root.channel)
            joinChannel(root.channel, root.channelId)
    }

    IrcChat {
        id: chat

        onConnectedChanged: {
            if (connected) {
                console.log("Connected to chat")
                if (root.channel) {
                    joinChannel(root.channel, root.channelId)
                }
            } else {
                console.log("Disconnected from chat")
            }
        }

        onMessageReceived: {
            root.setEmotePath(emoteDirPath)
            root.messageReceived(user, message, chatColor, subscriber, turbo, isAction, badges, isChannelNotice, systemMessage)
        }

        onNoticeReceived: {
            console.log("Notification received", message);
            root.messageReceived("--NOTIFICATION--", [message], null, null, false, false, {}, true, "")
        }

        onEmoteSetIDsChanged: {
            root.emoteSetIDsChanged(emoteSetIDs)
        }

        onDownloadComplete: {
            console.log("inner download complete");
            root.downloadComplete();
        }
    }
}
