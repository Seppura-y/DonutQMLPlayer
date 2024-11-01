import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Controls.Universal
import QtQml

import Donut.DonutQMLPlayer 1.0

Window
{
    id: window
    visible: true
    minimumWidth: 800
    minimumHeight: 450
    width: 1280
    height: 720
    title: "DonutQMLPlayer"

    property bool isMaterialUI: Utils.environmentVariable("QT_QUICK_CONTROLS_STYLE") == "Material"
    flags: Qt.Window | (isMaterialUI ? Qt.FramelessWindowHint : 0)

    color: SkinColor.windowBackground
    Material.theme: SkinColor.darkMode ? Material.Dark : Material.Light
    Material.accent: Material.Grey
    Universal.theme: SkinColor.darkMode ? Universal.Dark : Universal.Light

    DonutQMLAVManager
    {
        id: qmlAVManager

        Component.onCompleted:
        {
            console.log("DonutQMLAVManager initialized")
        }

        Connections
        {
            target: qmlAVManager
            onSigUpdateTotalDurationSec: (duration)=>
            {
                controlBar.duration = duration
            }

            onSigUpdateTimePosSec: (value)=>
            {
                controlBar.updateTimePosSec(value)
            }
        }
    }

    DonutScene
    {
        id: glScene
        anchors.fill: parent
        SequentialAnimation on delta_t_
        {
            NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
            NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
            loops: Animation.Infinite
            running: true
        }

        Component.onCompleted:
        {
            glScene.sigItemInitialized()
            //qmlAVManager.initManager()
            qmlAVManager.onVideoViewInitialized(glScene)
        }
    }

    CoverArea
    {
        id: coverArea
        anchors.fill: parent
        titleBar: titleBar
        sideBar: sideBar
        controlBar: controlBar
        contextMenu: contextMenu
        window: window
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

    VolumePopup
    {
        id: volumePopup
        width: 40
        height: 120
        z: 120

        onVolumeValueUpdate: (value)=>
        {
            //print("onVolumValueUpdate : " + value)
            if(value === 0)
            {
                controlBar.isMute = true
            }
            else
            {
                controlBar.isMute = false
            }
            qmlAVManager.setSoundVolume(value)
        }
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

            onPlayingIndexChanged: 
            {
                controlBar.isPlaying = true
            }

            onRotationX:(val)=>
            {
                qmlAVManager.setRotationX(val)
            }
            onRotationY:(val)=>
            {
                qmlAVManager.setRotationY(val)
            }

            onRotationZ:(val)=>
            {
                qmlAVManager.setRotationZ(val)
            }
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
            //focusPolicy: Qt.ClickFocus
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

            //onVolumeButtonClicked:(value)=>
            //{
            //    volumePopup.setValue(value)
            //}

            onVolumeButtonMute:
            {
                volumePopup.setMute()
            }

            onVolumeButtonEntered: (entered, pos)=>
            {
                if(entered)
                {
                    volumePopup.isAboutToClose = false
                }
                if(entered)
                {

                    volumePopup.x = pos.x - window.x - 13
                    volumePopup.y = pos.y - volumePopup.height - window.y
                    
                    volumePopup.visible = true
                }
                else if(!entered)
                {
                    volumePopup.isAboutToClose = true
                }
            }

            onSpeedUpButtonClicked:(rate)=>
            {
                qmlAVManager.setPlaybackRate(rate);
            }

            onSpeedDownButtonClicked:(rate)=>
            {
                qmlAVManager.setPlaybackRate(rate);
            }

            onSeekRequested: (value)=>
            {
                qmlAVManager.onSeekingTimePos(value)
            }

            onPlayPauseButtonClicked: (checked)=>
            {
                qmlAVManager.setPlayOrPause(!checked)
            }

            onPlayModeButtonClicked:
            {
                qmlAVManager.setPlayMode()
            }
        } // ControlBar
    } // GridLayout


    Component.onCompleted:
    {
        //qmlAVManager.onVideoViewInitialized(glScene)

        glScene.setQMLAvManager(qmlAVManager)
    }
}
