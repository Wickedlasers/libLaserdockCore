import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

import WickedLasers 1.0

Item {
    id: rootItem

    Window {
        id: mainWindow

        width: 480
        height: 640

        ColumnLayout {
            anchors.fill: parent

            RowLayout {
                Switch {
                    id:networkswitch
                    focusPolicy: Qt.NoFocus
                    text: qsTr("<font color='#fefefe'>WiFi")
                    checked: ldCore.dataDispatcher.isNetwork
                    onCheckedChanged: ldCore.dataDispatcher.isNetwork = networkswitch.checked
                    }

                Binding { target: ldCore.dataDispatcher; property: "isNetwork"; value: networkswitch.checked }

                Button {
                    focusPolicy: Qt.NoFocus
                    enabled: ldCore.laserController.connectedDevices > 0

                    text: checked ? qsTr("Stop") : qsTr("Start")

                    checked: ldCore.laserController.isActive

                    onClicked: ldCore.laserController.togglePlay()
                }

                ComboBox {
                    id: visComboBox

                    focusPolicy: Qt.NoFocus

                    model: ListModel {
                        ListElement {
                            text: "Circle"
                        }
                        ListElement {
                            text: "Square"
                        }
                        ListElement {
                            text: "Spectrum"
                        }
                        ListElement {
                            text: "GoGoGirl"
                        }
                        ListElement {
                            text: "Clock"
                        }
                        ListElement {
                            text: "Spiral Fighter"
                        }
                    }

                    onActivated: activateVis(index)
                }
            }

            RowLayout {
                visible: visComboBox.currentIndex === 5 // Spiral Fighter
                Button {
                    focusPolicy: Qt.NoFocus

                    text: game.state === LdGameState.Playing
                          ? qsTr("Pause Game")
                          : game.state === LdGameState.Paused
                            ? qsTr("Resume Game")
                            : qsTr("Start Game")

                    checked: game.state === LdGameState.Playing

                    onClicked: game.toggle()
                }
                Button {
                    focusPolicy: Qt.NoFocus

                    text: qsTr("Reset")

                    onClicked: game.reset()
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                LdSimulatorItem {
                    id: simulator
                    width: parent.width
                    height: width

                    autostart: true
                    clearColor: "black"
                }
            }
            Item {  Layout.fillHeight: true }

        }



    }

    Component.onCompleted: {
        console.debug("QML LOADED")

        mainWindow.show()
        setWindow(mainWindow)
    }
}
