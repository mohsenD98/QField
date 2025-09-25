import QtQuick
import QtQuick.Controls
import Theme

ListView {
  id: tabRow

  signal clicked

  orientation: Qt.Horizontal
  highlightFollowsCurrentItem: true
  currentIndex: 0

  highlight: Item {
    Rectangle {
      height: 2
      color: Theme.mainColor
      radius: 4
      width: parent.width
      anchors.bottom: parent.bottom
    }
  }

  delegate: TabButton {
    text: modelData
    height: tabRow.defaultHeight
    width: tabRow.parent.width / tabRow.count
    font: Theme.defaultFont
    checked: tabRow.currentIndex === index
    onClicked: {
      tabRow.currentIndex = index;
      tabRow.clicked();
    }
  }

  onCurrentIndexChanged: {
    tabRow.positionViewAtIndex(currentIndex, ListView.Contain);
  }

  property real defaultHeight: 48
}
