import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.treeview 1.0 as TreeView
import org.kde.kfeedreader 1.0

TreeView.TreeListView {
  id: treeView
  property QtObject model
  property bool withDragHandles: true

  signal activated(QtObject item)

  Timer {
    id: delayFirstActivation
    running: true
    interval: 100
  }

  sourceModel: MenuItemModel {
    id: itemModel
    root: page.model
  }

  delegate: TreeView.AbstractTreeItem {
    id: listItem
    property string displayName: model.displayName
    property url iconUrl: model.iconUrl
    property QtObject menuItem: model.menuItem

    ListView.onIsCurrentItemChanged: {
      if (ListView.isCurrentItem) {
        if (delayFirstActivation.running)
          ListView.view.currentIndex = -1;
        else
          treeView.activated(menuItem);
      }
    }

    Component.onCompleted: {
      if (menuItem.expanded)
        decoration.model.expandChildren(index);
    }

    Component.onDestruction: {
      menuItem.expanded = kDescendantExpanded;
    }

    Connections {
      target: menuItem
      function onBeforeSave() { menuItem.expanded = kDescendantExpanded; }
    }

    contentItem: Rectangle {
      Layout.fillWidth: true
      implicitHeight: Kirigami.Units.gridUnit * 1.5
      color: dragTarget.containsDrag ? Kirigami.Theme.activeBackgroundColor : "transparent"

      DropArea {
        id: dragTarget
        property QtObject targetItem: menuItem
        anchors.fill: parent
        enabled: !dragMouseArea.dragActive && treeView.withDragHandles
      }

      RowLayout {
        id: row
        spacing: 5
        width: parent.width
        height: parent.height

        Drag.active: dragMouseArea.drag.active
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2
        Drag.keys: ["menuItem"]

        MouseArea {
          id: dragMouseArea
          visible: treeView.withDragHandles
          drag.target: parent
          height: parent.height
          width: height
          cursorShape: Qt.DragMoveCursor
          onReleased: {
            const targetItem = row.Drag.target ? row.Drag.target.targetItem : null;

            if (targetItem)
              itemModel.reparent(targetItem, menuItem);
            else
              row.x = row.y = 0;
          }
          Kirigami.Icon {
            source: "drag-handle-symbolic"
            anchors.fill: parent
          }
        }

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
          Layout.fillWidth: true
          id: labelItem
          text: displayName
          clip: true
        }

        UnreadCountBox {
          model: menuItem 
        }
      }
    }
  }
}
