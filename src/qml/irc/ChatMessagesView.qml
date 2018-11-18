import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.1
import "../components"
import "../util.js" as Util
import app.orion 1.0

ListView {
    id: list

    property bool lock: true
    property int previousY: 0
    property alias chatModel: chatModel
    property bool hasUnreadMessages: false
    property int maxCount: isMobile() ? 100 : 300
    property bool moving: false

    function startMove() {
        moving = true;
        contentYSmoothing.enabled = false
        lock = Qt.binding(function() { return atYEnd; });
    }

    function endMove() {
        moving = false;
        contentYSmoothing.enabled = Qt.binding(function() { return Settings.autoScrollSmoothing; });
        lock = atYEnd;
    }

    function moveToIndex(index) {
        if (moving) return
        if (Settings.autoScrollSmoothing) {
            startMove()
            var prevContentY = contentY
            positionViewAtIndex(index, ListView.Beginning)
            var newContentY = contentY
            var newLock = atYEnd && lock
            contentY = prevContentY
            endMove()
            lock = newLock
            contentY = newContentY
        } else {
            positionViewAtIndex(index, ListView.Beginning)
        }
    }

    Behavior on contentY {
        id: contentYSmoothing
        enabled: Settings.autoScrollSmoothing
        PropertyAnimation {
            duration: 200
        }
    }

    function checkLock(immediate) {
        if (lock && !moving) {
            var end = Math.max(originY, originY + contentHeight - height)
            if (end == contentY) return;
            if (immediate) {
                contentYSmoothing.enabled = false
                contentY = end
                contentYSmoothing.enabled = Qt.binding(function() { return Settings.autoScrollSmoothing; });
            } else {
                contentY = end
            }
        }
    }

    onVisibleChanged: {
        if (visible) {
            hasUnreadMessages = false
            checkLock(true)
        }
    }

    onHeightChanged: Qt.callLater(checkLock, true)
    onLockChanged: checkLock(true)
    onAtYEndChanged: checkLock()
    onContentHeightChanged: checkLock()

    onDragStarted: startMove()
    onDragEnded: endMove()

    ScrollIndicator.vertical: ScrollIndicator { visible: isMobile() }

    ScrollBar.vertical: ResponsiveScrollBar {
        visible: !isMobile()
        onScrollBegin: list.startMove()
        onScrollEnd: list.endMove()
    }

    function fadoutIndex(index) {
        highlightBackground.fadeOut(index)
        return !!highlightBackground.target
    }

    function markIndex(index) {
        highlightBackground.show(index)
        return !!highlightBackground.target
    }

    Rectangle {
        id: highlightBackground
        visible: target
        z: -1
        property int index: -1
        width: target ? target.width : 0
        height: target ? target.height : 0
        parent: target ? target : list
        property Item target: null
        onIndexChanged: target = list.getVisibleDelegate(index)
        color: Settings.lightTheme ? Qt.darker(root.Material.background, 1.3) : Qt.lighter(root.Material.background, 2)
        onOpacityChanged: if (opacity === 0) index = -1
        Behavior on opacity {
            id: opacityBehavior
            PropertyAnimation {
                id: highlightAnim
                easing.type: Easing.InCubic
                duration: 3000
            }
        }
        function show(i) {
            if (i === -1 && highlightAnim.running) return
            index = i
            opacityBehavior.enabled = false
            opacity = 1
            opacityBehavior.enabled = true
        }
        function fadeOut(i) {
            index = i
            opacityBehavior.enabled = false
            opacity = 1
            opacityBehavior.enabled = true
            opacity = 0
        }
    }
    
    clip: true

    spacing: 10
    boundsBehavior: Flickable.StopAtBounds


    QtObject {
        id: chatModel
        property var content: []
        function addMessage(obj) {
            // workaround for https://bugreports.qt.io/browse/QTBUG-12117
            content.push(obj)
            chatModelImpl.append({});

            //Limit msg count in list
            if (count > maxCount) {
                content.shift()
                chatModelImpl.remove(0, 1)
            }
            if (!list.visible) {
                list.hasUnreadMessages = true
            }
        }
        function clear() {
            chatModel.content = []
            chatModelImpl.clear()
        }
    }

    model: ListModel {
        id: chatModelImpl
    }

    function getVisibleDelegate(index) {
        for (var i = 0; i < contentItem.visibleChildren.length; i++) {
            if (contentItem.visibleChildren[i].delegateIndex === index) {
                return contentItem.visibleChildren[i]
            }
        }
        return null
    }
    
    delegate: ChatMessage {
        property int delegateIndex: index
        // see above: workaround for https://bugreports.qt.io/browse/QTBUG-12117
        // force disable binding (index changes on removal)
        property var model: chatModel.content[index]
        Component.onCompleted: model = model
        user: model.user
        msg: model.message
        badgeEntries: model.badgeEntries
        isAction: model.isAction
        emoteDirPath: chat.emoteDirPath
        isChannelNotice: model.isChannelNotice
        systemMessage: model.systemMessage
        isWhisper: model.isWhisper
        width: parent.width
    }
    
    Layout.fillHeight: true
    Layout.fillWidth: true

    Column {
        anchors.right: parent.right
        IconButtonFlat {
            text: "\ue8ff"
            onClicked: Settings.textScaleFactor += 0.15
        }
        IconButtonFlat {
            text: "\ue900"
            onClicked: Settings.textScaleFactor -= 0.15
        }
    }

    IconButtonFlat {
        anchors {
            bottom: parent.bottom
            right: parent.right
            rightMargin: 5
        }
        visible: !list.lock
        onClicked: list.lock = true
        text: "\ue258"
    }
}
