import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Controls.Material
import QtQuick.Controls.Universal
import Qt.labs.settings 1.0 as QSettings

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
}
