import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Button
{
    id: root
    property string txt: String.fromCodePoint(0x00)
    hoverEnabled: true

    signal buttonChecked(bool isChecked)
    signal buttonClicked()

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
            text: root.txt
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
            if(!checked)
            {
                btnRect.color = "dimgray"
                btnText.color = "orange"
            }
            
        }

        onPressed:
        {
            if(!checked)
            {
                btnRect.color = "#FF8033"
                btnText.color = "white"
            }
        }

        onExited:
        {
            if(!checked)
            {
                btnRect.color = "#2d2d30"
                btnText.color = "silver"
            }
        }

        onReleased:
        {
            btnRect.color = "dimgray"
            btnText.color = "silver"
            print("onReleased")
        }

        onClicked:
        {
            if(root.checkable)
            {
                root.toggle()
            }
            buttonClicked()
        }
    }

    onCheckedChanged:
    {
        if(checked)
        {
            btnRect.color = "#1e1e1e"
            btnRect.border.width=1
            btnRect.border.color= "orange"
            btnText.color = "orange"
        }
        else
        {
            btnRect.color = "dimgray"
            btnRect.border.width=0
            btnText.color = "orange"
        }
        print("onCheckedChanged" + checked)
        buttonChecked(checked)
    }


}// Button
