import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Controls.Universal
import QtQml

import Donut.DonutQMLPlayer
//import Donut.DonutFramebufferItem

ApplicationWindow
{
    id: window
    visible: true
    minimumWidth: 800
    minimumHeight: 450
    width: 1024
    height: 600
    title: "DonutQMLPlayer"

    property bool isMaterialUI: Utils.environmentVariable("QT_QUICK_CONTROLS_STYLE") == "Material"
    flags: Qt.Window | (isMaterialUI ? Qt.FramelessWindowHint : 0)

    color: SkinColor.windowBackground
    Material.theme: SkinColor.darkMode ? Material.Dark : Material.Light
    Material.accent: Material.Grey
    Universal.theme: SkinColor.darkMode ? Universal.Dark : Universal.Light

    CoverArea
    {
        id: coverArea
        anchors.fill: parent
        titleBar: titleBar
        sideBar: sideBar
        controlBar: controlBar
        contextMenu: contextMenu
        window: window
        framebufferItem: frameItem
    }

    DropArea
    {
        id: dropArea
        anchors.fill: parent
        onDropped:
        {
            var url = drop.urls[0]
            PlaylistModel.addLocalFiles(drop.urls);
        }
    }
    DonutFramebufferItem
    {
        id: frameItem
        width: 800
        height: 600
        visible: true

        Component.onCompleted:
        {
            print("DonutFramebufferItem onCompleted")
            frameItem.update()
            print("width : " + frameItem.width)
        }
    }

    Menu
    {
        id: contextMenu
        width: 160
        padding: 5
        Action { text: qsTr("Open files"); onTriggered: fileDialog.open() }
        MenuSeparator { padding: 0 }
        Action { text: qsTr("Play next"); onTriggered: print("contextMenu : Play next") }
        Action { text: qsTr("Play previous"); onTriggered: print("contextMenu : Play previous") }
        Action { text: qsTr("Mute"); onTriggered: print("contextMenu : Mute") }

        MenuSeparator { padding: 0 }
        Action { text: qsTr("Video options"); onTriggered: sideBar.openVideoOptions() }

        Action { text: qsTr("Playlist"); onTriggered: sideBar.openPlaylist() }
        
        delegate: MenuItem { height: 25 }
    }

    Popup
    {
        id: volumePopup
        width: 40
        height: 120
        background: Rectangle
        {
            anchors.fill: parent
            color: "#2e3137"
            radius: 8
        }
        z: 120
        //Slider
        //{
        //    id: volumeSlider
        //    from: 0
        //    to: 100
        //    value: 100
        //    stepSize: 10
        //    snapMode: Slider.SnapAlways
        //    anchors.fill: parent
        //    orientation: Qt.Vertical
        //}

        Slider
        {
            id: slider
            from: 0
            to: 100
            value: 100
            stepSize: 10
            snapMode: Slider.SnapAlways
            anchors.fill: parent
            orientation: Qt.Vertical
            focusPolicy: Qt.NoFocus
            topPadding: 0
            bottomPadding: 0
            //leftPadding: 0
            //rightPadding: 0

            MouseArea
            {
                anchors.fill: parent
                property bool mouseClicked: false

                drag.target: handleRect
                drag.axis: Drag.YAxis // 修改为 Y 轴拖动
                drag.minimumY: slider.topPadding
                drag.maximumY: slider.height - slider.bottomPadding - handleRect.height

                onClicked: (mouse) =>
                {
                    var clickPos = mouse.y - (handleRect.height / 2)
                    var value = slider.from + (clickPos / (slider.height - slider.topPadding - slider.bottomPadding)) * (slider.to - slider.from)
                    slider.value = Math.max(slider.from, Math.min(value, slider.to))
                }
                onPressed:
                {
                    mouseClicked = true
                    var clickPos = Math.max(slider.topPadding, Math.min(mouse.y - (handleRect.height / 2), slider.height - slider.bottomPadding - (handleRect.height / 2)))
                    slider.value = slider.from + (clickPos / (slider.height - slider.topPadding - slider.bottomPadding)) * (slider.to - slider.from)
                    handleRect.y = clickPos
                }
                onPositionChanged: (mouse) =>
                {
                    if (drag.active)
                    {
                        var clickPos = Math.max(slider.topPadding, Math.min(mouse.y - (handleRect.height / 2), slider.height - slider.bottomPadding - (handleRect.height / 2)))
                        slider.value = slider.from + (clickPos / (slider.height - slider.topPadding - slider.bottomPadding)) * (slider.to - slider.from)
                        handleRect.y = clickPos
                    }
                }
                onReleased: mouseClicked = false
            } // MouseArea

            handle: Rectangle
            {
                id: handleRect
                width: 12
                height: 12
                radius: 6
                color: slider.pressed ? "orange" : "white"
                y: slider.topPadding + (slider.visualPosition * (slider.height - slider.topPadding - slider.bottomPadding - height))
                anchors.horizontalCenter: parent.horizontalCenter
                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    drag.target: handleRect
                    drag.axis: Drag.YAxis
                    drag.minimumY: slider.topPadding
                    drag.maximumY: slider.height - slider.bottomPadding - handleRect.height
                    onEntered: handleRect.color = "orange"
                    onExited: handleRect.color = "white"
                    onPressed: handleRect.color = "orange"
                    onReleased: handleRect.color = "white"
                    onPositionChanged:
                    {
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
                }
            }

            onValueChanged:
            {
                contentRect.height = (slider.visualPosition * (slider.height - slider.topPadding - slider.bottomPadding)) - (handleRect.height / 2)
            }

            Component.onCompleted:
            {
                contentRect.height = track.height
            }
        } // Slider

    }

    // Titlebar, Controlbar and sidebar
    GridLayout
    {
        id: mainLayout
        anchors.fill: parent
        rows: 2
        columns: 2
        rowSpacing: 0
        columnSpacing: 0

        // Titlebar
        Rectangle
        {
            id: titleBar
            color: SkinColor.titlebar
            Layout.fillWidth: true
            Layout.minimumHeight: 28
            Layout.columnSpan: 2
            z: 100

            RowLayout
            {
                anchors.fill: parent
                spacing: 0

                Item
                {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                FontButton
                {
                    id: miniButton
                    Layout.preferredWidth: 26
                    Layout.preferredHeight: 26

                    focusPolicy: Qt.NoFocus
                    txt: String.fromCodePoint(0x2013)

                    onButtonClicked:
                    {
                        window.showMinimized()
                        print("nextButton onClicked")
                    }
                }

                FontButton
                {
                    id: maxButton
                    Layout.preferredWidth: 26
                    Layout.preferredHeight: 26

                    focusPolicy: Qt.NoFocus
                    txt: String.fromCodePoint(0x23f9)

                    onButtonClicked:
                    {
                        window.visibility == Window.Maximized ? window.showNormal() : window.showMaximized()
                        print("nextButton onClicked")
                    }
                }

                FontButton
                {
                    id: closeButton
                    Layout.preferredWidth: 26
                    Layout.preferredHeight: 26

                    focusPolicy: Qt.NoFocus
                    txt: String.fromCodePoint(0x00d7)

                    onButtonClicked:
                    {
                        window.close()
                        print("nextButton onClicked")
                    }
                }
            }



        }

        // Empty item as placeholder
        Item
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        FileDialog
        {
            id: fileDialog
            title: qsTr("Please choose a file")
            fileMode: FileDialog.OpenFiles
            onAccepted: PlaylistModel.addLocalFiles(fileDialog.selectedFiles)
        }

        // Sidebar
        SideBar
        {
            id: sideBar
            Layout.fillHeight: true
            z: 100
            visible: false
            //mpv: mpv
            onOpenFileRequested: fileDialog.open()
            //onOpenUrlRequested: openUrlDialog.visible = true
        }

        // Controlbar
        ControlBar
        {
            id: controlBar

            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.minimumHeight: 50
            Layout.bottomMargin: 1

            duration: 360
            focusPolicy: Qt.ClickFocus
            z: 100
            // isPlaying: mpv.state == MpvObject.VIDEO_PLAYING || mpv.state == MpvObject.TV_PLAYING
            // time: mpv.time
            // duration: mpv.duration
            // onPlayPauseButtonClicked: mpv.state == MpvObject.VIDEO_PLAYING ? mpv.pause() : mpv.play()
            // onStopButtonClicked: mpv.stop()
            // onSettingsButtonClicked: sidebar.openSettings()
            onSidebarButtonClicked: sideBar.openPlaylist()
            onSettingsButtonClicked: sideBar.openVideoOptions()
            // onExplorerButtonClicked: sidebar.openExplorer()
            // onSeekRequested: mpv.seek(time);
            onVolumeButtonClicked:(pos)=>
            {
                print("onVolumeButtonClicked " + pos.x + " : " + pos.y)
                volumePopup.x = pos.x - window.x - 13
                volumePopup.y = pos.y - volumePopup.height - window.y
                volumePopup.visible = true
            }
        } // ControlBar
    } // GridLayout
}
