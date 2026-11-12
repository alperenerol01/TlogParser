import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic as Basic

Basic.TabButton {
    id: tab

    implicitHeight: 44
    font.pixelSize: 15
    font.bold: tab.checked

    contentItem: Label {
        text: tab.text
        font: tab.font
        color: tab.checked ? "#ffffff" : "#273444"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        color: tab.checked ? "#2868a8" : (tab.hovered ? "#dde5ee" : "#eef2f6")

        Behavior on color {
            ColorAnimation {
                duration: 100
            }
        }
    }
}
