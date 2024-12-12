import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import Donut.DonutQMLPlayer

ScrollView
{
	id:  videoOptionsDialog

	//property bool isMaterialUI: Utils.environmentVariable("QT_QUICK_CONTROLS_STYLE") == "Material"
	property bool isMaterialUI: true
	clip: true

	signal rotationXChanged(int value)
    signal rotationYChanged(int value)
    signal rotationZChanged(int value)

	ColumnLayout
	{
		//Label
		//{
		//	text: qsTr("Video")
		//	font.bold: true
		//}
		//
		//RowLayout
		//{
		//	Button
		//	{
		//		text: qsTr("Enable")
		//		onClicked: print("video Enabled")
		//	}
		//
		//	Button
		//	{
		//		text: qsTr("Disable")
		//		onClicked: print("video Disabled")
		//	}
		//}

		Label
		{
			text: qsTr("Rotation")
			font.bold: true
		}

		GridLayout
		{
			columns: 3
			rowSpacing: 0
			columnSpacing: 0

			Label { text: qsTr("RotationX") }
			Slider
			{
				id: rotationXSlider

				from: 0
				to: 360

				value: 0
				stepSize: 1
				snapMode: Slider.SnapAlways
				onValueChanged:
				{
					print("RotationX changed : " + value)
					rotationXChanged(value)
				}
				Layout.fillWidth: true
			}
			Label { text: rotationXSlider.value }

			Label { text: qsTr("RotationY") }
			Slider
			{
				id: rotationYSlider

				from: 0
				to: 360

				value: 0
				stepSize: 1
				snapMode: Slider.SnapAlways
				onValueChanged:
				{
					print("RotationY changed : " + value)
					rotationYChanged(value)
				}
				Layout.fillWidth: true
			}
			Label { text: rotationYSlider.value }

			Label { text: qsTr("RotationZ") }
			Slider
			{
				id: rotationZSlider

				from: 0
				to: 360

				value: 0
				stepSize: 1
				snapMode: Slider.SnapAlways
				onValueChanged:
				{
					print("RotationZ changed : " + value)
					rotationZChanged(value)
				}
				Layout.fillWidth: true
			}
			Label { text: rotationZSlider.value }
		} // GridLayout

		//Label
		//{
		//	text: qsTr("Video Speed")
		//	font.bold: true
		//}

		//GridLayout
		//{
		//	columns: 3
		//	rowSpacing: isMaterialUI ? -5 : 5
		//
		//	Button { text: "x0.50"; onClicked: print("playback speed: x" + 0.50) }
		//	Button { text: "x0.75"; onClicked: print("playback speed: x" + 0.75) }
		//	Button { text: "x1.00"; onClicked: print("playback speed: x" + 1.00) }
		//	Button { text: "x1.25"; onClicked: print("playback speed: x" + 1.25) }
		//	Button { text: "x1.50"; onClicked: print("playback speed: x" + 1.50) }
		//	Button { text: "x1.75"; onClicked: print("playback speed: x" + 1.75) }
		//	Button { text: "x2.00"; onClicked: print("playback speed: x" + 2.00) }
		//}
	}
}
