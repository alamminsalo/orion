import QtQuick 2.5
import QtGraphicalEffects 1.0

// Reusable round image component

Rectangle {
    id: root
    property alias source: img.source
    color: "black"
    radius: width / 2
    
    Image {
        id: img
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        smooth: true
        visible: false
    }
    
    OpacityMask {
        anchors {
            fill: root
            margins: root.border.width
        }

        source: img
        maskSource: parent
    }
}
