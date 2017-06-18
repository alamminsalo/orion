import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.1
import "../components"
import "../util.js" as Util
import "../"

Rectangle {
    id: viewerList
    property bool loading: true

   // height: enabled ? parent.height : 0
    
    anchors {
        bottom: parent.bottom
        left: parent.left
        right: parent.right
    }
    
    Behavior on height {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutCubic
        }
    }
    
    z: 10
    
    //opacity: root._opacity
    
    onEnabledChanged: {
        if (enabled) {
            viewerList.loading = true;
            viewerListModel.clear();
            g_cman.loadChatterList(chat.channel);
        }
    }
    
    BusyIndicator {
        id: spinner
        anchors.centerIn: parent
        running: viewerList.loading && viewerList.enabled
    }
    
    Item {
        id: viewerListHeading
        visible: viewerList.enabled
        anchors {
            bottom: parent.top
            left: parent.left
            right: parent.right
        }
        
        height: dp(40)
        
        Label {
            anchors.centerIn: parent
            text: "Viewer List"
            font.bold: true
        }
    }
    
    ListView {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            top: viewerListHeading.bottom
        }
        
        model: ListModel {
            id: viewerListModel
        }
        
        Connections {
            target: g_cman
            onChatterListLoaded: {
                viewerList.loading = false;
                
                var groupOrder = ["staff", "global_mods", "admins", "moderators", "viewers"];
                
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
            anchors {
                fill: parent
                leftMargin: dp(5)
                rightMargin: dp(5)
            }
            font.capitalization: Font.Capitalize
        }
        
        section {
            property: "groupName"
            criteria: ViewSection.FullString
            delegate: Row {
                height: dp(50)
                Label {
                    font.capitalization: Font.AllUppercase
                    text: section
                }
            }
        }
        
    }
}
