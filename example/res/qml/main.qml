import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

import WickedLasers 1.0

Item {
    id: rootItem

    Window {
        id: mainWindow

        width: 640
        height: 480

        RowLayout {
            anchors.fill: parent

            ColumnLayout {
                Layout.alignment: Qt.AlignTop


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


                Button {

                    visible: visComboBox.currentIndex === 5 // Spiral Fighter

                    focusPolicy: Qt.NoFocus

                    text: checked ? qsTr("Stop") : qsTr("Start")

                    checked: game.isPlaying

                    onClicked: game.toggle()
                }
                Button {

                    visible: visComboBox.currentIndex === 5 // Spiral Fighter

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
                    anchors.fill: parent

                    autostart: true
                    clearColor: "black"
                }
            }
        }



    }

    Component.onCompleted: {
        console.debug("QML LOADED")

        mainWindow.show()
        setWindow(mainWindow)
    }
}
