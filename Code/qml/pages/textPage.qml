import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 1.0

Item
{
    id: textPage

    property bool remove: false

    Connections
    {
        target: smd
        function onSendText(text){
           messageText.text = text
        }
        function onShowBack(){
           backButton.visible = true
        }

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
                anchors.leftMargin: smdLayer.implicitMargin
                anchors.rightMargin: smdLayer.implicitMargin
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "SMD";
                fontSizeMode: Text.Fit
            }
        }
    }

    Rectangle
    {
        id: textItem

        anchors.top: takeMedicationStatus.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        height: parent.height * 6 / 8

        border.color: "gray"
        border.width: 1
        radius: 10

        property alias message: messageText.text

        Text
        {
            id: messageText
            anchors.leftMargin: textItem.implicitMargin
            anchors.rightMargin: textItem.implicitMargin
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: ""
            fontSizeMode: Text.Fit
        }


        Component.onCompleted:
        {
            console.log(textPageAction)
            switch (textPageAction)
            {
                case "rfidTag":
                    smd.updateRFID(patientID)
                break
                case "removePatient":
                    smd.removePatient(patientID)
                break
                case "fingerprint":
                    smd.updateFingerprint(caregiverID)
                break
                case "removeCaregiver":
                    remove = smd.removeCaregiver(caregiverRem)
                break
                case "addCaregiver":
                    smd.addCaregiver(caregiverRem)
                break
                case "updatePin":
                    smd.updatePin(caregiverID,newPin)
            }
        }
    }

    Item
    {
        id: backButton

        anchors.top: textItem.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height  / 8

        visible: false

        Button
        {
            width: parent.width
            height: parent.height
            text: " Back"
            onClicked:
            {
                if(remove && caregiverID == caregiverRem)
                {
                    smd.caregiverState = 0
                    mystackview.replace("initialPage.qml");
                }
                else
                    mystackview.replace(textPreviousPage);
            }
        }
    }
}
