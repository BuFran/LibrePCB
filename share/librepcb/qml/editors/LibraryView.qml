import QtQuick 2.12
import QtQuick.Controls 2.12
import "LibraryView.js" as JS
import LibrePCB.Editors 1.0

Item {
    id: root

    LibraryInterface {
        id: iface
    }

    Text {
        id: content

        anchors.centerIn: parent
        text: qsTr("This is Library of SPARTA!!")
    }

}
