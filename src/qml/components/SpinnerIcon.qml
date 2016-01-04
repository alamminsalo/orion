import QtQuick 2.5

Icon {
    icon: "spinner"

    RotationAnimation on rotation {
        loops: Animation.Infinite
        duration: 800
        from: 0
        to: 360
    }
}
