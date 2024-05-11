import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigami.delegates as KirigamiDelegate
import org.kde.kfeedreader 1.0
import "."

Kirigami.ScrollablePage {
  property QtObject model
  id: page
  title: model.name

  actions: folderActions.actions

  FolderActions {
    id: folderActions
    model: page.model
  }

  ColumnLayout {
    spacing: 0

    Controls.ItemDelegate {
      Layout.fillWidth: true
      contentItem: ListItemDelegate {
        title: i18n("All")
        trailing: UnreadCountBox { model: page.model }
      }
      action: Controls.Action {
        checkable: true
        checked: pageStack.lastItem.folder == page.model
        onTriggered: pageStack.push(Qt.resolvedUrl("./AggregatedFeed.qml"), { folder: page.model })
      }
    }

    Repeater {
      focus: true
      model: page.model.items.length
      delegate: Controls.ItemDelegate {
        property QtObject item: page.model.items[index]
        Layout.fillWidth: true
        highlighted: action.checked
        contentItem: ListItemDelegate {
          title: item.name
          subtitle: item.description.replace(/\n/g, '')
          iconSource: item.faviconUrl
          trailing: UnreadCountBox {
            model: page.model.items[index]
          }
        }
        action: Controls.Action {
          checkable: true
          checked: pageStack.lastItem.model == item
          onTriggered: pageStack.push(item.view, { model: item })
        }
      } // END delegate
    }
  }

  footer: ColumnLayout {
    Controls.ProgressBar {
      visible: model.fetching
      from: 0
      to: 1
      value: model.progress
      Layout.fillWidth: true
    }
  }
}
