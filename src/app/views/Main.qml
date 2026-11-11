import QtQuick
import QtQuick.Controls
import TlogParser

ApplicationWindow {
    width: 1200
    height: 800
    visible: true
    title: "TLOG PARSER"

    LogSession {
        id: logSession
    }

    LoadPage {
        anchors.fill: parent
        visible: !logSession.loaded
        errorMessage: logSession.errorMessage
        loading: logSession.loading
        onFileSelected: (fileUrl) => logSession.load(fileUrl)
    }

    ViewerPage {
        anchors.fill: parent
        visible: logSession.loaded
        session: logSession
    }
}
