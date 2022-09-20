import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 1.0

Item
{
    id: pinPage

    signal pinActionSignal(string action)
    signal caregiverIdSignal(string id)
    signal newPinSignal(string pin)
    signal textPageSignal(string page)
    signal textActionSignal(string action)

    property variant loginPages:
    [
        "initialPage.qml",
        "mainMenuPage.qml"
    ]

    property int valid
    property string str: ""
    property int insertedID


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
        id: dataToInsert
        anchors.top: takeMedicationStatus.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height  / 8

        border.color: "gray"
        border.width: 1
        radius: 10

        property alias message: messageText.text

        Text
        {
            id: messageText
            anchors.leftMargin: dataToInsert.implicitMargin
            anchors.rightMargin: dataToInsert.implicitMargin
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "Caregiver " + pinNextAction
            fontSizeMode: Text.Fit
        }
    }


    Rectangle
    {
        id: inputPin
        anchors.top: dataToInsert.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height * 2 / 8
        border.color: "white"
        border.width: 1
        color: "#46a2da"
        Text
        {
            id: resultText
            anchors.leftMargin: keyboard.implicitMargin
            anchors.rightMargin: keyboard.implicitMargin
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: str
            color: "white"
            font.pixelSize: parent.height / 3
            fontSizeMode: Text.Fit
        }
   }

    Item
    {
        id: keyboard
        anchors.top: inputPin.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height * 3 / 8

        Repeater
        {
            model: ["7", "8", "9", "4", "5", "6", "1", "2", "3", "DEL", "0", "OK"]
            Button
            {
                x: (index % 3) * width
                y: Math.floor(index / 3 ) * height
                width: parent.width / 3
                height: parent.height / 4
                text: modelData
                onClicked:
                {
                    switch(text)
                    {
                        case "DEL":
                            str = str.slice(0,- 1)
                        break;
                        case "OK": 
                            switch(pinNextAction)
                            {
                                case "ID":
                                    pinPage.pinActionSignal.connect(setPinNextAction)
                                    pinPage.pinActionSignal("PIN")
                                    insertedID = parseInt(str)
                                    str = ""
                                    messageText.text = "Caregiver " + pinNextAction
                                break
                                case "PIN":
                                    if(str.length == 4)
                                    {
                                        switch(pinPreviousPage)
                                        {
                                            case "initialPage.qml":
                                                valid = smd.checkCaregiver(insertedID, str)
                                                pinPage.caregiverIdSignal.connect(setCaregiverID)
                                                if(valid)
                                                    pinPage.caregiverIdSignal(insertedID)
                                                else
                                                    pinPage.caregiverIdSignal(0)
                                                console.log(valid)
                                                mystackview.replace(loginPages[valid])
                                                break
                                            case "caregiverPage.qml":
                                                pinPage.textPageSignal.connect(setTextPreviousPage)
                                                pinPage.textPageSignal("caregiverPage.qml")
                                                pinPage.textActionSignal.connect(setTextPageAction)
                                                pinPage.textActionSignal("updatePin")
                                                pinPage.newPinSignal.connect(setCaregiverNewPin)
                                                pinPage.newPinSignal(str)
                                                mystackview.replace("textPage.qml")
                                                break
                                        }
                                    }
                            }
                        break
                        default:
                            if(str.length < 4)
                                str += text
                    }
                }
            }
        }
    }

    Item
    {
        id: back

        anchors.top: keyboard.bottom
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
            onClicked: mystackview.replace(pinPreviousPage);
        }
    }
}
