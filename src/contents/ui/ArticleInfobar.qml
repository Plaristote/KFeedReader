import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import "."

GridLayout {
  property QtObject model
  id: root
  columns: page.width > 800 ? 2 : 1

  Flickable {
    Layout.fillWidth: true
    Layout.preferredHeight: breadcrumbs.height
    contentWidth: breadcrumbs.width
    clip: true
    Breadcrumbs {
      id: breadcrumbs
      crumbs: root.model.crumbs
      anchors { top: parent.top; right: parent.right }
    }
  }

  ArticleMetadata {
    model: root.model
  }
}
