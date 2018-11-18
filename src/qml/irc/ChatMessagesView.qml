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
    property int scrollbuf: 0
    property int previousY: 0
    property alias chatModel: chatModel
    property bool hasUnreadMessages: false
    property int maxCount: isMobile() ? 100 : 300

    onVisibleChanged: {
        if (visible) {
            hasUnreadMessages = false
        }
    }

    ScrollIndicator.vertical: ScrollIndicator { visible: isMobile() }

    ScrollBar.vertical: ResponsiveScrollBar {
        visible: !isMobile()
        onScrollBegin: list.startMove()
        onScrollEnd: list.endMove()
    }
    
    model: ListModel {
        id: chatModel
        
        onCountChanged: {
            if (list.lock)
                list.positionViewAtEnd()
            
            //Limit msg count in list
            if (chatModel.count > maxCount) {
                chatModel.remove(0, 1)
            }

            if (!visible && !hasUnreadMessages) {
                hasUnreadMessages = true
            }
        }
    }
    
    clip: true
    highlightFollowsCurrentItem: false
    spacing: 10
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
        //highlightOpacity: root._opacity
        width: parent.width
    }
    
    anchors {
        fill: parent
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
        visible: !list.atYEnd
        onClicked: list.positionViewAtEnd()
        text: "\ue258"
    }
}
