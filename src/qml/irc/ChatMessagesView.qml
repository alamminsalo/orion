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

    ScrollIndicator.vertical: ScrollIndicator {}
    
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

    MouseArea {
        id: mArea
        anchors.fill: parent
        onWheel: {
            if (wheel.modifiers & Qt.ControlModifier) {
                Settings.textScaleFactor += (0.1 * wheel.angleDelta.y / 120)
            }
        }
    }
}
