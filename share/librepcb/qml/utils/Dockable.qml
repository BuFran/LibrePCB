import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import "Dockable.js" as JS

Item {
    id: root

    default property alias contents: placeholder.data
    property alias title: window.title

    implicitHeight: 190

    anchors {
        left: parent.left
        right: parent.right
    }

    Rectangle {
        id: content

        anchors.fill: parent
        width: 200
        height: implicitHeight
        state: "docked"
        color: "#cfcfcf"
        states: [
            State {
                name: "undocked"

                PropertyChanges {
                    target: root
                    height: 0
                }

                ParentChange {
                    target: content
                    parent: undockedContainer
                }

            },
            State {
                name: "docked"

                PropertyChanges {
                    target: root
                    height: implicitHeight
                }

                ParentChange {
                    target: content
                    parent: root
                }

            }
        ]

        ToolBar {
            id: titleBar

            visible: JS.isDocked()

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            Label {
                id: titleLabel

                text: window.title

                anchors {
                    left: parent.left
                    leftMargin: 8
                    verticalCenter: parent.verticalCenter
                }

            }

            Button {
                flat: true
                icon.source: "qrc:///img/actions/checks.png"
                onClicked: JS.setDocked(!JS.isDocked())

                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: 8
                }

            }

        }

        Item {
            id: placeholder

            anchors {
                top: titleBar.visible ? titleBar.bottom : parent.top
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

        }

    }

    Window {
        id: window

        width: 360
        height: 440
        flags: Qt.WindowTitleHint
        title: titleLabel.text
        visible: !JS.isDocked()
        onClosing: JS.setDocked(true)

        Item {
            id: undockedContainer

            anchors.fill: parent
        }

    }

}
