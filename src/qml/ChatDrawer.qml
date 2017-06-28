import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import "irc"
import "components"
import app.orion 1.0

Drawer {
    id: chatdrawer

    property alias chat: chatview
    property bool isBottom: Settings.chatEdge == 2

    height: !isBottom ? view.height :
                         // Fit playerview to 16:9
                         rootWindow.height - topbar.height - (rootWindow.width * 0.5625)
    width: !isBottom ? 330 : rootWindow.width

    y: header.visible ? header.height : 0
    interactive: !isBottom && !chatview.pinned
    modal: interactive
    Material.elevation: chatview.pinned ? 0 : 12
    dim: false
    edge: Qt.RightEdge //Initial value

    Connections {
        target: Settings
        onChatEdgeChanged: {
            switch (Settings.chatEdge) {
            case 0:
                edge = Qt.LeftEdge;
                break;
            case 1:
                edge = Qt.RightEdge;
                break;
            case 2:
                edge = Qt.BottomEdge
                break;
            }
        }
    }

    Component.onCompleted: {
        var maybeShowChat = function(){
            if (isBottom) {
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
    
    ChatView {
        id: chatview
        anchors.fill: parent
    }
    
    MouseArea {
        width: 10
        cursorShape: Qt.SplitHCursor
        visible: !interactive && chatdrawer.edge !== Qt.BottomEdge
        enabled: visible
        anchors {
            horizontalCenter: chatdrawer.edge === Qt.RightEdge ? parent.left : parent.right
            top: parent.top
            bottom: parent.bottom
        }
        propagateComposedEvents: false
        onPositionChanged: {
            var w = 0
            switch (chatdrawer.edge) {
            case Qt.LeftEdge:
                w = chatdrawer.width + mouseX
                break;
            case Qt.RightEdge:
                w = chatdrawer.width - mouseX
                break;
            }
            
            //Min/max
            w = Math.max(200, Math.min(rootWindow.width, w))
            chatdrawer.width = w
        }
    }
}
