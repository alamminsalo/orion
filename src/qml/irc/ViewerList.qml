import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.1
import app.orion 1.0
import "../components"

Item {
    id: root
    property bool loading: true
    
    anchors {
        bottom: parent.bottom
        left: parent.left
        right: parent.right
        margins: 10
    }
    
    onVisibleChanged: {
        if (visible) {
            root.loading = true;
            viewerListModel.clear()
            Viewers.loadChatterList(chat.channel);
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
        
        ScrollIndicator.vertical: ScrollIndicator { visible: isMobile() }
        ScrollBar.vertical: ResponsiveScrollBar { visible: !isMobile() }
        
        Connections {
            target: Viewers
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
        }
        
        section {
            property: "groupName"
            criteria: ViewSection.FullString
            delegate: Row {
                height: 50
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    font.capitalization: Font.AllUppercase
                    text: section
                    font.bold: true
                }
            }
        }
        
    }
}
