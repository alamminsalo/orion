import QtQuick 2.0
import QtQuick.Window 2.0
import "style"
import "components"

Item {

    property var notificationQueue: []
    property var activeNotification

    id: root


    //Positions:
    //          0 - topleft
    //          1 - topright
    //          2 - bottomleft
    //          3 - bottomright
    property int position: 1

    Connections {
        target: g_cman
        onPushNotification: {
            pushNotification(args)
        }
    }

    Timer {
        id: timer
        interval: 4000
        onTriggered: {
            triggerNotification()
        }
    }

    //args: QStringList[title, description, image url]
    function pushNotification(args){
        notificationQueue.push(args)

        if (!timer.running)
            triggerNotification()
    }

    function triggerNotification(){
        if (activeNotification){
            activeNotification.close()
            activeNotification = false
        }

        if (notificationQueue.length > 0){
            activeNotification = showNotification(notificationQueue.shift())

            timer.start()
        }
    }

    function showNotification(args){
        var component = Qt.createComponent("components/Notification.qml")

        if (component.status == Component.Error) {
            // Error Handling
            console.log("Error loading notification component:", component.errorString());
            return
        }

        var width = Dpi.scale(440)
        var height = Dpi.scale(140)
        var x = Screen.width / 2
        var y = Screen.height / 2
        var destY = 0

        switch (position){
        case 0:
            x =  Dpi.scale(50)
            y = -height
            destY = Dpi.scale(60)
            break

        case 1:
            x = Screen.width - width  - Dpi.scale(50)
            y = -height
            destY = Dpi.scale(60)
            break

        case 2:
            x = Dpi.scale(50)
            y = Screen.height
            destY = Screen.height - height  - Dpi.scale(60)
            break

        case 3:
            x = Screen.width - width  - Dpi.scale(50)
            y = Screen.height
            destY = Screen.height - height - Dpi.scale(60)
            break
        }

        console.log(args)

        return component.createObject(root, {
                                             "x": x,
                                             "y": y,
                                             "width": width,
                                             "height": height,
                                             "title": args[0],
                                             "description": args[1],
                                             "imgSrc": args[2],
                                             "destY": destY
                                          })
    }
}
