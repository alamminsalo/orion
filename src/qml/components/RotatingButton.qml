import QtQuick 2.5

//ChannelList.qml
IconButtonFlat {
    id: root
    property alias running: anim.running

    RotationAnimation on rotation {
        id: anim
        running: false
        loops: Animation.Infinite
        duration: 800
        from: 0
        to: 360
    }
}
