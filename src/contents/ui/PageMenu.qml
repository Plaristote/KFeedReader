import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

Controls.Menu {
  required property QtObject pageItem
  id: menuRoot

  Controls.MenuSeparator {}

  Controls.MenuItem {
    action: pageItem.actions.main
  }

  Instantiator {
    model: pageItem.actions.contextualActions
    delegate: Controls.MenuItem {
      action: pageItem.actions.contextualActions[index]
    }
    onObjectAdded: (index, object) => menuRoot.insertItem(index + 1, object)
    onObjectRemoved: (index, object) => menuRoot.removeItem(object)
  }
}
