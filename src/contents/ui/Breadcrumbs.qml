import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.fluxkap 1.0
import "."

RowLayout {
  required property var crumbs
  id: breadcrumbs
  spacing: 0

  Repeater {
    model: crumbs.length
    delegate: Controls.Button {
      Layout.fillWidth: index !== 0
      visible: crumbs[index].name.length > 0
      action: Kirigami.Action {
        text: crumbs[index].name
        icon.name: index === 0 ? "folder-root" : ""
        icon.source: crumbs[index].faviconUrl
        onTriggered: {
          if (index === 0) {
            pageStack.currentIndex = 0;
            return ;
          }
          for (var i = 0 ; i < pageStack.items.length ; ++i) {
            if (pageStack.items[i].model == crumbs[index]) {
              pageStack.currentIndex = i;
              return ;
            }
          }
          pageStack.replace(Qt.resolvedUrl(crumbs[index].view), { model: crumbs[index] })
        }
      }
    }
  }
}
