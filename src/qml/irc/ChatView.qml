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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.0
import "../fonts/fontAwesome.js" as FontAwesome
import "../styles.js" as Styles
import "../components"
import "../util.js" as Util
import "../"

Item {
    id: root

    //Visibity status:
    //0 - hidden
    //1 - solid visible
    //2 - opaque
    property int status: 0
    onStatusChanged: {
        if (status > 2)
            status = 0
    }

    property bool chatViewVisible: root.width > 0

    visible: status > 0

    property real _opacity: root.status > 1 ? 0.6 : 1.0
    property int chatWidth: width

    property bool viewerListEnabled: false

    Rectangle {
        anchors.fill: parent
        color: Styles.sidebarBg
        opacity: root._opacity
    }

    onVisibleChanged: {
        if (visible) {
            _input.forceActiveFocus()
        } else {
            _emotePicker.visible = false;
        }
    }

    function joinChannel(channel, channelId) {
        if (channel !== chat.channel || chat.replayMode) {
            viewerListEnabled = false;
            chatModel.clear()
            chat.joinChannel(channel, channelId)
        }
    }

    function leaveChannel() {
        chatModel.clear()
        chat.leaveChannel()
    }

    function replayChat(channelName, channelId, vodId, startEpochTime) {
        viewerListEnabled = false;
        chatModel.clear()
        chat.leaveChannel()
        chat.replayChat(channelName, channelId, vodId, startEpochTime);
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
        if (chat.lastEmoteSets) {
            _emotePicker.loading = true;
            //console.log("downloading chat.lastEmoteSets", chat.lastEmoteSets);
            _emoteButton.startDownload(chat.lastEmoteSets);
        }
        _emoteButton.pickerLoaded = true;
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


    Item {
        id: chatControls
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
        height: dp(40)

        IconButton {
            id: _viewerListButton
            icon: viewerListEnabled ? "times" : "list"

            enabled: (!isVod && currentChannel && currentChannel.name) ? true : false

            anchors {
                top: parent.top
                right: parent.right
                rightMargin: 5
                bottom: parent.bottom
            }
            width: height

            onClicked: {
                viewerListEnabled = !viewerListEnabled
                if (viewerListEnabled && (status == 0)) {
                    status++;
                }
            }

            ToolTip {
                visible: _viewerListButton.mouseArea.containsMouse
                delay: 666
                text: "Viewer List"
            }
        }
    }

	Item {
		id: chatContainer

		anchors {
            top: chatControls.bottom
			left: parent.left
			right: parent.right
            bottom: parent.bottom
		}

    Rectangle {
        id: viewerList
        enabled: viewerListEnabled
        property bool loading: true

        height: enabled? parent.height : 0

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        Behavior on height {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }

        z: 10

        color: Styles.sidebarBg
        opacity: root._opacity

        onEnabledChanged: {
            if (enabled) {
                viewerList.loading = true;
                viewerListModel.clear();
                g_cman.loadChatterList(chat.channel);
            }
        }

        SpinnerIcon {
            id: spinner
            anchors.centerIn: parent
            iconSize: parent.width * 0.1
            visible: viewerList.loading && viewerList.enabled
        }

        Item {
            id: viewerListHeading
            visible: viewerList.enabled
            anchors {
                bottom: parent.top
                left: parent.left
                right: parent.right
            }

            height: dp(40)

            Label {
                anchors.centerIn: parent

                text: "Viewer List"
                color: Styles.textColor
                font.pixelSize: Styles.titleFont.bigger
                font.bold: true
            }
        }

        ListView {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                top: viewerListHeading.bottom
            }

            model: ListModel {
                id: viewerListModel
            }

            Connections {
                target: g_cman
                onChatterListLoaded: {
                    viewerList.loading = false;

                    var groupOrder = ["staff", "global_mods", "admins", "moderators", "viewers"];

                    for (var j = 0; j < groupOrder.length; j++) {
                        var groupName = groupOrder[j];
                        var group = chatters[groupName];
                        if (!group) {
                            continue;
                        }

                        for (var i = 0; i < group.length; i++) {
                            var chatter = group[i];
                            viewerListModel.append({"groupName": groupName, "user": chatter});
                        }
                    }
                }
            }

            clip: true
            delegate: Item {
                height: dp(25)
                Text {
                    text: user
                    color: Styles.textColor
                    anchors {
                        fill: parent
                        leftMargin: dp(5)
                        rightMargin: dp(5)
                    }
                    font.capitalization: Font.Capitalize
                }
            }

            section {
                property: "groupName"
                criteria: ViewSection.FullString
                delegate: Item {
                    height: dp(50)
                    Text {
                        anchors {
                            leftMargin: dp(5)
                            rightMargin: dp(5)
                            bottomMargin: dp(5)
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }

                        font.capitalization: Font.AllUppercase
                        text: section
                        //color: Styles.textColor
                        color: Styles.purple
                        font.pixelSize: Styles.titleFont.smaller
                    }
                }

            }

        }
    }

    ListView {
        id: list

        visible: !viewerList.enabled && root.chatViewVisible

        property bool lock: true
        property int scrollbuf: 0
        property int previousY: 0

        model: ListModel {
            id: chatModel

            onCountChanged: {
                if (list.lock)
                    list.positionViewAtEnd()

                //Limit msg count in list
                if (chatModel.count > 300) {
                    chatModel.remove(0, 1)
                }
            }
        }

        clip: true
        highlightFollowsCurrentItem: false
        spacing: dp(10)
        boundsBehavior: Flickable.StopAtBounds

        delegate: ChatMessage {
            user: model.user
            msg: model.message
            jsonBadgeEntries: model.jsonBadgeEntries
            isAction: model.isAction
            emoteDirPath: chat.emoteDirPath
            isChannelNotice: model.isChannelNotice
            systemMessage: model.systemMessage
            isWhisper: model.isWhisper
            highlightOpacity: root._opacity

            anchors {
                left: parent.left
                right: parent.right
            }
        }

        width: chatWidth

        anchors {
            top: parent.top
            left: parent.left
            bottom: spacer.top
        }

        onContentYChanged: {
            if (atYEnd)
                lock = true;
            else if (scrollbuf < 0)
                lock = false
            else if (previousY > contentY)
                scrollbuf--

            previousY = contentY
        }
    }

    Rectangle {
        id: spacer
        anchors {
            left: parent.left
            right: parent.right
            bottom: inputArea.top
        }
        height: inputArea.visible ? dp(2) : 0
        color: Styles.border

        opacity: root._opacity
    }

    GridPicker {
        id: _emotePicker

        visible: false
        height: 0

        devicePixelRatio: chat.getHiDpi()? 2.0 : 1.0

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

        color: "#ffffff"

        anchors {
            bottom: inputArea.top
            left: inputArea.left
            right: inputArea.right
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

    Item {
        id: inputArea

        height: !chat.isAnonymous ? dp(45) : 0
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        visible: !chat.isAnonymous && !chat.replayMode && !viewerList.enabled

        Rectangle {
            anchors.fill: parent
            color: Styles.bg
            opacity: root._opacity
        }

        MouseArea {
            cursorShape: Qt.IBeamCursor
            anchors {
                fill: parent
            }

            TextInput{
                id: _input
                anchors {
                    left: parent.left
                    right: _emoteButton.left
                    top: parent.top
                    bottom: parent.bottom
                    //left: parent.left
                    margins: dp(5)
                }
                color: "#ffffff"
                clip:true
                selectionColor: Styles.purple
                focus: true
                selectByMouse: true
                font.pixelSize: Styles.titleFont.smaller
                verticalAlignment: Text.AlignVCenter

                Keys.onReturnPressed: sendMessage()
                Keys.onUpPressed: {
                    if (_emotePicker.visible) {
                        _emotePicker.focusEntersFromBottom();
                    } else {
                        _emotePicker.visible = true
                    }
                }
            }

            IconButton{
                id: _emoteButton
                property bool emotePickerDownloadsInProgress : false
                property var setsToDownload
                property var lastSet
                property var lastEmoteSets
                property int curDownloading
                property ListModel setsVisible: ListModel { }

                property bool pickerLoaded: false

                visible: root.chatViewVisible

                width: height

                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }

                icon: "smile"

                Connections {
                    target: _emotePicker
                    onVisibleChanged: {
                        if (_emotePicker.visible && !_emoteButton.pickerLoaded) {
                            loadEmotes();
                        }
                    }
                }

                onClicked: {
                    if (_emotePicker.visible) {
                        _emotePicker.startClosing();
                    } else {
                        _emotePicker.visible = true;
                    }
                }

                function addEmoteToChat(emoteName) {
                    var textToAdd = emoteName + " ";
                    var existingText = _input.text;
                    if (existingText != "" && existingText.charAt(existingText.length - 1) != " ") {
                       textToAdd = " " + textToAdd;
                    }
                    _input.text += textToAdd;
                }


                function decodeHtml(html) {
                    var entities = {
                        "amp": "&",
                        "lt": "<",
                        "gt": ">",
                        "quot": "\""
                    }

                    var cur = 0;
                    var parts = [];
                    while (true) {
                        var pos = html.indexOf("&", cur);
                        if (pos == -1) {
                            break;
                        }

                        parts.push(html.substring(cur, pos));

                        var end = html.indexOf(";", pos + 1);
                        if (end == -1) {
                            console.log("unterminated entity " + html.substring(pos));
                            break;
                        }

                        var entityName = html.substring(pos + 1, end);
                        var value = entities[entityName];

                        if (!entityName) {
                            console.log("unknown entity " + entityName);
                            break;
                        }

                        parts.push(value);

                        cur = end + 1;
                    }
                    parts.push(html.substring(cur));
                    return parts.join("");
                }

                function encodeHtml(unsafe) {
                    // per https://stackoverflow.com/questions/6234773/can-i-escape-html-special-chars-in-javascript
                    return unsafe
                         .replace(/&/g, "&amp;")
                         .replace(/</g, "&lt;")
                         .replace(/>/g, "&gt;")
                         .replace(/"/g, "&quot;")
                         .replace(/'/g, "&#039;");
                 }

                function inverseRegex(s) {
                    var out = [];
                    var unconfirmed = "";
                    var showDebug = false;
                    for (var i = 0; i < s.length; i++) {
                        var cur = s.charAt(i);
                        switch (cur) {
                        case "\\":
                            cur = s.charAt(++i);
                            out.push(unconfirmed)
                            unconfirmed = cur;
                            break;
                        case "?":
                            // previous was optional
                            // assume nope
                            unconfirmed = "";
                            break;
                        case "(":
                            // recurse on this part of the regex until | or ) at this depth
                            var start = i + 1;
                            var end = null;
                            var ch;
                            var running = true;
                            var depth = 0;
                            while (running) {
                                ch = s.charAt(++i);
                                switch (ch) {
                                    case "\\":
                                        i++;
                                        break;
                                    case "(":
                                        depth++;
                                        break;
                                    case ")":
                                        if (depth == 0) {
                                            if (end == null) {
                                                end = i;
                                            }
                                            running = false;
                                        } else {
                                            depth--;
                                        }
                                        break;
                                    case "|":
                                        if (depth == 0) {
                                            if (end == null) {
                                                end = i;
                                            }
                                        }
                                        break;
                                }
                            }
                            out.push(unconfirmed);
                            var regexPart = s.substring(start, end);
                            // console.log(s, "recursing on", regexPart);
                            // showDebug = true;
                            unconfirmed = inverseRegex(regexPart);
                            break;
                        case "[":
                            cur = s.charAt(++i);
                            if (cur == "\\") {
                                cur = s.charAt(++i);
                            }

                            var end = s.indexOf("]", i + 1);
                            if (end == -1) {
                                console.log("unterminated [");
                                showDebug = true;
                            }
                            i = end;

                            out.push(unconfirmed);
                            unconfirmed = cur;
                            break;
                        default:
                            out.push(unconfirmed);
                            unconfirmed = cur;
                        }
                    }

                    out.push(unconfirmed);
                    out = out.join("");

                    /*
                    // test the generated text
                    var testFailed;

                    try {
                        var r = new RegExp(s);
                        var match = r.exec(out);
                        testFailed = match == null || match[0] != out;
                    } catch (e) {
                        console.log(e, out);
                        testFailed = true;
                    }
                    if (testFailed || showDebug) {
                        // mismatch
                        console.log("Converted regex " + s + " output " + out + (testFailed? " doesn't match": ""));
                    }
                    */

                    return out;
                }

                function showLastSet() {
                    //console.log("showing last set", lastSet);
                    var lastSetMap = lastEmoteSets[lastSet];
                    for (var i in lastSetMap) {
                        setsVisible.append({"imageUrl": "image://emote/" + i, "emoteName": decodeHtml(inverseRegex(lastSetMap[i]))})
                    }
                    _emotePicker.updateFilter();
                }

                function nextDownload() {
                    if (emotePickerDownloadsInProgress) {
                        if (curDownloading < setsToDownload.length) {
                            var curSetID = setsToDownload[curDownloading];
                            lastSet = curSetID;
                            var curSetMap = lastEmoteSets[curSetID];
                            var curSetList = [];
                            for (var i in curSetMap) {
                                curSetList.push(i);
                            }
                            curDownloading ++;
                            console.log("Downloading emote set #", curDownloading, curSetID);
                            chat.bulkDownloadEmotes(curSetList);
                        } else {
                            emotePickerDownloadsInProgress = false;
                            _emotePicker.loading = false;
                        }
                    }
                }

                function startDownload(emoteSets) {
                    curDownloading = 0;
                    lastEmoteSets = emoteSets;
                    setsToDownload = [];
                    for (var i in emoteSets) {
                        setsToDownload.push(i);
                    }
                    //console.log("Starting download of emote sets", setsToDownload);
                    emotePickerDownloadsInProgress = true;

                    nextDownload();
                }

                Connections {
                    target: chat
                    onBulkDownloadComplete: {
                        //console.log("outer download complete");
                        if (_emoteButton.emotePickerDownloadsInProgress) {
                            //console.log("handling emote picker set finished");
                            _emoteButton.showLastSet();
                            _emoteButton.nextDownload();
                        }
                    }
                }
            }

        }

    }

    Chat {
        id: chat

        property variant colors:[]
        property string emoteDirPath
        property variant lastEmoteSetIDs
        property variant lastEmoteSets

        property variant _textEmotesMap
        property variant _regexEmotesList

        property var lastBadgeUrls: {}
        property var lastChannelBetaBadgeSetData: {}
        property var globalBetaBadgeSetData: {}
        property var lastBetaBadgeSetData: {}

        property bool debugOutput: false

        onLastEmoteSetsChanged: {
            initEmotesMaps();
        }

        function regexExactMatch(regex, text) {
            var match = regex.exec(text);
            return match && match[0] === text;
        }

        function initEmotesMaps() {
            var plainText = /^[\da-z]+$/i;
            _textEmotesMap = {};
            _regexEmotesList = [];
            var emoteSets = lastEmoteSets;
            for (var i in emoteSets) {
                //console.log("  ", i);
                var entry = emoteSets[i];
                for (var emoteId in entry) {
                    var emoteText = entry[emoteId];
                    if (regexExactMatch(plainText, emoteText)) {
                        //console.log("adding plain text emote", emoteText, emoteId);
                        _textEmotesMap[emoteText] = emoteId;
                    } else {
                        //Just checking whether our invert text has entities is fine for all the existing global emotes
                        //TODO actually parse the entire regex so we don't miss any cases that match html entities
                        var htmlText = _emoteButton.inverseRegex(emoteText);
                        var decodedText = _emoteButton.decodeHtml(htmlText);
                        var useHtmlDomain = htmlText != decodedText;
                        //console.log("adding regex emote", emoteText, emoteId, "useHtmlDomain:", useHtmlDomain);
                        _regexEmotesList.push({"regex": new RegExp(emoteText), "emoteId": emoteId, "useHtmlDomain": useHtmlDomain});
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
                    matchInput = _emoteButton.encodeHtml(word);
                    //console.log(matchInput)
                }
                if (regexExactMatch(entry.regex, matchInput)) {
                    return entry.emoteId;
                }
            }
            return null;
        }

        function getRandomColor() {
            var letters = '0123456789ABCDEF';
            var color = '#';
            var minBrightness = 85;
            var maxBrightness = 240;
            var brightnessRange = maxBrightness - minBrightness + 1;
            for (var i = 0; i < 3; i++ ) {
                var colorVal = minBrightness + Math.floor(brightnessRange * Math.random());
                color += letters[colorVal / 16] + letters[colorVal % 16];
            }
            return color;
        }

        onSetEmotePath: {
            emoteDirPath = value
        }

        function keysStr(obj) {
            var parts = [];
            for (var i in obj) {
                parts.push(i);
            }
            return parts.join(", ");
        }

        onMessageReceived: {
            if (debugOutput) console.log("ChatView chat override onMessageReceived; typeof message " + typeof(message) + " toString: " + message.toString());

            if (chatColor != "") {
                colors[user] = chatColor;
            }

            if (!colors[user]) {
                colors[user] = getRandomColor()
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
                        console.log("  available versions are", keysStr(badgeSetData))
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

            chatModel.append({"user": user, "message": serializedMessage, "isAction": isAction, "jsonBadgeEntries": jsonBadgeEntries, "isChannelNotice": isChannelNotice, "systemMessage": systemMessage, "isWhisper": isWhisper})
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

        function objectAssign() {
            var target = arguments[0];
            for (var i = 1; i < arguments.length; i++) {
                var source = arguments[i];
                for (var key in source) {
                    if (source.hasOwnProperty(key)) {
                        target[key] = source[key];
                    }
                }
            }
            return target;
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
            lastBetaBadgeSetData = objectAssign({}, globalBetaBadgeSetData, lastChannelBetaBadgeSetData);
        }

        onClear: {
            chatModel.clear()
        }
    }
}
}
