import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    required property string time
    required property string name
    required property string fields
    property bool highlighted: false

    implicitHeight: row.implicitHeight + 12
    color: root.highlighted ? "#d8e8fb" : "#ffffff"
    border.width: root.highlighted ? 2 : 1
    border.color: root.highlighted ? "#2868a8" : "#c6cdd5"
    radius: 3

    RowLayout {
        id: row
        anchors.fill: parent
        anchors.margins: 6
        spacing: 8

        Label {
            Layout.preferredWidth: 50
            Layout.alignment: Qt.AlignTop
            text: root.time
        }

        Rectangle {
            Layout.fillHeight: true
            implicitWidth: 1
            color: root.border.color
        }

        Label {
            Layout.preferredWidth: 240
            Layout.alignment: Qt.AlignTop
            text: root.name
            wrapMode: Text.WrapAnywhere
        }

        Rectangle {
            Layout.fillHeight: true
            implicitWidth: 1
            color: root.border.color
        }

        Label {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            text: root.fields
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }
}
