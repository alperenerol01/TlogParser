import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts
import TlogParser

Item {
    id: root

    required property LogSession session

    component PageTab: Basic.TabButton {
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

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Basic.TabBar {
            id: tabBar
            Layout.fillWidth: true
            spacing: 0

            background: Rectangle {
                color: "#eef2f6"
            }

            PageTab {
                text: "Harita"
            }
            PageTab {
                text: "Ham Veri"
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            Item {
            }
            RawDataTab {
                model: root.session.packetModel
                currentIndex: root.session.playback.currentIndex
                onPacketClicked: (index) => root.session.playback.selectPacket(index)
            }
        }

        TimelineBar {
            Layout.fillWidth: true
            Layout.margins: 8
            playback: root.session.playback
        }
    }
}
