import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 1.0

Item
{
    id: patientPage

    signal textPageActionSignal(string action)
    signal textPageSignal(string page)

    property int time: 0


    function setTime(t)
    {
        if( t > 23)
             time = 0;
        else if(t < 0)
            time = 23;
        else
            time = t;
    }

    function checkPatients()
    {
        if(patientsToTakeMed.includes(1) && patientsToTakeMed.includes(2))
            patientPage.state = "bothPatients"
        else if(patientsToTakeMed.includes(1))
             patientPage.state ="patient1"
        else if(patientsToTakeMed.includes(2))
             patientPage.state ="patient2"
        else
             patientPage.state ="smd"
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

    property variant patientDetailPages:
    [
        "textPage.qml",
        "textPage.qml",
        "notificationPage.qml"
    ]

    property variant textPageAction:
    [
        "removePatient",
        "rfidTag",
        "notifications"
    ]

    Item
    {

        id: patientDetails

        anchors.top: takeMedicationStatus.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 8

        Repeater
        {
            model: ["REMOVE PATIENT", "RFID TAG", "NOTIFICATIONS"]
            Button
            {
                x: (index % 3) * width
                y: Math.floor(index / 3) * height
                width: parent.width / 3
                height: parent.height
                text: modelData
                onClicked:
                {
                    patientPage.textPageActionSignal.connect(setTextPageAction)
                    patientPage.textPageActionSignal(textPageAction[index])
                    patientPage.textPageSignal.connect(setTextPreviousPage)
                    patientPage.textPageSignal("patientPage.qml")
                    mystackview.replace(patientDetailPages[index]);
                }
            }
        }
    }

    Item
    {
        id: time_menu

        anchors.top: patientDetails.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 8

        property alias timeText:  actualTime.text

        Button
        {
            id: previosTime

            anchors.left: parent.left
            width: parent.width / 8
            height: parent.height

            text: "Previous"
            onClicked:
            {
                setTime(time - 1)
                actualTime.text = time + " Hours : 00 Minutes"
                quantity1.text = smd.getMedicinePrescription(patientID, 1, time + 1)
                quantity2.text = smd.getMedicinePrescription(patientID, 2, time + 1)
            }
        }

        Rectangle
        {
            id: actualTimeRec

            anchors.left: previosTime.right
            width: parent.width * 3 / 4
            height: parent.height

            border.color: "gray"
            border.width: 1
            radius: 5

            Text
            {
                id: actualTime
                anchors.leftMargin: patientDetails.implicitMargin
                anchors.rightMargin: patientDetails.implicitMargin
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: time + " Hours : 00 Minutes"
                font.pixelSize: parent.height / 3
                fontSizeMode: Text.Fit
            }
        }

        Button
        {
            anchors.left: actualTimeRec.right
            width: parent.width / 8
            height: parent.height

            text: "Next"
            onClicked:
           {
                setTime(time + 1)
                actualTime.text = time + " Hours : 00 Minutes"
                quantity1.text = smd.getMedicinePrescription(patientID, 1, time + 1)
                quantity2.text = smd.getMedicinePrescription(patientID, 2, time + 1)

           }
        }
    }

    Item
    {
       id: medication

        anchors.top: time_menu.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 4

        Repeater
        {
            model: ["MEDICATION 1", "MEDICATION 2"]

            Rectangle
            {
                width: parent.width / 2
                height: parent.height

                border.color: "gray"
                border.width: 1
                radius: 5

                x: (index % 2) * width
                y: Math.floor(index / 2) * height

                Text
                {
                    anchors.leftMargin: patientDetails.implicitMargin
                    anchors.rightMargin: patientDetails.implicitMargin
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: modelData
                    fontSizeMode: Text.Fit
                }
            }
        }
    }

    Item
    {
       id: quantity_menu1

        anchors.top: medication.bottom
        anchors.left: parent.left
        width: parent.width / 2
        height: parent.height / 4

        property alias quantity:  quantity1.text

        Button
        {
            id: minus1

            anchors.left: parent.left
            width: parent.width / 4
            height: parent.height

            text: "-"
            onClicked: quantity1.text = smd.updatePrescription(patientID, 1, time + 1, "-")
        }

        Rectangle
        {
            id: quantity1Rec

            anchors.left: minus1.right
            width: parent.width / 2
            height: parent.height

            border.color: "gray"
            border.width: 1
            radius: 10

            Text
            {
                id: quantity1
                anchors.leftMargin: patientDetails.implicitMargin
                anchors.rightMargin: patientDetails.implicitMargin
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                property int quantity : smd.getMedicinePrescription(patientID, 1, time + 1)
                text: quantity;
                fontSizeMode: Text.Fit
            }
        }

        Button
        {
            anchors.left: quantity1Rec.right
            width: parent.width / 4
            height: parent.height
            text: "+"
            onClicked: quantity1.text = smd.updatePrescription(patientID, 1, time + 1, "+")
        }
    }

    Item
    {
       id: quantity_menu2

        anchors.top: medication.bottom
        anchors.left: quantity_menu1.right
        anchors.right: parent.right
        height: parent.height / 4

        property alias quantity:  quantity2.text

        Button
        {
            id: minus2

            anchors.left: parent.left
            width: parent.width / 4
            height: parent.height
            text: "-"
            onClicked: quantity2.text = smd.updatePrescription(patientID, 2, time + 1, "-")
        }

        Rectangle
        {
            id: quantity2Rec

            anchors.left: minus2.right
            width: parent. width/ 2
            height: parent.height

            border.color: "gray"
            border.width: 1
            radius: 10

            Text
            {
                id: quantity2
                anchors.leftMargin: patientDetails.implicitMargin
                anchors.rightMargin: patientDetails.implicitMargin
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                property int quantity : smd.getMedicinePrescription(patientID, 2, time + 1)
                text: quantity;
                fontSizeMode: Text.Fit
            }
        }

        Button
        {
            anchors.left: quantity2Rec.right
            width: parent.width / 4
            height: parent.height
            text: "+"
            onClicked: quantity2.text = smd.updatePrescription(patientID, 2, time + 1, "+")
        }
    }

    Button
    {
        anchors.top: quantity_menu2.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 8
        text: "BACK"
        onClicked: mystackview.replace("mainMenuPage.qml");
    }
}
