import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kfeedreader
import "."

TreeView {
  id: treeView
  delegate: Rectangle {
    id: treeItem
    required property QtObject menuItem
    required property bool expanded
    required property bool hasChildren
    required property int depth
    property bool dropEnabled: true

    color: dragOnTarget.containsDrag ? Kirigami.Theme.activeBackgroundColor : "transparent"
    implicitWidth: treeView.width
    implicitHeight: itemDelegate.height
    z: 1

    Component.onCompleted: {
      if (menuItem.expanded)
        treeView.expand(row)
    }

    onExpandedChanged: menuItem.expanded = expanded

    // Drop zone management
    DropArea {
      id: dragPreviousTarget
      property QtObject targetItem: menuItem
      property string dropPosition: "previous"
      anchors { top: parent.top; left: parent.left; right: parent.right; }
      height: Kirigami.Units.smallSpacing
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
      height: Kirigami.Units.smallSpacing
      enabled: dropEnabled

      Rectangle {
        visible: parent.containsDrag
        anchors.fill: parent
        color: Kirigami.Theme.activeBackgroundColor
      }
    }
    // END Drop zone management

    Controls.ItemDelegate {
      property QtObject menuItem: treeItem.menuItem
      property QtObject dragHandleItem
      id: itemDelegate
      width: parent.width

      Drag.active: dragHandleItem && dragHandleItem.drag.active
      Drag.hotSpot.x: width / 2
      Drag.hotSpot.y: height / 2
      Drag.keys: ["menuItem"]

      Connections {
        target: itemDelegate.Drag
        function onActiveChanged() {
          treeItem.z = itemDelegate.Drag.active ? 2 : 1;
          treeItem.dropEnabled = !itemDelegate.Drag.active;
        }
      }

      contentItem: TreeItemDelegate {
        Component.onCompleted: itemDelegate.dragHandleItem = contentItem.dragHandleItem
        id: contentItem
        menuItem: treeItem.menuItem
        title: menuItem.name
        iconSource: menuItem.faviconUrl
        dragHandle: MouseArea {
          drag.target: itemDelegate
          height: 16
          width: 16
          cursorShape: Qt.DragMoveCursor
          onReleased: {
            const targetItem = drag.target.Drag.target ? drag.target.Drag.target.targetItem : null;
            const targetPosition = drag.target.Drag.target ? drag.target.Drag.target.dropPosition : "inside"

            if (targetItem) {
              try {
                switch (targetPosition) {
                case "inside":
                  return treeView.model.reparent(targetItem, menuItem);
                case "previous":
                  return treeView.model.appendBeforeSibling(menuItem, targetItem);
                case "next":
                  return treeView.model.appendNextToSibling(menuItem, targetItem);
                }
              } catch (err) {
                console.log("Failed to reparent:", err);
              }
            }
            drag.target.x = drag.target.y = 0;
          }
          Kirigami.Icon {
            source: "drag-handle-symbolic"
            anchors.fill: parent
          }
        } // END MouseArea
      } // END ContentItem
    } // END Item
  } // END TreeView delegate
}
