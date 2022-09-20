import QtQuick 2.12
import QtQuick.Window 2.12
import QtQml 2.12
import QtQuick.Controls 2.12
//import QtQuick.Dialogs 1.2

Window {
    id: mainWindow
    width: 800
    height: 480
    visible: true
    title: qsTr("SMD")

    property string pinNextAction
    property string pinPreviousPage
    property int caregiverID: 1
    property int caregiverRem
    property int patientID
    property string textPageAction
    property string textPreviousPage
    property string newPin
    property var patientsToTakeMed: []

    FontLoader
    {
            id: fontId; source: "qrc:Font/OpenSans-Bold.ttf"
    }

    Connections
    {
        target: smd

        function onSendToQml(text) {
            mystackview.replace(text)
        }
        function onSetCaregiverID(id) {
            caregiverID = id
        }
        function onSetPatients(id) {
            console.log("emit qml")
            patientsToTakeMed.push(id)
        }
        function onRemovePatients(id) {
            const index = patientsToTakeMed.indexOf(id)
            if(index > -1)
                patientsToTakeMed.splice(index, 1)
        }
    }

    StackView
    {
            id: mystackview
            anchors.fill: parent
//            initialItem: "initialPage.qml"
            initialItem: "mainMenuPage.qml"
//            initialItem: "medicationPage.qml"
//            initialItem: "patientPage.qml"
//            initialItem: "pinPage.qml"

    }

    function setPinNextAction(action) {
        pinNextAction = action
    }

    function setPinPreviousPage(page) {
        pinPreviousPage = page
    }

    function setCaregiverID(id) {
        caregiverID = id
    }

    function setTextPageAction(action) {
        textPageAction = action
    }

    function setTextPreviousPage(page) {
        textPreviousPage = page
    }

    function setCaregiverRemove(id) {
        caregiverRem = id
    }

    function setCaregiverNewPin(pin) {
        newPin = pin
    }

}
