function createDock(name, text) {
    var component = Qt.createComponent("utils/Dockable.qml");
    var sprite = component.createObject(dock);

    sprite.title = qsTr("UAHAHA");
    sprite.contents = Qt.createQmlObject('import QtQuick.Controls 2.0; Label {anchors.centerIn: parent; text: "XXX"; font.pointSize: 48}', sprite);
}