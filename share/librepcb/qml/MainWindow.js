function createDock(name, text) {
    var component = Qt.createComponent("utils/Dockable.qml");
    var sprite = component.createObject(dock);

    sprite.title = qsTr("UAHAHA");
    sprite.contents = Qt.createQmlObject('import "editors"; BoardView {anchors.centerIn: parent}', sprite);
}