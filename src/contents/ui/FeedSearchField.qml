import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15

RowLayout {
  property var filter: null
  property alias text: field.text
  id: searchField
  onVisibleChanged: {
    if (visible)
      field.forceActiveFocus()
  }

  signal close()

  function matches(item, query) {
    return (!query && filter === null) || item.matchSearch(query ? query : filter);
  }

  Controls.Action {
    id: refreshAction
    icon.name: "search"
    onTriggered: {
      filter = field.text;
      //searchField.filter = new RegExp(field.text, 'i');
    }
  }

  Controls.Action {
    id: closeAction
    icon.name: "dialog-close"
    shortcut: Shortcut {
      sequence: "Esc"
      enabled: field.activeFocus
      onActivated: closeAction.trigger()
      onActivatedAmbiguously: if (field.activeFocus) { closeAction.trigger() }
    }
    onTriggered: searchField.visible = false
  }

  Controls.Label {
    text: i18n("Search")
  }

  Controls.TextField {
    Layout.fillWidth: true
    id: field
    onEditingFinished: refreshAction.trigger()
  }

  Controls.Button {
    action: refreshAction
  }

  Controls.Button {
    action: closeAction
  }
}
