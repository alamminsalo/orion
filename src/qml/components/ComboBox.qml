import QtQuick 2.5
import "../styles.js" as Styles

Rectangle {
    property var entries //QStringList
    property var names: []
    property var selectedItem
    property bool open: list.visible

    signal indexChanged(int index)

    function select(item){
        if (item && selectedItem !== item){
            selectedItem = item
            label.text = selectedItem.str

            indexChanged(selectedItem.index)
        }
    }

    function close(){
        list.visible = false
    }

    function setIndex(index){
        label.text = names[index]
    }

    //clip: true
    color: Styles.shadeColor

    Rectangle {
        id: rect
        color: "transparent"
        width: dp(100)
        height: dp(50)
        border {
            color: Styles.border
            width: dp(1)
        }
        anchors {
            bottom: parent.bottom
            left: parent.left
        }

        Text {
            id: label
            text: "-"
            anchors.centerIn: parent
            color: Styles.textColor
            font.family: "Droid Sans"
            font.pointSize: Styles.titleFont.smaller
            font.bold: true
            ////renderType: Text.NativeRendering
        }
    }


    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onClicked: {
            list.visible = !list.visible
        }

        onHoveredChanged: {
            rect.border.color = containsMouse ? Styles.textColor : Styles.border
        }
    }

    onEntriesChanged: {
        console.log("Setting new entries")
        srcModel.clear()
        for (var i = entries.length - 1; i > -1; i--){
            if (entries[i] && entries[i].length > 0){
                srcModel.append( { "itemIndex": i})
            }
        }
    }

    ListView {

        property var hoveredItem

        id: list
        interactive: false
        anchors {
            bottom: parent.top
        }

        width: dp(100)
        height: srcModel.count * dp(50)

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                if (list.itemAt(mouseX, mouseY)){
                    list.currentIndex = list.indexAt(mouseX, mouseY)
                    select(list.currentItem)
                }

                list.visible = false
            }

            onPositionChanged: {
                var item = list.itemAt(mouseX, mouseY)
                if (item && list.hoveredItem !== item){
                    if (list.hoveredItem)
                        list.hoveredItem.color = Styles.shadeColor
                    item.color = Styles.ribbonHighlight
                    list.hoveredItem = item
                }
            }
        }

        visible: false

        model: ListModel {
            id: srcModel
        }
        delegate: Rectangle {
            property int index: itemIndex
            property string str: names[index] ? names[index] : ""
            width: dp(100)
            height: dp(50)

            color: Styles.shadeColor

            Text {
                text: parent.str
                anchors.centerIn: parent
                color: Styles.textColor
                font.family: "Droid Sans"
                font.pointSize: Styles.titleFont.smaller
                ////renderType: Text.NativeRendering
            }
        }
    }
}
