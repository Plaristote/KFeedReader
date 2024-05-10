import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kfeedreader 1.0
import "."

Kirigami.Page {
  property QtObject model
  id: page
  title: model.name
  actions.main: folderActions.mainAction
  actions.contextualActions: folderActions.contextualActions

  FolderActions {
    id: folderActions
    model: page.model
  }

  ColumnLayout {
    spacing: 0
    anchors.fill: parent

    Controls.ProgressBar {
      visible: model.fetching
      from: 0
      to: 1
      value: model.progress
      Layout.fillWidth: true
    }

    Controls.ItemDelegate {
      bold: true
      label: i18n("All")
      action: Controls.Action {
        checkable: true
        checked: pageStack.lastItem.folder == page.model
        onTriggered: pageStack.push(Qt.resolvedUrl("./AggregatedFeed.qml"), { folder: page.model })
      }
      UnreadCountBox {
        model: page.model
        Layout.alignment: Qt.VerticalCenter
      }
    }

    FolderTreeView {
      model: page.model
      withDragHandles: false
      onActivated: pageStack.push(item.view, { model: item });
      clip: true
      Layout.fillWidth: true
      Layout.fillHeight: true
    }
  }
}
