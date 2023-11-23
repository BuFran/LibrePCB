import QtQuick 2.12
import QtQuick.Controls 2.12
import "BoardView.js" as JS;
import LibrePCB.Editors 1.0

Item {
    id: root

    BoardInterface {
        id: iface
    }

    Text {
        id: content

        anchors.centerIn: parent
        text: qsTr("This is Board of SPARTA!!")
    }

}
