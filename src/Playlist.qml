import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
//import QtQuick.Controls.Material
import Donut.DonutQMLPlayer

Item
{
	id: playList

	signal openFileRequested()
	signal playingIndexChanged()
	GridLayout
	{
		anchors.fill: parent
		columns: 3

		Label
		{
			text: qsTr("Playlist")
			color: "silver"
			font.pointSize: 16
			font.bold: true
			Layout.columnSpan: 3
		}

		ScrollView
		{
			Layout.columnSpan: 3
			Layout.fillWidth: true
			Layout.fillHeight: true

			ListView
			{
				id: listView
				model: PlaylistModel
				delegate: Rectangle
				{
					width: parent.width
					height: 20
					radius: 4
					clip: true
					color: index == listView.currentIndex ? "orange" : (index == PlaylistModel.playingIndex ? SkinColor.listItemCurrentActive : "transparent")
					Label
					{
						text: model.title
						color: "white"
					}
					MouseArea
					{
						anchors.fill: parent
						onClicked: listView.currentIndex = index
						onDoubleClicked: PlaylistModel.playItem(index)
					}
				}
			} // ListView
		} // ScrollView

		Button
		{
			id: addButton
			text: qsTr("Add")
			onClicked:
			{
				addMenu.x = addButton.x
				addMenu.y = addButton.y - addMenu.height
				addMenu.open()
			}
		}

		Button
		{
			id: delButton
			text: qsTr("Del")
			onClicked: PlaylistModel.removeItem(listView.currentIndex)
		}

		Button
		{
			id: clearButton
			text: qsTr("Clear")
			onClicked: PlaylistModel.clear()
		}
	} // GridLayout

	Menu
	{
		id: addMenu
		width: 100
		MenuItem
		{
			text: qsTr("File...")
			onTriggered: openFileRequested()
		}
	}

	Connections
	{
		target: PlaylistModel
		onPlayingSingleDrop:
		{
			PlaylistModel.playItem(index);
		}

		onPlayingIndexChanged:
		{
			playingIndexChanged()
		}
	}
}