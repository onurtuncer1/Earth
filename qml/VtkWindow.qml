import QtQuick 2.19
import Vtk 1.0 as Vtk
import QtQuick.Controls 2.15

Item {
    id: root
    property bool isVisible: true

    Vtk.Viewer {
        anchors.fill: parent
        height: 100
        visible: root.isVisible
        objectName: "viewer"
    }
}