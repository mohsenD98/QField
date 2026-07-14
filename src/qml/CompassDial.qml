import QtQuick
import QtQuick.Shapes
import org.qfield
import Theme

/**
 * A compass dial face made of a two-toned needle surrounded by cardinal
 * direction letters and intercardinal tick marks. The letters and tick marks
 * remain fixed while the needle turns to match \c mapRotation.
 * \ingroup qml
 */
Item {
  id: compassDial

  //! Map canvas rotation in degrees represented by the needle
  property real mapRotation: 0

  //! Color of the needle half pointing north as well as the north letter
  property color northColor: Theme.mainColor

  //! Color of the needle half pointing south and the rim circling the dial disc
  property color southColor: Theme.gray

  //! Color of the remaining cardinal letters and the tick marks
  property color markingsColor: Theme.lightGray

  //! Color of the dial disc and the needle hub ring
  property color dialColor: Theme.darkGray

  readonly property real centerX: width / 2
  readonly property real centerY: height / 2
  readonly property real needleHalfLength: height * 0.26
  readonly property real needleHalfWidth: width * 0.09
  readonly property real hubRadius: height * 0.085
  readonly property real letterRadius: height * 0.35
  readonly property real tickRadius: height * 0.41

  Rectangle {
    anchors.fill: parent
    radius: width / 2
    color: compassDial.dialColor
    opacity: 0.94
    border.color: compassDial.southColor
    border.width: Math.max(1.5, width * 0.028)
    antialiasing: true

    layer.enabled: true
    layer.effect: QfDropShadow {
      transparentBorder: true
      samples: 16
      color: "#66000000"
      horizontalOffset: 0
      verticalOffset: 1
    }
  }

  Repeater {
    model: 4

    delegate: Rectangle {
      readonly property real angleRadians: (45 + index * 90) * Math.PI / 180

      x: compassDial.centerX + compassDial.tickRadius * Math.sin(angleRadians) - width / 2
      y: compassDial.centerY - compassDial.tickRadius * Math.cos(angleRadians) - height / 2
      width: Math.max(1.5, compassDial.width * 0.032)
      height: compassDial.height * 0.055
      radius: width / 2
      rotation: 45 + index * 90
      color: compassDial.markingsColor
      opacity: 0.45
      antialiasing: true
    }
  }

  Repeater {
    model: [qsTr("N", "north"), qsTr("E", "east"), qsTr("S", "south"), qsTr("W", "west")]

    delegate: Text {
      readonly property bool isNorth: index === 0
      readonly property real angleRadians: index * 90 * Math.PI / 180

      x: compassDial.centerX + compassDial.letterRadius * Math.sin(angleRadians) - width / 2
      y: compassDial.centerY - compassDial.letterRadius * Math.cos(angleRadians) - height / 2
      text: modelData
      color: isNorth ? compassDial.northColor : compassDial.markingsColor
      opacity: isNorth ? 1 : 0.8
      font.pixelSize: Math.round(compassDial.height * (isNorth ? 0.19 : 0.14))
      font.bold: isNorth
    }
  }

  Shape {
    anchors.fill: parent
    rotation: compassDial.mapRotation
    preferredRendererType: Shape.CurveRenderer

    ShapePath {
      strokeColor: "transparent"
      fillColor: compassDial.northColor
      startX: compassDial.centerX
      startY: compassDial.centerY - compassDial.needleHalfLength

      PathLine {
        x: compassDial.centerX + compassDial.needleHalfWidth
        y: compassDial.centerY
      }
      PathLine {
        x: compassDial.centerX - compassDial.needleHalfWidth
        y: compassDial.centerY
      }
    }

    ShapePath {
      strokeColor: "transparent"
      fillColor: compassDial.southColor
      startX: compassDial.centerX
      startY: compassDial.centerY + compassDial.needleHalfLength

      PathLine {
        x: compassDial.centerX + compassDial.needleHalfWidth
        y: compassDial.centerY
      }
      PathLine {
        x: compassDial.centerX - compassDial.needleHalfWidth
        y: compassDial.centerY
      }
    }
  }

  Rectangle {
    x: compassDial.centerX - compassDial.hubRadius
    y: compassDial.centerY - compassDial.hubRadius
    width: compassDial.hubRadius * 2
    height: compassDial.hubRadius * 2
    radius: compassDial.hubRadius
    color: Theme.light
    border.color: compassDial.dialColor
    border.width: Math.max(1, compassDial.hubRadius * 0.45)
    antialiasing: true
  }
}
