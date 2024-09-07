import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami

QtObject {
  required property QtObject model
  required property QtObject feed
  property QtObject nextArticle: feed.findNextArticle(model)
  property QtObject previousArticle: feed.findPreviousArticle(model)

  signal requestArticleChange(QtObject article)

  property list<QtObject> contextualActions: [
    Kirigami.Action {
      text: i18n("Next")
      icon.name: "arrow-up"
      tooltip: i18n("Go to the next article")
      shortcut: "Alt+Up"
      enabled: nextArticle
      onTriggered: requestArticleChange(nextArticle)
    },
    Kirigami.Action {
      text: i18n("Previous")
      icon.name: "arrow-down"
      tooltip: i18n("Go to the previous article")
      shortcut: "Alt+Down"
      enabled: previousArticle
      onTriggered: requestArticleChange(previousArticle)
    },
    Kirigami.Action {
      text: i18n("Comments")
      icon.name: "comment-symbolic"
      tooltip: i18n("When available, opens the comment section for this article")
      enabled: page.model.comments.toString().length > 0
      onTriggered: pageStack.layers.push(Qt.resolvedUrl("Comments.qml"), { model: page.model })
    },
    Kirigami.Action {
      text: i18n("Open")
      icon.name: "window-new"
      tooltip: i18n("Open the current article in a web browser")
      shortcut: "Ctrl+Alt+O"
      onTriggered: Qt.openUrlExternally(page.model.link)
    },
    Kirigami.Action {
      text: i18n("Mark as unread")
      icon.name: "mail-mark-unread"
      onTriggered: page.model.read = false
    },
    Kirigami.Action {
      id: closeAction
      text: i18n("Close")
      icon.name: "view-right-close"
      shortcut: Shortcut {
        sequence: "Esc"
        onActivatedAmbiguously: {
          if (!window.isFullScreen) { closeAction.trigger() }
          else { window.fullScreenWidget.closeFullScreen() }
        }
        onActivated: closeAction.trigger()
      }
      onTriggered: pageStack.pop()
    }
  ]
}
