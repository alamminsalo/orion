import QtQuick 2.0

Rectangle {
    height: parent.height * .33
    gradient: Gradient {
        GradientStop { position: 0.0; color: "transparent" }
        GradientStop { position: 1.0; color: "#111111" }
    }

    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }

    //parent.onHeightChanged: height = height * .33
}
