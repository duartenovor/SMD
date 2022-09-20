import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 1.0

Item
{
    id: mainMenuPage

    function checkPatients()
    {
        if(patientsToTakeMed.includes(1) && patientsToTakeMed.includes(2))
            mainMenuPage.state = "bothPatients"
        else if(patientsToTakeMed.includes(1))
             mainMenuPage.state ="patient1"
        else if(patientsToTakeMed.includes(2))
             mainMenuPage.state ="patient2"
        else
             mainMenuPage.state ="smd"
    }

    Item
    {
        Component.onCompleted: checkPatients()
    }

    Timer
    {
            interval: 1000; running: true; repeat: true

            onTriggered: checkPatients()
     }

    states:
    [
        State
        {
            name: "bothPatients"
            PropertyChanges { target: patient1MedicationLayer; visible: true }
            PropertyChanges { target: patient2MedicationLayer; visible: true }
            PropertyChanges { target: smdLayer; visible: false }
        },
        State
        {
            name: "patient1"
            PropertyChanges { target: patient1MedicationLayer; visible: true }
            PropertyChanges { target: patient2MedicationLayer; visible: false }
            PropertyChanges { target: smdLayer; visible: false }
        },
        State
        {
            name: "patient2"
            PropertyChanges { target: patient1MedicationLayer; visible: false }
            PropertyChanges { target: patient2MedicationLayer; visible: true }
            PropertyChanges { target: smdLayer; visible: false }
        },
        State
        {
            name: "smd"
            PropertyChanges { target: patient2MedicationLayer; visible: false }
            PropertyChanges { target: patient1MedicationLayer; visible: false }
            PropertyChanges { target: smdLayer; visible: true }
        }

    ]

    transitions:
            Transition
            {
                to: "*"
                ColorAnimation { target: takeMedicationLayer; duration: 1000}
                ColorAnimation { target: smdLayer; duration: 1000}

            }

    Item
    {
        id: takeMedicationStatus

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 8
        width: parent.width

        Rectangle
        {
            id: smdLayer

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height

            color: "gray"

            Text
            {
                anchors.leftMargin: takeMedicationLayer.implicitMargin
                anchors.rightMargin: takeMedicationLayer.implicitMargin
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "SMD";
                fontSizeMode: Text.Fit
            }
        }

        Item
        {
            id: takeMedicationLayer

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom


            Item
            {
                id: patient1MedicationLayer

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                width: parent.width / 2

                Rectangle
                {
                    id: patient1Message

                    anchors.top: parent.top
                    anchors.left: parent.left
                    height: parent.height
                    width: parent.width * 3 / 4

                    border.color: "gray"
                    border.width: 1
                    radius: 10

                    Text
                    {
                        anchors.leftMargin: patient1Message.implicitMargin
                        anchors.rightMargin: patient1Message.implicitMargin
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: "Supply Medication Patient 1?";
                        fontSizeMode: Text.Fit
                    }
                }

                Item
                {
                    anchors.top: parent.top
                    anchors.left: patient1Message.right
                    height: parent.height
                    width: parent.width / 4

                    Button
                    {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        anchors.left: parent.left
                        text: "Yes"
                        onClicked: smd.supplyMedicationDecision(1)
                    }
                }
            }

            Item
            {
                id: patient2MedicationLayer

                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: patient1MedicationLayer.right
                width: parent.width / 2

                Rectangle
                {
                    id: patient2Message

                    anchors.top: parent.top
                    anchors.left: parent.left
                    height: parent.height
                    width: parent.width * 3 / 4

                    border.color: "gray"
                    border.width: 1
                    radius: 10

                    Text
                    {
                        anchors.leftMargin: patient1Message.implicitMargin
                        anchors.rightMargin: patient1Message.implicitMargin
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: "Supply Medication Patient 2?";
                        fontSizeMode: Text.Fit
                    }
                }

                Item
                {
                    anchors.top: parent.top
                    anchors.left: patient2Message.right
                    height: parent.height
                    width: parent.width / 4

                    Button
                    {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        anchors.left: parent.left
                        text: "Yes"

                        onClicked: smd.supplyMedicationDecision(2)
                    }
                }
            }
        }
    }

    property variant pages:
    [
        "caregiverPage.qml",
        "medicationPage.qml",
    ]

    Item
    {
        id: caregiverMedicationMenu

        anchors.top: takeMedicationStatus.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height * 3 / 8

        Repeater
        {
            model: ["CAREGIVER", "MEDICATION"]
            Button
            {
                x: (index % 1) * width
                y: Math.floor(index) * height
                width: parent.width
                height: parent.height / 2
                text: modelData
                onClicked:  mystackview.replace(pages[index])
            }
        }
    }

    Item
    {
        id: patientsMenu

        anchors.top: caregiverMedicationMenu.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height * 3 / 8

        Repeater
        {
            model: ["PATIENT1", "PATIENT2"]
            Button
            {
                x: (index % 1) * width
                y: Math.floor(index) * height
                width: parent.width
                height: parent.height / 2
                text: modelData
                onClicked:
                {
                    patientID = index + 1
                    smd.time = 0
                    mystackview.replace("patientPage.qml")
                }
            }
        }
    }

    Item
    {
        anchors.top: patientsMenu.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height  / 8

        Button
        {
            width: parent.width
            height: parent.height
            text: "Close Session"
            onClicked:
            {
                smd.caregiverState = 0
                mystackview.replace("initialPage.qml")
            }
        }
    }
}
