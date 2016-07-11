import QtQuick 2.0
import Communi 3.0

Item {
    id: root

    property int hardlimit: 20          //Leave space for ping messages
    property var sentCommandTimes: []   //Array holding timestamps for sent out messages
    property var commandQueue: []       //Outgoing msg queue

    signal messageReceived(string user, string message)
    signal clear()

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

        if (channelName) {
            root.channel = "#" + channelName
            _queueCommand(cmd.createJoin(root.channel))
        }
    }

    function leaveChannel() {
        //Leaves current channel
        if (root.channel) {
            _queueCommand(cmd.createPart(root.channel))
        }
        root.channel = undefined
    }

    function sendChatMessage(message) {
        if (root.channel) {
            _queueCommand(cmd.createMessage(root.channel,message))
        }

        else console.log("Not currently in a channel")
    }

    function _sendCommand(command) {
        conn.sendCommand(command)
        sentCommandTimes.push(Date.now())

        //Echo message back to local chat if needed
        var message = command.toMessage(conn.nickName, conn)

        if (message.command === "JOIN") {
            root.clear()
            root.messageReceived("Joined channel", root.channel)
        }

        else if (message.command === "PRIVMSG")
            root.messageReceived(message.nick, message.parameters[1])
    }

    function _queueCommand(message) {
        commandQueue.push(message)
    }

    function _handleQueue() {
        //Remove timestamps older than 30 seconds
        var time = Date.now()

        var i = 0;
        while (i < sentCommandTimes.length && time - sentCommandTimes[i] < 30000) {
            i++
        }
        if (i > 0) {
            sentCommandTimes.splice(0, i)
        }

        //Send messages from queue
        while (commandQueue.length > 0 && sentCommandTimes.length < hardlimit) {
            _sendCommand(commandQueue.splice(0,1)[0])
        }
    }

    IrcCommand {
        id: cmd
    }

    IrcConnection {
        id: conn
        host: "irc.chat.twitch.tv"
        port:  6667
        nickName: userName
        realName: userName

        onConnected: {
            console.log("Connected to chat")
        }

        onConnectedChanged: {
            //Errors
        }

        onDisconnected: {
            console.log("Disconnected from channel.")
        }

        function reconnect() {
            close()
            if (userName && password) {
                //console.log("Connecting as " + userName + ":" + password + "...")
                open()
            }
        }

        onMessageReceived: {
            console.log("Message received: " + message.toData())

            if (message.command === "PING") {
                //Reply to ping request
                _queueCommand(cmd.createPong("tmi.twitch.tv"))
            }

            else if (message.command === "PRIVMSG") {
                //Emit messageReceived signal
                root.messageReceived(message.nick, message.parameters[1])
            }

            else {
                //Something else
                console.log("Message received: " + message.toData())
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
