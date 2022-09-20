import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 1.0

Item
{
    id: medicationPage

    function checkPatients()
    {
        if(patientsToTakeMed.includes(1) && patientsToTakeMed.includes(2))
            medicationPage.state = "bothPatients"
        else if(patientsToTakeMed.includes(1))
             medicationPage.state ="patient1"
        else if(patientsToTakeMed.includes(2))
             medicationPage.state ="patient2"
        else
             medicationPage.state ="smd"
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

    Item
    {
       id: medicationBox

       anchors.top: takeMedicationStatus.bottom
       anchors.left: parent.left
       anchors.right: parent.right
       height: parent.height * 3 / 8

        Repeater
        {
            model: ["MEDICINE 1", "MEDICINE 2"]
            Rectangle
            {
                width: parent.width / 2
                height: parent.height

                border.color: "gray"
                border.width: 1
                radius: 10

                x: (index % 2) * width
                y: Math.floor(index / 2) * height

                Text
                {
                    anchors.leftMargin: medicationBox.implicitMargin
                    anchors.rightMargin: medicationBox.implicitMargin
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

        anchors.top: medicationBox.bottom
        anchors.left: parent.left
        width: parent.width / 2
        height: parent.height * 3 / 8

       property alias quantity :  quantity1.text

        Button
        {
            id: minus1

            anchors.left: parent.left
            width: parent.width / 4
            height: parent.height
            text: "-"
            onClicked: quantity1.text = smd.updateMedicineQuantity(1, "-")
        }

        Rectangle
        {
            id: quantity1_menu

            anchors.left: minus1.right
            width: parent.width / 2
            height: parent.height

            border.color: "gray"
            border.width: 1
            radius: 10

            Text
            {
                id: quantity1
                anchors.leftMargin: quantity_menu1.implicitMargin
                anchors.rightMargin: quantity_menu1.implicitMargin
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text : smd.getMedicineQuantity(1)
                fontSizeMode: Text.Fit

            }
        }

        Button
        {
            anchors.left: quantity1_menu.right
            width: parent.width / 4
            height: parent.height
            text: "+"
            onClicked: quantity1.text = smd.updateMedicineQuantity(1, "+")
        }
    }

    Item
    {
       id: quantity_menu2

        anchors.top: medicationBox.bottom
        anchors.left: quantity_menu1.right
        anchors.right: parent.right
        height: parent.height * 3 / 8

        property alias quantity :  quantity2.text

        Button
        {
            id: minus2

            anchors.left: parent.left
            width: parent.width / 4
            height: parent.height
            text: "-"
            onClicked: quantity2.text = smd.updateMedicineQuantity(2, "-")
        }

        Rectangle
        {
            id: quantity2_menu

            anchors.left: minus2.right
            width: parent.width / 2
            height: parent.height

            border.color: "gray"
            border.width: 1
            radius: 10

            Text
            {
                id: quantity2
                anchors.leftMargin: quantity_menu2.implicitMargin
                anchors.rightMargin: quantity_menu2.implicitMargin
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text : smd.getMedicineQuantity(2)
                fontSizeMode: Text.Fit
            }
        }

        Button
        {
            anchors.left: quantity2_menu.right
            width: parent.width / 4
            height: parent.height
            text: "+"
            onClicked: quantity2.text = smd.updateMedicineQuantity(2, "+")
        }
    }

    Item
    {
        anchors.top: quantity_menu1.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height / 8

        Button
        {
            width: parent.width
            height: parent.height

            text: "BACK"
            onClicked:
            {
                mystackview.replace("mainMenuPage.qml");
            }
        }
    }
}
