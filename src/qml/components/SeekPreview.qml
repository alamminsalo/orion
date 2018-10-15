import QtQuick 2.5
import QtGraphicalEffects 1.0
import "../util.js" as Util

Item {
    id: root
    property string source
    property real from: 0
    property real to: d.duration
    property real value: 0
    property int fillMode: Image.PreserveAspectFit
    property real blur: 0
    property var color: "black"

    implicitWidth: image.implicitWidth
    implicitHeight: image.implicitHeight

    QtObject {
        id: d

        property string baseUrl

        property int index: 0

        property int width: 0
        property int height: 0
        property int cols: 0
        property int rows: 0
        property int count: 0

        property real duration: 0
        property var images: []

        function updateImage() {
            index = Math.min(count - 1, Math.round((value - root.from) / (root.to - root.from) * count))
        }

        function updateInfo() {
            duration = 0
            width = 0
            height = 0
            count = 0
            images = []
            if (!source) return
            baseUrl = source.substring(0, source.lastIndexOf("/"))
            Util.requestJSON(root.source, function(resp) {
                var info = resp[0]
                for(var i = 1; i < resp.length; i++) {
                    if (resp[i].width > info.width) {
                        info = resp[i]
                    }
                }
                duration = info.count * info.interval
                width = info.width
                height = info.height
                rows = info.rows
                cols = info.cols
                count = info.count
                images = info.images
                updateImage()
            })
        }
    }

    Connections {
        target: root
        onSourceChanged: d.updateInfo()
        onFromChanged: d.updateImage()
        onToChanged: d.updateImage()
        onValueChanged: d.updateImage()
    }

    Rectangle {
        visible: root.color && root.color != "transparent"
        color: root.color
        anchors.fill: parent
    }

    Item {
        id: image
        visible: d.count > 0

        implicitWidth: d.width
        implicitHeight: d.height

        property real fitToWidth: root.fillMode === Image.PreserveAspectFit ? implicitWidth * root.height > root.width * implicitHeight : true
        property real fitToHeight: root.fillMode === Image.PreserveAspectFit ? !fitToWidth : true

        property real paintedWidth: fitToWidth ? root.width : paintedHeight / implicitHeight * implicitWidth
        property real paintedHeight: fitToHeight ? root.height : paintedWidth / implicitWidth * implicitHeight

        x: (root.width - paintedWidth) / 2
        y: (root.height - paintedHeight) / 2

        transform: Scale {
            xScale: image.paintedWidth / image.implicitWidth
            yScale: image.paintedHeight / image.implicitHeight
        }

        Repeater {
            model: d.count
            delegate: Item {
                visible: index === d.index
                clip: true
                width: d.width
                height: d.height

                property int fullRow: Math.floor(index / d.cols)
                property int column: index % d.cols
                property int page: Math.floor(fullRow / d.rows)
                property int row: fullRow % d.rows

                Image {
                    x: -parent.width * parent.column
                    y: -parent.height * parent.row
                    source: d.baseUrl + "/" + d.images[parent.page]
                    asynchronous: true
                    cache: true
                }

                GaussianBlur {
                    visible: root.blur && root.blur > 0
                    anchors.fill: parent
                    // according to docs setting source of GaussianBlur to the parent is not allowed, but it works and is
                    // better than adding a second repeater and trying find the source via Repeater.itemAt
                    source: parent
                    radius: root.blur
                    samples: Math.floor(radius * 2 + 1)
                    cached: true
                }
            }
        }
    }
}
