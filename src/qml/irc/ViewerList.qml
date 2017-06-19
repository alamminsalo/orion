import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.1

Item {
    id: root
    property bool loading: true
    
    anchors {
        bottom: parent.bottom
        left: parent.left
        right: parent.right
    }
    
    onVisibleChanged: {
        if (visible) {
            root.loading = true;
            viewerListModel.clear()
            g_cman.loadChatterList(chat.channel);
        }
    }
    
    BusyIndicator {
        id: spinner
        anchors.centerIn: parent
        running: root.loading
    }
    
    ListView {
        anchors.fill: parent

        model: ListModel {
            id: viewerListModel
        }
        
        Connections {
            target: g_cman
            onChatterListLoaded: {
                root.loading = false;
                var groupOrder = ["staff", "global_mods", "admins", "moderators", "viewers"];

                var viewers = []
                for (var j = 0; j < groupOrder.length; j++) {
                    var groupName = groupOrder[j];
                    var group = chatters[groupName];
                    if (!group) {
                        continue;
                    }
                    
                    for (var i = 0; i < group.length; i++) {
                        var chatter = group[i];
                        viewerListModel.append({"groupName": groupName, "user": chatter});
                    }
                }
            }
        }
        
        clip: true
        delegate: Label {
            text: user
            //font.capitalization: Font.Capitalize
        }
        
        section {
            property: "groupName"
            criteria: ViewSection.FullString
            delegate: Row {
                height: dp(50)
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    font.capitalization: Font.AllUppercase
                    text: section
                }
            }
        }
        
    }
}
