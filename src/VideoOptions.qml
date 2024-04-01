import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import Donut.DonutQMLPlayer

ScrollView
{
	id:  videoOptionsDialog

	property bool isMaterialUI: Utils.environmentVariable("QT_QUICK_CONTROLS_STYLE") == "Material"
	clip: true

	ColumnLayout
	{
		Label
		{
			text: qsTr("Video")
			font.bold: true
		}

		RowLayout
		{
			Button
			{
				text: qsTr("Enable")
				onClicked: print("video Enabled")
			}

			Button
			{
				text: qsTr("Disable")
				onClicked: print("video Disabled")
			}
		}

		Label
		{
			text: qsTr("Eualizer")
			font.bold: true
		}

		GridLayout
		{
			columns: 3
			rowSpacing: 0
			columnSpacing: 0

			Label { text: qsTr("Brightness") }
			Slider
			{
				id: brightnessSlider

				from: -10
				to: 10

				value: 0
				stepSize: 1
				snapMode: Slider.SnapAlways
				onValueChanged: print("brightness changed : " + value)
				Layout.fillWidth: true
			}
			Label { text: brightnessSlider.value }

			Label { text: qsTr("Contrast") }
			Slider
			{
				id: contrastSlider

				from: -10
				to: 10

				value: 0
				stepSize: 1
				snapMode: Slider.SnapAlways
				onValueChanged: print("contrast changed : " + value)
				Layout.fillWidth: true
			}
			Label { text: contrastSlider.value }

			Label { text: qsTr("Gamma") }
			Slider
			{
				id: gammaSlider

				from: -10
				to: 10

				value: 0
				stepSize: 1
				snapMode: Slider.SnapAlways
				onValueChanged: print("gamma changed : " + value)
				Layout.fillWidth: true
			}
			Label { text: gammaSlider.value }
		} // GridLayout

		Label
		{
			text: qsTr("Video Speed")
			font.bold: true
		}

		GridLayout
		{
			columns: 3
			rowSpacing: isMaterialUI ? -5 : 5

			Button { text: "x0.50"; onClicked: print("playback speed: x" + 0.50) }
			Button { text: "x0.75"; onClicked: print("playback speed: x" + 0.75) }
			Button { text: "x1.00"; onClicked: print("playback speed: x" + 1.00) }
			Button { text: "x1.25"; onClicked: print("playback speed: x" + 1.25) }
			Button { text: "x1.50"; onClicked: print("playback speed: x" + 1.50) }
			Button { text: "x1.75"; onClicked: print("playback speed: x" + 1.75) }
			Button { text: "x2.00"; onClicked: print("playback speed: x" + 2.00) }
		}
	}
}