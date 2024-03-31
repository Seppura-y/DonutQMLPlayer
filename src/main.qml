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
        Rectangle {
            id: titlebar
            // color: SkinColor.titlebar
            Layout.fillWidth: true
            Layout.minimumHeight: 28
            Layout.columnSpan: 2
            z: 100
            // Button {
            //     id: closeButton
            //     width: 14
            //     height: 14
            //     anchors.verticalCenter: parent.verticalCenter
            //     anchors.right: parent.right
            //     anchors.rightMargin: 8
            //     background: Rectangle { color: SkinColor.closeButton; radius: 7; anchors.fill: parent }
            //     onClicked: window.close()
            // }
            // Button {
            //     id: maxButton
            //     width: 14
            //     height: 14
            //     anchors.verticalCenter: parent.verticalCenter
            //     anchors.right: closeButton.left
            //     anchors.rightMargin: 6
            //     background: Rectangle { color: SkinColor.maxButton; radius: 7; anchors.fill: parent }
            //     onClicked: window.visibility == Window.Maximized ? window.showNormal() : window.showMaximized()
            // }
            // Button {
            //     id: minButton
            //     width: 14
            //     height: 14
            //     anchors.verticalCenter: parent.verticalCenter
            //     anchors.right: maxButton.left
            //     anchors.rightMargin: 6
            //     background: Rectangle { color: SkinColor.minButton; radius: 7; anchors.fill: parent }
            //     onClicked: window.showMinimized()
            // }
        }

        // Empty item as placeholder
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        // Sidebar
        // Sidebar {
        //     id: sidebar
        //     Layout.fillHeight: true
        //     z: 100
        //     visible: false
        //     mpv: mpv
        //     onOpenFileRequested: fileDialog.open()
        //     onOpenUrlRequested: openUrlDialog.visible = true
        // }

        // Controlbar
        ControlBar {
            id: controlBar
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.minimumHeight: 50
            focusPolicy: Qt.ClickFocus
            z: 100
            // isPlaying: mpv.state == MpvObject.VIDEO_PLAYING || mpv.state == MpvObject.TV_PLAYING
            // time: mpv.time
            // duration: mpv.duration
            // onPlayPauseButtonClicked: mpv.state == MpvObject.VIDEO_PLAYING ? mpv.pause() : mpv.play()
            // onStopButtonClicked: mpv.stop()
            // onSettingsButtonClicked: sidebar.openSettings()
            // onSidebarButtonClicked: sidebar.openPlaylist()
            // onExplorerButtonClicked: sidebar.openExplorer()
            // onSeekRequested: mpv.seek(time);
            // onVolumeButtonClicked: {
            //     volumePopup.x = mpv.mapFromItem(volumeButton, 0, 0).x;
            //     volumePopup.y = mpv.mapFromItem(volumeButton, 0, 0).y - volumePopup.height;
            //     volumePopup.visible = true;
            // }
        }
    }
}
