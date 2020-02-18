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
import Qt.labs.settings 1.0 as Labs
import "../components"
import "../util.js" as Util

import app.orion 1.0

Page {
    id: root

    anchors.fill: parent
    property bool pinned: pinBtn.checked && chatdrawer.position > 0
    property alias hasUnreadMessages: chatList.hasUnreadMessages

    onVisibleChanged: {
        if (visible && !isMobile()) {
            _input.forceActiveFocus()
        } else {
            _emotePicker.startClosing();
        }
    }

    function addEmoteToChat(emoteName) {
        _input.remove(_input.selectionStart, _input.selectionEnd);
        var textToAdd = emoteName + " ";
        var existingText = _input.getText(0, _input.selectionEnd);
        if (existingText !== "" && existingText.charAt(existingText.length - 1) !== " ") {
            textToAdd = " " + textToAdd;
        }
        _input.insert(_input.selectionEnd, textToAdd);
    }

    function cleanupPrevChannel() {
        if (chat.lastBttvChannelEmotes != null) {
            _emoteButton.clearChannelSpecificEmotes()
            chat.lastBttvChannelEmotes = null;
        }
        chatList.chatModel.clear()
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
        chatList.positionViewAtEnd()
    }

    function loadEmoteSets() {
        //console.log("current emote set ids")
        //console.log(chat.lastEmoteSetIDs)
        if (chat.lastEmoteSetIDs) {
            // load the emote sets so that we know what icons to display
            Emotes.loadEmoteSets(false, chat.lastEmoteSetIDs);
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
        target: Emotes
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

    // Tab bar header for switching chat/viewers
    header: ToolBar {
        Material.theme: rootWindow.Material.theme
        Material.background: rootWindow.Material.background
        visible: Settings.chatEdge < 2
        RowLayout {
            anchors.fill: parent
        ToolButton {
            id: pinBtn
            checkable: true
            font.family: "Material Icons"
            focusPolicy: Qt.NoFocus
            text: checked ? "\ue897" : "\ue898"
            Labs.Settings {
                property alias chatPinned: pinBtn.checked
            }
        }

        TabBar {
            Layout.fillWidth: true
            font.family: "Material Icons"
            font.pointSize: 12
            currentIndex: chatContainer.currentIndex

            TabButton {
                text: "\ue0b7"
                focusPolicy: Qt.NoFocus
                onClicked: chatContainer.currentIndex = 0
            }

            TabButton {
                text: "\ue7fb"
                focusPolicy: Qt.NoFocus
                onClicked: chatContainer.currentIndex = 1
            }
        }
        }
    }

    StackLayout {
        id: chatContainer
        anchors.fill: parent

        ChatMessagesView {
            id: chatList
        }

        ViewerList {
            id: viewerList
        }

        Chat {
            id: chat

            property variant colors:[]
            property variant originalColors:[]

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

            onChannelChanged: {
                if (!channel) return
                var streamer = channel.toLowerCase();
                if (!originalColors[streamer]) {
                    originalColors[streamer] = Qt.rgba(0.97, 0.26, 0.26, 1)
                    colors[streamer] = Settings.pastelColors ? convertToPastel(originalColors[streamer]) : originalColors[streamer]
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
                if (_textEmotesMap == null) {
                    console.log("FIXME: emotes haven't initialized");
                    return;
                }
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

            function convertToPastel(color) {
                //convert string to qml color
                color = Qt.lighter(color, 1);
                var h = color.hslHue, s = color.hslSaturation, l = color.hslLightness, a = color.a;
                //adjust saturation to range 0.3 - 0.8
                s = s * 0.5 + 0.3;
                // adjust brightness to create minimum contrast of 0.3 hsl-lightness from background
                var offset;
                if (Material.background.hslLightness < 0.5) {
                    offset = Material.background.hslLightness + 0.3
                    l = l * (1.0 - offset) + offset;
                } else {
                    offset = Material.background.hslLightness - 0.3;
                    l = l * offset;
                }
                return Qt.hsla(h, s, l, a)
            }

            Connections {
                target: Settings
                function updateColors() {
                    var newColors = []
                    for (var u in chat.originalColors) {
                        newColors[u] = Settings.pastelColors ? chat.convertToPastel(chat.originalColors[u]) : chat.originalColors[u]
                    }
                    chat.colors = newColors; //invalidate all old colors
                }
                onLightThemeChanged: updateColors()
                onPastelColorsChanged: updateColors()
            }

            onSetEmotePath: {
                emoteDirPath = value
            }

            onMessageReceived: {
                if (debugOutput) console.log("ChatView chat override onMessageReceived; typeof message " + typeof(message) + " toString: " + message.toString());

                var u = user.toLowerCase();

                originalColors[u] = chatColor || originalColors[u] || Util.getRandomColor()
                colors[u] = Settings.pastelColors ? convertToPastel(originalColors[u]) : originalColors[u]

                if (debugOutput) console.log("onMessageReceived: passing: " + JSON.stringify(message));

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

                chatList.chatModel.addMessage({"user": user, "message": message, "isAction": isAction, "badgeEntries": badgeEntries, "isChannelNotice": isChannelNotice, "systemMessage": systemMessage, "isWhisper": isWhisper})
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
        Material.theme: rootWindow.Material.theme
        Material.background: rootWindow.Material.background
        Material.elevation: 10
        visible: chatContainer.currentIndex === 0 && !chat.isAnonymous
        padding: 5

        ColumnLayout {
            anchors.fill: parent

            EmotePicker {
                id: _emotePicker
                Layout.fillHeight: true
                Layout.fillWidth: true
                property real preferredHeight: 0
                Layout.preferredHeight: preferredHeight
                Layout.preferredWidth: parent.width
                clip: true
                focus: true

                visible: preferredHeight > 0

                //devicePixelRatio: Settings.hiDpi() ? 2.0 : 1.0

                function startClosing() {
                    preferredHeight = 0;
                    if (!isMobile() && _input.visible)
                        _input.forceActiveFocus()
                }

                function show() {
                    focusFilterInput();
                    preferredHeight = 320
                }

                onCloseRequested: {
                    startClosing();
                    _input.forceActiveFocus();
                }

                Behavior on preferredHeight {
                    NumberAnimation {
                        id: preferredHeightAnimation
                        duration: 200
                        easing.type: Easing.OutCubic
                    }
                }

                //            color: "#ffffff"

                model: _emoteButton.setsVisible

                filterTextProperty: "emoteName"

                onItemClicked: {
                    var item = _emoteButton.setsVisible.get(index);
                    addEmoteToChat(item.emoteName);
                }

                onMoveFocusDown: {
                    _input.forceActiveFocus();
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredWidth: parent.width
                Layout.alignment: Qt.AlignBottom

                TextArea {
                    id: _input
                    placeholderText: "Send your message"
                    selectByMouse: true
                    wrapMode: TextEdit.Wrap
                    activeFocusOnTab: true

                    Layout.fillWidth: true

                    TextMenu { }

                    Keys.onUpPressed: {
                        var selectedFirstLine = !getText(0, selectionStart).match('[\r\n]')
                        if (event.modifiers === Qt.NoModifier) {
                            if (selectedFirstLine) {
                                if (!_emotePicker.visible) {
                                    _emotePicker.show();
                                }
                                _emotePicker.focusEntersFromBottom();
                            } else {
                                event.accepted = false
                            }
                        } else if (event.modifiers === Qt.ShiftModifier) {
                            if (selectedFirstLine) {
                                _input.select(0, selectionEnd);
                            } else {
                                event.accepted = false;
                            }
                        } else {
                            event.accepted = false;
                        }
                    }

                    Keys.onDownPressed: {
                        if (selectionStart !== selectionEnd) {
                            _input.select(selectionEnd, selectionEnd)
                        } else {
                            event.accepted = false
                        }
                    }

                    Keys.onTabPressed: {
                        if (!_emotePicker.visible) {
                            _emotePicker.show();
                        }
                        _emotePicker.focusFilterInput();
                    }

                    // will not work right now because of https://bugreports.qt.io/browse/QTBUG-68711
                    Keys.onShortcutOverride: {
                        event.accepted = true
                    }

                    Keys.onEscapePressed: {
                        _emotePicker.startClosing()
                    }

                    onTextChanged: {
                        text = text.replace(/^[\n\r\s]+/g, '');
                    }

                    function _onReturnPressed(event) {
                        if (!(event.modifiers & Qt.ShiftModifier)) {
                            if (!(event.modifiers & Qt.ControlModifier)) {
                                _emotePicker.startClosing()
                            }

                            text = text.trim()
                            if (_input.text.length > 0)
                                sendMessage()
                        } else {
                            if (text.length > 0) {
                                remove(selectionStart, selectionEnd)
                                insert(selectionEnd, "\r\n")
                            }
                        }
                        event.accepted = true
                    }

                    Keys.onReturnPressed: _onReturnPressed(event)
                    Keys.onEnterPressed: _onReturnPressed(event)

                }

                EmoteSelector {
                    id: _emoteButton
                }
            }
        }


    }
}
