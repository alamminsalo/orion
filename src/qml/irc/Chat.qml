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
    signal bulkDownloadComplete()
    signal channelBadgeUrlsLoaded(int channelId, var badgeUrls)
    signal channelBadgeBetaUrlsLoaded(string channel, var badgeSetData)
    signal bttvEmotesLoaded(string channel, var emotesByCode)

    property alias isAnonymous: chat.anonymous
    property var channel: undefined
    property var channelId: undefined
    property var singleShot: undefined
    property var replayMode: false

    function getHiDpi() {
        return chat.getHiDpi();
    }

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
            root.channelBadgeUrlsLoaded(channelId, badgeUrls);
        }

        onChannelBadgeBetaUrlsLoaded: {
            console.log("onChannelBadgeBetaUrlsLoaded", "channel", channel, "badgeSetData", badgeSetData);
            root.channelBadgeBetaUrlsLoaded(channel, badgeSetData);
        }
    }

    function enterChannelCommon(channelName, channelId) {
        root.channel = channelName
        root.channelId = channelId
        g_cman.loadChannelBadgeUrls(channelId);
        g_cman.loadChannelBetaBadgeUrls(channelId);
        g_cman.loadChannelBitsUrls(channelId);
        g_cman.loadChannelBttvEmotes(channelName);
    }

    function joinChannel(channelName, channelId) {
        chat.join(channelName, channelId)
        enterChannelCommon(channelName, channelId);
        if (root.replayMode) {
            chat.replayStop();
        }
        root.replayMode = false;
        messageReceived("notice", null, "", false, false, false, [], true, "Joined channel #" + channelName, false)
    }

    function replayChat(channelName, channelId, vodId, startEpochTime) {
        chat.replay(channelName, channelId, vodId, startEpochTime, 0)
        enterChannelCommon(channelName, channelId);
        root.replayMode = true
        messageReceived("notice", null, "", false, false, false, [], true, "Starting chat replay #" + channelName + " v" + vodId, false)
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

    function downloadBttvEmotesGlobal() {
        return chat.downloadBttvEmotesGlobal();
    }

    function downloadBttvEmotesChannel() {
        return chat.downloadBttvEmotesChannel();
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
                if (root.channel) {
                    if (root.replayMode) {
                        console.log("Reconnected; chat replay may resume")
                    } else {
                        console.log("Connected to chat")
                        joinChannel(root.channel, root.channelId)
                    }
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

        onBulkDownloadComplete: {
            root.bulkDownloadComplete();
        }

        onBttvEmotesLoaded: {
            root.bttvEmotesLoaded(channel, emotesByCode);
        }
    }
}
