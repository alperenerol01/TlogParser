import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts
import TlogParser

Item {
    id: root

    required property LogSession session

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

            MapTab {
                session: root.session
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
