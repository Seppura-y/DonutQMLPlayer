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

	property real lastMouseX: 0
	property real lastMouseY: 0
	property int activeEdges: 0
	property bool moveable: false
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

	onDoubleClicked:
	{
		if(window.visibility == Window.FullScreen)
		{
			window.showNormal()
		}
		else
		{
			window.showFullScreen()
		}

		moveable = false
	}

	onPressed: (mouse)=>
	{
		if(mouse.button !== Qt.LeftButton ||
			window.visibility == Window.Maximized || 
			window.visibility == Window.FullScreen)
		{
			return
		}

		activeEdges = 0
		if(mouseX < 8) activeEdges |= Qt.LeftEdge
		if(mouseY < 8) activeEdges |= Qt.TopEdge
		if(mouseX > window.width - 8) activeEdges |= Qt.RightEdge
		if(mouseY > window.height - 8) activeEdges |= Qt.BottomEdge
		moveable = (activeEdges === 0)

		if(!controlBar.contains(controlBar.mapFromItem(mouseArea, mouse.x, mouse.y))
			&& !titleBar.contains(titleBar.mapFromItem(mouseArea, mouse.x, mouse.y)))
		{
			moveable = false
		}

		if(Qt.platform.os !== "osx")
		{
			if(activeEdges !== 0)
			{
				window.startSystemResize(activeEdges)
			}
		}
		else
		{
			lastMouseX = mouseX
			lastMouseY = mouseY
		}

	}

	onReleased:
	{
		activeEdges = 0
		moveable = false
	}

	onPositionChanged: (mouse)=>
	{
		if(Qt.platform.os !== "osx" && moveable)
		{
			window.startSystemMove()
			moveable = false
		}

		if(isMaterialUI)
		{
			titleBar.visible = true
		}

		if(autoHideBars)
		{
			timer.restart()
		}

		if(!mouseArea.pressed)
		{
			if(window.visibility !== Window.Windowed)
			{
				mouseArea.cursorShape = Qt.ArrowCursor
			}
			else if((mouse.x < 8 && mouse.y < 8) || (mouse.x > width - 8 && mouse.y > height - 8))
			{
				mouseArea.cursorShape = Qt.SizeFDiagCursor
			}
			else if((mouse.x < 8 && mouse.y > height - 8) || (mouse.x > width - 8 && mouse.y < 8))
			{
				mouseArea.cursorShape = Qt.SizeBDiagCursor
				print("SizeBDiagCursor")
			}
			else if(mouse.x < 8 || mouse.x > width - 8)
			{
				mouseArea.cursorShape = Qt.SizeHorCursor
			}
			else if(mouse.y < 8  || mouse.y > height - 8)
			{
				mouseArea.cursorShape = Qt.SizeVerCursor
				print("SizeVerCursor")
			}
			else
			{
				mouseArea.cursorShape = Qt.ArrowCursor
			}
		}

		if(window.startSystemMove !== undefined && Qt.playform !== "osx")
		{
			return
		}

		if (window.visibility == Window.Maximized || window.visibility == Window.FullScreen || !pressed)
		{
			return
		}

		if(activeEdges & Qt.LeftEdge)
		{
			window.width -= (mouse.x - lastMouseX)
			window.x += (mouse.x - lastMouseX)
		}
		else if(activeEdges & Qt.RightEdge)
		{
			window.width += (mouse.x - lastMouseX)
			lastMouseX = mouse.x
		}

		if(activeEdges & Qt.TopEdge)
		{
			window.height -= (mouse.y - lastMouseY)
			window.y += (mouse.y - lastMouseY)
		}
		else if(activeEdges & Qt.BottomEdge)
		{
			window.height += (mouse.y - lastMouseY)
			lastMouseY = mouse.y
		}

		if(moveable)
		{
			window.x += (mouse.x - lastMouseX)
			window.y += (mouse.y - lastMouseY)
		}
	}

	onExited:
	{
		activeEdges = 0
		lastMouseX = 0
		lastMouseY = 0
	}


	Timer
	{
		id: timer
		interval: 2000
		onTriggered:
		{
			if(mouseArea.pressed === true || sideBar.visible)
			{
				return
			}

			mouseArea.cursorShape = Qt.BlankCursor
			if(!controlBar.contains(controlBar.mapFromItem(mouseArea, MouseArea.mouseX, mouseArea.mouseY)))
			{
				controlBar.visible = false
			}

			if(!titleBar.contains(titleBar.mapFromItem(mouseArea, MouseArea.mouseX, mouseArea.mouseY)))
			{
				titleBar.visible = false
			}
		}
	}
}