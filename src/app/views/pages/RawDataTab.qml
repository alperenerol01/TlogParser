import QtQuick
import QtQuick.Controls
import TlogParser

ListView {
    id: root

    signal packetClicked(int index)

    clip: true
    spacing: 4
    highlightMoveDuration: 0

    onCurrentIndexChanged: {
        if (currentIndex >= 0) {
            positionViewAtIndex(currentIndex, ListView.Contain)
        }
    }

    ScrollBar.vertical: ScrollBar {
    }

    delegate: PacketRow {
        id: packetRow

        required property int index

        width: ListView.view.width
        highlighted: ListView.isCurrentItem

        TapHandler {
            onTapped: root.packetClicked(packetRow.index)
        }
    }
}
