import QtQuick
import QtQuick.Window
import Donut.DonutQMLPlayer

MouseArea
{
	id: mouseArea

	property bool autoHideBars: false
	property var contextMenu
	property var controlBar
	property var sideBar
	property var titleBar
	property var window

	property bool isMaterialUI: Utils.environmentVariable("QT_QUICK_CONTROLS_STYLE") == "Material"
	hoverEnabled: true
	acceptedButtons: Qt.LeftButton | Qt.RightButton

	onClicked: (mouse)=>
	{
		if (mouse.button === Qt.LeftButton && !sideBar.contains(sideBar.mapFromItem(mouseArea, mouse.x, mouse.y)))
		{
			sideBar.visible = false
		}

		if (mouse.button === Qt.RightButton)
		{
			contextMenu.x = mouse.x
			contextMenu.y = mouse.y
			contextMenu.open()
		}
	}
}