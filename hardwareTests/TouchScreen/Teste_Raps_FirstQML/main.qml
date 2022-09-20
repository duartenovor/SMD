import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width:800
    height: 480
    visible: true
    title: qsTr("Hello World")

    FontLoader {
        id: fontId; source: "qrc:Font/OpenSans-Bold.ttf"
    }

    Text{
        id: strText
        anchors.centerIn: parent
        text: qsTr("ESRG 2021/2022")
        color: "black"
        font.pointSize: 35
    }

    Rectangle
    {
        id: button
        width: 400
        height: 100
        y: 300
        anchors{
            horizontalCenter: parent.horizontalCenter
        }

        color: " blue "

        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                console.log ( "Clicked " )
                strText.text= " Clicked "
                strText.color= " black "
            }
        }
    }
}
