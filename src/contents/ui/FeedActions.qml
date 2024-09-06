import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami

Item {
  required property QtObject model
  property bool focused: false
  property bool readOnly: false
  property alias mainAction: updateAction
  property list<QtObject> contextualActions
  property list<QtObject> actions: [mainAction].concat(contextualActions)
  id: root

  signal requireDestroy()
  signal toggleSearch()
  signal markAsRead()

  contextualActions: readOnly ? [searchAction, markAsReadAction] : [searchAction, markAsReadAction, editAction, removeAction]

  Kirigami.Action {
    id: updateAction
    text: i18n("Update")
    icon.name: "cloud-download"
    tooltip: i18n("Download new articles from the remote server")
    onTriggered: root.model.fetch()
  }

  Kirigami.Action {
    id: searchAction
    text: i18n("Search")
    icon.name: "search"
    tooltip: i18n("Filter through the titles of feed articles")
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
  }

  Kirigami.Action {
    id: markAsReadAction
    text: i18n("Mark all as read")
    icon.name: "mail-mark-read"
    tooltip: i18n("Mark all the articles in this feed as having already been read")
    onTriggered: root.markAsRead()
  }

  Kirigami.Action {
    id: editAction
    text: i18n("Edit")
    icon.name: "edit-entry"
    tooltip: i18n("Change the settings of a feed")
    onTriggered: pageStack.push(Qt.resolvedUrl("./EditFeed.qml"), { source: root.model })
  }

  Kirigami.Action {
    id: removeAction
    text: i18n("Remove")
    icon.name: "edit-delete-remove"
    tooltip: i18n("Permanently remove the feed from the application")
    onTriggered: root.requireDestroy()
  }
}
