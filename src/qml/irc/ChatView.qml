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
import "../fonts/fontAwesome.js" as FontAwesome
import "../styles.js" as Styles
import "../components"

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

    visible: status > 0

    property real _opacity: root.status > 1 ? 0.6 : 1.0

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
            isAction: model.isAction
            emoteDirPath: chat.emoteDirPath

            anchors {
                left: parent.left
                right: parent.right
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

        height: dp(320)

        color: "#ffffff"

        anchors {
            bottom: inputArea.top
            left: inputArea.left
            right: inputArea.right
        }

        model: _emoteButton.setsVisible
        visible: false
        text: "Emote Picker"

        filterTextProperty: "emoteName"

        onItemClicked: {
            var item = _emoteButton.setsVisible.get(index);
            _emoteButton.addEmoteToChat(item.emoteName);
        }

        onCloseRequested: {
            visible = false;
            _input.focus = true;
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

        visible: !chat.isAnonymous

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

            Button{
                id: _emoteButton
                property bool emotePickerDownloadsInProgress : false
                property var setsToDownload
                property var lastSet
                property var lastEmoteSets
                property int curDownloading
                property ListModel setsVisible: ListModel { }

                property bool pickerLoaded: false

                width: dp(38)

                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }

                text: "emotes"

                style: ButtonStyle {
                    background: Rectangle {
                        color: "#000000"
                    }

                    label: Text {
                        text: FontAwesome.fromText("smile")
                        color: "#ffffff"
                        font.family: "FontAwesome"
                        font.pointSize: 18
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter

                    }
                }

                Connections {
                    target: _emotePicker
                    onVisibleChanged: {
                        if (_emotePicker.visible && !_emoteButton.pickerLoaded) {
                            console.log("current emote set ids")
                            console.log(chat.lastEmoteSetIDs)
                            if (chat.lastEmoteSetIDs) {
                                // load the emote sets so that we know what icons to display
                                g_cman.loadEmoteSets(false, chat.lastEmoteSetIDs);
                                _emoteButton.pickerLoaded = true;
                            }
                        }
                        if (_emotePicker.visible) {
                            _emotePicker.focusFilterInput();
                        }
                    }
                }

                onClicked: {
                    _emotePicker.visible = !_emotePicker.visible;
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
                        testFailed = r.exec(out) == null;
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
                            var waitForDownload = chat.bulkDownloadEmotes(curSetList);

                            if (!waitForDownload) {
                                showLastSet();
                                nextDownload();
                            }
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
                    //setsVisible = [];
                    for (var i in emoteSets) {
                        setsToDownload.push(i);
                    }
                    console.log("Starting download of emote sets", setsToDownload);
                    emotePickerDownloadsInProgress = true;

                    nextDownload();
                }

                Connections {
                    target: g_cman
                    onEmoteSetsLoaded: {
                        console.log("emote sets loaded:");
                        //console.log(emoteSets);

                        if (emoteSets) {
                            _emoteButton.startDownload(emoteSets);
                        }

                    }
                }

                Connections {
                    target: chat
                    onDownloadComplete: {
                        console.log("outer download complete");
                        if (_emoteButton.emotePickerDownloadsInProgress) {
                            console.log("handling emote picker set finished");
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

        function getRandomColor() {
            var letters = '0123456789ABCDEF';
            var color = '#';
            for (var i = 0; i < 6; i++ ) {
                color += letters[Math.floor(Math.random() * (letters.length - 1))];
            }
            return color;
        }

        onSetEmotePath: {
            emoteDirPath = value
        }

        onMessageReceived: {
            //console.log("ChatView chat override onMessageReceived; typeof message " + typeof(message) + " toString: " + message.toString())

            if (chatColor != "") {
                colors[user] = chatColor;
            }

            if (!colors[user]) {
                colors[user] = getRandomColor()
            }

            // ListElement doesn't support putting in an array value, ugh.
            var serializedMessage = JSON.stringify(message);
            //console.log("Sending: " + serializedMessage);
            chatModel.append({"user": user, "message": serializedMessage, "isAction": isAction})
            list.scrollbuf = 6
        }

        onEmoteSetIDsChanged: {
            lastEmoteSetIDs = emoteSetIDs
        }

        onClear: {
            chatModel.clear()
        }
    }
}
