import QtQuick 2.0
import Communi 3.0

Item {
    id: root

    property int hardlimit: 10          //Leave space for ping messages
    property var sentMessageTimes: []   //Array holding timestamps for sent out messages
    property var messageQueue: []       //Outgoing msg queue

    signal messageReceived(string user, string message)

    property string accesstoken: g_cman.accesstoken
    onAccesstokenChanged: {
        console.log("Setting chat password")
        conn.password = "oauth:" + accesstoken
        conn.reconnect()
    }

    property var channel: undefined

    Connections {
        target: g_cman
        onUserNameUpdated: {
            console.log("Setting chat username: " + name)
            conn.userName = name
            conn.reconnect()
        }
    }

    Component.onCompleted: {
        timer.start()
    }

    function joinChannel(channelName) {
        leaveChannel()

        root.channel = channelName
        _queueMessage("JOIN #%1".arg(root.channel))

        console.log("Joining channel " + channelName)
    }

    function leaveChannel() {
        //Leaves current channel
        if (root.channel) {
            _queueMessage("PART #%1".arg(root.channel))
        }
        root.channel = undefined
    }

    function sendChatMessage(message) {
        if (root.channel) {
            _queueMessage("PRIVMSG #%1 :%2".arg(root.channel).arg(message))
        }

        else console.log("Not currently in a channel")
    }

    function _sendMessage(message) {
        conn.sendRaw(message)
        sentMessageTimes.push(Date.now())
    }

    function _queueMessage(message) {
        messageQueue.push(message)
    }

    function _handleQueue() {
        //Remove timestamps older than 30 seconds
        var time = Date.now()

        var i = 0;
        while (i < sentMessageTimes.length && time - sentMessageTimes[i] < 30000) {
            i++
        }
        if (i > 0) {
            sentMessageTimes.splice(0, i)
        }

        //Send messages from queue
        while (messageQueue.length > 0 && sentMessageTimes.length < hardlimit) {
            _sendMessage(messageQueue.splice(0,1))
        }
    }

    IrcConnection {
        id: conn
        host: "irc.chat.twitch.tv"
        port:  6667
        //enabled: false
        //password: "oauth:" + accesstoken
        //userName: username
        nickName: userName
        realName: userName

        onConnected: {
            console.log("Connected to chat")
        }

        onConnectedChanged: {
            //Errors
        }

        function reconnect() {
            close()
            console.log(userName, password)
            if (userName && password) {
                console.log("Connecting as " + userName + ":" + password + "...")
                open()
            }
        }

        onMessageReceived: {
            //console.log(message.toData())

            if (message.command === "PING") {
                _sendMessage("PONG :tmi.twitch.tv")  //Reply to ping request instantly
            }

            else if (message.command === "PRIVMSG") {
                root.messageReceived(message.ident, message.parameters[1])  //Emit messageReceived signal
            }
        }
    }

    Timer {
        id: timer
        interval: 1000
        repeat: true
        onTriggered: {
            _handleQueue()
        }
    }
}
