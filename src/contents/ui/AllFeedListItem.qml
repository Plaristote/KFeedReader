import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import "."

Controls.ItemDelegate {
  id: root
  required property string label
  required property string iconSource
  required property QtObject model
  required property var nextPage
  property bool unreadOnly: false

  contentItem: ListItemDelegate {
    title: root.label
    iconSource: root.iconSource
    trailing: UnreadCountBox { model: root.model }
  }
  action: Controls.Action {
    checkable: true
    checked: nextPage != null && nextPage.folder == root.model
    onTriggered: pageStack.push(Qt.resolvedUrl("./AggregatedFeed.qml"), {
      folder: root.model,
      showUnreadOnly: root.unreadOnly
    })
  }
}
