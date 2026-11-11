import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic as Basic
import QtQuick.Dialogs
import QtQuick.Layouts

Rectangle {
    id: root

    property string errorMessage: ""
    property bool loading: false

    signal fileSelected(url fileUrl)

    color: "#eef2f6"

    Rectangle {
        anchors.centerIn: parent
        width: Math.min(460, root.width - 48)
        height: content.implicitHeight + 64
        radius: 8
        color: "#ffffff"
        border.color: "#c6cdd5"
        border.width: 1

        ColumnLayout {
            id: content
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 32
            spacing: 20

            Label {
                Layout.fillWidth: true
                text: "TLOG PARSER"
                font.pixelSize: 36
                font.bold: true
                color: "#273444"
                horizontalAlignment: Text.AlignHCenter
            }

            Basic.Button {
                id: loadButton
                Layout.fillWidth: true
                implicitHeight: 48
                text: "TLOG Dosyası Yükle"
                enabled: !root.loading
                opacity: loadButton.enabled ? 1.0 : 0.5
                scale: loadButton.pressed ? 0.96 : 1.0
                onClicked: fileDialog.open()

                Behavior on scale {
                    NumberAnimation {
                        duration: 100
                    }
                }

                background: Rectangle {
                    radius: 6
                    color: loadButton.pressed ? "#1f5486"
                                              : (loadButton.hovered ? "#2f78bf" : "#2868a8")

                    Behavior on color {
                        ColorAnimation {
                            duration: 100
                        }
                    }
                }

                contentItem: Label {
                    text: loadButton.text
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Basic.BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: root.loading
                visible: root.loading
            }

            Label {
                Layout.fillWidth: true
                visible: root.loading
                text: "Dosya okunuyor…"
                color: "#273444"
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                Layout.fillWidth: true
                visible: root.errorMessage !== ""
                text: root.errorMessage
                color: "#a52828"
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "TLOG dosyası seç"
        nameFilters: ["TLOG dosyaları (*.tlog)", "Tüm dosyalar (*)"]
        onAccepted: root.fileSelected(selectedFile)
    }
}
