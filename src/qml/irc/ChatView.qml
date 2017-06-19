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
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import "../components"
import "../util.js" as Util

Page {
    id: root

    onVisibleChanged: {
        if (visible) {
            _input.forceActiveFocus()
        } else {
            _emotePicker.visible = false;
        }
    }

    function cleanupPrevChannel() {
        if (chat.lastBttvChannelEmotes != null) {
            _emoteButton.clearChannelSpecificEmotes()
            chat.lastBttvChannelEmotes = null;
        }
        list.chatModel.clear()
    }

    function joinChannel(channel, channelId) {
        if (channel !== chat.channel || chat.replayMode) {
            chatContainer.currentIndex = 0
            cleanupPrevChannel()
            chat.joinChannel(channel, channelId)
        }
    }

    function leaveChannel() {
        cleanupPrevChannel()
        chat.leaveChannel()
    }

    function replayChat(channelName, channelId, vodId, startEpochTime, startPos) {
        chatContainer.currentIndex = 0
        cleanupPrevChannel()
        chat.leaveChannel()
        chat.replayChat(channelName, channelId, vodId, startEpochTime, startPos);
    }

    function playerSeek(newOffset) {
        if (chat.replayMode) {
            chat.replaySeek(newOffset);
        }
    }

    function playerPositionUpdate(newOffset) {
        if (chat.replayMode) {
            chat.replayUpdate(newOffset);
        }
    }

    function sendMessage() {
        var message = _input.text;
        var relevantEmotes = {};
        var words = message.split(" ");
        for (var i = 0; i < words.length; i++) {
            var word = words[i];
            var emoteId = chat.lookupEmote(word);
            if (emoteId != null) {
                //console.log("Adding relevant emote", word, emoteId);
                relevantEmotes[word] = emoteId;
            }
        }
        chat.sendChatMessage(message, relevantEmotes)
        _input.text = ""
        list.positionViewAtEnd()
    }

    function loadEmoteSets() {
        //console.log("current emote set ids")
        //console.log(chat.lastEmoteSetIDs)
        if (chat.lastEmoteSetIDs) {
            // load the emote sets so that we know what icons to display
            g_cman.loadEmoteSets(false, chat.lastEmoteSetIDs);
        }
    }

    function loadEmotes() {
        //console.log("loadEmotes()", chat.lastEmoteSets);
        if (!_emoteButton.pickerLoaded) {
            if (chat.lastEmoteSets) {
                _emotePicker.loading = true;
                //console.log("downloading chat.lastEmoteSets", chat.lastEmoteSets);
                _emoteButton.startDownload(chat.lastEmoteSets);
            }
        } else if (_emoteButton.pickerChannelLoaded != chat.channel) {
            // just download channel emotes we don't have yet
            _emotePicker.loading = true;
            _emoteButton.startDownload(null);
        }

        _emoteButton.pickerLoaded = true;
        _emoteButton.pickerChannelLoaded = chat.channel;
    }

    Connections {
        target: g_cman
        onEmoteSetsLoaded: {
            //console.log("emote sets loaded:");
            for (var i in emoteSets) {
                //console.log("  ", i);
                var entry = emoteSets[i];
                for (var j in entry) {
                    //console.log("    ", j, entry[j]);
                }
            }

            if (emoteSets) {
                chat.lastEmoteSets = emoteSets;
            }

        }
    }

    Connections {
        target: g_rootWindow

        onHeightChanged: {
            list.positionViewAtEnd()
        }
    }


    header: ToolBar {
        id: chatControls
        Material.background: Material.background

        IconButtonFlat {
            id: _viewerListButton
            text: "\ue7fb"

            enabled: (!isVod && currentChannel && currentChannel.name) ? true : false
            highlighted: chatContainer.currentIndex === 1

            onClicked: {
                chatContainer.currentIndex = chatContainer.currentIndex === 1  ? 0 : 1
            }
        }
    }

    StackLayout {
        id: chatContainer

        anchors.fill: parent

        ChatMessagesView {
            id: list
        }

        ViewerList {
            id: viewerList
        }

        EmotePicker {
            id: _emotePicker

            visible: false
            height: 0

            devicePixelRatio: chat.getHiDpi()? 2.0 : 1.0

            //fontPixelSize: Styles.titleFont.smaller * g_cman.textScaleFactor

            onVisibleChanged: {
                if (visible) {
                    focusFilterInput();
                    height = dp(320);
                }
            }

            function startClosing() {
                //visible = false;
                height = 0;
                _emotePickerCloseTimer.start();
            }

            onCloseRequested: {
                startClosing();
                _input.focus = true;
            }

            Behavior on height {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutCubic
                }
            }

            Timer {
                id: _emotePickerCloseTimer
                interval: 200
                repeat: false
                onTriggered: {
                    _emotePicker.visible = false
                }
            }

//            color: "#ffffff"

            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            model: _emoteButton.setsVisible

            filterTextProperty: "emoteName"

            onItemClicked: {
                var item = _emoteButton.setsVisible.get(index);
                _emoteButton.addEmoteToChat(item.emoteName);
            }

            onMoveFocusDown: {
                _input.focus = true;
            }
        }

        Chat {
            id: chat

            property variant colors:[]
            property string emoteDirPath
            property variant lastEmoteSetIDs
            property variant lastEmoteSets
            property variant lastBttvChannelEmotes
            property variant lastBttvGlobalEmotes

            property variant _textEmotesMap
            property variant _regexEmotesList

            property var lastBadgeUrls: ({})
            property var lastChannelBetaBadgeSetData: ({})
            property var globalBetaBadgeSetData: ({})
            property var lastBetaBadgeSetData: ({})

            property bool debugOutput: false

            onLastEmoteSetsChanged: {
                initEmotesMaps();
            }

            onBttvEmotesLoaded: {
                /*
            console.log("received bttv emotes for", channel);
            for (var i in emotesByCode) {
                console.log("code", i, "id", emotesByCode[i]);
            }
            */

                if (channel == "GLOBAL") {
                    chat.lastBttvGlobalEmotes = emotesByCode;
                } else if (channel == chat.channel) {
                    chat.lastBttvChannelEmotes = emotesByCode;
                } else {
                    //console.log("bttv emotes loaded for a different channel", channel);
                }
            }

            function initEmotesMaps() {
                var plainText = /^[\da-z]+$/i;
                chat._textEmotesMap = {};
                chat._regexEmotesList = [];
                var emoteSets = lastEmoteSets;
                for (var i in emoteSets) {
                    //console.log("  ", i);
                    var entry = emoteSets[i];
                    for (var emoteId in entry) {
                        var emoteText = entry[emoteId];
                        if (Util.regexExactMatch(plainText, emoteText)) {
                            //console.log("adding plain text emote", emoteText, emoteId);
                            chat._textEmotesMap[emoteText] = emoteId;
                        } else {
                            //Just checking whether our invert text has entities is fine for all the existing global emotes
                            //TODO actually parse the entire regex so we don't miss any cases that match html entities
                            var htmlText = Util.inverseRegex(emoteText);
                            var decodedText = Util.decodeHtml(htmlText);
                            var useHtmlDomain = htmlText != decodedText;
                            //console.log("adding regex emote", emoteText, emoteId, "useHtmlDomain:", useHtmlDomain);
                            chat._regexEmotesList.push({"regex": new RegExp(emoteText), "emoteId": emoteId, "useHtmlDomain": useHtmlDomain});
                        }
                    }
                }
            }

            function lookupEmote(word) {
                var emoteId = _textEmotesMap[word];
                if (emoteId != null) {
                    return emoteId;
                }
                for (var i = 0; i < _regexEmotesList.length; i++) {
                    var entry = _regexEmotesList[i];

                    var matchInput = word;
                    if (entry.useHtmlDomain) {
                        //console.log("using html domain for", entry.regex);
                        matchInput = Util.encodeHtml(word);
                        //console.log(matchInput)
                    }
                    if (Util.regexExactMatch(entry.regex, matchInput)) {
                        return entry.emoteId;
                    }
                }
                return null;
            }

            onSetEmotePath: {
                emoteDirPath = value
            }

            onMessageReceived: {
                if (debugOutput) console.log("ChatView chat override onMessageReceived; typeof message " + typeof(message) + " toString: " + message.toString());

                if (chatColor != "") {
                    colors[user] = chatColor;
                }

                if (!colors[user]) {
                    colors[user] = Util.getRandomColor()
                }

                // ListElement doesn't support putting in an array value, ugh.
                var serializedMessage = JSON.stringify(message);
                if (debugOutput) console.log("onMessageReceived: passing: " + serializedMessage);

                var badgeEntries = [];
                var imageFormatToUse = "image";
                var badgesSeen = {};

                if (debugOutput) console.log("badges for this message:")
                for (var k = 0; k < badges.length; k++) {
                    var badgeName = badges[k][0];
                    var versionStr = badges[k][1];
                    if (debugOutput) console.log("  badge", badgeName, versionStr);

                    if (badgesSeen[badgeName]) {
                        continue;
                    } else {
                        badgesSeen[badgeName] = true;
                    }

                    var curBadgeAdded = false;

                    var badgeLocalUrl = chat.getBadgeLocalUrl(badgeName + "-" + versionStr);

                    var badgeSetData = lastBetaBadgeSetData[badgeName];
                    if (badgeSetData != null) {
                        var versionObj = badgeSetData[versionStr];
                        if (versionObj == null) {
                            console.log("  beta badge set for", badgeName, "has no version entry for", versionStr);
                            console.log("  available versions are", Util.keysStr(badgeSetData))
                        } else {
                            var devicePixelRatio = 1.0;
                            if (Util.endsWith(badgeLocalUrl, "-image_url_2x")) {
                                devicePixelRatio = 2.0;
                            } else if (Util.endsWith(badgeLocalUrl, "-image_url_4x")) {
                                devicePixelRatio = 3.0;
                            }
                            var entry = {"name": versionObj.title, "url": badgeLocalUrl, "click_action": versionObj.click_action, "click_url": versionObj.click_url, "devicePixelRatio": devicePixelRatio}
                            if (debugOutput) console.log("adding entry", JSON.stringify(entry));

                            badgeEntries.push(entry);
                            curBadgeAdded = true;
                        }
                    }

                    var badgeUrls = lastBadgeUrls[badgeName];
                    if (!curBadgeAdded && badgeUrls != null) {
                        if (debugOutput) {
                            console.log("  badge urls:")
                            for (var j in badgeUrls) {
                                console.log("    key", j, "value", badgeUrls[j]);
                            }
                        }
                        var entry = {"name": badgeName, "url": badgeLocalUrl, "devicePixelRatio": 1.0};
                        if (debugOutput) console.log("adding entry", JSON.stringify(entry));
                        badgeEntries.push(entry);
                        curBadgeAdded = true;
                    }

                    if (!curBadgeAdded) {
                        console.log("  Unknown badge", badgeName);
                    }
                }

                var jsonBadgeEntries = JSON.stringify(badgeEntries);

                list.chatModel.append({"user": user, "message": serializedMessage, "isAction": isAction, "jsonBadgeEntries": jsonBadgeEntries, "isChannelNotice": isChannelNotice, "systemMessage": systemMessage, "isWhisper": isWhisper})
                list.scrollbuf = 6
            }

            onEmoteSetIDsChanged: {
                lastEmoteSetIDs = emoteSetIDs
                loadEmoteSets()
            }

            onChannelBadgeUrlsLoaded: {
                console.log("onChannelBadgeUrlsLoaded for channel", channelId, "current channel is", chat.channelId);
                if (channelId == chat.channelId) {
                    console.log("saving lastBadgeUrls", badgeUrls)
                    for (var i in badgeUrls) {
                        console.log("  ", i, badgeUrls[i]);
                    }
                    lastBadgeUrls = badgeUrls;
                }
            }

            onChannelBadgeBetaUrlsLoaded: {
                console.log("onChannelBadgeBetaUrlsLoaded for channel", channel, "current channel is", chat.channelId.toString());
                if (channel == chat.channelId.toString()) {
                    console.log("saving lastBetaBadgeUrls", badgeSetData)
                    lastChannelBetaBadgeSetData = badgeSetData;
                }
                else if (channel == "GLOBAL") {
                    console.log("saving globalBetaBadgeUrls", badgeSetData)
                    globalBetaBadgeSetData = badgeSetData;
                }
                else return;

                for (var i in badgeSetData) {
                    console.log("  ", i, badgeSetData[i]);
                }

                // assemble
                lastBetaBadgeSetData = Util.objectAssign({}, globalBetaBadgeSetData, lastChannelBetaBadgeSetData);
            }

            onClear: {
                cleanupPrevChannel()
            }
        }
    }

    footer: ToolBar {
        Material.background: Material.background
        Material.elevation: 10
        visible: !chat.isAnonymous
        padding: 5

        RowLayout {
            anchors.fill: parent

            TextField {
                id: _input
                Layout.fillWidth: true

                Keys.onUpPressed: {
                    if (_emotePicker.visible) {
                        _emotePicker.focusEntersFromBottom();
                    } else {
                        _emotePicker.visible = true
                    }
                }

                onAccepted: {
                    sendMessage()
                }
            }

            EmoteSelector {
                id: _emoteButton
            }
        }
    }
}
