import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kfeedreader
import "."

Kirigami.Page {
  property QtObject model
  property var nextPage: window.getNextPageInStack(this, window.pageStack.lastItem)
  id: page
  title: model.name
  actions: folderActions.actions

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

    AllFeedListItem {
      label: i18n("All")
      iconSource: "mail-message-new-list"
      model: page.model
      nextPage: page.nextPage
      Layout.fillWidth: true
    }

    AllFeedListItem {
      unreadOnly: true
      label: i18n("Unread")
      iconSource: "mail-mark-unread-new"
      model: page.model
      nextPage: page.nextPage
      Layout.fillWidth: true
    }

    TreeView {
      id: treeView
      Layout.fillWidth: true
      Layout.fillHeight: true

      Timer {
        interval: 300
        running: true
        onTriggered: parent.model = page.model
      }

      delegate: Controls.ItemDelegate {
        id: treeItem
        required property bool expanded
        required property bool hasChildren
        required property int depth
        required property string displayName
        required property url iconUrl
        required property QtObject menuItem

        Component.onCompleted: {
          if (menuItem.expanded)
            treeView.expand(row)
        }

        onExpandedChanged: menuItem.expanded = expanded

        implicitWidth: treeView.width

        contentItem: TreeItemDelegate {
          title: displayName
          iconSource: iconUrl
          trailing: UnreadCountBox { model: menuItem }
        }

        action: Controls.Action {
          checkable: true
          checked: nextPage != null && nextPage.model == menuItem
          onTriggered: pageStack.push(menuItem.view, { model: menuItem })
        }
      }
    }
  }

  footer: ColumnLayout {
    Controls.ProgressBar {
      visible: model.fetching
      from: 0
      to: 1
      value: model.progress
      Layout.fillWidth: true
      Layout.preferredHeight: 10
    }
  }
}
