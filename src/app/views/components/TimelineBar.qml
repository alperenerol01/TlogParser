import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts
import TlogParser

RowLayout {
    id: root

    required property PlaybackController playback

    spacing: 8

    Button {
        text: root.playback.playing ? "Duraklat" : "Oynat"
        enabled: root.playback.durationMs > 0
        onClicked: {
            if (root.playback.playing) {
                root.playback.pause()
            } else {
                root.playback.play()
            }
        }
    }

    Button {
        text: "2x"
        checkable: true
        checked: root.playback.doubleSpeed
        onToggled: root.playback.doubleSpeed = checked
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
