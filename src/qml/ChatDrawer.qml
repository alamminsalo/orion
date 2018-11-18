import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import "irc"
import "components"
import app.orion 1.0
import "util.js" as Util

Drawer {
    id: chatdrawer

    property alias chat: chatview
    property alias hasUnreadMessages : chatview.hasUnreadMessages

    readonly property int orientation: edge === Qt.RightEdge || edge === Qt.LeftEdge ? Qt.Horizontal : Qt.Vertical
    property real chatSize: 330

    height: orientation === Qt.Horizontal ? parent.height :
                         // Fit playerview to 16:9
                         Math.max(chatSize, parent.height - topbar.height - (parent.width * 0.5625))

    width: orientation === Qt.Horizontal ? chatSize : parent.width

    interactive: orientation === Qt.Horizontal && !chat.pinned
    modal: interactive
    Material.elevation: chat.pinned ? 0 : 12
    dim: false
    edge: {
            switch (Settings.chatEdge) {
            case 0:
            return Qt.LeftEdge;
            case 1:
            return Qt.RightEdge;
            case 2:
            return Qt.BottomEdge;
        case 3:
            return Qt.TopEdge;
        }
    }

    Component.onCompleted: {
        var maybeShowChat = function(){
            if (orientation !== Qt.Horizontal) {
                visible = view.playerVisible && isPortraitMode// && (isMobile() || !appFullScreen)
            }
        };
        if (isMobile()) {
            //Setup mobile connections
            edge = Qt.BottomEdge;
            interactive = false;
            rootWindow.isPortraitModeChanged.connect(maybeShowChat);
            view.currentIndexChanged.connect(maybeShowChat);
        }
        rootWindow.appFullScreenChanged.connect(maybeShowChat)
    }
    
    onActiveFocusChanged: if (activeFocus) chatview.forceActiveFocus()

    ChatView {
        id: chatview
        anchors.fill: parent
    }
    
    MouseArea {
        id: resizeBar
        z: 1

        x: chatdrawer.orientation == Qt.Horizontal ? (chatdrawer.edge === Qt.RightEdge ? parent.x : parent.x + parent.width) - width / 2 : 0
        y: chatdrawer.orientation == Qt.Vertical ? (chatdrawer.edge === Qt.BottomEdge ? parent.y : parent.y + parent.height) - height / 2 : 0
        width: chatdrawer.orientation === Qt.Horizontal ? 10 : parent.width
        height: chatdrawer.orientation === Qt.Horizontal ? parent.height : 10

        cursorShape: chatdrawer.orientation === Qt.Horizontal ? Qt.SplitHCursor : Qt.SplitVCursor
        enabled: visible

        acceptedButtons: Qt.LeftButton
        propagateComposedEvents: false
        preventStealing: true
        property var initalPos
        property var intialSize
        onPressed: {
            intialSize = chatdrawer.orientation === Qt.Horizontal ? chatdrawer.width : chatdrawer.height
            initalPos = Util.globalPosition(this, mouseX, mouseY)
        }
        onReleased: initalPos = undefined

        onPositionChanged: {
            var currentPos = Util.globalPosition(this, mouseX, mouseY)
            var w = 0
            switch (chatdrawer.edge) {
            case Qt.LeftEdge:
                w = intialSize + (currentPos.x - initalPos.x)
                break;
            case Qt.RightEdge:
                w = intialSize - (currentPos.x - initalPos.x)
                break;
            case Qt.BottomEdge:
                w = intialSize - (currentPos.y - initalPos.y)
                break;
            case Qt.TopEdge:
                w = intialSize + (currentPos.y - initalPos.y)
            }
            
            //Min/max
            w = Math.max(150, Math.min(chatdrawer.orientation === Qt.Horizontal ? chatdrawer.parent.width : chatdrawer.parent.height, w))
            chatSize = w
        }
    }
}
