import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl

Popup {
  id: control
  padding: 5
  modal: true

  background: Rectangle {
    radius: control.Material.roundedScale
    color: Theme.mainBackgroundColor

    layer.enabled: control.Material.elevation > 0
    layer.effect: RoundedElevationEffect {
      elevation: control.Material.elevation
      roundedScale: control.Material.roundedScale
    }
  }
}
