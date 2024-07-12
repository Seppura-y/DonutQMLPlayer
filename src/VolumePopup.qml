
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts


Popup
{
  id: volumePopup
  width: 40
  height: 120
  z: 120

  background: Rectangle
  {
      anchors.fill: parent
      color: "#2e3137"
      radius: 8

      MouseArea
      {
          anchors.fill: parent
          hoverEnabled: true
          onEntered:
          {
              volumePopup.isAboutToClose = false
          }
          onPositionChanged:(value)=> 
          {
              volumePopup.isAboutToClose = false
          }
          onExited: 
          {
              volumePopup.isAboutToClose = true
          }
      }
  }

  closePolicy: Popup.NoAutoClose

  property bool isAboutToClose: false

  signal volumeValueUpdate(int value)

  function setValue(value)
  {
      slider.value = value
      //slider.increase()
  }

  function setMute()
  {
      if(slider.value != slider.from)
          slider.value = slider.from
      else
          slider.value = slider.to
  }



  Slider
  {
      id: slider
      from: 0
      to: 100
      value: 100
      stepSize: 1
      snapMode: Slider.SnapAlways
      anchors.fill: parent
      orientation: Qt.Vertical
      //focusPolicy: Qt.NoFocus
      topPadding: 0
      bottomPadding: 0
      //leftPadding: 0
      //rightPadding: 0

      property int upperValue: 100
      property int lowerValue: 0

      MouseArea
      {
          anchors.fill: parent
          //hoverEnabled: true
          property bool mouseClicked: false

          drag.target: handleRect
          drag.axis: Drag.YAxis // 修改为 Y 轴拖动
          drag.minimumY: slider.topPadding
          drag.maximumY: slider.height - slider.bottomPadding - handleRect.height

          onClicked: (mouse) =>
          {
              var clickPos = mouse.y - (handleRect.height / 2)
              var value = slider.from + (clickPos / (slider.height - slider.topPadding - slider.bottomPadding)) * (slider.to - slider.from)
              slider.value = Math.max(slider.from, Math.min(value, slider.to))
          }
          onPressed: (mouse)=>
          {
              mouseClicked = true

              var clickPos = Math.max(
                                      slider.topPadding,
                                      Math.min(
                                              mouse.y - (handleRect.height / 2),
                                              slider.height - slider.bottomPadding - (handleRect.height / 2)
                                      )
              )

              slider.value = slider.from + (clickPos / (slider.height - slider.topPadding - slider.bottomPadding)) * (slider.to - slider.from)
              handleRect.y = clickPos
          }
          onPositionChanged: (mouse) =>
          {
              volumePopup.isAboutToClose = false
              if (drag.active)
              {
                  var clickPos = Math.max(slider.topPadding,
                      Math.min(
                          mouse.y - (handleRect.height / 2),
                          slider.height - slider.bottomPadding - (handleRect.height / 2)
                      )
                  )
                  slider.value = slider.from + (clickPos / (slider.height - slider.topPadding - slider.bottomPadding)) * (slider.to - slider.from)
                  handleRect.y = clickPos
              }
          }
          onEntered:
          {
              volumePopup.isAboutToClose = false
          }
          onExited:
          {
              volumePopup.isAboutToClose = true
          }

          onReleased: mouseClicked = false
      } // MouseArea

      handle: Rectangle
      {
          id: handleRect
          width: 12
          height: 12
          radius: 6
          //anchors.bottom: parent.bottom
          color: slider.pressed ? "orange" : "white"
          //y: slider.visualPosition * (slider.height - slider.topPadding)
          y: slider.topPadding + (slider.visualPosition * (slider.height - slider.topPadding - slider.bottomPadding - height))
          anchors.horizontalCenter: parent.horizontalCenter
          MouseArea
          {
              anchors.fill: parent
              hoverEnabled: true
              drag.target: handleRect
              drag.axis: Drag.YAxis
              drag.minimumY: slider.topPadding
              drag.maximumY: slider.height - slider.bottomPadding - handleRect.height
              onEntered:
              {
                  handleRect.color = "orange"
                  volumePopup.isAboutToClose = false
              }
              onExited:
              {
                  handleRect.color = "white"
                  //volumePopup.isAboutToClose = true
              }
              onPressed: handleRect.color = "orange"
              onReleased: handleRect.color = "white"
              onPositionChanged:
              {
                  volumePopup.isAboutToClose = false
                  if (drag.active)
                  {
                      slider.value = slider.from + (handleRect.y / (slider.height - slider.topPadding - slider.bottomPadding)) * (slider.to - slider.from)
                  }
              }
          }
      } // handle: Rectangle

      background: Rectangle
      {
          id: track
          width: 6
          anchors.horizontalCenter: parent.horizontalCenter
          color: "#bdbebf"
          radius: 8
          MouseArea
          {
              anchors.fill: parent
              hoverEnabled: true
              onEntered:
              {
                  volumePopup.isAboutToClose = false
              }
              onExited:
              {
                  //volumePopup.isAboutToClose = true
              }

              onPositionChanged:
              {
                  volumePopup.isAboutToClose = false
              }
          }
      }

      contentItem: Item
      {
          Rectangle
          {
              id: contentRect
              x: track.x - track.width
              width: track.width
              // 为解决初始化值时，contentRectheight为负值，在onCompleted中设置把height的赋值移到onValueChanged中
              //height: (slider.visualPosition * (slider.height - slider.topPadding - slider.bottomPadding)) - (handleRect.height / 2)
              anchors.bottom: parent.bottom
              color: "orange"
              radius: 8
              MouseArea
              {
                  anchors.fill: parent
                  hoverEnabled: true
                  onEntered:
                  {
                      volumePopup.isAboutToClose = false
                  }
                  onExited:
                  {
                      //volumePopup.isAboutToClose = true
                  }
                  onPositionChanged:
                  {
                      volumePopup.isAboutToClose = false
                  }
              }
          }
      }

      onValueChanged:
      {
          print("main slider onValueChanged : " + slider.visualPosition)
          if(slider.visualPosition > 0.5)
          {
              contentRect.height = (slider.visualPosition * slider.height) - (handleRect.height / 2)
              handleRect.y = slider.height - contentRect.height - (handleRect.height / 2)
          }
          else
          {
              contentRect.height = (slider.visualPosition * slider.height)
              handleRect.y = slider.height - (handleRect.height) - slider.topPadding
          }
          volumeValueUpdate(slider.visualPosition * upperValue)
      }

      Component.onCompleted:
      {
          contentRect.height = track.height
          timer.start()
          value = from
          value = value + 1
          value = value - 1
      }
  } // Slider

    Timer
    {
        id: timer
        interval: 500
        onTriggered:
        {
            volumePopup.close()
        }
    }

  Connections
  {
      target: volumePopup
      function onIsAboutToCloseChanged()
      {
          if(volumePopup.isAboutToClose)
          {
              timer.start()
          }
          else
          {
              visible = true
              timer.stop()
          }
      }
  }
} // Popup