import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 1.0

Item
{
    id: notificationPage

    Connections
    {
        target: smd

        function onSendNotification(text) {
            notificationText.text = text
        }
        function onPreviousButton(visible) {
            previousNotification.visible = visible
        }
        function onNextButton(visible) {
            nextNotifcation.visible = visible
        }
        function onRemoveButton(visible) {
                removeButton.visible = visible
        }
    }

    function checkPatients()
    {
        if(patientsToTakeMed.includes(1) && patientsToTakeMed.includes(2))
            notificationPage.state = "bothPatients"
        else if(patientsToTakeMed.includes(1))
             notificationPage.state = "patient1"
        else if(patientsToTakeMed.includes(2))
             notificationPage.state = "patient2"
        else
             notificationPage.state = "smd"
    }

    Item
    {
        Component.onCompleted:
        {
            checkPatients()
            smd.getNotification(patientID)
        }
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
                        onClicked:smd.supplyMedicationDecision(2)
                    }
                }
            }
        }
    }

    Item
    {
        id: removeButton

        anchors.top: takeMedicationStatus.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height * 1 / 8


        Button
        {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Remove Notification"
            onClicked: smd.removeNotification()
        }

    }

    Item
    {
        id: notificationsLayer

        anchors.top: removeButton.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height * 5 / 8


        Button
        {
            id : previousNotification
            anchors.top: parent.top
            anchors.left: parent.left
            height: parent.height
            width: parent.width / 8
            text: "Previous"
            onClicked:  smd.decrementNofication()
        }

        Rectangle
        {
            id: notification

            anchors.left: previousNotification.right
            height: parent.height
            width: parent.width * 3 / 4

            border.color: "gray"
            border.width: 1
            radius: 5

            Text
            {
                id: notificationText
                anchors.leftMargin: notification.implicitMargin
                anchors.rightMargin: notification.implicitMargin
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "Notification"
                fontSizeMode: Text.Fit
            }
        }

        Item
        {
            id : nextNotifcation
            anchors.top: parent.top
            anchors.left: notification.right
            height: parent.height
            width: parent.width / 8;
            Button
            {
                width: parent.width
                height: parent.height
                text: "Next"
                onClicked: smd.incrementNofication()
            }
        }
    }


    Button
    {
        anchors.top: notificationsLayer.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 8

        text: "BACK"
        onClicked:
        {
           mystackview.replace("patientPage.qml");
        }
    }

}
