import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts


Popup
{
    id: volumePopup
    width: 40
    height: 120
    z: 120
    
    closePolicy: Popup.NoAutoClose
    
    property bool isAboutToClose: false
    property bool isMute: false
    property int upperValue: 100
    property int lowerValue: 0
    property int volumeValue: 20
    
    
    signal volumeValueUpdate(int value)
    
    function setBounds(upper, lower)
    {
        lowerValue = lower
        upperValue = upper

        slider.from = lower
        slider.to = upper
    }
    
    function setValue(value)
    {
        print("setValue")
        //slider.value = slider.to * (value / upperValue)
        volumeValue = value
        slider.value = slider.to * (1.0 - volumeValue / upperValue)

        //slider.value = value
    }

    function getValue()
    {
        return slider.visualPosition * upperValue
    }
    
    function setMute()
    {
        if(slider.value != slider.from && slider.value != slider.to)
        {
            volumeValue = slider.visualPosition * upperValue
            slider.value = slider.to
        }
        else if(slider.value === slider.from)
        {
            volumeValue = slider.visualPosition * upperValue
            slider.value = slider.to
        }
        else if(slider.value === slider.to)
        {
            slider.value = slider.to * (1.0 - volumeValue / upperValue)
        }
    }
    
    
    background: Rectangle
    {
        anchors.fill: parent
        color: "#2e3137"
        radius: 8
    
        MouseArea
        {
            anchors.fill: parent
            hoverEnabled: true
            onEntered:
            {
                volumePopup.isAboutToClose = false
            }
            onPositionChanged:(value)=> 
            {
                volumePopup.isAboutToClose = false
            }
            onExited: 
            {
                volumePopup.isAboutToClose = true
            }
        }
    }
    
    
    
    Slider
    {
        id: slider
        from: 0
        to: 100
        value: 100
        stepSize: 1
        snapMode: Slider.SnapAlways
        anchors.fill: parent
        orientation: Qt.Vertical
        //focusPolicy: Qt.NoFocus
        topPadding: 0
        bottomPadding: 0
        //leftPadding: 0
        //rightPadding: 0
    
        property int upperValue: 100
        property int lowerValue: 0
    
        MouseArea
        {
            anchors.fill: parent
            //hoverEnabled: true
            property bool mouseClicked: false
    
            drag.target: handleRect
            drag.axis: Drag.YAxis // 修改为 Y 轴拖动
            drag.minimumY: slider.topPadding
            drag.maximumY: slider.height
            //drag.maximumY: slider.height - slider.bottomPadding - handleRect.height
    
            onClicked: (mouse) =>
            {
                var clickPos = mouse.y - (handleRect.height / 2)
                var value = slider.from + (clickPos / (slider.height - slider.topPadding - slider.bottomPadding)) * (slider.to - slider.from)
                slider.value = Math.max(slider.from, Math.min(value, slider.to))
            }
            onPressed: (mouse)=>
            {
                mouseClicked = true
    
                var clickPos = Math.max(
                                        slider.topPadding,
                                        Math.min(
                                                mouse.y - (handleRect.height / 2),
                                                slider.height - slider.bottomPadding - (handleRect.height / 2)
                                        )
                )
    
                slider.value = slider.from + (clickPos / (slider.height - slider.topPadding - slider.bottomPadding)) * (slider.to - slider.from)
                if(slider.visualPosition > 0.5)
                    handleRect.y = clickPos
                else
                    handleRect.y = clickPos
            }
            onPositionChanged: (mouse) =>
            {
                volumePopup.isAboutToClose = false
                if (drag.active)
                {
                    var clickPos = Math.max(slider.topPadding,
                        Math.min(
                            mouse.y - (handleRect.height / 2),
                            slider.height - slider.bottomPadding
                            //slider.height - slider.bottomPadding - (handleRect.height / 2)
                        )
                    )
                    slider.value = slider.from + (clickPos / (slider.height - slider.topPadding - slider.bottomPadding)) * (slider.to - slider.from)
                    
                    if(slider.visualPosition > 0.5)
                        handleRect.y = clickPos
                    else
                        handleRect.y = clickPos - (handleRect.height / 2)

                }
            }
            onEntered:
            {
                volumePopup.isAboutToClose = false
            }
            onExited:
            {
                volumePopup.isAboutToClose = true
            }
    
            onReleased: mouseClicked = false
        } // MouseArea
    
        handle: Rectangle
        {
            id: handleRect
            width: 12
            height: 12
            radius: 6
            y: slider.topPadding + (slider.visualPosition * (slider.height - slider.topPadding - slider.bottomPadding - height))

            anchors.horizontalCenter: parent.horizontalCenter

            color: slider.pressed ? "orange" : "white"
            
            MouseArea
            {
                anchors.fill: parent
                hoverEnabled: true
                drag.target: handleRect
                drag.axis: Drag.YAxis
                drag.minimumY: slider.topPadding
                drag.maximumY: slider.height - slider.bottomPadding - handleRect.height
                onEntered:
                {
                    handleRect.color = "orange"
                    volumePopup.isAboutToClose = false
                }
                onExited:
                {
                    handleRect.color = "white"
                    //volumePopup.isAboutToClose = true
                }
                onPressed: handleRect.color = "orange"
                onReleased: handleRect.color = "white"
                onPositionChanged:
                {
                    volumePopup.isAboutToClose = false
                    if (drag.active)
                    {
                        slider.value = slider.from + (handleRect.y / (slider.height - slider.topPadding - slider.bottomPadding)) * (slider.to - slider.from)
                    }
                }
            }
        } // handle: Rectangle
    
        background: Rectangle
        {
            id: track
            width: 6
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#bdbebf"
            radius: 8
            MouseArea
            {
                anchors.fill: parent
                hoverEnabled: true
                onEntered:
                {
                    volumePopup.isAboutToClose = false
                }
                onExited:
                {
                    //volumePopup.isAboutToClose = true
                }
    
                onPositionChanged:
                {
                    volumePopup.isAboutToClose = false
                }
            }
        }
    
        contentItem: Item
        {
            Rectangle
            {
                id: contentRect
                x: track.x - track.width
                width: track.width
                // 为解决初始化值时，contentRectheight为负值，在onCompleted中设置把height的赋值移到onValueChanged中
                //height: (slider.visualPosition * (slider.height - slider.topPadding - slider.bottomPadding)) - (handleRect.height / 2)
                anchors.bottom: parent.bottom
                color: "orange"
                radius: 8
                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered:
                    {
                        volumePopup.isAboutToClose = false
                    }
                    onExited:
                    {
                        //volumePopup.isAboutToClose = true
                    }
                    onPositionChanged:
                    {
                        volumePopup.isAboutToClose = false
                    }
                }
            }
        }
    
        onValueChanged:
        {
            //print("main slider onValueChanged : " + slider.visualPosition)
            if(slider.visualPosition > 0.5)
            {
                contentRect.height = (slider.visualPosition * slider.height) - (handleRect.height / 2)
                handleRect.y = slider.height - contentRect.height - (handleRect.height / 2)
            }
            else
            {
                contentRect.height = (slider.visualPosition * slider.height) - (handleRect.height / 2)
                handleRect.y = slider.height - contentRect.height - (handleRect.height / 2)
                if(slider.visualPosition === 0)
                {
                    handleRect.y = slider.height - (handleRect.height / 2)
                }
            }
            volumePopup.volumeValueUpdate(slider.visualPosition * upperValue)
        }
    
        Component.onCompleted:
        {
            contentRect.height = track.height
            timer.start()
            //slider.value = slider.to * (1.0 - volumeValue / upperValue)
            volumePopup.setValue(20)
        }
    } // Slider
    
    Timer
    {
        id: timer
        interval: 500
        onTriggered:
        {
            volumePopup.close()
            visible = false
            //print("volume timer visible = false")
        }
    }
    
    Connections
    {
        target: volumePopup
        function onIsAboutToCloseChanged()
        {
            if(volumePopup.isAboutToClose)
            {
                timer.start()
                //print("onIsAboutToCloseChanged timer start")
            }
            else
            {
                visible = true
                timer.stop()
                //print("onIsAboutToCloseChanged visible = true")
            }
        }
    }
} // Popup