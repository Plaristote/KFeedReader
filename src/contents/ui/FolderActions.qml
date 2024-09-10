import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.fluxkap 1.0
import "."

Item {
  required property QtObject model
  property bool focused: false
  property alias mainAction: updateAction
  property list<QtObject> contextualActions
  property list<QtObject> actions: [mainAction].concat(contextualActions)
  id: root

  signal toggleSearch()
  
  Kirigami.Action {
    id: updateAction
    text: i18n("Update")
    icon.name: "view-refresh"
    tooltip: i18n("Refreshes all the feed contained in this folder")
    onTriggered: root.model.fetch()
  }

  contextualActions: [
    Kirigami.Action {
      text: i18n("Add feed")
      icon.name: "list-add"
      tooltip: i18n("Adds an RSS feed to the current folder")
      onTriggered: pageStack.push(Qt.resolvedUrl("AddFeed.qml"), { parentFolder: root.model })
    },
    Kirigami.Action {
      text: i18n("Add folder")
      icon.name: "list-add"
      tooltip: i18n("Adds a sub-folder in the current folder")
      onTriggered: pageStack.push(Qt.resolvedUrl("AddFolder.qml"), { parentFolder: root.model })
    },
    Kirigami.Action {
      id: markAsReadAction
      text: i18n("Mark all as read")
      icon.name: "mail-mark-read"
      tooltip: i18n("Mark all the articles in this feed as having already been read")
      onTriggered: root.model.markAsRead()
    },
    Kirigami.Action {
      id: searchAction
      text: i18n("Search")
      icon.name: "search"
      tooltip: i18n("Find a feed by name")
      onTriggered: root.toggleSearch()
      shortcut: Shortcut {
        sequence: "Ctrl+F"
        enabled: root.focused
        onActivated: searchAction.trigger()
        onActivatedAmbiguously: {
          const page = window.pageStack.currentItem;
          page.toggleSearch && page.toggleSearch();
        }
      }
    },
    Kirigami.Action {
      text: i18n("Configure")
      icon.name: "configure"
      tooltip: i18n("Change a folder's identity and re-arrange its contents")
      onTriggered: pageStack.push(Qt.resolvedUrl("EditFolder.qml"), { model: root.model })
    },
    Kirigami.Action {
      text: i18n("Remove")
      visible: root.model != App.rootFolder
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
      root.model.remove();
      pageStack.pop();
    }
  }
}
