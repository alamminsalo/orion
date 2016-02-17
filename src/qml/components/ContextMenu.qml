import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "../styles.js" as Styles

Menu {
    property variant item
    property int state

    id: root
    visible: false

    function display(item){
        root.item = item
    }

    onAboutToShow: {
        g_contextMenuVisible = true
    }
    onAboutToHide: {
        g_contextMenuVisible = false
    }

    style: MenuStyle {
        frame: Rectangle {
                color: Styles.sidebarBg
        }

        itemDelegate {

            label: Item {
                height: dp(33)
                width: _icon.width + _label.width

                Icon {
                    id: _icon
                    icon: styleData.text.split(";")[1]
                    iconSize: dp(12)
                    iconColor: styleData.enabled ? Styles.iconColor : Styles.disabled
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                        left: parent.left
                    }
                }

                Text {
                    id:_label
                    text: styleData.text.split(";")[0]
                    color: styleData.enabled ? Styles.textColor : Styles.disabled
                    font.pointSize: dp(Styles.titleFont.smaller)
                    verticalAlignment: Text.AlignVCenter
                    anchors {
                        top: _icon.top
                        bottom: _icon.bottom
                        left: _icon.right
                    }
                    renderType: Text.NativeRendering
                }
            }

            background: Rectangle {
                color: "#ffffff"
                opacity: 0.1
                visible: styleData.selected && styleData.enabled
            }
        }

    }
}
