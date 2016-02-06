import QtQuick 2.0
import "../styles.js" as Styles

Item {

    property string text
    property int selection: 1

    onSelectionChanged: {
        setSelection()
    }

    function setSelection(){
        option0.color = Styles.sidebarBg
        option1.color = Styles.sidebarBg
        option2.color = Styles.sidebarBg
        option3.color = Styles.sidebarBg

        switch(selection){
        case 0:
            option0.color = Styles.borderBright
            break

        case 1:
            option1.color = Styles.borderBright
            break

        case 2:
            option2.color = Styles.borderBright
            break

        case 3:
            option3.color = Styles.borderBright
            break
        }
    }

    Component.onCompleted: setSelection()

    id: root
    height: dp(60)

    Text {
        font.family: "Droid Sans"
        color: Styles.textColor
        font.pixelSize: dp(Styles.titleFont.bigger)
        text: root.text
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: dp(10)
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        renderType: Text.NativeRendering
    }

    Item {
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            margins: dp(10)
        }

        width: dp(70)
        height: dp(60)

        Rectangle {
            id: option0
            property int index: 0
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.verticalCenter
                right: parent.horizontalCenter
            }
            color: Styles.sidebarBg

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onHoveredChanged: {
                    if (selection != parent.index)
                        parent.color = containsMouse ? Styles.highlight2 : Styles.sidebarBg
                }
                onClicked: selection = parent.index
            }
        }

        Rectangle {
            id: option1
            property int index: 1
            anchors {
                top: parent.top
                left: parent.horizontalCenter
                bottom: parent.verticalCenter
                right: parent.right
            }
            color: Styles.sidebarBg

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onHoveredChanged: {
                    if (selection != parent.index)
                        parent.color = containsMouse ? Styles.highlight2 : Styles.sidebarBg
                }
                onClicked: selection = parent.index
            }
        }

        Rectangle {
            id: option2
            property int index: 2
            anchors {
                top: parent.verticalCenter
                left: parent.left
                bottom: parent.bottom
                right: parent.horizontalCenter
            }
            color: Styles.sidebarBg

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onHoveredChanged: {
                    if (selection != parent.index)
                        parent.color = containsMouse ? Styles.highlight2 : Styles.sidebarBg
                }
                onClicked: selection = parent.index
            }
        }

        Rectangle {
            id: option3
            property int index: 3
            anchors {
                top: parent.verticalCenter
                left: parent.horizontalCenter
                bottom: parent.bottom
                right: parent.right
            }
            color: Styles.sidebarBg

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onHoveredChanged: {
                    if (selection != parent.index)
                        parent.color = containsMouse ? Styles.highlight2 : Styles.sidebarBg
                }
                onClicked: selection = parent.index
            }
        }
    }
}
