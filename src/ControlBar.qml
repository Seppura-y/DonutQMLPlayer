
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Control
{
    id: controlBar

    enum Playmode
    {
        PlaymodeFileOnce,
        PlaymodeFileLoop,
        PlaymodeListOnce,
        PlaymodeListLoop
    }

    FontLoader
    {
        id: fontAwesome
        source: "qrc:/fonts/Font Awesome 6 Pro-Light-300.otf"
    }

    background: Rectangle
    {
        implicitHeight: 50
        color: SkinColor.controlbar
        radius: 8
        //color: "lightgreen"
    }

    signal playPauseButtonClicked(bool paused)
    signal previousButtonClicked()
    signal nextButtonClicked()
    signal stopButtonClicked()
    signal speedUpButtonClicked(var rate)
    signal speedDownButtonClicked(var rate)
    signal playModeButtonClicked(int mode)
    signal volumeButtonClicked(int value)
    signal volumeButtonMute()
    signal volumeButtonEntered(bool entered, point pos)
    signal settingsButtonClicked()
    signal fullscreenButtonClicked()
    signal sidebarButtonClicked()

    signal seekRequested(double value)

    property bool isPlaying: false
    property bool isMute: false
    property double playbackSpeed: 1.0
    property int duration: 120
    property int currentTime: 0
    property int volume: 20
    property int curPlaymode: ControlBar.Playmode.PlaymodeFileOnce

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

    function updateTimePosSec(seconds)
    {
        if(!slider.mousePressed)
        {
            currentTime = seconds
        }
    }
    
    function setPlaymodeListOnce()
    {
        playModeButton.txt = String.fromCodePoint(0xE0B5)
    }

    function setPlaymodeListLoop()
    {
        playModeButton.txt = String.fromCodePoint(0xF364)
    }

    function setPlaymodeFileOnce()
    {
        playModeButton.txt = String.fromCodePoint(0xE256)
    }

    function setPlaymodeFileLoop()
    {
        playModeButton.txt = String.fromCodePoint(0xF366)
    }

    ColumnLayout
    {
        spacing: 0
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        Slider
        {
            id: slider
            from: 0
            to: duration
            value: currentTime
            focusPolicy: Qt.NoFocus
            Layout.fillWidth: true
            Layout.preferredHeight: 12
            stepSize: 1
            leftPadding: 0

            property bool mousePressed: false

            MouseArea
            {
                anchors.fill: parent
                

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

                onPressed: (mouse)=>
                {
                    slider.mousePressed = true
                    var clickPos = Math.max(slider.leftPadding, Math.min(mouse.x - (handleRect.width / 2), slider.width - slider.rightPadding - (handleRect.width / 2)));
                    slider.value = slider.from + (clickPos / (slider.width - slider.leftPadding - slider.rightPadding)) * (slider.to - slider.from);
                    handleRect.x = clickPos;
                }

                onPositionChanged: (mouse)=>
                {
                    if (drag.active)
                    {
                        var clickPos = Math.max(slider.leftPadding, Math.min(mouse.x - (handleRect.width / 2), slider.width - slider.rightPadding - (handleRect.width / 2)));
                        slider.value = slider.from + (clickPos / (slider.width - slider.leftPadding - slider.rightPadding)) * (slider.to - slider.from);
                        handleRect.x = clickPos;
                    }
                    else
                    {

                    }
                }
                onReleased:
                {
                    seekRequested(slider.value / slider.to)
                    slider.mousePressed = false
                }
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
                    //width: slider.visualPosition * track.width + 6
                    width: (slider.visualPosition * (slider.width - slider.leftPadding - slider.rightPadding)) - (handleRect.width / 2) + 6
                    height: track.height
                    anchors.verticalCenter: parent.verticalCenter
                    color: "orange"
                    radius: 6
                }
            }

            onValueChanged:
            {
                if(!slider.mousePressed)
                {
                    var newPosition = slider.leftPadding + (value - from) / (to - from) * (width - slider.leftPadding - slider.rightPadding);
                    handleRect.x = Math.min(newPosition, width - slider.rightPadding - handleRect.width);
                    currentTime = slider.value
                    //print("slider value : " + slider.value)
                }
                else
                {

                }
            }

            onWidthChanged:
            {
                handleRect.x = slider.leftPadding + (slider.visualPosition * (slider.width - slider.leftPadding - slider.rightPadding - handleRect.width));
            }

            onPressedChanged:
            {
                if (!pressed)
                {
                    // released
                    seekRequested(slider.value / slider.to)
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
                    playPauseButtonClicked(isChecked)
                }

                //onButtonChecked 和 onButtonClicked 相互作用：
                //当按钮被点击时，checked 状态会自动更新，触发 onButtonChecked。
                //在 onButtonClicked 中又手动改变 checked 状态，这会导致额外的 onButtonChecked 触发。
                //onButtonClicked:
                //{
                //    if(controlBar.isPlaying)
                //    {
                //        playPauseButtonClicked(false)
                //        controlBar.isPlaying = false
                //        //playPauseButton.checked = false
                //    }
                //    else
                //    {
                //        playPauseButtonClicked(true)
                //        controlBar.isPlaying = true
                //        //playPauseButton.checked = true
                //    }
                //}
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
                id: speedDownButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26

                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0x23EA)

                onButtonClicked:
                {
                    playbackSpeed -= 0.25;
                    if(playbackSpeed < 0.5)
                    {
                        playbackSpeed = 0.5;
                    }
                    speedDownButtonClicked(playbackSpeed)
                    print("speedDownButton onClicked")
                }
            }

            Label
            {
                id: speedText
                color: "silver"
                font.family: "Consolas"
                font.pixelSize: 16
                text: "x" + Number(playbackSpeed).toFixed(2)
            }

            FontButton
            {
                id: speedUpButton
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26

                focusPolicy: Qt.NoFocus
                txt: String.fromCodePoint(0x23E9)

                onButtonClicked:
                {
                    playbackSpeed += 0.25;
                    if(playbackSpeed > 2.0)
                    {
                        playbackSpeed = 2.0;
                    }
                    speedUpButtonClicked(playbackSpeed)
                    print("speedUpButton onClicked")
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
                        verticalAlignment: Text.AlignVCenter
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
                txt: String.fromCodePoint(0xE256)

                onButtonClicked:
                {
                    curPlaymode = (curPlaymode + 1) % 4
                    switch(curPlaymode)
                    {
                        case ControlBar.Playmode.PlaymodeFileOnce:
                            controlBar.setPlaymodeFileOnce()
                            break;
                        case ControlBar.Playmode.PlaymodeFileLoop:
                            controlBar.setPlaymodeFileLoop()
                            break;
                        case ControlBar.Playmode.PlaymodeListOnce:
                            controlBar.setPlaymodeListOnce()
                            break;
                        case ControlBar.Playmode.PlaymodeListLoop:
                            controlBar.setPlaymodeListLoop()
                            break;
                    }

                    //playModeButtonClicked(int(curPlaymode))
                    playModeButtonClicked(curPlaymode)
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
                    if(isMute)
                    {
                        isMute = false
                        //volumeButtonClicked(volume)
                        volumeButtonMute()
                        txt = String.fromCodePoint(0xf028)
                    }
                    else
                    {
                        isMute = true
                        //volumeButtonClicked(0)
                        volumeButtonMute()
                        txt = String.fromCodePoint(0xf2e2)
                    }

                }


                onButtonEntered: (value)=>
                {
                    var globalPos = volumeButton.mapToGlobal(Qt.point(5, 0))
                    volumeButtonEntered(value, globalPos)
                    //print("volumeButton onMouseEnter " + value)
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

    Connections
    {
        target: controlBar
        function onIsMuteChanged()
        {
            if(isMute)
            {
                volumeButton.txt = String.fromCodePoint(0xf2e2)
            }
            else
            {
                volumeButton.txt = String.fromCodePoint(0xf028)
            }
        }

        function onIsPlayingChanged()
        {
            playPauseButton.txt = isPlaying ? String.fromCodePoint(0x23f8) :
                                    String.fromCodePoint(0x25b6)

            playPauseButton.checked = isPlaying
        }

        function onCurrentTimeChanged()
        {
            slider.value = currentTime
        }
    }

}
