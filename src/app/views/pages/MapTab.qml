import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning
import TlogParser

Item {
    id: root

    required property LogSession session
    property bool routeShown: false

    function showRoute() {
        if (routeShown || root.session.flightPath.length === 0) {
            return
        }
        mapView.map.fitViewportToMapItems([routeLine])
        routeShown = true
    }

    onVisibleChanged: {
        if (visible) {
            Qt.callLater(root.showRoute)
        }
    }
    Plugin {
        id: mapPlugin
        name: "osm"

        PluginParameter {
            name: "osm.mapping.custom.host"
            value: "https://tile.openstreetmap.org/"
        }
        PluginParameter {
            name: "osm.mapping.providersrepository.disabled"
            value: true
        }
        PluginParameter {
            name: "osm.useragent"
            value: "TlogParser"
        }
    }

    MapView {
        id: mapView
        anchors.fill: parent
        map.plugin: mapPlugin
        map.activeMapType: map.supportedMapTypes[map.supportedMapTypes.length - 1]
    }

    MapPolyline {
        id: routeLine
        path: root.session.flightPath
        line.width: 3
        line.color: "red"
    }

    MapQuickItem {
        id: marker
        coordinate: root.session.playback.currentCoordinate
        visible: coordinate.isValid
        anchorPoint.x: dot.width / 2
        anchorPoint.y: dot.height / 2
        sourceItem: Rectangle {
            id: dot
            width: 16
            height: 16
            radius: 8
            color: "blue"
            border.width: 2
            border.color: "white"
        }
    }

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        width: positionColumn.implicitWidth + 16
        height: positionColumn.implicitHeight + 12
        visible: marker.visible
        color: "white"
        border.color: "black"
        border.width: 1

        Column {
            id: positionColumn
            anchors.centerIn: parent
            spacing: 2

            Label {
                text: "Lat: " + marker.coordinate.latitude.toFixed(7)
            }
            Label {
                text: "Lon: " + marker.coordinate.longitude.toFixed(7)
            }
            Label {
                text: "Alt: " + marker.coordinate.altitude.toFixed(2) + " m"
            }
        }
    }

    Component.onCompleted: {
        mapView.map.addMapItem(routeLine)
        mapView.map.addMapItem(marker)
    }
}
