import QtQuick 2.0
//import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
//import "../fonts/fontAwesome.js" as FontAwesome
//import "../styles.js" as Styles
import "../components"
import "../util.js" as Util
import "../"

RoundButton {
    id: _emoteButton
    property bool emotePickerDownloadsInProgress : false
    property var setsToDownload
    property var lastSet
    property var lastEmoteSets
    property int curDownloading
    property ListModel setsVisible: ListModel { }
    
    property bool pickerLoaded: false
    property var pickerChannelLoaded: null
    
    visible: root.chatViewVisible
    
    width: height
    
    anchors {
        right: parent.right
        top: parent.top
        bottom: parent.bottom
    }
    
    font.family: "Material Icons"
    flat: true
    text: "\ue87c"
    
    Connections {
        target: _emotePicker
        onVisibleChanged: {
            if (_emotePicker.visible) {
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
    
    function showLastSet() {
        //console.log("showing last set", lastSet);
        switch(lastSet) {
        case "bttvGlobal":
            for (var i in chat.lastBttvGlobalEmotes) {
                setsVisible.append({"imageUrl": "image://bttvemote/" + chat.lastBttvGlobalEmotes[i], "emoteName": i});
            }
            break;
        case "bttvChannel":
            for (var i in chat.lastBttvChannelEmotes) {
                setsVisible.append({"imageUrl": "image://bttvemote/" + chat.lastBttvChannelEmotes[i], "emoteName": i});
            }
            break;
        default:
            var lastSetMap = lastEmoteSets[lastSet];
            for (var i in lastSetMap) {
                setsVisible.append({"imageUrl": "image://emote/" + i, "emoteName": decodeHtml(inverseRegex(lastSetMap[i]))})
            }
            break;
        }
        _emotePicker.updateFilter();
    }
    
    function clearChannelSpecificEmotes() {
        //console.log("clearChannelSpecificEmotes()")
        var channelEmotes = chat.lastBttvChannelEmotes;
        if (channelEmotes != null) {
            for (var i = 0; i < setsVisible.count; ) {
                var obj = setsVisible.get(i);
                if (channelEmotes.hasOwnProperty(obj.emoteName)) {
                    //console.log("remove channel emote", obj.emoteName, i);
                    setsVisible.remove(i);
                } else {
                    i++;
                }
            }
        }
        _emoteButton.pickerChannelLoaded = null;
    }
    
    function nextDownload() {
        if (emotePickerDownloadsInProgress) {
            if (curDownloading < setsToDownload.length) {
                var curSetID = setsToDownload[curDownloading];
                lastSet = curSetID;
                curDownloading ++;
                console.log("Downloading emote set #", curDownloading, curSetID);
                if (curSetID == "bttvGlobal") {
                    chat.downloadBttvEmotesGlobal();
                } else if (curSetID == "bttvChannel") {
                    chat.downloadBttvEmotesChannel();
                } else {
                    var curSetMap = lastEmoteSets[curSetID];
                    var curSetList = [];
                    for (var i in curSetMap) {
                        curSetList.push(i);
                    }
                    chat.bulkDownloadEmotes(curSetList);
                }
            } else {
                console.log("Emote set downloads complete");
                emotePickerDownloadsInProgress = false;
                _emotePicker.loading = false;
            }
        }
    }
    
    function startDownload(emoteSets) {
        curDownloading = 0;
        setsToDownload = [];
        if (emoteSets != null) {
            lastEmoteSets = emoteSets;
            for (var i in emoteSets) {
                setsToDownload.push(i);
            }
            setsToDownload.push("bttvGlobal");
        }
        if (chat.lastBttvChannelEmotes != null) {
            setsToDownload.push("bttvChannel");
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
