import QtQuick 2.12
import QtQuick.Controls 2.15

ApplicationWindow {
    id: app
    width: 640
    height: 480

    maximumHeight: height
    maximumWidth: width

    minimumHeight: height
    minimumWidth: width

    visible: true
    title: qsTr("Компресія зображень «barch»")

    ImageList {}
}
