import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Control
{
    id: controlBar

    
    FontLoader
    {
        id: fontAwesome
        source: "qrc:/fonts/Font Awesome 6 Pro-Light-300.otf"
    }

    background: Rectangle
    {
        implicitHeight: 50
        color: SkinColor.controlbar
        //color: "lightgreen"
    }

    signal playPauseButtonClicked()
    signal previousButtonClicked()
    signal nextButtonClicked()
    signal stopButtonClicked()
    signal speedUpButtonClicked()
    signal speedDownButtonClicked()
    signal playModeButtonClicked()
    signal volumeButtonClicked()
    signal settingsButtonClicked()
    signal fullscreenButtonClicked()
    signal sidebarButtonClicked()

    property bool isPlaying: false
    property double playbackSpeed: 1.0
    property int duration: 0
    property int currentTime: 0

    function toHHMMSS(seconds)
    {
        var hours = Math.floor(seconds / 3600);
        seconds -= hours*3600;
        var minutes = Math.floor(seconds / 60);
        seconds -= minutes*60;

        if (hours   < 10) {hours   = "0"+hours;}
        if (minutes < 10) {minutes = "0"+minutes;}
        if (seconds < 10) {seconds = "0"+seconds;}
        return hours+':'+minutes+':'+seconds;
    }

    ColumnLayout
    {
        spacing: 0
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        Slider
        {
            id: slider
            from: 0
            to: 1000
            focusPolicy: Qt.NoFocus
            Layout.fillWidth: true
            Layout.preferredHeight: 12

            MouseArea 
            {
                anchors.fill: parent
                property bool mouseClicked: false

                drag.target: handleRect
                drag.axis: Drag.XAxis
                drag.minimumX: slider.leftPadding
                drag.maximumX: slider.width - slider.rightPadding - handleRect.width

                onClicked: (mouse)=>
                {
                    var clickPos = mouse.x - (handleRect.width / 2)
                    var value = slider.from + (clickPos / (slider.width - slider.leftPadding - slider.rightPadding)) * (slider.to - slider.from)
                    slider.value = Math.max(slider.from, Math.min(value, slider.to))
                }

                onPressed:
                {
                    mouseClicked = true
                    var clickPos = mouse.x - (handleRect.width / 2)
                    var value = slider.from + (clickPos / (slider.width - slider.leftPadding - slider.rightPadding)) * (slider.to - slider.from)
                    slider.value = Math.max(slider.from, Math.min(value, slider.to))
                }

                onPositionChanged:(mouse)=>
                {
                    if (drag.active)
                    {
                        var clickPos = mouse.x - (handleRect.width / 2);
                        var value = slider.from + (clickPos / (slider.width - slider.leftPadding - slider.rightPadding)) * (slider.to - slider.from);
                        slider.value = Math.max(slider.from, Math.min(value, slider.to));
                        handleRect.x = clickPos;
                    }
                }

                onReleased: mouseClicked = false
            }

            handle: Rectangle
            {
                id: handleRect
                width: 12
                height: 12
                radius: 6
                color: slider.pressed ? "orange" : "white"
                x: slider.leftPadding + (slider.visualPosition * (slider.width - slider.leftPadding - slider.rightPadding - width))
                anchors.verticalCenter: parent.verticalCenter
                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true

                    drag.target: handleRect
                    drag.axis: Drag.XAxis
                    drag.minimumX: slider.leftPadding
                    drag.maximumX: slider.width - slider.rightPadding - handleRect.width

                    onEntered: handleRect.color = "orange"
                    onExited: handleRect.color = "white"

                    onPressed: handleRect.color = "orange"

                    onReleased: handleRect.color = "white"

                    onPositionChanged:
                    {
                        if (drag.active)
                        {
                            slider.value = slider.from + (handleRect.x / (slider.width - slider.leftPadding - slider.rightPadding)) * (slider.to - slider.from)
                        }
                    }
                }
            }

            background: Rectangle
            {
                id: track
                height: 6
                anchors.verticalCenter: parent.verticalCenter
                color: "#bdbebf"
                radius: 8
            }

            contentItem: Item
            {
                Rectangle
                {
                    width: slider.visualPosition * track.width + 6
                    height: track.height
                    anchors.verticalCenter: parent.verticalCenter
                    color: "orange"
                    radius: 6
                }
            }

            onPressedChanged:
            {
                if (!pressed)
                {
                    // released
                    seekRequested(value)
                }
            }
        } // Slider


        RowLayout
        {
            anchors.leftMargin: 10
            anchors.rightMargin: 10

            FontButton
            {
                id: prevButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26

                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0xf048)

                onButtonClicked:
                {
                    previousButtonClicked()
                    print("prevButton onClicked")
                }
            }

            FontButton
            {
                id: playPauseButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26

                focusPolicy: Qt.NoFocus
                checkable: true
                txt: String.fromCodePoint(0x25b6)

                onButtonChecked:(isChecked)=>
                {
                    txt = isChecked ? String.fromCodePoint(0x23f8) :
                                    String.fromCodePoint(0x25b6)
                    controlBar.isPlaying = isChecked
                }
            }

            FontButton
            {
                id: nextButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26

                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0xf051)

                onButtonClicked:
                {
                    nextButtonClicked()
                    print("nextButton onClicked")
                }
            }

            FontButton
            {
                id: stopButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26

                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0x23f9)

                onButtonClicked:
                {
                    stopButtonClicked()
                    print("stopButton onClicked")
                }
            }

            FontButton
            {
                id: speedUpButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26

                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0x23EA)

                onButtonClicked:
                {
                    speedUpButtonClicked()
                    print("speedUpButton onClicked")
                }
            }

            Label
            {
                id: speedText
                anchors.verticalCenter: parent.verticalCenter
                color: "silver"
                font.family: "Consolas"
                font.pixelSize: 16
                text: "x" + Number(playbackSpeed).toFixed(2)
            }

            FontButton
            {
                id: speedDownButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26

                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0x23E9)

                onButtonClicked:
                {
                    speedDownButtonClicked()
                    print("speedDownButton onClicked")
                }
            }
        
            Item
            {
                id: durationLabel

                Layout.fillWidth: true
                Layout.fillHeight: true

                Rectangle
                {
                    anchors.fill: parent
                    Text
                    {
                        id: durationText
                        anchors.centerIn: parent
                        color: "silver"
                        font.family: "Consolas"
                        font.pixelSize: 16
                        horizontalAlignment: Text.AlignCenter
                        verticalAlignment: Text.AlignCenter
                        text: toHHMMSS(controlBar.currentTime) + " / " + toHHMMSS(controlBar.duration)
                    }

                    color: "transparent"
                }
            }
            
            FontButton
            {
                id: playModeButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26

                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0xF364)

                onButtonClicked:
                {
                    playModeButtonClicked()
                    print("playModeButton onClicked")
                }
            }

            FontButton
            {
                id: volumeButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26

                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0xf028)

                onButtonClicked:
                {
                    volumeButtonClicked()
                    print("volumeButton onClicked")
                }
            }
            

            FontButton {
                id: settingsButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26
                
                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0x2699)

                onButtonClicked:
                {
                    settingsButtonClicked()
                    print("settingsButton onClicked")
                }
            }

            FontButton {
                id: fullscreenButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26
                
                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0xf065)

                onButtonClicked:
                {
                    fullscreenButtonClicked()
                    print("fullscreenButton onClicked")
                }
            }
            
            FontButton {
                id: sidebarButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26
                
                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0xf036)

                onButtonClicked:
                {
                    sidebarButtonClicked()
                    print("sidebarButton onClicked")
                }
            }
	    } // RowLayout
    } // ColumnLayout



}
