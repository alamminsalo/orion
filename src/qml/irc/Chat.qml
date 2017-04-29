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

    signal messageReceived(string user, variant message, string chatColor, bool subscriber, bool turbo, bool isAction, var badges, bool isChannelNotice, string systemMessage, bool isWhisper)
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
    property var replayMode: false

    Component.onCompleted: {
        chat.hookupChannelProviders(g_cman)
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
        chat.join(channelName, channelId)
        root.channel = channelName
        root.channelId = channelId
        if (root.replayMode) {
            chat.replayStop();
        }
        root.replayMode = false;
        messageReceived("notice", null, "", false, false, false, [], true, "Joined channel #" + channelName, false)
        g_cman.loadChannelBadgeUrls(channelName);
        g_cman.loadChannelBetaBadgeUrls(channelId);
    }

    function replayChat(channelName, channelId, vodId, startEpochTime) {
        chat.replay(channelName, channelId, vodId, startEpochTime, 0)
        root.channel = channelName
        root.channelId = channelId
        root.replayMode = true
        messageReceived("notice", null, "", false, false, false, [], true, "Starting chat replay #" + channelName + " v" + vodId, false)
        g_cman.loadChannelBadgeUrls(channelName);
        g_cman.loadChannelBetaBadgeUrls(channelId);
    }

    function durationStr(duration) {
        var hours = Math.floor(duration / 3600);
        var mins = Math.floor((duration % 3600) / 60);
        var secs = Math.floor(duration % 60);
        var out = mins.toString() + ":" + (secs < 10 ? "0" : "") + secs.toString();
        if (hours > 0) {
            out = hours.toString() + ":" + (mins < 10 ? "0" : "") + out;
        }
        return out;
    }

    function replaySeek(newOffset) {
        messageReceived("notice", null, "", false, false, false, [], true, "Seeking to " + durationStr(newOffset), false);
        chat.replaySeek(newOffset);
    }

    function replayUpdate(newOffset) {
        chat.replayUpdate(newOffset);
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

    function getBadgeLocalUrl(key) {
        return chat.getBadgeLocalUrl(key);
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
            root.messageReceived(user, message, chatColor, subscriber, turbo, isAction, badges, isChannelNotice, systemMessage, isWhisper)
        }

        onNoticeReceived: {
            console.log("Notification received", message);
            root.messageReceived("channel", [], null, null, false, false, {}, true, message, false)
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
