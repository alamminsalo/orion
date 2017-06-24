import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

Transition {
    id: trans
    SequentialAnimation {
        PropertyAnimation {
            properties: "opacity";
            from: 1
            to: 0
            duration: 0
        }
        PauseAnimation {
            duration: (trans.ViewTransition.index -
                       trans.ViewTransition.targetIndexes[0]) * 15
        }
        ParallelAnimation {
            NumberAnimation {
                properties: "opacity";
                from: 0
                to: 1
                duration: 600
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                properties: "y";
                from: trans.ViewTransition.destination.y + 50
                duration: 620
                easing.type: Easing.OutCubic
            }
        }
    }
}
