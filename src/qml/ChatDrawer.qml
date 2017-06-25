import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import "irc"
import "components"
import app.orion 1.0

Drawer {
    id: chatdrawer
    edge: Settings.chatEdge == 0 ?
              Qt.LeftEdge : (Settings.chatEdge == 1 ?
                                 Qt.RightEdge : Qt.BottomEdge)

    property alias chat: chatview
    property alias pinned: chatview.pinned
    
    height: edge !== Qt.BottomEdge ? view.height :
                                     // Fit playerview to 16:9
                                     rootWindow.height - topbar.height - (rootWindow.width * 0.5625)

    width: edge !== Qt.BottomEdge ? 330 : rootWindow.width

    y: header.visible ? header.height : 0
    interactive: !chatview.pinned
    modal: interactive
    
    onAboutToHide: {
        chatview.pinned = false
    }
    
    Material.elevation: chatview.pinned ? 0 : 12
    dim: false
    
    ChatView {
        id: chatview
        anchors.fill: parent
    }
    
    MouseArea {
        width: 10
        cursorShape: Qt.SplitHCursor
        visible: chatview.pinned && chatdrawer.edge !== Qt.BottomEdge
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
