import QtQuick 2.12
import QtQuick.Controls 2.15
import ImageListModel

Frame {
    implicitWidth: 638
    implicitHeight: 478
    anchors.fill: parent

    CheckBox {
        x: 210
        checked: true
        onCheckStateChanged: function (event) {
            view.model.toggleBMP()
        }
    }
    Label {
        x: 225
        text: "bmp"
    }

    CheckBox {
        x: 260
        checked: true
        onCheckStateChanged: function (event) {
            view.model.togglePNG()
        }
    }
    Label {
        x: 275
        text: "png"
    }

    CheckBox {
        x: 310
        checked: true
        onCheckStateChanged: function (event) {
            view.model.toggleBarch()
        }
    }
    Label {
        x: 325
        text: "barch"
    }

    ListView {
        id: view
        x: 24
        y: 18
        implicitWidth: 638
        implicitHeight: 458

        interactive: false

        Dialog {
            id: error_dialog
            x: 5
            implicitHeight: 65
            modal: true
            closePolicy: Popup.CloseOnEscape
            title: qsTr("Помилка")

            Button {
                id: control
                text: qsTr("Ок")

                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                contentItem: Text {
                    text: control.text
                    font: control.font
                    color: control.down ? "#17a81a" : "#be212b"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                background: Rectangle {
                    implicitWidth: 60
                    implicitHeight: 20
                    border.color: control.down ? "#17a81a" : "#be212b"
                    border.width: 1
                    radius: 1
                    color: control.down ? "gray" : "white"
                }

                onClicked: {
                    error_dialog.close()
                    frame.enabled = true
                }
            }
        }

        model: ImageListModel {
            onProcessingError: function(file_name, row) {
                error_dialog.y = row * 20 + 50
                error_dialog.title = "Невідомий файл: " + file_name
                error_dialog.visible = true
                frame.enabled = false
                error_dialog.open()
            }
        }

        header: Row {
            width: 638
            height: 25

            TextField {
                width: 500
                text: view.model.headerData(0, Qt.Horizontal)
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                readOnly: true
            }

            TextField {
                width: 90
                text: view.model.headerData(1, Qt.Horizontal)
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                readOnly: true
            }
        }

        delegate: Row {
            width: 638
            height: 20

            TextField {
                width: 500
                leftPadding: 5
                rightPadding: 5

                text: model.name
                readOnly: true
                onPressed: {
                    view.model.processImage(index)
                }
            }

            TextField {
                width: 90
                leftPadding: 5
                rightPadding: 5

                text: model.size
                readOnly: true
                horizontalAlignment: Text.AlignRight
            }
        }
    }
}
