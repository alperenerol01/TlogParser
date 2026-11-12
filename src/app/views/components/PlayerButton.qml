import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic as Basic

Basic.Button {
    id: button

    implicitHeight: 36
    implicitWidth: Math.max(64, implicitContentWidth + leftPadding + rightPadding)
    horizontalPadding: 16
    font.pixelSize: 14
    font.bold: button.checked

    contentItem: Label {
        text: button.text
        font: button.font
        color: button.checked ? "#ffffff" : (button.enabled ? "#273444" : "#9aa5b1")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        radius: 4
        color: button.checked ? "#2868a8"
                              : (button.down ? "#cfd9e4" : (button.hovered ? "#dde5ee" : "#eef2f6"))
        border.width: 1
        border.color: button.checked ? "#2868a8" : "#c6cdd5"

        Behavior on color {
            ColorAnimation {
                duration: 100
            }
        }
    }
}
