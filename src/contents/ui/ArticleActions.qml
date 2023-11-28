import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami

QtObject {
  required property QtObject model
  property list<QtObject> contextualActions: [
    Kirigami.Action {
      text: i18n("Open")
      icon.name: "internet-web-browser"
      tooltip: i18n("Open the current article in a web browser")
      onTriggered: Qt.openUrlExternally(page.model.link)
    },
    Kirigami.Action {
      text: i18n("Mark as unread")
      icon.name: "mail-mark-unread"
      onTriggered: page.model.read = false
    },
    Kirigami.Action {
      text: i18n("Close")
      icon.name: "view-right-close"
      onTriggered: pageStack.pop()
    }
  ]
}
