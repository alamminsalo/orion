import QtQuick 2.0
import "../styles.js" as Styles

Rectangle {
    id: root

    implicitHeight: dp(400)
    implicitWidth: dp(200)

    color: Styles.sidebarBg

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
    }

    property bool _toend: false
    onVisibleChanged: {
        if (visible)
            _toend = true
    }

    ListView {
        id: list
        visible: root.width > 0

        model: ListModel {
            id: chatModel
        }

        clip: true
        highlightFollowsCurrentItem: true
        spacing: dp(2)

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: spacer.top
        }

        delegate: ChatMessage {
            user: model.user
            msg: model.message
        }

        onFlickingVerticallyChanged: {
            if (root.width > 0 && list.contentHeight - (list.contentY + list.height) < 80)
                list.positionViewAtEnd()
        }
    }

    Rectangle {
        id: spacer
        anchors {
            left: parent.left
            right: parent.right
            bottom: inputArea.top
        }
        height: dp(3)
        color: Styles.border
    }

    Rectangle {
        id: inputArea

        height: dp(30)
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        color: Styles.bg

        MouseArea {
            cursorShape: Qt.IBeamCursor
            anchors {
                fill: parent
            }

            TextInput{
                id: _input
                anchors {
                    fill: parent
                    leftMargin: dp(5)
                    rightMargin: dp(5)
                }
                color: "#ffffff"
                clip:true
                selectionColor: Styles.purple
                focus: true
                selectByMouse: true
                font.pixelSize: Styles.titleFont.smaller
                verticalAlignment: Text.AlignVCenter

                Keys.onReturnPressed: sendMessage()
            }
        }
    }

    Chat {
        id: chat

        onMessageReceived: {
            chatModel.append({"user": user, "message": message})

            //Limit msg count in list
            var max = 1000
            if (chatModel.count > max) {
                chatModel.remove(0, chatModel.count - max)
            }

            if (_toend || list.atYEnd)//list.contentHeight - (list.contentY + list.height) < 10 || !root.visible)
                list.positionViewAtEnd()

            if (_toend)
                _toend = false
        }

        onClear: {
            chatModel.clear()
        }
    }
}
