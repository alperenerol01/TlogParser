import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts
import TlogParser

RowLayout {
    id: root

    required property PlaybackController playback

    spacing: 8

    PlayerButton {
        id:playButton
        Layout.preferredWidth: 104
        text: root.playback.playing ? "Duraklat" : "Oynat"
        checked: root.playback.playing
        enabled: root.playback.durationMs > 0
        onClicked: {
            if (root.playback.playing) {
                root.playback.pause()
            } else {
                root.playback.play()
            }
        }
    }

    ComboBox {
        width: 200
        textRole: "text"
        valueRole: "value"
        model: [
            {text: "1x", value: 1},
            {text: "2x", value: 2},
            {text: "4x", value: 4},
            {text: "8x", value: 8}
        ]
        onActivated: root.playback.speed = currentValue

    }

    Label {
        text: root.playback.positionText
        Layout.minimumWidth: 48
        horizontalAlignment: Text.AlignRight
    }

    Basic.Slider {
        Layout.fillWidth: true
        from: 0
        to: root.playback.durationMs
        value: root.playback.positionMs
        onPressedChanged: {
            if (pressed) {
                root.playback.pause()
            }
        }
        onMoved: root.playback.positionMs = value
    }

    Label {
        text: root.playback.durationText
    }
}
