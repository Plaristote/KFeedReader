import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

Controls.Menu {
  required property QtObject pageItem
  id: menuRoot

  Instantiator {
    model: pageItem.actions
    delegate: Controls.MenuItem {
      action: pageItem.actions[index]
    }
    onObjectAdded: (index, object) => menuRoot.insertItem(index + 1, object)
    onObjectRemoved: (index, object) => menuRoot.removeItem(object)
  }
}
