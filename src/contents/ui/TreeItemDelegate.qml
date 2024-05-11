import QtQuick
import org.kde.kirigami as Kirigami
import "."

ListItemDelegate {
  required property QtObject menuItem
  property Component dragHandle
  property QtObject dragHandleItem

  leading: Row {
    Rectangle {
      height: 1
      width: depth * 16
      color: "transparent"
    }

    Loader {
      sourceComponent: dragHandle
      onLoaded: dragHandleItem = item
    }

    Kirigami.Icon {
      source: treeItem.expanded ? "arrow-down" : "arrow-right"
      visible: hasChildren
      width: 16; height: 16
      MouseArea { anchors.fill: parent; onClicked: treeView.toggleExpanded(row) }
    }

    Rectangle {
      color: "transparent"
      visible: !hasChildren
      width: 16; height: 16;
    }
  }
}
