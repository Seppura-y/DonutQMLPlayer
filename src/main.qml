import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Controls.Universal
import QtQml

import Donut.DonutQMLPlayer

Window
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
    Popup
    {
        id: volumePopup
        width: 40
        height: 120
        Slider
        {
            id: volumeSlider
            from: 0
            to: 100
            value: 100
            stepSize: 10
            snapMode: Slider.SnapAlways
            anchors.fill: parent
            orientation: Qt.Vertical
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
            id: titlebar
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
            id: sidebar
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
            duration: 360
            focusPolicy: Qt.ClickFocus
            z: 100
            // isPlaying: mpv.state == MpvObject.VIDEO_PLAYING || mpv.state == MpvObject.TV_PLAYING
            // time: mpv.time
            // duration: mpv.duration
            // onPlayPauseButtonClicked: mpv.state == MpvObject.VIDEO_PLAYING ? mpv.pause() : mpv.play()
            // onStopButtonClicked: mpv.stop()
            // onSettingsButtonClicked: sidebar.openSettings()
            onSidebarButtonClicked: sidebar.openPlaylist()
            onSettingsButtonClicked: sidebar.openVideoOptions()
            // onExplorerButtonClicked: sidebar.openExplorer()
            // onSeekRequested: mpv.seek(time);
            onVolumeButtonClicked: {
                volumePopup.x = mpv.mapFromItem(volumeButton, 0, 0).x;
                volumePopup.y = mpv.mapFromItem(volumeButton, 0, 0).y - volumePopup.height;
                volumePopup.visible = true;
            }
        }
    }
}
