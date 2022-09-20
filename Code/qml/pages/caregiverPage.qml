import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 1.0

Item
{
    id: caregiverPage

    signal pinActionSignal(string action)
    signal pinPageSignal(string page)
    signal textActionSignal(string action)
    signal textPageSignal(string page)
    signal caregiverRemoveSignal(int id)

    property int otherCaregiverID: (((caregiverID - 1) ^ 1) + 1)
    property int caregiverExists: smd.caregiverExists(otherCaregiverID)
    property string addRemove: caregiverExists ? ("Remove Caregiver " + otherCaregiverID) :  ("Add Caregiver " + otherCaregiverID )

    function checkPatients()
    {
        if(patientsToTakeMed.includes(1) && patientsToTakeMed.includes(2))
            caregiverPage.state = "bothPatients"
        else if(patientsToTakeMed.includes(1))
             caregiverPage.state ="patient1"
        else if(patientsToTakeMed.includes(2))
             caregiverPage.state ="patient2"
        else
             caregiverPage.state ="smd"
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
        "pinPage.qml",
        "textPage.qml",
        "textPage.qml",
        "textPage.qml"
    ]

    property variant otherCaregiverActions:
    ["addCaregiver", "removeCaregiver"]

    property variant actions:
    [
        "",
        "fingerprint",
        "removeCaregiver",
         ""
    ]

    property variant carID: [ 0, 0, caregiverID , otherCaregiverID]

    Item
    {
        id: caregiverMenu

        anchors.top: takeMedicationStatus.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        width: parent.width
        height: parent.height * 6 / 8

        Rectangle
        {
            id: caregiverName

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height / 6

            border.color: "gray"
            border.width: 1
            radius: 10

            Text
            {
                anchors.leftMargin: caregiverName.implicitMargin
                anchors.rightMargin: caregiverName.implicitMargin
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text : "CAREGIVER " + caregiverID
                fontSizeMode: Text.Fit
            }
        }

        Item
        {
            anchors.top: caregiverName.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            Repeater
            {
                model: ["UPDATE PIN", "UPDATE FINGERPRINT", "AUTO REMOVE", addRemove]
                Button
                {
                    x: (index % 1) * width
                    y: Math.floor(index) * height
                    width: parent.width
                    height: parent.height / 4
                    text: modelData
                    onClicked:
                    {                    
                        switch(index)
                        {
                            case 0: // update pin
                                caregiverPage.pinPageSignal.connect(setPinPreviousPage)
                                caregiverPage.pinPageSignal("caregiverPage.qml")
                                caregiverPage.pinActionSignal.connect(setPinNextAction)
                                caregiverPage.pinActionSignal("PIN")
                                break
                            case 1: // update fingerprint
                                caregiverPage.textPageSignal.connect(setTextPreviousPage)
                                caregiverPage.textPageSignal("caregiverPage.qml")
                                caregiverPage.textActionSignal.connect(setTextPageAction)
                                caregiverPage.textActionSignal(actions[index])
                                break
                            case 2: // remove actual caregiver
                                caregiverPage.textPageSignal.connect(setTextPreviousPage)
                                caregiverPage.textPageSignal("caregiverPage.qml")
                                caregiverPage.textActionSignal.connect(setTextPageAction)
                                caregiverPage.textActionSignal(actions[index])
                                caregiverPage.caregiverRemoveSignal.connect(setCaregiverRemove)
                                caregiverPage.caregiverRemoveSignal(carID[index])
                                break
                            case 3: // remove/add other caregiver
                                caregiverPage.textPageSignal.connect(setTextPreviousPage)
                                caregiverPage.textPageSignal("caregiverPage.qml")
                                caregiverPage.textActionSignal.connect(setTextPageAction)
                                caregiverPage.textActionSignal(otherCaregiverActions[caregiverExists])
                                caregiverPage.caregiverRemoveSignal.connect(setCaregiverRemove)
                                caregiverPage.caregiverRemoveSignal(carID[index])
                                break
                        }
                        mystackview.replace(pages[index])
                    }
                }
            }
        }


    }

    Item
    {
        anchors.top: caregiverMenu.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 8

        Button
        {
            width: parent.width
            height: parent.height
            text: " Back"
            onClicked:  mystackview.replace("mainMenuPage.qml");
        }
    }

}
