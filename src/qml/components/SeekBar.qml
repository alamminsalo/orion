import QtQuick 2.0

import "../styles.js" as Styles

Item {
    property int duration
    property int position

    id: root

    signal userChangedPosition(real position)

    function setPosition(fraction, duration){
        if (root.duration !== duration)
            root.duration = duration
        root.position = Math.floor(duration * fraction)
        fillBar.width = Math.floor(fraction * seekBar.width)

        time.updateTime()
    }

    onDurationChanged: {
        time.duration = time.getTime(duration)
    }

    Rectangle {
        id: seekBar
        color: Styles.seekBar

        anchors {
            left: parent.left
            right: parent.right
            leftMargin: dp(10)
            rightMargin: dp(10)

            verticalCenter: parent.verticalCenter
        }

        height: dp(6)

        Rectangle {
            color: "white"
            id: fillBar

            width: 0
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
        }
    }

    MouseArea {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: seekBar.left
            right: seekBar.right
            topMargin: dp(10)
            bottomMargin: dp(10)
        }

        propagateComposedEvents: false
        onClicked: {
            userChangedPosition(mouseX / seekBar.width)
        }
    }

    Item {

        property string duration

        id: time
        anchors {
            top: parent.top
            topMargin: dp(6)
            horizontalCenter: parent.horizontalCenter
        }

        height: _time.contentHeight
        width: _time.contentWidth

        function updateTime() {
            _time.text = getTime(root.position) + "/" + duration
        }

        function getTime(totalSec){
            var hours = parseInt(totalSec / 3600) % 24;
            var minutes = parseInt(totalSec / 60) % 60;
            var seconds = totalSec % 60;

            var result = "";

            if (hours > 0)
                result += hours + ":";
            result += (minutes < 10 ? "0" + minutes : minutes) + ":";
            result += (seconds  < 10 ? "0" + seconds : seconds);

            return result
        }

        Text {
            id: _time
            text: "hh:mm:ss / hh:mm:ss"
            color: Styles.iconColor
            font.bold: true
            font.family: "Droid Sans"
            font.pointSize: dp(12)
            wrapMode: Text.WordWrap
            renderType: Text.NativeRendering
        }
    }
}
