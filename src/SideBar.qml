import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Control {
    id: sideBar

    signal openFileRequested()
    signal openUrlRequested()

//    property MpvObject mpv: null

    function openPlaylist()
    {
        layout.currentIndex = 0;
        visible = true;
    }

    function openSettings()
    {
        layout.currentIndex = 1;
        visible = true;
    }

    function openVideoOptions()
    {
        layout.currentIndex = 3;
        visible = true;
    }

    // Width
    property var widths:
    [
        250,  // Playlist
        320,  // Settings
        300,  // Video options
    ]

    // Color settings
    background: Rectangle
    {
        implicitWidth: widths[layout.currentIndex]
        color: SkinColor.sidebar
    }

    StackLayout
    {
        id: layout
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 5

        // Playlist
        Playlist
        {
            id: playlist
            onOpenFileRequested: sideBar.openFileRequested()
            //onOpenUrlRequested: sideBar.openUrlRequested()
        }

        // Settings
//        Settings {
//            id: settings
//        }
//
//        // Video Options
//        VideoOptions {
//            id: videoOptions
//            mpvObject: mpv
//        }
    }
}
