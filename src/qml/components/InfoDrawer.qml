import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import "."

Drawer {
    id: root
    property var item: undefined
    dim: false
    interactive: visible

    function show(channelItem) {
        item = channelItem

        img.source = ""
        title.text = "N/A"
        description.text = "N/A"
        viewerCount.text = "N/A"

        if (item) {
            img.source = item.preview || item.logo || ""
            title.text = item.title
            viewerCount.text = item.viewers + " viewers"
            description.text = item.info
        }

        open()
    }

    height: 200

    Image {
        id: img
        fillMode: Image.PreserveAspectFit
        anchors {
            top: parent.top
            bottom: parent.bottom
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.rightMargin: 10

        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Label {
                id: title
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Qt.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Label {
                id: viewerCount
                Layout.fillWidth: true
                horizontalAlignment: Qt.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Label {
                id: description
                Layout.fillWidth: true
                horizontalAlignment: Qt.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }

        ColumnLayout {
            Layout.fillHeight: true
            spacing: 0

            IconButtonFlat {
                id: watchBtn
                text: "\ue038"
                onClicked: {
                    if (item) {
                        playerView.getStreams(item)
                    }
                    close()
                }

                ToolTip {
                    text: "Start watching"
                }
            }

            IconButtonFlat {
                id: favoriteBtn
                text: "\ue87d"
                highlighted: item ? item.favourite : false
                onClicked: {
                    if (item) {
                        if (item.favourite)
                            g_cman.addToFavourites(item)
                        else
                            g_cman.removeFromFavourites(item)
                    }
                }
                ToolTip {
                    text: !favoriteBtn.highlighted ? "Add favourite" : "Remove favourite"
                }
            }

            IconButtonFlat {
                id: vodBtn
                text: "\ue04a"
                onClicked: {
                    if (item) {
                        vodsView.search(item)
                    }
                    close()
                }
                ToolTip {
                    text: "Videos"
                }
            }

            IconButtonFlat {
                id: openChatBtn
                text: "\ue0ca"
                onClicked: {
                    if (item) {
                        playerView.getChat(item)
                    }
                    close()
                }
                ToolTip {
                    text: "Videos"
                }
            }
        }
    }
}
