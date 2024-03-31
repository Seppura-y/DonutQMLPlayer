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
        implicitHeight: 40
        color: SkinColor.controlbar
        //color: "lightgreen"
    }

    signal playPauseButtonClicked()

    property bool isPlaying: false

    RowLayout
    {
        spacing: 10
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10

        ImageButton
        {
            id: playPauseButton
            image: isPlaying?
                    (SkinColor.darkMode ? "../images/pause_lightgrey.png" : "../images/pause_grey.png") :
                    (SkinColor.darkMode ? "../images/play_lightgrey.png" : "../images/play_grey.png")

            hoverImage: isPlaying ?
                    (SkinColor.darkMode ? "../images/pause_lightgrey_on.png" : "../images/pause_grey_on.png") :
                    (SkinColor.darkMode ? "../images/play_lightgrey_on.png" : "../images/play_grey_on.png")

            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            onClicked: playPauseButtonClicked()
        }

        ImageButton {
            id: stopButton
            image: SkinColor.darkMode ? "../images/stop_lightgrey.png" : "../images/stop_grey.png"
            hoverImage: SkinColor.darkMode ? "../images/stop_lightgrey_on.png" : "../images/stop_grey_on.png"
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            onClicked: stopButtonClicked()
        }

        ImageButton {
            id: volumeButton
            image: SkinColor.darkMode ? "../images/volume_lightgrey.png" : "../images/volume_grey.png"
            hoverImage: SkinColor.darkMode ? "../images/volume_lightgrey_on.png" : "../images/volume_grey_on.png"
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            onClicked: volumeButtonClicked()
        }

//        Button
//        {
//            focusPolicy: Qt.NoFocus
//            Layout.preferredWidth: 26
//            Layout.preferredHeight: 26
//
//            Rectangle
//            {
//                id: btnRect
//                anchors.fill: parent
//                color: "#2d2d30"
//                radius: 8
//
//                Text
//                {
//                    id: btnText
//                    anchors.centerIn: parent
//                    text: String.fromCodePoint(0xf028)
//                    font.family: fontAwesome.name // 使用加载的字体
//                    font.pointSize: 16 // 设置字体大小
//                    color: "silver"
//                }
//                MouseArea
//                {
//                    anchors.fill: parent
//                    hoverEnabled: true
//                    onEntered:
//                    {
//                        btnRect.color = "dimgray"
//                        btnText.color = "orange"
//                    }
//
//                    onPressed:
//                    {
//                        btnRect.color = "#FF8033"
//                        btnText.color = "white"
//                    }
//
//                    onExited:
//                    {
//                        btnRect.color = "#2d2d30"
//                        btnText.color = "silver"
//                    }
//
//                    onReleased:
//                    {
//                        btnRect.color = "dimgray"
//                        btnText.color = "orange"
//                    }
//
//                }
//            }// Rectangle
//        }// Button
        
        FontButton
        {
            Layout.preferredWidth: 26
            Layout.preferredHeight: 26
            txt: String.fromCodePoint(0xf028)
        }

        Label {
            id: timeText
            text: toHHMMSS(time)
        }
        
        Slider {
            id: slider
            from: 0
            to: 1000
            focusPolicy: Qt.NoFocus
            Layout.fillWidth: true
            Layout.preferredHeight: 24

            MouseArea
            {
                anchors.fill: parent
                property bool mouseClicked: false

                drag.target: handleRect
                drag.axis: Drag.XAxis
                drag.minimumX: slider.leftPadding
                drag.maximumX: slider.width - slider.rightPadding - handleRect.width


                onClicked:(mouse)=>
                {
                    var clickPos = mouse.x - (handleRect.width / 2)
                    var value = slider.from + (clickPos / slider.width) * (slider.to - slider.from)
                    slider.value = Math.max(slider.from, Math.min(value, slider.to))

                    handleRect.x = clickPos
                }

                onPressed: mouseClicked=true

                onPositionChanged:(mouse)=>
                {
                    if(drag.active)
                    {
                        slider.value = slider.from + (handleRect.x / slider.width) * (slider.to - slider.from);
                    }


                    
                }

                onReleased: mouseClicked = false
            }


            handle: Rectangle
            {
                id: handleRect
                width: 16
                height: 16
                radius: 8
                color: slider.pressed ? "orange" : "white"
                anchors.verticalCenter: parent.verticalCenter
                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true

                    drag.target: handleRect
                    drag.axis: Drag.XAxis
                    drag.minimumX: slider.leftPadding
                    drag.maximumX: slider.width - slider.rightPadding - handleRect.width

                    //drag.minimumX: 0
                    //drag.maximumX: slider.width

                    onEntered: handleRect.color="orange"
                    onExited: handleRect.color="white"

                    onPressed: handleRect.color="orange"

                    onReleased: handleRect.color="white"

                    onPositionChanged:
                    {
                        if(drag.active)
                        {
                            slider.value = slider.from + (handleRect.x / slider.width) * (slider.to - slider.from);
                        }
                    }
                }
            }

            background: Rectangle
            {
                id: track
                height: 10
                anchors.verticalCenter: parent.verticalCenter
                color: "#bdbebf"
                radius: 10 // 滑槽的圆角半径
            }

            contentItem:
            Item
            {
                Rectangle
                {
                    width: slider.visualPosition * track.width + 10
                    height: track.height
                    anchors.verticalCenter: parent.verticalCenter
                    color: "orange" // 滑过区域的颜色
                    radius: 10
                }

            }




            onPressedChanged: {
                if (!pressed)  // released
                    seekRequested(value);
            }
        }

        Label {
            id: durationText
            text: toHHMMSS(duration)
        }

        ImageButton {
            id: explorerButton
            image: SkinColor.darkMode ? "../images/net_lightgrey.png" : "../images/net_grey.png"
            hoverImage: SkinColor.darkMode ? "../images/net_lightgrey_on.png" : "../images/net_grey_on.png"
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            onClicked: explorerButtonClicked()
        }

        ImageButton {
            id: settingsButton
            image: SkinColor.darkMode ? "../images/settings_lightgrey.png" : "../images/settings_grey.png"
            hoverImage: SkinColor.darkMode ? "../images/settings_lightgrey_on.png" : "../images/settings_grey_on.png"
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            onClicked: settingsButtonClicked()
        }

        ImageButton {
            id: sidebarButton
            image: SkinColor.darkMode ? "../images/playlist_lightgrey.png" : "../images/playlist_grey.png"
            hoverImage: SkinColor.darkMode ? "../images/playlist_lightgrey_on.png" : "../images/playlist_grey_on.png"
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            onClicked: sidebarButtonClicked()
        }
	}

}
