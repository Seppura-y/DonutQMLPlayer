import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Button
{
    id: button
    //focusPolicy: Qt.NoFocus
    property string txt: String.fromCodePoint(0x00)
    hoverEnabled: true
    FontLoader
    {
        id: fontAwesome
        source: "qrc:/fonts/Font Awesome 6 Pro-Light-300.otf"
    }

    background: Rectangle
    {
        id: btnRect
        anchors.fill: parent
        radius: 8
        color: "#2d2d30"
        Text
        {
            id: btnText
            anchors.centerIn: parent
            text: button.txt
            font.family: fontAwesome.name
            font.pointSize: 16
            color: "silver"
        }
    }// Rectangle

    MouseArea
    {
        anchors.fill: parent
        hoverEnabled: true
        onEntered:
        {
            btnRect.color = "dimgray"
            btnText.color = "orange"
            print("onEntered")
        }

        onPressed:
        {
            btnRect.color = "#FF8033"
            btnText.color = "white"
            print("onPressed")
        }

        onExited:
        {
            btnRect.color = "#2d2d30"
            btnText.color = "silver"
            print("onExited")
        }

        onReleased:
        {
            btnRect.color = "dimgray"
            btnText.color = "silver"
            print("onReleased")
        }

    }

    onToggled:
    {
        if(checked)
        {
            btnRect.color = "#1e1e1e"
            btnText.color = "orange"
        }
    }

}// Button
