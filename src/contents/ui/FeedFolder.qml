import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0

Kirigami.ScrollablePage {
  property QtObject model
  id: page
  title: model.name

  ColumnLayout {
    spacing: 0

    Controls.ProgressBar {
      visible: model.fetching
      from: 0
      to: 1
      value: model.progress
      Layout.fillWidth: true
    }

    Kirigami.BasicListItem {
      bold: true
      label: i18n("All")
      action: Controls.Action {
        checkable: true
        checked: pageStack.lastItem.folder == page.model
        onTriggered: pageStack.push(Qt.resolvedUrl("./AggregatedFeed.qml"), { folder: page.model })
      }
    }

    Repeater {
      focus: true
      model: page.model.items
      delegate: Kirigami.BasicListItem {
        property QtObject item: page.model.items[index]
        bold: true
        label: item.name
        subtitle: item.description
        leadingFillVertically: false
        leading: Item {
          implicitWidth: 16
          Image {
            id: faviconImage
            source: item.faviconUrl
            height: parent.height > 16 ? 16 : parent.height
            width: 16
            anchors.centerIn: parent
          }
        }
        Rectangle {
          anchors.verticalCenter: parent.verticalCenter
          visible: item.unreadCount > 0
          color: "orange"
          implicitHeight: 16
          implicitWidth: unreadText.width + 10
          Text {
            id: unreadText
            anchors.centerIn: parent
            text: item.unreadCount
            color: "white"
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

  actions.main: Kirigami.Action {
    text: i18n("Update")
    icon.name: "cloud-download"
    tooltip: i18n("Refreshes all the feed contained in this folder")
    onTriggered: page.model.fetch()
  }

  actions.contextualActions: [
    Kirigami.Action {
      text: i18n("Add feed")
      icon.name: "list-add"
      tooltip: i18n("Adds an RSS feed to the current folder")
      onTriggered: pageStack.push(Qt.resolvedUrl("AddFeed.qml"), { parentFolder: page.model })
    },
    Kirigami.Action {
      text: i18n("Add folder")
      icon.name: "list-add"
      tooltip: i18n("Adds a sub-folder in the current folder")
      onTriggered: pageStack.push(Qt.resolvedUrl("AddFolder.qml"), { parentFolder: page.model })
    },
    Kirigami.Action {
      text: i18n("Remove")
      visible: page.model != App.rootFolder
      enabled: visible
      icon.name: "edit-delete-remove"
      tooltip: i18n("Permanently remove the folder and all its feeds from the application")
      onTriggered: destroyConfirmDialog.open()
    }
  ]

  Kirigami.PromptDialog {
    id: destroyConfirmDialog
    title: i18n("Removing folder")
    subtitle: i18n("Are you sure you want to permanently remove this folder ?")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    onAccepted: {
      page.model.remove();
      pageStack.pop();
    }
  }
}
