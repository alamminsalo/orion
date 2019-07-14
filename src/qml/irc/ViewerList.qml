import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.1
import app.orion 1.0
import "../components"

Item {
    id: root
    property bool loading: false

    Layout.fillHeight: true
    Layout.fillWidth: true
    
    onVisibleChanged: {
        if (visible && chat.channel) {
            root.loading = true;
            viewerListModel.clear()
            Viewers.loadChatterList(chat.channel);
        }
    }
    
    BusyIndicator {
        id: spinner
        visible: running
        hoverEnabled: false
        anchors.centerIn: parent
        running: root.loading
    }
    
    ListView {
        id: list

        anchors.fill: parent
        anchors.margins: 10

        model: ListModel {
            id: viewerListModel
        }

        ScrollIndicator.vertical: ScrollIndicator { visible: isMobile() }
        ScrollBar.vertical: ResponsiveScrollBar { visible: !isMobile() }
        
        Connections {
            target: Viewers
            onChatterListLoaded: {
                root.loading = false;
                var standardGroupOrder = ["broadcaster", "staff", "global_mods", "admins", "moderators", "vips", "viewers"];
                var extraGroupsPos = standardGroupOrder.indexOf("moderators") + 1;

                var extraGroups = [];

                for (var otherGroupName in chatters) {
                    if (standardGroupOrder.indexOf(otherGroupName) == -1) {
                        extraGroups.push(otherGroupName)
                    }
                }
                extraGroups.sort();

                //var groupOrder = standardGroupOrder.slice();
                //groupOrder.splice.apply([extraGroupsPos, 0].concat(extraGroups));
                var groupOrder = standardGroupOrder.slice(0, extraGroupsPos).concat(extraGroups, standardGroupOrder.slice(extraGroupsPos));


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
            font.pointSize: 8
            color: chat.colors[user.toLowerCase()] || Material.foreground
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
