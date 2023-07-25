import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

import org.qgis 1.0
import org.qfield 1.0

import Theme 1.0

Popup {
  id: popup

  property var layerTree
  property var index

  property bool zoomToButtonVisible: false
  property bool showFeaturesListButtonVisible: false
  property bool showVisibleFeaturesListDropdownVisible: false
  property bool reloadDataButtonVisible: false

  property bool trackingButtonVisible: false
  property var trackingButtonText

  property bool opacitySliderVisible: false

  parent: mainWindow.contentItem
  width: Math.min(childrenRect.width, mainWindow.width - Theme.popupScreenEdgeMargin)
  x: (mainWindow.width - width) / 2
  y: (mainWindow.height - height) / 2
  padding: 0

  onClosed: {
    index = undefined
  }

  onIndexChanged: {
    if (index === undefined)
      return

    updateTitle()
    updateCredits()

    itemVisibleCheckBox.checked = layerTree.data(index, FlatLayerTreeModel.Visible)
    itemLabelsVisibleCheckBox.checked = layerTree.data(index, FlatLayerTreeModel.LabelsVisible)

    expandCheckBox.text = layerTree.data( index, FlatLayerTreeModel.Type ) === 'group' ? qsTr('Expand group') : qsTr('Expand legend item')
    expandCheckBox.checked = !layerTree.data(index, FlatLayerTreeModel.IsCollapsed)

    reloadDataButtonVisible = layerTree.data(index, FlatLayerTreeModel.CanReloadData)
    zoomToButtonVisible = layerTree.data(index, FlatLayerTreeModel.HasSpatialExtent)
    showFeaturesListButtonVisible = isShowFeaturesListButtonVisible()
    showVisibleFeaturesListDropdownVisible = isShowVisibleFeaturesListDropdownVisible()

    trackingButtonVisible = isTrackingButtonVisible()
    trackingButtonText = trackingModel.layerInTracking(layerTree.data(index, FlatLayerTreeModel.VectorLayerPointer))
        ? qsTr('Stop tracking')
        : qsTr('Setup tracking')

    // the layer tree model returns -1 for items that do not support the opacity setting
    opacitySliderVisible = layerTree.data(index, FlatLayerTreeModel.Opacity) > -1
  }

  Page {
    id: popupContent
    width: parent.width
    padding: 0
    header: RowLayout {
      spacing: 2
      Label {
        id: titleLabel
        Layout.fillWidth: true
        Layout.leftMargin: 10
        topPadding: 10
        bottomPadding: 10
        text: ''
        font: Theme.strongFont
        horizontalAlignment: Text.AlignLeft
        wrapMode: Text.WordWrap
      }
      QfToolButton {
        id: zoomInButton
        Layout.alignment: Qt.AlignTop
        Layout.rightMargin: 0
        round: true
        visible: reloadDataButtonVisible

        bgcolor: "transparent"
        iconSource: Theme.getThemeVectorIcon( 'refresh_24dp' )
        iconColor: Theme.mainTextColor

        onClicked: {
          layerTree.data(index, FlatLayerTreeModel.MapLayerPointer).reload()
          close()
          dashBoard.visible = false
          displayToast(qsTr('Reload of layer %1 triggered').arg(layerTree.data(index, Qt.DisplayName)))
        }
      }
    }

    ScrollView {
      padding: 10
      ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
      ScrollBar.vertical.policy: ScrollBar.AsNeeded
      contentWidth: popupLayout.childrenRect.width
      contentHeight: popupLayout.childrenRect.height
      height: Math.min(popupLayout.childrenRect.height + 20, mainWindow.height - mainWindow.sceneTopMargin - mainWindow.sceneBottomMargin)
      clip: true

      ColumnLayout {
        id: popupLayout
        width: popupContent.width - 20
        spacing: 4

        FontMetrics {
          id: fontMetrics
          font: lockText.font
        }

        Text {
          id: invalidText
          visible: index !== undefined && !layerTree.data(index, FlatLayerTreeModel.IsValid)
          Layout.fillWidth: true
          bottomPadding: 15

          wrapMode: Text.WordWrap
          textFormat: Text.RichText
          text:  qsTr('This layer is invalid. This might be due to a network issue, a missing file or a misconfiguration of the project.')
          font: Theme.tipFont
          color: Theme.errorColor
        }

        CheckBox {
          id: expandCheckBox
          Layout.fillWidth: true
          topPadding: 5
          bottomPadding: 5
          text: qsTr('Expand legend item')
          font: Theme.defaultFont
          visible: index && layerTree.data(index, FlatLayerTreeModel.HasChildren) ? true : false

          onClicked: {
            layerTree.setData(index, checkState === Qt.Unchecked, FlatLayerTreeModel.IsCollapsed);
            close()
          }
        }

        CheckBox {
          id: itemVisibleCheckBox
          Layout.fillWidth: true
          topPadding: 5
          bottomPadding: 5
          text: qsTr('Show on map')
          font: Theme.defaultFont
          // visible for all layer tree items but nonspatial layers
          visible: index && layerTree.data(index, FlatLayerTreeModel.HasSpatialExtent) ? true : false
          indicator.height: 16
          indicator.width: 16
          indicator.implicitHeight: 24
          indicator.implicitWidth: 24

          onClicked: {
            layerTree.setData(index, checkState === Qt.Checked, FlatLayerTreeModel.Visible);
            flatLayerTree.mapTheme = '';
            projectInfo.saveLayerTreeState();
            close();
          }
        }

        CheckBox {
          id: itemLabelsVisibleCheckBox
          Layout.fillWidth: true
          topPadding: 5
          bottomPadding: 5
          text: qsTr('Show labels')
          font: Theme.defaultFont
          visible: index && layerTree.data(index, FlatLayerTreeModel.HasLabels) ? true : false
          indicator.height: 16
          indicator.width: 16
          indicator.implicitHeight: 24
          indicator.implicitWidth: 24

          onClicked: {
            layerTree.setData(index, checkState === Qt.Checked, FlatLayerTreeModel.LabelsVisible);
            projectInfo.saveLayerStyle(layerTree.data(index, FlatLayerTreeModel.MapLayerPointer))
            close();
          }
        }

        RowLayout {
          id: opacitySlider

          Layout.fillWidth: true
          Layout.topMargin: 4
          Layout.bottomMargin:4
          spacing: 4
          visible: opacitySliderVisible

          QfToolButton {
            Layout.alignment: Qt.AlignVCenter | Qt.alignHCenter
            Layout.leftMargin: 3
            Layout.rightMargin: 1
            width: 24
            height: 24
            padding: 0
            enabled: false

            icon.source: Theme.getThemeVectorIcon("ic_opacity_black_24dp")
            icon.color: Theme.mainTextColor
          }

          ColumnLayout {
            Layout.alignment: Layout.Center
            Layout.rightMargin: 6
            spacing: 0

            Text {
              Layout.fillWidth: true
              text: qsTr("Opacity")
              font: Theme.defaultFont
              color: Theme.mainTextColor
            }

            QfSlider {
              Layout.fillWidth: true

              id: slider
              value: index !== undefined ? layerTree.data(index, FlatLayerTreeModel.Opacity) * 100 : 0
              from: 0
              to: 100
              stepSize: 1
              suffixText: " %"
              height: 40

              onMoved: function () {
                layerTree.setData(index, value / 100, FlatLayerTreeModel.Opacity)
                projectInfo.saveLayerStyle(layerTree.data(index, FlatLayerTreeModel.MapLayerPointer))
              }
            }
          }
        }

        QfButton {
          id: zoomToButton
          Layout.fillWidth: true
          Layout.topMargin: 5
          text: index ? layerTree.data( index, FlatLayerTreeModel.Type ) === 'group'
                        ? qsTr('Zoom to group')
                        : layerTree.data( index, FlatLayerTreeModel.Type ) === 'legend'
                          ? qsTr('Zoom to parent layer')
                          : qsTr('Zoom to layer') : ''
          visible: zoomToButtonVisible
          icon.source: Theme.getThemeVectorIcon( 'zoom_out_map_24dp' )

          onClicked: {
            mapCanvas.mapSettings.extent = layerTree.nodeExtent(index, mapCanvas.mapSettings);
            close()
            dashBoard.visible = false
          }
        }

        QfButton {
          id: showFeaturesList
          Layout.fillWidth: true
          Layout.topMargin: 5
          dropdown: showVisibleFeaturesListDropdownVisible
          text: qsTr('Show features list')
          visible: showFeaturesListButtonVisible
          icon.source: Theme.getThemeVectorIcon( 'ic_list_black_24dp' )

          onClicked: {
            if ( parseInt(layerTree.data(index, FlatLayerTreeModel.FeatureCount)) === 0 ) {
              displayToast( qsTr( "The layer has no features" ) )
            } else {
              var vl = layerTree.data(index, FlatLayerTreeModel.VectorLayerPointer)
              var filter = layerTree.data(index, FlatLayerTreeModel.FilterExpression)
              featureForm.model.setFeatures(vl, filter)
              if (layerTree.data(index, FlatLayerTreeModel.HasSpatialExtent)) {
                mapCanvas.mapSettings.extent = layerTree.nodeExtent(index, mapCanvas.mapSettings)
              }
            }

            close()
            dashBoard.visible = false
          }

          onDropdownClicked: {
            showFeaturesMenu.popup(showFeaturesList.width - showFeaturesMenu.width + 10, showFeaturesList.y + 10)
          }
        }

        QfButton {
          id: trackingButton
          Layout.fillWidth: true
          Layout.topMargin: 5
          text: trackingButtonText
          visible: trackingButtonVisible
          icon.source: Theme.getThemeVectorIcon( 'directions_walk_24dp' )

          onClicked: {
            //start track
            if ( trackingModel.layerInTracking( layerTree.data(index, FlatLayerTreeModel.VectorLayerPointer) ) ) {
              trackingModel.stopTracker(layerTree.data(index, FlatLayerTreeModel.VectorLayerPointer));
              displayToast( qsTr( 'Track on layer %1 stopped' ).arg( layerTree.data(index, FlatLayerTreeModel.VectorLayerPointer).name  ) )
            } else {
              trackingModel.createTracker(layerTree.data(index, FlatLayerTreeModel.VectorLayerPointer), itemVisibleCheckBox.checked );
            }
            close()
          }
        }

        Text {
          id: lockText

          property var padlockIcon: Theme.getThemeIcon('ic_lock_black_24dp')
          property var padlockSize: fontMetrics.height - 5

          property bool isReadOnly: index !== undefined && layerTree.data(index, FlatLayerTreeModel.ReadOnly)
          property bool isGeometryLocked: index !== undefined && layerTree.data(index, FlatLayerTreeModel.GeometryLocked)

          visible: isReadOnly || isGeometryLocked
          Layout.fillWidth: true
          topPadding: 5

          wrapMode: Text.WordWrap
          textFormat: Text.RichText
          text: isReadOnly ? qsTr('Read-only layer') : qsTr('Geometry-locked layer')
          font: Theme.tipFont
          color: Theme.secondaryTextColor

          MouseArea {
            anchors.fill: parent
            onClicked: {
              if ( lockText.isReadOnly )
                displayToast(qsTr('This layer is configured as "Read-Only" which disables adding, deleting and editing features.'))
              else
                displayToast(qsTr('This layer is configured as "Lock Geometries" which disables adding and deleting features, as well as modifying the geometries of existing features.'))
            }
          }
        }

        Text {
          id: creditsText
          Layout.fillWidth: true
          Layout.topMargin: 5
          wrapMode: Text.WordWrap
          textFormat: Text.RichText
          text: ''
          font.pointSize: Theme.tipFont.pointSize
          font.italic: true
          color: Theme.secondaryTextColor

          onLinkActivated: (link) => { Qt.openUrlExternally(link) }
        }
      }
    }
  }

  Menu {
    id: showFeaturesMenu
    title: qsTr( "Show Features Menu" )

    width: {
      var result = 0;
      var padding = 0;
      for (var i = 0; i < count; ++i) {
        var item = itemAt(i);
        result = Math.max(item.contentItem.implicitWidth, result);
        padding = Math.max(item.padding, padding);
      }
      return result + padding * 2;
    }

    MenuItem {
      text: qsTr('Show visible features list')

      font: Theme.defaultFont
      height: 48
      leftPadding: 10

      onTriggered: {
        if ( parseInt(layerTree.data(index, FlatLayerTreeModel.FeatureCount)) === 0 ) {
          displayToast( qsTr( "The layer has no features" ) )
        } else {
          var vl = layerTree.data( index, FlatLayerTreeModel.VectorLayerPointer )
          var filter = layerTree.data(index, FlatLayerTreeModel.FilterExpression)
          featureForm.model.setFeatures( vl, filter, mapCanvas.mapSettings.visibleExtent )
        }

        close()
        dashBoard.visible = false
      }
    }
  }

  Connections {
    target: layerTree

    function onDataChanged(topleft, bottomright, roles) {
      if (index === undefined)
        return;

      if (roles.includes(FlatLayerTreeModel.FeatureCount)) {
        updateTitle();
      }
    }
  }

  function updateTitle() {
    if (index === undefined)
      return

    var title = layerTree.data(index, Qt.Name)
    var type = layerTree.data(index, FlatLayerTreeModel.Type)
    var vl = layerTree.data(index, FlatLayerTreeModel.VectorLayerPointer)
    if (vl) {
      if (type === 'legend') {
        title += ' (' + vl.name + ')'
      } else if (type === 'layer' && layerTree.data(index, FlatLayerTreeModel.IsValid)) {
        var count = layerTree.data(index, FlatLayerTreeModel.FeatureCount)
        if (count !== undefined && count >= 0) {
            var countSuffix = ' [' + count + ']'

            if (!title.endsWith(countSuffix))
                title += countSuffix
        }
      }
    }
    titleLabel.text = title
  }

  function updateCredits() {
    var credits = ''
    if (index !== undefined) {
      credits = StringUtils.insertLinks(layerTree.data(index, FlatLayerTreeModel.Credits))
    } else {
      credits = ''
    }

    creditsText.text = credits
    creditsText.visible = credits !== ''
  }

  function isTrackingButtonVisible() {
    if ( !index )
      return false

    return layerTree.data( index, FlatLayerTreeModel.Type ) === 'layer'
        && !layerTree.data( index, FlatLayerTreeModel.ReadOnly )
        && layerTree.data( index, FlatLayerTreeModel.Trackable )
        && positionSource.active
  }

  function isShowFeaturesListButtonVisible() {
    return layerTree.data( index, FlatLayerTreeModel.IsValid )
        && layerTree.data( index, FlatLayerTreeModel.LayerType ) === 'vectorlayer'
  }

  function isShowVisibleFeaturesListDropdownVisible() {
    return isShowFeaturesListButtonVisible()
        && layerTree.data(index, FlatLayerTreeModel.HasSpatialExtent)
  }
}
