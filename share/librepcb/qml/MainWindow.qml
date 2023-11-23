import QtQuick 2.0
import QtQuick.Controls 1.5
import QtQuick.Controls 2.0 as Controls2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.0
import "utils"
import "editors" as Editors;
import "MainWindow.js" as JS
import LibrePCB.Editors 1.0

ApplicationWindow {
    id: window

    width: 1024
    height: 768
    visible: true
    title: cppWindow.title
    color: "#4f4f4f"

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 50
            Layout.fillHeight: true
            color: "#353535"
            visible: true

            Column {
                anchors.fill: parent

                SideButton {
                    id: btnHome

                    name: qsTr("Home")
                    iconSource: "qrc:///img/actions/home.svg"
                    checked: true
                }

                SideButton {
                    id: btnLibraries

                    name: qsTr("Libraries")
                    iconSource: "qrc:///img/actions/libraries.png"
                    checkable: false
                    autoExclusive: false
                    onClicked: cppApp.openLibraryManager()
                }

                SideButton {
                    id: btnProject

                    name: qsTr("Projects")
                    iconSource: "qrc:///img/actions/projects.png"
                    enabled: !cppApp.openedProjects.empty
                }

                SideButton {
                    id: btnChecks

                    name: qsTr("Checks")
                    iconSource: "qrc:///img/actions/checks.png"
                    enabled: !cppApp.openedProjects.empty
                }

                SideButton {
                    id: btnSearch

                    name: qsTr("Find")
                    iconSource: "qrc:///img/actions/find.png"
                    enabled: !cppApp.openedProjects.empty
                }

            }

        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: btnHome.checked

            Button {
                text: "Create Project"
                Layout.alignment: Qt.AlignCenter
                onClicked: {
                    if (cppWindow.createProject())
                        btnProject.checked = true;

                }
            }

            Button {
                text: "Open Project"
                Layout.alignment: Qt.AlignCenter
                onClicked: {
                    if (cppWindow.openProject())
                        btnProject.checked = true;

                }
            }

            // TODO make this as collection of Docks managed by C++
            Button {
                text: "BuFran's docking UI test"
                Layout.alignment: Qt.AlignCenter
                onClicked: JS.createDock("Name1", "TEXT1");
            }

        }

        Column  {
            id: dock
            width: 250
            spacing: 8

            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
                margins: 8
            }
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: !btnHome.checked

            Column {
                Layout.minimumWidth: 100
                width: 300

                ProjectPanel {
                    width: parent.width
                    height: parent.height
                    visible: btnProject.checked
                }

                LibrariesPanel {
                    width: parent.width
                    height: parent.height
                    visible: btnLibraries.checked
                }

            }

            SchematicEditor {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 100
            }

        }

    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")

            MenuItem {
                text: qsTr("&New...")
            }

            MenuItem {
                text: qsTr("&Open...")
            }

            MenuItem {
                text: qsTr("&Save")
            }

            MenuItem {
                text: qsTr("Save &As...")
            }

            MenuItem {
                text: qsTr("&Quit")
            }

        }

        Menu {
            title: qsTr("&Extras")

            MenuItem {
                text: qsTr("Workspace Settings") + "..."
                iconSource: "qrc:///img/actions/settings.png"
                shortcut: "Ctrl+,"
                onTriggered: cppApp.openWorkspaceSettings()
            }

        }

        Menu {
            title: qsTr("&Help")

            MenuItem {
                text: qsTr("&About")
            }

        }

    }

    statusBar: StatusBar {
        implicitHeight: 22

        RowLayout {
            anchors.fill: parent

            Label {
                text: "Workspace: " + cppApp.wsPath
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                Layout.leftMargin: 6
                Layout.fillWidth: true
            }

        }

    }

}
