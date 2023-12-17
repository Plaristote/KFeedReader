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
    property bool dropEnabled: treeView.withDragHandles && !dragMouseArea.dragActive

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
      color: dragOnTarget.containsDrag ? Kirigami.Theme.activeBackgroundColor : "transparent"

      // Drop zone management
      DropArea {
        id: dragPreviousTarget
        property QtObject targetItem: menuItem
        property string dropPosition: "previous"
        anchors { top: parent.top; left: parent.left; right: parent.right; }
        height: parent.height / 5
        enabled: dropEnabled

        Rectangle {
          visible: parent.containsDrag
          anchors.fill: parent
          color: Kirigami.Theme.activeBackgroundColor
        }
      }

      DropArea {
        id: dragOnTarget
        property QtObject targetItem: menuItem
        property string dropPosition: "inside"
        anchors {
          top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom;
          topMargin: dragPreviousTarget.height
          bottomMargin: dragNextTarget.height
        }
        enabled: dropEnabled
      }

      DropArea {
        id: dragNextTarget
        property QtObject targetItem: menuItem
        property string dropPosition: "next"
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right; }
        height: parent.height / 5;
        enabled: dropEnabled

        Rectangle {
          visible: parent.containsDrag
          anchors.fill: parent
          color: Kirigami.Theme.activeBackgroundColor
        }
      }
      // END Drop zone management

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
            const targetPosition = row.Drag.target ? row.Drag.target.dropPosition : "inside"

            if (targetItem) {
              switch (targetPosition) {
              case "inside":
                return itemModel.reparent(targetItem, menuItem);
              case "previous":
                return itemModel.appendBeforeSibling(menuItem, targetItem);
              case "next":
                return itemModel.appendNextToSibling(menuItem, targetItem);
              }
            }
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
