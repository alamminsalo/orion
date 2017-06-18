import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.1
//import "components"
import "irc"

RoundButton {
    id: chatBtn
    font.pointSize: 14
    flat: true
    font.family: "Material Icons"
    onClicked: chatdrawer.open()
}
