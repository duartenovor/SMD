import QtQuick 2.0
import QtQuick.Controls 2.12

Item
{
    id: initialPage

    signal pinActionSignal(string action)
    signal pinPageSignal(string page)

    Button
    {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        width: parent.width
        height: parent.height

        palette
        {
                button: "black"
        }

        onClicked:
        {
            initialPage.pinPageSignal.connect(setPinPreviousPage)
            initialPage.pinPageSignal("initialPage.qml")
            initialPage.pinActionSignal.connect(setPinNextAction)
            initialPage.pinActionSignal("ID")
            mystackview.replace("pinPage.qml")
        }
    }
}
