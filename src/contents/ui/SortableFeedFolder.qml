import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.treeview 1.0 as TreeView
import org.kde.kfeedreader 1.0

Kirigami.ScrollablePage {
  property QtObject model
  id: page
  title: model.name

  MenuItemModel {
    id: itemModel
    root: page.model
  }

  TreeView.TreeListView {
    id: treeView
    sourceModel: itemModel
    delegate: TreeView.AbstractTreeItem {
      id: listItem
      required property string displayName
      required property url iconUrl
      required property QtObject menuItem

      contentItem: Row {
        Item {
          implicitWidth: 16
          implicitHeight: 16
          Image {
            id: faviconImage
            source: iconUrl
            height: parent.height > 16 ? 16 : parent.height
            width: 16
            anchors.centerIn: parent
          }
        }

        Controls.Label {
          id: labelItem
          text: menuItem.name
        }
      }
    }
  }
}
