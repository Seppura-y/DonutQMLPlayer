pragma Singleton

import QtQuick
import QtQml

QtObject {
    // Palette object
    property var sysPalette: SystemPalette { id: sysPalette; colorGroup: SystemPalette.Active }

    // theme from settings, 0 == Classic, 1 == Material, 2 == Win10
    property int theme: 1

    // Dark mode, classic theme must follow system's settings
    property bool darkModeSet: true
    //property bool darkMode: theme === 0 ? (sysPalette.window.hsvValue < 0.3) : darkModeSet
    property bool darkMode:  (sysPalette.window.hsvValue < 0.3) 

    // Colors for titlebar
    property color titlebar: darkMode ? "#2e3137" : "#d8e0e0e0"
    property color closeButton: "#fa564d"
    property color maxButton: "#ffbf39"
    property color minButton: "#53cb43"

    // Colors for controlbar and sidebar
    property color controlbar: theme === 0 ? sysPalette.window : darkMode ? "#2e3137" : "#d0e0e0e0"
    property color sidebar: theme === 0 ? sysPalette.window : darkMode ? "#2e3137" : "#f0e0e0e0"

    // Colors for window background
    property color windowBackground: darkMode || theme === 0 ? "black" : "#fafafa"

    // Colors for listView
    property color listItemHovered: darkMode ? "#888888" : "#eeeeee"
    property color listItemSelected: darkMode ? "steelblue" : "lightsteelblue"
    property color listItemCurrentActive: darkMode ? "grey" : "lightgrey"

    // Colors for file dialog
    property color fileItem: darkMode ? "grey" : "lightgrey"
    property color folderItem: darkMode ? "darkgoldenrod" : "wheat"


    Component.onCompleted:
    {
        print("Singleton onCompleted")
        console.log("sysPalette.colorGroup:", sysPalette.colorGroup);
        console.log("sysPalette.window:", sysPalette.window);
    }

}
