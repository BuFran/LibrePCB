import QtQuick 2.12
import QtQuick.Controls 2.12
import "Schematiciew.js" as JS
import LibrePCB.Editors 1.0

Item {
    id: root

    SchematicInterface {
        id: iface
    }

    Text {
        id: content

        anchors.centerIn: parent
        text: qsTr("This is Schematic of SPARTA!!")
    }
}
